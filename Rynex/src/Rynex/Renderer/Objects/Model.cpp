#include "rypch.h"
#include "Model.h"

#include "Rynex/Asset/Base/AssetManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

namespace Rynex {

    Model::Model(const std::string& path)
    {
        std::string directory;
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.c_str(),
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "Fehler beim Laden der Datei: " << importer.GetErrorString() << std::endl;
            return;
        }

        directory = path.substr(0, path.find_first_of("/"));

        //ProcessNode(scene->mRootNode, scene);


    }

    Model::Model(std::vector<Ref<Mesh>>& meshes, std::vector<MeshRootData>& meshRootDatas)
        : m_Meshes(meshes), m_MeshRootDatas(meshRootDatas)
    {
        std::lock_guard<std::mutex> lock(m_Accese);
    }

   

    std::vector<MeshRenderData>&& Model::GetMeshRenderData()
    {
        RY_CORE_ASSERT(false);
        std::vector<MeshRenderData> meshRenderData;
        for (auto& mesh : m_Meshes)
        {

        }
        return std::move(meshRenderData);
    }

    void Model::Draw(const Ref<Shader>& shader)
    {
        std::lock_guard<std::mutex> lock(m_Accese);
        for (auto& mesh : m_Meshes)
            mesh->OnDraw(shader);
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        uint32_t size = node->mNumMeshes;
        m_Meshes.reserve(size);
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.emplace_back(ProcessMesh(mesh, scene));
        }
        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Ref<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<MeshVertex>     vertices;
        std::vector<uint32_t>       indices;
        std::vector<MeshTexture>    texures;


        bool hasNormals = mesh->mNormals != nullptr;
        bool hasTexCorrds = mesh->HasTextureCoords(0);
        uint32_t size = mesh->mNumVertices;
        vertices.resize(size);
        for (uint32_t i = 0; i < size; i++)
        {
            vertices.push_back({
                {
                    mesh->mVertices[i].x,
                    mesh->mVertices[i].y,
                    mesh->mVertices[i].z
                },
                {
                    hasNormals ? mesh->mNormals[i].x : 0.0f,
                    hasNormals ? mesh->mNormals[i].y : 0.0f,
                    hasNormals ? mesh->mNormals[i].z : 0.0f
                },
                {
                    hasTexCorrds ? mesh->mTextureCoords[0][i].x : 0.0f,
                    hasTexCorrds ? mesh->mTextureCoords[0][i].y : 0.0f,
                }
                });
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial* materials = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<MeshTexture> deffuseMaps = LoadMaterialTextures(materials, aiTextureType_DIFFUSE, "u_Texture_Diffuse");
        texures.insert(texures.end(), deffuseMaps.begin(), deffuseMaps.end());

        std::vector<MeshTexture> speculareMaps = LoadMaterialTextures(materials, aiTextureType_SPECULAR, "u_Texture_Speculare");
        texures.insert(texures.end(), speculareMaps.begin(), speculareMaps.end());

        std::vector<MeshTexture> normaleMaps = LoadMaterialTextures(materials, aiTextureType_HEIGHT, "u_Texture_Normale");
        texures.insert(texures.end(), normaleMaps.begin(), normaleMaps.end());

        std::vector<MeshTexture> heigthMaps = LoadMaterialTextures(materials, aiTextureType_AMBIENT, "u_Texture_Heigth");
        texures.insert(texures.end(), heigthMaps.begin(), heigthMaps.end());

        return CreateRef<Mesh>(std::move(vertices), std::move(indices), std::move(texures));
    }

    std::vector<MeshTexture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, const std::string& typeName)
    {
        std::vector<MeshTexture> textures;

        for (uint32_t i = 0; i < material->GetTextureCount(type); i++)
        {
            aiString str;
            material->GetTexture(type, i, &str);
            bool skip = false;


            if (!skip)
            {
                textures.push_back({
                    TextureFromFile(str.C_Str()),
                    typeName,
                    str.C_Str()
                    });
            }
        }
        return textures;
    }

    Ref<Texture> Model::TextureFromFile(const std::filesystem::path& toFile, bool gamma)
    {

        //Ref<Texture> texture = AssetManager::GetAsset<Texture>(m_Directory / toFile);
        return Ref<Texture>();
    }

}
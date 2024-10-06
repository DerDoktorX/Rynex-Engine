#include "rypch.h"
#include "ModelImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Renderer/Objects/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

namespace Rynex {

    namespace Utils {
        
        static Ref<Texture> TextureFromFile(const std::filesystem::path& toFile, const std::filesystem::path& directory, bool gamma = false)
        {
            Ref<Texture> texture = AssetManager::GetAsset<Texture>(directory / toFile);
            return texture;
        }

        static std::vector<MeshTexture>LoadMaterialTextures(aiMaterial* material, aiTextureType type, const std::string& typeName, const std::filesystem::path& directory)
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
                        TextureFromFile(str.C_Str(), directory),
                        typeName,
                        str.C_Str()
                    });
                }
            }
            return textures;
        }


        static Ref<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::filesystem::path& directory)
        {
            std::vector<MeshVertex>     vertices;
            std::vector<unsigned int>   indices;
            std::vector<MeshTexture>    texures;

          
            bool hasNormals = mesh->mNormals != nullptr;
            bool hasTexCorrds = mesh->HasTextureCoords(0);
            
            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
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
            std::vector<MeshTexture> deffuseMaps = LoadMaterialTextures(materials, aiTextureType_DIFFUSE, "u_Texture_Diffuse", directory);
            texures.insert(texures.end(), deffuseMaps.begin(), deffuseMaps.end());

            std::vector<MeshTexture> speculareMaps = LoadMaterialTextures(materials, aiTextureType_SPECULAR, "u_Texture_Speculare", directory);
            texures.insert(texures.end(), speculareMaps.begin(), speculareMaps.end());

            std::vector<MeshTexture> normaleMaps = LoadMaterialTextures(materials, aiTextureType_HEIGHT, "u_Texture_Normale", directory);
            texures.insert(texures.end(), normaleMaps.begin(), normaleMaps.end());

            std::vector<MeshTexture> heigthMaps = LoadMaterialTextures(materials, aiTextureType_AMBIENT, "u_Texture_Heigth", directory);
            texures.insert(texures.end(), heigthMaps.begin(), heigthMaps.end());

            return CreateRef<Mesh>(vertices, indices, texures);
        }

        static void ProcessNode(aiNode* node, const aiScene* scene,std::vector<MeshRootData>& meshRootDatas, std::vector<Ref<Mesh>>& meshes, const std::filesystem::path& directory)
        {
             for (uint32_t i = 0; i < node->mNumMeshes; i++)
             {
                    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                    aiMatrix4x4 matrix = node->mTransformation;
                    aiString name = node->mName;
                    std::string strName(name.data, name.length);
                    meshRootDatas.push_back({
                        (
                            matrix.a1, matrix.a2, matrix.a3, matrix.a4,
                            matrix.b1, matrix.b2, matrix.b3, matrix.b4,
                            matrix.c1, matrix.c2, matrix.c3, matrix.c4
                        ),
                        strName
                    });
                 meshes.push_back(ProcessMesh(mesh ,scene, directory));
             }
             for (uint32_t i = 0; i < node->mNumChildren; i++)
             {
                 ProcessNode(node->mChildren[i], scene, meshRootDatas, meshes, directory);
             }
        }
        
    }

    Ref<Model> ModelImporter::ImportModel(AssetHandle handle, const AssetMetadata& metadata)
    {
        return LoadModel(metadata.FilePath.string());
    }

    Ref<Model> ModelImporter::LoadModel(const std::filesystem::path& path)
    { 
        RY_PROFILE_FUNCTION();
        RY_CORE_INFO("Beginn Loding Moddel");
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.string().c_str(),
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            RY_CORE_ASSERT(false,"Error Faild to loade File: {0}", importer.GetErrorString());
            std::cout << "Fehler beim Laden der Datei: " << importer.GetErrorString() << std::endl;
            return nullptr;
        }

       
        std::vector<Ref<Mesh>> meshes;
        std::vector<MeshRootData> meshRootDatas;
        Utils::ProcessNode(scene->mRootNode, scene, meshRootDatas, meshes, path.parent_path());
        Ref<Model> model = CreateRef<Model>(meshes, meshRootDatas);
        RY_CORE_INFO("Ende Loding Moddel");
        return model;
    }

    void ModelImporter::ReLoadeModel(AssetHandle handle, const std::filesystem::path& path)
    {
    }
}
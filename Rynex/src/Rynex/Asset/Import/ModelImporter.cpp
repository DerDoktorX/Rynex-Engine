#include "rypch.h"
#include "ModelImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Renderer/Objects/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <execution>

#include <future>
#include <chrono>

namespace Rynex {

    struct Data
    {
        std::filesystem::path CurentFilePathExtention = "";
    };
    static Data s_Data = Data();


    namespace Utils {


        static glm::vec2 SetCoords(bool hasCorrds, aiVector3D* coords, uint32_t index)
        {
            if (!hasCorrds)
                return glm::vec2(0.0f, 0.0f);

            if (s_Data.CurentFilePathExtention == ".gltf")
                return glm::vec2(coords[index].x, -coords[index].y);

            return glm::vec2(coords[index].x, coords[index].y);
        }

        static glm::vec3 SetVec3(bool hasCorrds, aiVector3D* vec3, uint32_t index)
        {
            if (!hasCorrds)
                return glm::vec3(0.0f, 0.0f, 0.0f);

            return glm::vec3(vec3[index].x, vec3[index].y, vec3[index].z);
        }

#if RY_EDITOR_ASSETMANGER_THREADE ? 0 : 0
        static std::vector<MeshTexture> LoadMaterialTexturesAsync(aiMaterial* material, aiTextureType type, const std::string& typeName, const std::filesystem::path& directory)
        {
            std::vector<MeshTexture> textures;

            for (uint32_t i = 0; i < material->GetTextureCount(type); i++)
            {
                aiString str;
                material->GetTexture(type, i, &str);
                bool skip = false;

                if (!skip)
                {
                    textures.emplace_back(AssetManager::GetAsset<Texture>(directory / str.C_Str()),
                        typeName,
                        str.C_Str());
                }
            }
            return textures;
        }
#endif

        static void LoadMaterialTextures(std::vector<MeshTexture>& textures ,aiMaterial* material, aiTextureType type, const std::string& typeName, const std::filesystem::path& directory, bool async = false)
        {
            

            for (uint32_t i = 0; i < material->GetTextureCount(type); i++)
            {
                aiString str;
                material->GetTexture(type, i, &str);
                bool skip = false;

                if (!skip)
                {
                    textures.emplace_back(typeName,directory / str.C_Str());
                }
            }
        }

        static Ref<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, const std::filesystem::path& directory, bool async = false)
        {
            std::vector<MeshVertex>     vertices;
            std::vector<unsigned int>   indices;
            std::vector<MeshTexture>    texures;


            bool hasNormals = mesh->mNormals != nullptr;
            bool hasTexCorrds = mesh->HasTextureCoords(0);
            uint32_t size = mesh->mNumVertices;
            vertices.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                vertices.emplace_back(
                   SetVec3(true, mesh->mVertices, i),
                   SetVec3(hasNormals, mesh->mNormals, i),
                   SetCoords(hasTexCorrds, mesh->mTextureCoords[0], i));
            }
            size = mesh->mNumFaces;
            indices.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (uint32_t j = 0; j < face.mNumIndices; j++)
                    indices.emplace_back(face.mIndices[j]);
            }
           
            aiMaterial* materials = scene->mMaterials[mesh->mMaterialIndex];
            LoadMaterialTextures(texures, materials, aiTextureType_DIFFUSE, "u_Texture_Diffuse", directory, async);
            LoadMaterialTextures(texures, materials, aiTextureType_SPECULAR, "u_Texture_Speculare", directory, async);
            LoadMaterialTextures(texures, materials, aiTextureType_HEIGHT, "u_Texture_Normale", directory, async);
            LoadMaterialTextures(texures, materials, aiTextureType_AMBIENT, "u_Texture_Heigth", directory, async);


            // Ref<Mesh> meshObject = CreateRef<Mesh>(std::move(vertices), std::move(indices), std::move(texures), async);
            return CreateRef<Mesh>(std::move(vertices), std::move(indices), std::move(texures), async);
        }

#if RY_EDITOR_ASSETMANGER_THREADE ? 0 : 0
        static Ref<Mesh> ProcessMeshAsync(aiMesh* mesh, const aiScene* scene, const std::filesystem::path& directory)
        {
            std::vector<MeshVertex>     vertices;
            std::vector<uint32_t>   indices;
            std::vector<MeshTexture>    texures;

            bool hasNormals = mesh->mNormals != nullptr;
            bool hasTexCorrds = mesh->HasTextureCoords(0);
            uint32_t size = mesh->mNumVertices;
            vertices.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                vertices.emplace_back(
                    SetVec3(true, mesh->mVertices, i),
                    SetVec3(hasNormals, mesh->mNormals, i),
                    SetCoords(hasTexCorrds, mesh->mTextureCoords[0], i));
            }
            size = mesh->mNumFaces;
            indices.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                aiFace face = mesh->mFaces[i];
                for (uint32_t j = 0; j < face.mNumIndices; j++)
                    indices.emplace_back(face.mIndices[j]);
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

            return CreateRef<Mesh>(std::move(vertices), std::move(indices), std::move(texures));
        }
#endif

#if RY_EDITOR_ASSETMANGER_THREADE ? 0 : 0
        static void ProcessNodeAsync(aiNode* node, const aiScene* scene, std::vector<MeshRootData>& meshRootDatas, std::vector<Ref<Mesh>>& meshData, const std::filesystem::path& directory)
        {
            uint32_t size = node->mNumMeshes;

            for (uint32_t i = 0; i < size; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                aiMatrix4x4 matrix = node->mTransformation;
                aiString name = node->mName;
                std::string strName(name.data, name.length);
                meshRootDatas.emplace_back((
                    matrix.a1, matrix.a2, matrix.a3, matrix.a4,
                    matrix.b1, matrix.b2, matrix.b3, matrix.b4,
                    matrix.c1, matrix.c2, matrix.c3, matrix.c4
                    ), strName);
                meshData.emplace_back(ProcessMeshAsync(mesh, scene, directory));
            }


            for (uint32_t i = 0; i < node->mNumChildren; i++)
            {
                ProcessNodeAsync(node->mChildren[i], scene, meshRootDatas, meshData, directory);
            }
        }
#endif

        static void ProcessNode(aiNode* node, const aiScene* scene, std::vector<MeshRootData>& meshRootDatas, std::vector<Ref<Mesh>>& meshes, const std::filesystem::path& directory, bool async = false)
        {
            uint32_t size = node->mNumMeshes;


#if 0
            uint32_t cpuCorse = std::thread::hardware_concurrency();
            std::vector<uint32_t> m_MeshSize(size);
            std::for_each(std::execution::par, m_MeshSize.begin(), m_MeshSize.end(),
                [](uint32_t i)
                {
                    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                    aiMatrix4x4 matrix = node->mTransformation;
                    aiString name = node->mName;
                    std::string strName(name.data, name.length);
                    meshRootDatas.emplace_back((
                        matrix.a1, matrix.a2, matrix.a3, matrix.a4,
                        matrix.b1, matrix.b2, matrix.b3, matrix.b4,
                        matrix.c1, matrix.c2, matrix.c3, matrix.c4
                        ), strName);
                    meshes.push_back(ProcessMesh(mesh, scene, directory));
                });
#else
            for (uint32_t i = 0; i < size; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                aiMatrix4x4 matrix = node->mTransformation;
                aiString name = node->mName;
                std::string strName(name.data, name.length);
                meshRootDatas.emplace_back((
                    matrix.a1, matrix.a2, matrix.a3, matrix.a4,
                    matrix.b1, matrix.b2, matrix.b3, matrix.b4,
                    matrix.c1, matrix.c2, matrix.c3, matrix.c4
                    ), strName);
                
                meshes.emplace_back(ProcessMesh(mesh, scene, directory, async));
            }
#endif

            for (uint32_t i = 0; i < node->mNumChildren; i++)
            {
                ProcessNode(node->mChildren[i], scene, meshRootDatas, meshes, directory, async);
            }
        }

    }

    Ref<Model> ModelImporter::ImportModel(AssetHandle handle, const AssetMetadata& metadata, bool async)
    {
        Ref<Model> model = LoadModel((Project::GetActiveProjectDirectory()/metadata.FilePath).string(), async);
        return model;
    }

    

    Ref<Model> ModelImporter::LoadModel(const std::filesystem::path& path, bool async)
    {
        RY_PROFILE_FUNCTION();
        std::chrono::time_point<std::chrono::steady_clock> startTimePoint;
        std::chrono::time_point<std::chrono::steady_clock> endeTimePoint;
        RY_CORE_INFO("Beginn Loding Moddel");
        startTimePoint = std::chrono::high_resolution_clock::now();
        // s_Data.CurentFilePathExtention == path.extension();

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.string().c_str(),
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            RY_CORE_ASSERT(false, ("Error Faild to loade File: {0}", importer.GetErrorString()));
            return nullptr;
        }
        

        std::vector<Ref<Mesh>> meshes;
        std::vector<MeshRootData> meshRootDatas;
        Utils::ProcessNode(scene->mRootNode, scene, meshRootDatas, meshes, path.parent_path(), async);
        Ref<Model> model = CreateRef<Model>(meshes, meshRootDatas);
        endeTimePoint = std::chrono::high_resolution_clock::now();
        int64_t pastTime = std::chrono::time_point_cast<std::chrono::microseconds>(endeTimePoint).time_since_epoch().count()
            - std::chrono::time_point_cast<std::chrono::microseconds>(startTimePoint).time_since_epoch().count();
        RY_CORE_INFO("Ende Loding Moddel. Time {0}", pastTime);
        return model;
    }

#if RY_EDITOR_ASSETMANGER_THREADE ? 0 : 0 
    
    Ref<Model> ModelImporter::ImportModelAsync(AssetHandle handle, const AssetMetadata& metadata)
    {
        return LoadModelAsync((Project::GetActiveProjectDirectory() / metadata.FilePath).string());
    }

    Ref<Model> ModelImporter::LoadModelAsync(const std::filesystem::path& path)
    {
        RY_PROFILE_FUNCTION();
        std::chrono::time_point<std::chrono::steady_clock> startTimePoint;
        std::chrono::time_point<std::chrono::steady_clock> endeTimePoint;
        RY_CORE_INFO("Beginn Loding Moddel");
        startTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.CurentFilePathExtention == path.extension();

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.string().c_str(),
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            RY_CORE_ASSERT(false, ("Error Faild to loade File: {0}", importer.GetErrorString()));
            return nullptr;
        }


        std::vector<Ref<Mesh>> meshes;
        std::vector<MeshRootData> meshRootDatas;
        Utils::ProcessNodeAsync(scene->mRootNode, scene, meshRootDatas, meshes, path.parent_path());
        Ref<Model> model = CreateRef<Model>(meshes, meshRootDatas);
        
        endeTimePoint = std::chrono::high_resolution_clock::now();
        int64_t pastTime = std::chrono::time_point_cast<std::chrono::microseconds>(endeTimePoint).time_since_epoch().count()
            - std::chrono::time_point_cast<std::chrono::microseconds>(startTimePoint).time_since_epoch().count();
        RY_CORE_INFO("Ende Loding Moddel. Time {0}", pastTime);
        return model;
    }
#endif

    void ModelImporter::ReLoadeModel(AssetHandle handle, const std::filesystem::path& path, bool async )
    {
    }
}
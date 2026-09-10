#include "rypch.h"
#include "ModelImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"


#ifdef RY_ASSIMP_MODEL_LODER
    #include <assimp/Importer.hpp>
    #include <assimp/scene.h>
    #include <assimp/postprocess.h>
#endif // RY_PLATFORM_WINDOWS


#include <stb_image/stb_image.h>
#include <execution>

#include <future>
#include <chrono>

#ifdef RY_ASSIMP_MODEL_LODER

namespace Rynex {



    struct MeshLodingData
    {
        Ref<Mesh> MeshR;
        uint32_t MaterielIndex;
    };

    struct Data
    {
        std::filesystem::path CurentFilePathExtention = "";
    };

    static Data s_Data = Data();


    namespace Utils {


        static void ProcessHirachie( aiNode* node, MeshSource::EntityNodes& entityNodes)
        {
            aiString aiName = node->mName;
            aiNode* parent = node->mParent;


            aiMatrix4x4 aiMatrix = node->mTransformation;
  
            

           

            entityNodes.NodeName = std::string{ aiName.data, aiName.length };

            entityNodes.Matrics = glm::mat4{
                aiMatrix.a1,    aiMatrix.b1,    aiMatrix.c1,    aiMatrix.d1,
                aiMatrix.a2,    aiMatrix.b2,    aiMatrix.c2,    aiMatrix.d2,
                aiMatrix.a3,    aiMatrix.b3,    aiMatrix.c3,    aiMatrix.d3,
                aiMatrix.a4,    aiMatrix.b4,    aiMatrix.c4,    aiMatrix.d4 
            };


            uint32_t meshSize = node->mNumMeshes;
            entityNodes.ObjectMeshIndexVec.reserve(meshSize);
            uint32_t* aiMeshIndexPtr = node->mMeshes;

            for (uint32_t i = 0; i < meshSize; aiMeshIndexPtr++, i++)
                entityNodes.ObjectMeshIndexVec.emplace_back(*aiMeshIndexPtr);

            uint32_t childrenSize = node->mNumChildren;
            aiNode** nodeChildrens = node->mChildren;
           
           

            entityNodes.Childrens.reserve(childrenSize);
            for (uint32_t i = 0; i < childrenSize; nodeChildrens++, i++)
            {
                ProcessHirachie(
                    *nodeChildrens,
                    entityNodes.Childrens.emplace_back<MeshSource::EntityNodes>(
                        MeshSource::EntityNodes{}
                    )
                );
            }

        }

#pragma region _Material

        static void ReadTexureFilePaths(aiMaterial* materiel, aiTextureType type, const std::filesystem::path& assetPath, std::vector<std::filesystem::path>& pathTextures)
        { 
            uint32_t size = materiel->GetTextureCount(type);

            if (size == 0u)
                return;

            pathTextures.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                aiString fileNameString;
                materiel->GetTexture(type, i, &fileNameString);
                std::filesystem::path texPath = assetPath / fileNameString.C_Str();
                texPath = texPath.generic_string();
                texPath = texPath.lexically_normal();
                pathTextures.emplace_back(texPath);
            }
        }

        static MeshSource::_Material ReadMaterielData(aiMaterial* materiel,const std::filesystem::path& assetPath)
        {
            std::vector<std::filesystem::path> filePaths;
            aiString aiName = materiel->GetName();
           
            ReadTexureFilePaths(materiel, aiTextureType_DIFFUSE, assetPath, filePaths);
            ReadTexureFilePaths(materiel, aiTextureType_SPECULAR, assetPath, filePaths);
            ReadTexureFilePaths(materiel, aiTextureType_HEIGHT, assetPath, filePaths);

            aiColor4D color = aiColor4D(1.0f, 1.0f, 1.0f, 1.0f);
            aiReturn result = aiGetMaterialColor(materiel, AI_MATKEY_COLOR_DIFFUSE, &color); 
            glm::vec4 baseColor;
            if(result == aiReturn_SUCCESS)
                baseColor = glm::vec4{ color.r, color.g, color.b, color.a };
            else
                baseColor = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };

            return MeshSource::_Material{
                std::string{ aiName.data, aiName.length },
                static_cast<uint32_t>(-1),
                std::move(filePaths),
                baseColor
            };
        }

        static void ProcessMateriels(const aiScene* scene, std::vector<MeshSource::_Material>& materielVec, const std::filesystem::path& assetPath)
        {
            uint32_t size = scene->mNumMaterials;
            aiMaterial** aiMaterials = scene->mMaterials;
            materielVec.reserve(size);

            for (uint32_t i = 0; i < size; aiMaterials++, i++)
            {
                MeshSource::_Material& materiel = materielVec.emplace_back<MeshSource::_Material>(
                    ReadMaterielData(*aiMaterials, assetPath)
                );
                materiel.MaterielIndex = i;
            }

        }

#pragma endregion

#pragma region Mesh

        static float ReadMeshVertex(std::vector<MeshVerteices>& vertexData, aiVector3D* vericiesPtr, uint32_t size, glm::vec3 centerPos)
        {
            float boundingSpehreRadius = 0.0f;
            vertexData.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                MeshVerteices& mesVert = vertexData.emplace_back<MeshVerteices>(
                    MeshVerteices{
                        glm::vec3{vericiesPtr->x, vericiesPtr->y, vericiesPtr->z},
                        glm::vec2{0.0f, 0.0f},
                        glm::vec3{0.0f, 0.0f, 0.0f}
                    }
                );
                glm::vec3 center = mesVert.Postion - centerPos;
                float distenz = glm::length(center);
                boundingSpehreRadius = boundingSpehreRadius < distenz ? distenz : boundingSpehreRadius;
                vericiesPtr++;
            }
            return boundingSpehreRadius;

        }

        static float ReadMeshVertex(std::vector<MeshVerteices>& vertexData, aiVector3D* vericiesPtr, aiVector3D* normalesPtr, uint32_t size, glm::vec3 centerPos)
        {
            float boundingSpehreRadius = 0.0f;
            vertexData.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                MeshVerteices& mesVert = vertexData.emplace_back<MeshVerteices>(
                    MeshVerteices{
                        glm::vec3(vericiesPtr->x, vericiesPtr->y, vericiesPtr->z),
                        glm::vec2{0.0f, 0.0f },
                        glm::vec3(normalesPtr->x, normalesPtr->y, normalesPtr->z)
                    }
                );
                glm::vec3 center = mesVert.Postion - centerPos;
                float distenz = glm::length(center);
                boundingSpehreRadius = boundingSpehreRadius < distenz ? distenz : boundingSpehreRadius;

                normalesPtr++;
                vericiesPtr++;
            }
            return boundingSpehreRadius;
        }

        static float ReadMeshVertex(std::vector<MeshVerteices>& vertexData, aiVector3D* vericiesPtr, aiVector3D* normalesPtr, aiVector3D* coordes, uint32_t size, glm::vec3 centerPos)
        {
            float boundingSpehreRadius = 0.0f;
            vertexData.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                MeshVerteices& mesVert = vertexData.emplace_back<MeshVerteices>(
                    MeshVerteices{
                        glm::vec3{vericiesPtr->x, vericiesPtr->y, vericiesPtr->z},
                        glm::vec2{coordes->x, -coordes->y},
                        glm::vec3{normalesPtr->x, normalesPtr->y, normalesPtr->z}
                    }
                );
                glm::vec3 center = mesVert.Postion - centerPos;
                float distenz = glm::length(center);
                boundingSpehreRadius = boundingSpehreRadius < distenz ? distenz : boundingSpehreRadius;
                coordes++;
                normalesPtr++;
                vericiesPtr++;
            }
            return boundingSpehreRadius;
        }


        static void ReadIndeices(std::vector<uint32_t>& vertexData, uint32_t* indiciesPtr, uint32_t size)
        {
            vertexData.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                vertexData.emplace_back(*indiciesPtr);
                indiciesPtr++;
            }
        }

        static uint32_t GetIndexOfIdenticalPtrList(const std::vector<aiVector3D*>& vertexPosArrayPtrList, const aiVector3D* vertexPosArrayPtr)
        {
            for (uint32_t i = 0; i < vertexPosArrayPtrList.size(); i++)
            {
                const aiVector3D* ptr = vertexPosArrayPtrList.at(i);
                if (vertexPosArrayPtr == ptr)
                    return i;
            }
            return MAXUINT32;
        }

        static MeshSource::SourceMesh ReadVertexData(aiMesh* mesh, std::vector<MeshSource::SourceVertex>& vertexSourceData, std::vector<aiVector3D*>& vertexPosArrayPtrList)
        {
            std::vector<MeshVerteices> vertexData;
            std::vector<uint32_t> indciesData;
            aiString aiName = mesh->mName;
            
            uint32_t materielIndex = mesh->mMaterialIndex;

            uint32_t verticesSize = mesh->mNumVertices;
            aiVector3D* vericiesPtr = mesh->mVertices;
            aiVector3D* normalesPtr = mesh->mNormals;
            aiVector3D* coordes = mesh->mTextureCoords[0];

            const aiAABB& aabb = mesh->mAABB;
            const aiVector3D* maxPtr = &aabb.mMax;
            const aiVector3D* minPtr = &aabb.mMin;
            
            glm::vec3 max(maxPtr->x, maxPtr->y, maxPtr->z);
            glm::vec3 min(minPtr->x, minPtr->y, minPtr->z);


            std::string nameStr(aiName.data, aiName.length);
            BoundingVolume volume(max, min);
            const glm::vec3& center = volume.GetCenter();            
            RY_CORE_INFO("Mesh: {}, has aabb center x:{}, y:{}, z:{}", nameStr, center.x, center.y, center.z);
            float boundingSpehreRadius = 0.0f;

            
            if (mesh->HasTextureCoords(0) && mesh->HasNormals() && mesh->HasPositions())
                boundingSpehreRadius = ReadMeshVertex(vertexData, vericiesPtr, normalesPtr, coordes, verticesSize, center);
            else if (mesh->HasNormals() && mesh->HasPositions())
                boundingSpehreRadius = ReadMeshVertex(vertexData, vericiesPtr, normalesPtr, verticesSize, center);
            else if (mesh->HasPositions())
                boundingSpehreRadius = ReadMeshVertex(vertexData, vericiesPtr, verticesSize, center);
            else
                RY_CORE_ASSERT(false, "Not Prepart Stata");

            volume = BoundingVolume(max, min, center, boundingSpehreRadius);

            uint32_t facesSize = mesh->mNumFaces;
            aiFace* face = mesh->mFaces;
            for (uint32_t i = 0; i < facesSize; i++)
            {
                ReadIndeices(indciesData, face->mIndices, face->mNumIndices);
                face++;
            }

            
            uint32_t indexPtr = GetIndexOfIdenticalPtrList(vertexPosArrayPtrList, vericiesPtr);
            uint32_t indexValue =MeshSource::GetIndexVertexArraysIdentical(vertexSourceData, &vertexData, &indciesData);
            uint32_t index = indexPtr == indexValue ? indexPtr : indexValue;
            if (index == MAXUINT)
            {
                index = vertexSourceData.size();
               
                vertexSourceData.emplace_back<MeshSource::SourceVertex>(
                    MeshSource::SourceVertex{
                        vertexData,
                        indciesData
                    }
                );
            }

           
            return MeshSource::SourceMesh{
                std::move(vertexData),
                std::move(indciesData),
                materielIndex,
                volume,
                nameStr,
                static_cast<uint32_t>(-1),
                index,
                UUID()
            };
        }


        static void ProcessMeshes(const aiScene* scene, std::vector<MeshSource::SourceMesh>& meshes, std::vector<MeshSource::SourceVertex>& vertexSourceData)
        {
            uint32_t size = scene->mNumMeshes;
            aiMesh** aiMeshes = scene->mMeshes;
            meshes.reserve(size);
            vertexSourceData.reserve(size);

            std::vector<aiVector3D*> vertexPosArrayPtrList;

            for (uint32_t i = 0; i < size; aiMeshes++, i++)
            {
                MeshSource::SourceMesh& vertexSource = meshes.emplace_back<MeshSource::SourceMesh>(
                    ReadVertexData(*aiMeshes, vertexSourceData, vertexPosArrayPtrList)
                );
                vertexSource.MeshIndex = i;
            }
            vertexSourceData.shrink_to_fit();
        }



#pragma endregion

        static void ReadeSourceData(const aiScene* scene, std::vector<MeshSource::SourceMesh>& meshes, std::vector<MeshSource::SourceVertex>& vertexSourceData, std::vector<MeshSource::_Material>& materieles, MeshSource::EntityNodes& entityNodes, const std::filesystem::path& assetPath)
        {
            ProcessMeshes(scene, meshes, vertexSourceData);
            RY_CORE_INFO("Finshed Loading Meshes {} and {} Mesh Gemotry", meshes.size(), vertexSourceData.size());
            ProcessMateriels(scene, materieles, assetPath);
            RY_CORE_INFO("Finshed Loading Materiels {}", materieles.size());
            ProcessHirachie(scene->mRootNode, entityNodes);
            RY_CORE_INFO("Finshed Loading Hirachiy");
        }

    }

  

    
    Ref<MeshSource> ModelImporter::ImportModel(AssetHandle handle, const AssetMetadata& metadata)
    {
        std::filesystem::path filePath = metadata.AbsolutePath;
        Ref<MeshSource> model = LoadModel(filePath);
        return model;
    }
    Ref<MeshSource> ModelImporter::LoadModel(const std::filesystem::path& path)
    {
       
        RY_PROFILE_FUNCTION();
        std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;
        std::chrono::time_point<std::chrono::high_resolution_clock> endeTimePoint;
        RY_CORE_INFO("Beginn Loding Moddel {}", path.string());
        startTimePoint = std::chrono::high_resolution_clock::now();

        s_Data.CurentFilePathExtention = path.extension();

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.string().c_str(),
            aiProcess_Triangulate 
            | aiProcess_GenSmoothNormals 
            | aiProcess_FlipUVs
            | aiProcess_JoinIdenticalVertices
            | aiProcess_OptimizeMeshes
            | aiProcess_FindInstances
            | aiProcess_GenBoundingBoxes
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            RY_CORE_ASSERT(false, ("Error Faild to loade File: {0}", importer.GetErrorString()));
            return nullptr;
        }

        std::vector<MeshSource::SourceMesh> meshes;
        std::vector<MeshSource::_Material> materieles;
        MeshSource::EntityNodes entityNodes;
        std::vector<MeshSource::SourceVertex> vertexSourceData;
        Utils::ReadeSourceData(scene, meshes, vertexSourceData, materieles, entityNodes, path.parent_path());
        Ref<MeshSource> model = CreateRef<MeshSource>(std::move(materieles), std::move(meshes), std::move(vertexSourceData), std::move(entityNodes));
        
        return model;
    }



    bool ModelImporter::ReLoadeModel(AssetHandle handle, const std::filesystem::path& path)
    {
        RY_CORE_NOT_IMPL();
        Ref<MeshSource> model = AssetManager::GetAsset<MeshSource>(handle);
        
        return false;
    }
}

#endif
#include "rypch.h"
#include "ModelImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <execution>

#include <future>
#include <chrono>

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
#if RY_MESH_LOADING


        static void ProcessHirachie( aiNode* node, MeshSource::EntityNodes& entityNodes)
        {
            aiString aiName = node->mName;
            aiNode* parent = node->mParent;


            aiMatrix4x4 aiMatrix = node->mTransformation;
   
#if 0
            if (nullptr != parent)
            {
                aiMatrix = (parent->mTransformation * aiMatrix);
                // aiMatrix = aiMatrix;
            }
#endif
            

           

            entityNodes.NodeName = std::string{ aiName.data, aiName.length };
#if 0 
            aiMatrix = aiMatrix.Transpose();
            entityNodes.Matrics = glm::mat4{ 
                aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4,
                aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4,
                aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4,
                aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4
            };
#else
            entityNodes.Matrics = glm::mat4{
                aiMatrix.a1,    aiMatrix.b1,    aiMatrix.c1,    aiMatrix.d1,
                aiMatrix.a2,    aiMatrix.b2,    aiMatrix.c2,    aiMatrix.d2,
                aiMatrix.a3,    aiMatrix.b3,    aiMatrix.c3,    aiMatrix.d3,
                aiMatrix.a4,    aiMatrix.b4,    aiMatrix.c4,    aiMatrix.d4 
            };
#endif

#if RY_MODEL_IMPORTER_MATRIX_AXIS_FIX
            glm::mat4 axisFix(1.0f);
            axisFix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
            
            entityNodes.Matrics = entityNodes.Matrics * axisFix;
#endif
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
#if RY_ENABLE_INSTANCE_BUFFER

            
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
#else
            uint32_t index = MAXUINT;
#endif
           
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

#else
        static glm::vec2 SetCoords(bool hasCorrds, aiVector3D* coords, uint32_t index, const std::filesystem::path& extension)
        {
            if (!hasCorrds)
                return glm::vec2(0.0f, 0.0f);

            if (extension == ".gltf")
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

#if 0
        static Ref<Material> ProcessMaterial(aiMaterial* material, const aiScene* scene, const std::filesystem::path& directory)
        {
            std::vector<MeshTexture> texures;
            aiString name = material->GetName();
            std::string strName(name.data, name.length);
            
            uint32_t size = material->mNumProperties;
            aiMaterialProperty* properties = material->mProperties;
            for (uint32_t i = 0; i < size; i++)
            {

            }

            LoadMaterialTextures(texures, material, aiTextureType_DIFFUSE, "u_Texture_Diffuse", directory, false);
            LoadMaterialTextures(texures, material, aiTextureType_SPECULAR, "u_Texture_Speculare", directory, false);
            LoadMaterialTextures(texures, material, aiTextureType_HEIGHT, "u_Texture_Normale", directory, false);
            LoadMaterialTextures(texures, material, aiTextureType_AMBIENT, "u_Texture_Heigth", directory, false);

            return Material::CreateImport(std::move(strName), std::move(texures));
        }
#endif 
        
        static void LoadeMaterielTexture(aiMaterial* material, aiTextureType type, std::vector<std::filesystem::path>& textures, const std::filesystem::path& directory)
        {
            uint32_t size = material->GetTextureCount(type);
            RY_CORE_ASSERT(size != 0 || size != 1);
            for (uint32_t i = 0; i < size; i++)
            {
                aiString fileNameString;
                material->GetTexture(type, i, &fileNameString);
                textures.push_back(directory / fileNameString.C_Str());
            }
        }

        static Ref<Material> LoadeMaterial(aiMaterial* materiel, const std::filesystem::path& directory)
        {
            std::vector<std::filesystem::path> texturesMatieral;
            
            LoadeMaterielTexture(materiel, aiTextureType_DIFFUSE, texturesMatieral, directory);
            // LoadeMaterielTexture(materiel, aiTextureType_EMISSIVE, texturesMatieral, directory);
            LoadeMaterielTexture(materiel, aiTextureType_SPECULAR, texturesMatieral, directory);
            // LoadeMaterielTexture(materiel, aiTextureType_HEIGHT, texturesMatieral, directory);

            return Material::CreateBasic(texturesMatieral);
        }
        
        static void LoadeAllMateriels(const aiScene* scene, const std::filesystem::path& directory, std::vector<Ref<Material>>& materials)
        {
            uint32_t size = scene->mNumMaterials;
            aiMaterial** aiMaterialPtr = scene->mMaterials;
            aiMesh** aiMeshPtr = scene->mMeshes;
            materials.reserve(size);

            for (uint32_t i = 0; i < size; i++)
            {
                materials.emplace_back(LoadeMaterial(aiMaterialPtr[i], directory));
            }
        }

#if 0

        static std::vector<Ref<Texture>>&& LoadeTextures(const aiScene* scene, const std::filesystem::path& directory)
        {
            std::vector<Ref<Texture>> textures;
            uint32_t size = scene->mNumTextures;
            aiTexture** aiTextureslPtr = scene->mTextures;
            for (uint32_t i = 0; i < size; i++)
            {
                aiTexture* aiTexture = aiTextureslPtr[i];
                aiString str = aiTexture->mFilename;
                textures.emplace_back(AssetManager::GetAsset<Texture>( directory / str.C_Str() ));
            }
            return std::move(textures);
        }

        static Ref<Mesh> LoadeMeshes(aiMesh* mesh, Ref<Material> materiel)
        {

            std::vector<MeshVerteices>     vertices;
            std::vector<unsigned int>   indices;

            
            bool hasNormals = mesh->mNormals != nullptr;
            bool hasTexCorrds = mesh->HasTextureCoords(0);
            uint32_t size = mesh->mNumVertices;
            aiVector3D* vertecies = mesh->mVertices;
            aiVector3D* normales = mesh->mNormals;
            aiVector3D* coordes = mesh->mTextureCoords[0];


            vertices.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                vertices.emplace_back(
                    SetVec3(true, vertecies, i),
                    SetVec3(hasNormals, normales, i),
                    SetCoords(hasTexCorrds, coordes, i));
            }
            size = mesh->mNumFaces;
            indices.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                aiFace face = mesh->mFaces[i];

                for (uint32_t j = 0; j < face.mNumIndices; j++)
                    indices.emplace_back(face.mIndices[j]);
            }
            return CreateRef<Mesh>(std::move(vertices), std::move(indices), materiel);
        }

        static void LoadeAllMeshes(const aiScene* scene, std::vector<Ref<Mesh>>& mehes, const std::filesystem::path& directory)
        {
          
            std::vector<Ref<Material>> materieles;

            aiMesh** aiMeshPtr = scene->mMeshes;
            aiMaterial** aiMaterialPtr = scene->mMaterials;
            uint32_t sizeMesh = scene->mNumMeshes;
            uint32_t sizeMaterial = scene->mNumMaterials;
            mehes.reserve(sizeMesh);
            materieles.resize(sizeMaterial);

            for (uint32_t i = 0; i < sizeMesh; i++)
            {
                uint32_t materielIndex = aiMeshPtr[i]->mMaterialIndex;
                if (materieles.at(materielIndex) == nullptr)
                {
                    materieles[materielIndex] = LoadeMaterial(aiMaterialPtr[materielIndex], directory);
                }

                mehes.emplace_back(LoadeMeshes(aiMeshPtr[i], materieles.at(materielIndex)));
            }
            materieles.clear();
            materieles.shrink_to_fit();


           
        }

        static void LoadeNodeHierachy(aiNode* node, const std::vector<Ref<Mesh>>& meshes, std::vector<NodeData>& nodeData, int indexCurent, int indexParent)
        {    
            int index = indexCurent;
            NodeData nodeCurentData;
            nodeCurentData.Name = node->mName.C_Str();
            nodeCurentData.Parent = indexParent;

            uint32_t sizeMesh = node->mNumMeshes;
            nodeCurentData.Meshes.reserve(sizeMesh);

            aiMatrix4x4 aiM4x4 = node->mTransformation;
            nodeCurentData.Matrix = glm::mat4(
                aiM4x4.a1, aiM4x4.a2, aiM4x4.a3, aiM4x4.a4,
                aiM4x4.b1, aiM4x4.b2, aiM4x4.b3, aiM4x4.b4,
                aiM4x4.c1, aiM4x4.c2, aiM4x4.c3, aiM4x4.c4,
                aiM4x4.d1, aiM4x4.d2, aiM4x4.d3, aiM4x4.d4
            );
            
            for (uint32_t i = 0; i < sizeMesh; i++)
            {
                int indexMesh = node->mMeshes[i];
                nodeCurentData.Meshes.emplace_back(meshes.at(indexMesh));
            }


            uint32_t sizeChildren = node->mNumChildren;
            nodeCurentData.Children.reserve(sizeChildren);


            for (uint32_t i = 0; i < sizeChildren; i++)
            {
                index++;
                LoadeNodeHierachy(node->mChildren[i], meshes, nodeData, index, indexCurent);
                nodeCurentData.Children.emplace_back(index);
            }
            
            nodeData.push_back(nodeCurentData);

        }

        static void LoadeObject(const aiScene* scene, const std::filesystem::path& directory, std::vector<NodeData>& nodeData)
        {
            // std::vector<Ref<Material>> materiels = LoadeAllMateriels(scene, directory);
            std::vector<Ref<Mesh>> meshes;
            LoadeAllMeshes(scene, meshes, directory);

            

            aiNode* aiNodeEntry = scene->mRootNode;
            
            

            LoadeNodeHierachy(aiNodeEntry, meshes, nodeData, 0, -1);

            // materiels.clear();
            // materiels.shrink_to_fit();

            meshes.clear();
            meshes.shrink_to_fit();

            
        }

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
            std::vector<MeshVerteices>     vertices;
            std::vector<unsigned int>   indices;
            std::vector<MeshTexture>    texures;


            bool hasNormals = mesh->mNormals != nullptr;
            bool hasTexCorrds = mesh->HasTextureCoords(0);
            uint32_t size = mesh->mNumVertices;
            aiVector3D* vertecies = mesh->mVertices;
            aiVector3D* normales = mesh->mNormals;
            aiVector3D* coordes = mesh->mTextureCoords[0];
            

            vertices.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                vertices.emplace_back(
                   SetVec3(true, vertecies, i),
                   SetVec3(hasNormals, normales, i),
                   SetCoords(hasTexCorrds, coordes, i));
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
            std::vector<MeshVerteices>     vertices;
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
            std::vector<Ref<Mesh>> meshesNode;
            meshesNode.reserve(size);
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
                Ref<Mesh> meshRef = meshes.emplace_back(ProcessMesh(mesh, scene, directory, async));
                meshesNode.emplace_back(meshRef);
                
                
            }
            
            aiString name = node->mName;
            std::string strName(name.data, name.length);
            aiMatrix4x4 matrix = node->mTransformation;
            
            meshRootDatas.emplace_back(glm::mat4{
                    matrix.a1, matrix.a2, matrix.a3, matrix.a4,
                    matrix.b1, matrix.b2, matrix.b3, matrix.b4,
                    matrix.c1, matrix.c2, matrix.c3, matrix.c4,
                    matrix.d1, matrix.d2, matrix.d3, matrix.d4
                }, strName, std::move(meshesNode));
#endif

            for (uint32_t i = 0; i < node->mNumChildren; i++)
            {
                ProcessNode(node->mChildren[i], scene, meshRootDatas, meshes, directory, async);
            }
        }

#else
    
        static MeshLodingData LoadeMeshes(aiMesh* meshPtr, const std::filesystem::path& extension)
        {
            std::vector<MeshVerteices>     vertices;
            std::vector<unsigned int>   indices;


            bool hasNormals = meshPtr->mNormals != nullptr;
            bool hasTexCorrds = meshPtr->HasTextureCoords(0);
            uint32_t size = meshPtr->mNumVertices;
            aiVector3D* vertecies = meshPtr->mVertices;
            aiVector3D* normales = meshPtr->mNormals;
            aiVector3D* coordes = meshPtr->mTextureCoords[0];


            vertices.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                vertices.emplace_back(
                    SetVec3(true, vertecies, i),
                    SetVec3(hasNormals, normales, i),
                    SetCoords(hasTexCorrds, coordes, i, extension));
            }
            size = meshPtr->mNumFaces;
            // size = 2;
            uint32_t maxSize = 0;
            for (uint32_t i = 0; i < size; i++)
            {
                aiFace face = meshPtr->mFaces[i];
                uint32_t faceSize = face.mNumIndices;
                maxSize += faceSize;
                indices.reserve(faceSize);
                uint32_t* facePtr = face.mIndices;
                for (uint32_t j = 0; j < faceSize; j++)
                    indices.emplace_back(facePtr[j]);
            }

            uint32_t materielIndex = meshPtr->mMaterialIndex;
            return MeshLodingData{ 
                CreateRef<Mesh>(std::move(vertices), std::move(indices)), 
                materielIndex
            };
        }

        static void LoadeAllMeshes(const aiScene* scene, const std::filesystem::path& extension, std::vector<MeshLodingData>& mehsData)
        {
            aiMesh** aiMeshPtr = scene->mMeshes;
            aiMaterial** aiMaterialPtr = scene->mMaterials;
            uint32_t sizeMesh = scene->mNumMeshes;
            uint32_t sizeMaterial = scene->mNumMaterials;
            mehsData.reserve(sizeMesh);

            for (uint32_t i = 0; i < sizeMesh; i++)
            {
                MeshLodingData mesh = LoadeMeshes(aiMeshPtr[i], extension);
                mehsData.emplace_back(mesh);
            }
        }


        static void LoadeNodeHierachy(const aiScene* scene, aiNode* node, NodeLodingData& nodeData, std::vector<Ref<Mesh>>& meshesLoading)
        {
            
            aiString name = node->mName;
            nodeData.Name = std::string(name.data, name.length);

            aiMatrix4x4 aiM4x4 = node->mTransformation;
            nodeData.Matrix = glm::mat4(
                aiM4x4.a1, aiM4x4.a2, aiM4x4.a3, aiM4x4.a4,
                aiM4x4.b1, aiM4x4.b2, aiM4x4.b3, aiM4x4.b4,
                aiM4x4.c1, aiM4x4.c2, aiM4x4.c3, aiM4x4.c4,
                aiM4x4.d1, aiM4x4.d2, aiM4x4.d3, aiM4x4.d4
            );
            
            uint32_t* meshIndexPtr = node->mMeshes;
            uint32_t sizeMeshes = node->mNumMeshes;
            nodeData.MeshesR.reserve(sizeMeshes);
            
            for (uint32_t i = 0; i < sizeMeshes; i++)
            {
                uint32_t index = meshIndexPtr[i];
                Ref<Mesh> mesh = meshesLoading.at(index);

                nodeData.MeshesR.push_back(mesh);
            }

            uint32_t sizeChildren = node->mNumChildren;
            nodeData.Node.reserve(sizeChildren);
            aiNode** nodeChildrenPtr = node->mChildren;



            for (uint32_t i = 0; i < sizeChildren; i++)
            {
                LoadeNodeHierachy(scene, nodeChildrenPtr[i], nodeData.Node.emplace_back<NodeLodingData>( NodeLodingData{} ), meshesLoading);
            }
        }


        static void LoadeObject(const aiScene* scene, const std::filesystem::path& directory, const std::filesystem::path& extension, NodeLodingData& nodeData, std::vector<Ref<Mesh>>& meshLoading, std::vector<Ref<Material>>& materialLoading)
        {
            aiNode* node = scene->mRootNode;

            std::vector<MeshLodingData> meshesLoadingData;
            LoadeAllMateriels(scene, directory, materialLoading);
            LoadeAllMeshes(scene, extension, meshesLoadingData);
            uint32_t size = meshesLoadingData.size();
            meshLoading.reserve(size);
            for (uint32_t i = 0; i < size; i++)
            {
                const MeshLodingData& meshData = meshesLoadingData.at(i);
                Ref<Mesh> mesh = meshData.MeshR;
                Ref<Material> materiel = materialLoading.at(meshData.MaterielIndex);
                meshLoading.emplace_back(mesh);
                mesh->SetMateriel(materiel);
                
            }

            LoadeNodeHierachy(scene, node, nodeData, meshLoading);
        }
#endif

#if 0
        static Ref<Material>&& ProcessMaterielData(aiMaterial* material);

        static Ref<Mesh>&& ProcessMeshData(aiMesh* mesh);

        static void ProcessData(const aiScene* scene, const std::filesystem::path& directory, std::vector<Ref<Mesh>>& meshes, std::vector<Ref<Material>>& materiels, std::vector<Ref<MeshRootData>>& nodes)
        {
            for ()
            {

            }
        }
#endif


#endif
    }

  

    
#if RY_MESH_LOADING

    Ref<MeshSource> ModelImporter::ImportModel(AssetHandle handle, const AssetMetadata& metadata)
    {
#if 0
        std::filesystem::path filePath = (Project::GetActiveProjectDirectory() / metadata.FilePath).string();
#else
        std::filesystem::path filePath = metadata.AbsolutePath;
#endif
        Ref<MeshSource> model = LoadModel(filePath);
        return model;
    }
    Ref<MeshSource> ModelImporter::LoadModel(const std::filesystem::path& path)
    {
       
        RY_PROFILE_FUNCTION();
        std::chrono::time_point<std::chrono::steady_clock> startTimePoint;
        std::chrono::time_point<std::chrono::steady_clock> endeTimePoint;
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
#if RY_MODLE_MESH_INSTECENC_OPTI
            | aiProcess_FindInstances
            | aiProcess_GenBoundingBoxes
#endif

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

#else  
    
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
        s_Data.CurentFilePathExtention == path.extension();

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path.string().c_str(),
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs 

        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            RY_CORE_ASSERT(false, ("Error Faild to loade File: {0}", importer.GetErrorString()));
            return nullptr;
        }


#if RY_MODEL_NODE
        // std::vector<NodeData>  nodeData;
        // Utils::LoadeObject(scene, path.parent_path(), nodeData);
        std::vector<Ref<Mesh>> mesh;
        std::vector<Ref<Material>> materiel;
        NodeLodingData nodeLoding;
        Utils::LoadeObject(scene, path.parent_path(), path.extension(), nodeLoding, mesh, materiel);
        // Ref<Model> model = CreateRef<Model>(std::move(nodeData));
        Ref<Model> model = CreateRef<Model>(std::move(mesh), std::move(materiel), std::move(nodeLoding));
#else
        std::vector<Ref<Mesh>> meshes;
        std::vector<MeshRootData> meshRootDatas;
        Utils::ProcessNode(scene->mRootNode, scene, meshRootDatas, meshes, path.parent_path(), async);
        Ref<Model> model = CreateRef<Model>(meshes, meshRootDatas);
#endif
        endeTimePoint = std::chrono::high_resolution_clock::now();
        int64_t pastTime = std::chrono::time_point_cast<std::chrono::microseconds>(endeTimePoint).time_since_epoch().count()
            - std::chrono::time_point_cast<std::chrono::microseconds>(startTimePoint).time_since_epoch().count();
        RY_CORE_INFO("Ende Loding Moddel. Time {0}", pastTime);
        return model;
    }
#endif

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

    bool ModelImporter::ReLoadeModel(AssetHandle handle, const std::filesystem::path& path)
    {
        RY_CORE_NOT_IMPL();
        Ref<MeshSource> model = AssetManager::GetAsset<MeshSource>(handle);
        
        return false;
    }
}
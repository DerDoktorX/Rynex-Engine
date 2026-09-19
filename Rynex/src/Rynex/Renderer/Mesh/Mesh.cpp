#include "rypch.h"
#include "Mesh.h"

#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/Mesh/MeshSource.h>
#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Renderer/Materials/DefaultMaterial.h>


#include <numeric>



namespace Rynex {
    
    namespace Utils {

        namespace GenrateLoop{

            static void MaterielBuffer(const MeshSource::SourceMesh& meshSource, const std::vector<MeshSource::MaterialMesh>& materielSource, std::vector<Mesh::MeshMaterielIndex>& materielBuffer, std::vector<Ref<Material>>& materiels)
            {
                int materielIndex = static_cast<int>(meshSource.m_MaterielIndex);
                
                Ref<Material> materiel = CreateRef<DefaultMaterial>();
                materielBuffer.emplace_back<Mesh::MeshMaterielIndex>(
                    Mesh::MeshMaterielIndex{
                        materielIndex
                    }
                );


                materiels.emplace_back<Ref<Material>>(
                    Ref<Material>{
                        materiel
                    }
                );
            }

            static void PerDrawObjectBuffer(const MeshSource::SourceMesh& meshSource, std::vector<Mesh::PerDrawObject>& pdo, std::vector<uint32_t>& indicies, uint32_t vertexCount)
            { 
                const std::vector<uint32_t>& meshIndex = meshSource.m_ObjectMeshIndexVec;

                uint32_t indiciesSize = static_cast<uint32_t>(indicies.size());
                uint32_t meshSize = static_cast<uint32_t>(meshIndex.size());
                uint32_t instence = 0u;
                int baseVertex = static_cast<int>(vertexCount);
                uint32_t baseInstance = 0u;

                pdo.emplace_back<Mesh::PerDrawObject>(
                    Mesh::PerDrawObject{
                        meshSize,
                        instence,
                        indiciesSize,
                        baseVertex,
                        baseInstance
                    }
                );
            }

            static void CreateMeshID(const MeshSource::SourceMesh& meshSource, std::vector<UUID>& meshesID)
            {
                UUID meshHandle = UUID();
                meshesID.emplace_back<UUID>(
                    UUID{ 
                        meshHandle 
                    }
                );
            }

            static void BatchIncicies(const MeshSource::SourceMesh& meshSource, std::vector<uint32_t>& indicies)
            {
                const std::vector<uint32_t>& meshIndex = meshSource.m_ObjectMeshIndexVec;
                std::vector<uint32_t>::const_iterator beginMeshIndex = meshIndex.begin();
                std::vector<uint32_t>::const_iterator endMeshIndex = meshIndex.end();

                std::vector<uint32_t>::const_iterator postionBatchedIndex = indicies.end();

                indicies.insert<std::vector<uint32_t>::const_iterator>(postionBatchedIndex, beginMeshIndex, endMeshIndex);

            }

            static void BatchVertices(const MeshSource::SourceMesh& meshSource, std::vector<MeshVertices>& vertices)
            {
                const std::vector<MeshVertices>& meshVertices = meshSource.m_MeshVertices;
                std::vector<MeshVertices>::const_iterator beginMeshVertices = meshVertices.begin();
                std::vector<MeshVertices>::const_iterator endMeshVertices = meshVertices.end();

                std::vector<MeshVertices>::const_iterator postionBatchedVertices = vertices.end();

                vertices.insert<std::vector<MeshVertices>::const_iterator>(postionBatchedVertices, beginMeshVertices, endMeshVertices);

            }
        }


   

        static void GenartaeStaticMatrixBufferNode( std::vector<Mesh::MeshRenderObject>& objectVec, const glm::mat4& localeParent, const MeshSource::EntityNodes& sourceNodes, std::vector<uint32_t>& sortMatrix, const Ref<MeshSource>& source)
        {

            const std::vector<Ref<MeshSingle>>& meshSingleVec = source->GetMeshSingleVecConst();
            const std::vector<MeshSource::SourceMesh>& meshSourcesVec = source->GetMeshSourcesConst();
            const std::vector<Ref<Material>>& materialVec = source->GetMaterialsVecConst();

            uint32_t meshSize = sourceNodes.m_ObjectMeshIndexVec.size();
            
            glm::mat4 meshMatrix;
            


            meshMatrix = localeParent * sourceNodes.m_Matrics;

            if(meshSize != 0u)
            {
                objectVec.reserve(meshSize);
                sortMatrix.reserve(meshSize);
            }

            uint32_t sizeSourceMesh = meshSourcesVec.size();
            uint32_t sizeObjectMesh = meshSingleVec.size();
            RY_CORE_ASSERT(sizeSourceMesh == sizeObjectMesh, "Not Equal Size Differenz {}", static_cast<int>(static_cast<int>(sizeSourceMesh) - static_cast<int>(sizeObjectMesh)) );

            uint32_t sizeMateriel = materialVec.size();
            const std::string& nameNode = sourceNodes.m_NodeName;

            for (const uint32_t& objectMeshIndex : sourceNodes.m_ObjectMeshIndexVec)
            {
                RY_CORE_ASSERT(objectMeshIndex < sizeObjectMesh, "Higer Mesh Index then Expected {}", objectMeshIndex);
                const MeshSource::SourceMesh& vertexSource = meshSourcesVec.at(objectMeshIndex);
                const uint32_t& indexMateriel = vertexSource.m_MaterielIndex;

                RY_CORE_ASSERT(indexMateriel < sizeMateriel, "Higer Materiel Index then Expected {}", indexMateriel);

                const Ref<Material>& material = materialVec.at(indexMateriel);
                const Ref<MeshSingle>& singleMesh = meshSingleVec.at(objectMeshIndex);
                
                sortMatrix.emplace_back(objectMeshIndex);
                objectVec.emplace_back<Mesh::MeshRenderObject>(
                    Mesh::MeshRenderObject{ 
                        singleMesh, 
                        material,
                        nameNode,
                        meshMatrix, 
                        
                        objectMeshIndex,
                        indexMateriel 
                    }
                );
               
            }
            
            for (const MeshSource::EntityNodes& childrens : sourceNodes.m_Childrens)
            {
                GenartaeStaticMatrixBufferNode(objectVec, meshMatrix, childrens, sortMatrix, source);
            }

        }

        static void GenartaeStaticMatrixBuffer(const MeshSource::EntityNodes& sourceNodes, std::vector<Mesh::MeshRenderObject>& meshRenderObject, const Ref<MeshSource>& source)
        {
            std::vector<uint32_t> orderMatrixIndicies;
            std::vector<Mesh::MeshRenderObject> unoredeObjectVec;


            GenartaeStaticMatrixBufferNode(unoredeObjectVec, glm::mat4(1.0f), sourceNodes, orderMatrixIndicies, source);

            RY_CORE_ASSERT(unoredeObjectVec.size() == orderMatrixIndicies.size());
            std::vector<size_t> indicesSort(unoredeObjectVec.size());
            std::iota(indicesSort.begin(), indicesSort.end(), 0ull);

            std::sort(indicesSort.begin(), indicesSort.end(), 
                [&orderMatrixIndicies](size_t a, size_t b)
                {
                    return orderMatrixIndicies[a] < orderMatrixIndicies[b]; 
                }
            );

            meshRenderObject.reserve(unoredeObjectVec.size());
            for (size_t i : indicesSort)
            {
                Mesh::MeshRenderObject& object = unoredeObjectVec[i];
                meshRenderObject.emplace_back(object);
            }

        }

        template<typename T>
        static void ResizeToGreater(uint32_t gretesrSize, std::vector<T>& vec, const T& defaultValue)
        {
            uint32_t size = vec.size();
            if (size < gretesrSize)
            {
                uint32_t offset = gretesrSize - size;
                vec.resize(gretesrSize); 
            }

           
        }
       

        static MeshStatic::SingleObjectMeshData&& CreateSingleMeshData(Mesh::MeshRenderObject& object)
        {
            return MeshStatic::SingleObjectMeshData{
                object.m_Mesh,
                object.m_Material,
                object.m_Matrix,
                object.m_NodeName,
                object.m_LocaleIndexMesh,
                object.m_LocaleIndexMateriel
            };
        }

        static std::vector<MeshStatic::SingleObjectMeshData> GenartaeMeshStaicPackedSigleMeshData( std::vector<Mesh::MeshRenderObject>& objectVec)
        {

            uint32_t size = objectVec.size();
       
            ResizeToGreater(size, objectVec, Mesh::MeshRenderObject{ 
                Ref<MeshSingle>(nullptr), 
                Ref<Material>(nullptr), 
                std::string("Not Set"), 
                glm::mat4(-1.5f), 
                static_cast<uint32_t>(-1),
                static_cast<uint32_t>(-1)
                }
            );

            std::vector<MeshStatic::SingleObjectMeshData> singleMeshData;
            singleMeshData.reserve(size);

            for (uint32_t i = 0; i < size; i++)
            {

                Mesh::MeshRenderObject& object = objectVec.at(i);
               
                singleMeshData.emplace_back<MeshStatic::SingleObjectMeshData>(
                   MeshStatic::SingleObjectMeshData{
                        object.m_Mesh,
                        object.m_Material,
                        object.m_Matrix,
                        object.m_NodeName,
                        object.m_LocaleIndexMesh,
                        object.m_LocaleIndexMateriel
                   }
                );
            }

            return singleMeshData;
        }
    }



   
    // old Version
    Ref<MeshStatic> Mesh::CreateStaticMeshOld(Ref<MeshSource> source)
    {
        Ref<MeshStatic> meshStatic = source->GetStaticMesh();
        if (meshStatic != nullptr)
        {
            return meshStatic;
        }
        uint32_t i = 0;
        const std::vector<Ref<VertexBuffer>>& vabVec = source->GetVertexBufferVec();
        const std::vector<Ref<IndexBuffer>>& iabVec = source->GetShadeIndexBufferVec();

        const Ref<StorageBuffer>& ssboM = source->GetOffsetBuffer();

        const MeshSource::EntityNodes& node = source->GetNodes();
        std::vector<Ref<Material>>& materiels = source->GetMaterialsVec();


        if (!source->HasMeshDataGenerated())
        {
            source->GenerateMeshDataIndividuallyBuffer();
        }
        
        std::vector<Mesh::PerDrawObject>& pdo = source->GetPerDrawObjectsShadeVec();


        std::vector<Mesh::MeshRenderObject> meshRenderObject;
        Utils::GenartaeStaticMatrixBuffer(node, meshRenderObject, source);

        std::vector<MeshStatic::SingleObjectMeshData> singleMeshData = std::move(
            Utils::GenartaeMeshStaicPackedSigleMeshData(meshRenderObject)
        );


        meshStatic = CreateRef<MeshStatic>(source, singleMeshData);

        source->SetStaticMesh(meshStatic);

        return meshStatic;
    }


    // new Version
    Ref<MeshStatic> Mesh::CreateStaticMesh(Ref<MeshSource> source)
    {
        Ref<MeshStatic> meshStatic = source->GetStaticMesh();
        if (meshStatic != nullptr)
        {
            return meshStatic;
        }
        uint32_t i = 0;
        const std::vector<Ref<VertexBuffer>>& vabVec = source->GetVertexBufferVec();
        const std::vector<Ref<IndexBuffer>>& iabVec = source->GetShadeIndexBufferVec();


        const MeshSource::EntityNodes& node = source->GetNodes();
        std::vector<Ref<Material>>& materiels = source->GetMaterialsVec();


        if (!source->HasMeshDataGenerated())
        {
            source->GenerateMeshDataIndividuallyBuffer();
        }


        meshStatic = CreateRef<MeshStatic>(source);

        RY_CORE_ASSERT(meshStatic);
        source->SetStaticMesh(meshStatic);
        RY_CORE_WARN("Materiels are not Checked!");
        return meshStatic;
    }

    Ref<MeshStatic> Mesh::CreateStaticMesh(const FileSystem::Path& path)
    {
        Ref<MeshSource> source = AssetManager::GetAsset<MeshSource>(path);
        Ref<MeshStatic> meshStatic = Mesh::CreateStaticMesh(source);
        return meshStatic;
    }

}
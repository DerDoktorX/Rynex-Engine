#include "rypch.h"
#include "StaticeRenderProxys.h"

#include <Rynex/Renderer/RenderProxy/Proxy.h>

#include <Rynex/Renderer/Objects/BoundingVolume.h>
#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/RenderCommand.h>
#include <Rynex/Renderer/Rendering/RenderTarget.h>
#include <Rynex/Memory/ElementVectorFunc.h>

namespace Rynex {


    StaticeRenderProxys::StaticeRenderProxys()
        : m_BatchManager()
        , m_RenderProxysListData()
        , m_ProxyDrawCallGenarterMain()
        , m_ProxyDrawCallGenarterCurent()
        , m_DrawCallsVec()
    { 
    }

    StaticeRenderProxys::~StaticeRenderProxys()
    {
        Clear();
    }

    void StaticeRenderProxys::Clear()
    {

        m_BatchManager.Clear();
        m_DrawCallsVec.clear();

        m_ProxyDrawCallGenarterMain.Clear();
        m_ProxyDrawCallGenarterCurent.Clear();

        m_RenderProxysListData.Clear();
    }

    void StaticeRenderProxys::BatchesClear()
    {
        m_BatchManager.Clear();
        m_ProxyDrawCallGenarterMain.ClearBatchManger();
        m_ProxyDrawCallGenarterCurent.ClearBatchManger();
    }

    void StaticeRenderProxys::Add(int entity, uint32_t subMesh, const Ref<MeshSingle>& mesh, const Ref<Material>& materiel, const glm::mat4& model)
    {
        m_RenderProxysListData.Add(entity, subMesh, mesh, materiel, model);
    }

    void StaticeRenderProxys::UpdateTrasform(int entity, uint32_t subMesh, const glm::mat4& model)
    {
        m_RenderProxysListData.UpdateTrasform(entity, subMesh, model);
    }

    void StaticeRenderProxys::Remove(int entity)
    {
        m_RenderProxysListData.Remove(entity);
    }





    bool StaticeRenderProxys::HasEntity(int entity) 
    {     
        return m_RenderProxysListData.HasEntity(entity);
    }

    void StaticeRenderProxys::RenderProxysMainSubmiteDrawList()
    {
        SubmitDrawList();
    }
    void StaticeRenderProxys::EventCallback()
    {
        m_RenderProxysListData.EventCallback();
    }


    void StaticeRenderProxys::RenderProxysMainGenarte()
    {

        if(s_TestDrawCall)
        {
  
            if (m_RenderProxysListData.HasChangedMain())
            {
                m_ProxyDrawCallGenarterMain.ClearDrawList();

                if (CreateDrawListMain())
                    m_RenderProxysListData.ResetChangedMain();

            }
        }
        else
        {
            m_ProxyDrawCallGenarterMain.ClearDrawList();
            CreateDrawListMain();
        }

    }

    void StaticeRenderProxys::RenderProxysCurentGenarte()
    {
        if (s_TestDrawCall)
        {
            if (m_RenderProxysListData.HasChangedCurent())
            { 
                m_ProxyDrawCallGenarterCurent.ClearDrawList();

                if (CreateDrawListCurent())
                    m_RenderProxysListData.ResetChangedCurent();

            }
        }
        else
        {
            m_ProxyDrawCallGenarterCurent.ClearDrawList();
            CreateDrawListCurent();
        }
    }

    void StaticeRenderProxys::RenderProxysCurentSubmiteDrawList()
    {
        m_ProxyDrawCallGenarterCurent.SubmiteShaderDrawListCurent();
    }


    
    bool StaticeRenderProxys::CreateDrawListMain()
    {
        using namespace Batch;
        auto& renderProxyVec = m_RenderProxysListData.GetRenderProxyVec();


        uint32_t proxyIndex = 0u;
        uint32_t proxyCount = renderProxyVec.Size();


       
        if(s_TestDrawCall)
        {
            RY_CORE_TRACE("Test proxy render print msg funktion.");
        }


        m_ProxyDrawCallGenarterMain.SetRenderTargetLayoute(BufferLayout({
           { SDT::Float4, "Color" },
           { SDT::Int, "Entity" }
        }));
#if 0
        uint32_t drawIndex = 0;

        while (proxyIndex < proxyCount)
        {        
            // Renderer::GetDrawContext().PushScope("DrawCall", drawIndex);

            RenderProxy& proxy = renderProxyVec.AtSortedProxy(proxyIndex);


            m_ProxyDrawCallGenarterMain.NextDrawCall(proxy);

            // Renderer::GetDrawContext().PushScope("Entity", proxyIndex);
#if 1
            m_ProxyDrawCallGenarterMain.AddProxyData();
            // Renderer::GetDrawContext().PopScope();

            proxyIndex++;
#endif
            while(CheckProxyFromIndex(proxyIndex, proxyCount, m_ProxyDrawCallGenarterMain))
            {
                // Renderer::GetDrawContext().PushScope("Entity", proxyIndex);
                m_ProxyDrawCallGenarterMain.AddProxyData();
                // Renderer::GetDrawContext().PopScope();

                proxyIndex++;
            }
            m_ProxyDrawCallGenarterMain.SetubShaderDrawList();
            // Renderer::GetDrawContext().PopScope();
            drawIndex++;

        }
#elif 0
        uint32_t drawIndex = 0;

        while (proxyIndex < proxyCount)
        {
            // Renderer::GetDrawContext().PushScope("DrawCall", drawIndex);

            RenderProxy& proxy = renderProxyVec.AtSortedProxy(proxyIndex);
            RenderProxyKey renderProxyKey = proxy.GetKey();

            ProxyGroupView groupView = renderProxyVec.GetGroupView(renderProxyKey);
#if 1
            m_ProxyDrawCallGenarterMain.NextDrawCall(proxy);

            // Renderer::GetDrawContext().PushScope("Entity", proxyIndex);
            m_ProxyDrawCallGenarterMain.AddProxyData();
            // Renderer::GetDrawContext().PopScope();

            proxyIndex++;

            while (CheckProxyFromIndex(proxyIndex, proxyCount, m_ProxyDrawCallGenarterMain))
            {
                // Renderer::GetDrawContext().PushScope("Entity", proxyIndex);
                m_ProxyDrawCallGenarterMain.AddProxyData();
                // Renderer::GetDrawContext().PopScope();

                proxyIndex++;
            }
            m_ProxyDrawCallGenarterMain.SetubShaderDrawList();
            // Renderer::GetDrawContext().PopScope();
            drawIndex++;
#else
            m_ProxyDrawCallGenarterMain.NextDrawCall(groupView.At(0));

            m_ProxyDrawCallGenarterMain.AddProxyData();
            uint32_t curentFirstIndex = proxyIndex;
            proxyIndex++;
            while (CheckProxyFromIndex(proxyIndex, proxyCount, m_ProxyDrawCallGenarterMain))
            {
                RY_CORE_ASSERT(renderProxyKey == renderProxyVec.AtSortedProxy(proxyIndex).GetKey());

                // Renderer::GetDrawContext().PushScope("Entity", proxyIndex);
                m_ProxyDrawCallGenarterMain.AddProxyData();
                // Renderer::GetDrawContext().PopScope();

                proxyIndex++;
            }
            uint32_t countIndex = proxyIndex - curentFirstIndex;
            RY_CORE_ASSERT(countIndex == groupView.Size());
            if(proxyIndex < proxyCount)
            {
                RY_CORE_ASSERT(renderProxyKey != renderProxyVec.AtSortedProxy(proxyIndex).GetKey());
            }


            drawIndex++;
            m_ProxyDrawCallGenarterMain.SetubShaderDrawList();
#endif
        }
#else
        
        renderProxyVec.ForeachGroupView(
            [this, proxyCount, &proxyIndex](RenderProxyKey renderProxyKey, ProxyGroupView groupView)
            {
#if 1
                m_ProxyDrawCallGenarterMain.SetProxyData(groupView.At(0));
                m_ProxyDrawCallGenarterMain.NextDrawCall(groupView.At(0));
                m_ProxyDrawCallGenarterMain.AddProxyData();
                proxyIndex++;
                for (uint32_t i = 1; i < groupView.Size(); i++)
                {
                    
                    RenderProxy& renderProxy = groupView.At(i);
                    m_ProxyDrawCallGenarterMain.SetProxyData(renderProxy);
                    m_ProxyDrawCallGenarterMain.AddProxyData();
                    proxyIndex++;
                }
                

                m_ProxyDrawCallGenarterMain.SetubShaderDrawList();
#else
                m_ProxyDrawCallGenarterMain.SubmiteDraw(groupView);
#endif
            }
        );
           
#endif
        
        return proxyIndex == proxyCount;
    }

    bool StaticeRenderProxys::CreateDrawListCurent()
    {
        using namespace Batch;
  
        auto& renderProxyVec = m_RenderProxysListData.GetRenderProxyVec();



        uint32_t proxyIndex = 0u;
        uint32_t proxyCount = renderProxyVec.Size();


        if (s_TestDrawCall)
        {
            RY_CORE_TRACE("Test proxy render print msg funktion.");
        }


        m_ProxyDrawCallGenarterCurent.SetRenderTargetLayoute(BufferLayout({}));
#if 1
        uint32_t drawIndex = 0;
        while (proxyIndex < proxyCount)
        {
            RenderProxy& proxy = renderProxyVec.AtSortedProxy(proxyIndex);
            RenderProxyKey renderProxyKey = proxy.GetKey();
            // Renderer::GetDrawContext().PushScope("DrawCall", drawIndex);

            m_ProxyDrawCallGenarterCurent.NextDrawCall(proxy);

            // Renderer::GetDrawContext().PushScope("Entitiy", proxyIndex);
            m_ProxyDrawCallGenarterCurent.AddProxyData();
            // Renderer::GetDrawContext().PopScope();

            proxyIndex++;

            while (CheckProxyFromIndex(proxyIndex, proxyCount, m_ProxyDrawCallGenarterCurent))
            {
                RY_CORE_ASSERT(renderProxyKey == renderProxyVec.AtSortedProxy(proxyIndex).GetKey());
                // Renderer::GetDrawContext().PushScope("Entitiy", proxyIndex);
                m_ProxyDrawCallGenarterCurent.AddProxyData();
                // Renderer::GetDrawContext().PopScope();

                proxyIndex++;
            }
            m_ProxyDrawCallGenarterCurent.SetubShaderDrawList();
            // Renderer::GetDrawContext().PopScope();
            drawIndex++;
            RY_CORE_ASSERT(renderProxyKey != renderProxyVec.AtSortedProxy(proxyIndex).GetKey());

        }
#else

        renderProxyVec.ForeachGroupView(
            [this](RenderProxyKey renderProxyKey, ProxyGroupView groupView)
            {
                auto& drawCallGenrater = m_ProxyDrawCallGenarterMain;

                m_ProxyDrawCallGenarterMain.NextDrawCall(groupView.At(0));
                for (RenderProxy& renderProxy : groupView)
                {
                    bool result = drawCallGenrater.SetProxyData(renderProxy);
                    RY_CORE_ASSERT(!result);
                    drawCallGenrater.AddProxyData();
                }
                m_ProxyDrawCallGenarterMain.SetubShaderDrawList();
            }
        );
#endif

        return proxyIndex == proxyCount;
    }
    
    uint32_t StaticeRenderProxys::CreateToDrawList(const DrawRenderResource& drawResources, uint32_t proxyIndex)
    {
#ifdef RY_SORT_KEY
        RY_CORE_NOT_IMPL();
        return -1;
#else
        constexpr int lodLevel = 0;
   
        auto& renderProxyVec = m_RenderProxysListData.GetRenderProxyVec();

        using namespace Batch;
        uint32_t proxyCount = renderProxyVec.Size();

        const RenderProxy& firstProxy = renderProxyVec.AtSortedProxy(proxyIndex);

        const glm::mat4& firstModelMatrix = firstProxy.model;
        glm::mat4 firstNormlaeMatrix = glm::inverse(glm::transpose(firstModelMatrix));

        const Ref<Material>& materiel = firstProxy.materiel;
        const Ref<MeshSingle>& meshSingle = firstProxy.mesh;

        Ref<Shader>	shader = materiel->GetShaderForDraw(drawResources.layoutOutPut, lodLevel);
        int	renderMode = materiel->GetRenderMode(drawResources.layoutOutPut, lodLevel);

        Ref<UniformBuffer> materielUniform = materiel->GetMaterielUniformBuffer();
        Ref<Texture> texture = materiel->GetAlbedoTextures();
        Batch3DKey batchHandle = Batch3DKey(materiel, meshSingle, drawResources.layoutOutPut, lodLevel);

        BatchVAO& storeSubmiteVAO = m_BatchManager.GetBatchVAO(batchHandle);
        storeSubmiteVAO.Check();
        storeSubmiteVAO.Reset();




        BatchedRenderObjectVAO* batchRendererObjectVAO = nullptr;
        batchRendererObjectVAO = storeSubmiteVAO.GetDataEndPtr();


        batchRendererObjectVAO->Reset();
        batchRendererObjectVAO->materielUB = materielUniform;

        Render3DMeshObject renderObject = {
            firstModelMatrix
            , firstNormlaeMatrix
            , glm::ivec4{firstProxy.entity, -10.0f, -21.0f, -32.0f}
        };
        batchRendererObjectVAO->AddRenderObject(renderObject);


        proxyIndex++;


        if (!s_RenderProxyInstaecing)
        {
            SetupVertexBufferRenderObject(*batchRendererObjectVAO, firstProxy.mesh, lodLevel);

            DrawCallResource drawCallResource{
                texture,
                shader,
                (*batchRendererObjectVAO),
                firstProxy.mesh->GetShadePerDrawObjectIndrect(),
                renderMode
            };


            GenarteDrawCall(drawResources, drawCallResource);

            
            storeSubmiteVAO.Incroment();
            storeSubmiteVAO.Check(); 
            batchRendererObjectVAO = storeSubmiteVAO.GetDataEndPtr();
            batchRendererObjectVAO->Reset();
            batchRendererObjectVAO->materielUB = materielUniform;
            
        }



        while (proxyIndex < proxyCount)
        {
            if (!s_RenderProxyInstaecing)
            {
                batchRendererObjectVAO->Reset();
                batchRendererObjectVAO->materielUB = materielUniform;
            }
            const RenderProxy& proxy = renderProxyVec.AtSortedProxy(proxyIndex);

            if (proxy.materiel != firstProxy.materiel || proxy.mesh != firstProxy.mesh)
                break;


            const glm::mat4& modelMatrix = proxy.model;
            glm::mat4 normaleMatrix = glm::inverse(glm::transpose(modelMatrix));

            Render3DMeshObject renderObject = {
               modelMatrix
                , normaleMatrix
                , glm::ivec4{proxy.entity, -10.0f, -21.0f, -32.0f}
            };
            batchRendererObjectVAO->AddRenderObject(renderObject);

            proxyIndex++;
            if (!s_RenderProxyInstaecing)
            {
                SetupVertexBufferRenderObject(*batchRendererObjectVAO, firstProxy.mesh, lodLevel);


                DrawCallResource drawCallResource{
                    texture,
                    shader,
                    (*batchRendererObjectVAO),
                    firstProxy.mesh->GetShadePerDrawObjectIndrect(),
                    renderMode
                };

                GenarteDrawCall(drawResources, drawCallResource);

                storeSubmiteVAO.Incroment();
                storeSubmiteVAO.Check();
                batchRendererObjectVAO = storeSubmiteVAO.GetDataEndPtr();



            }

        }
        if (s_RenderProxyInstaecing)
        {

            DrawCallResource drawCallResource{
                   texture,
                   shader,
                   (*batchRendererObjectVAO),
                   firstProxy.mesh->GetShadePerDrawObjectIndrect(),
                   renderMode
            };

            SetupVertexBufferRenderObject((*batchRendererObjectVAO), firstProxy.mesh, lodLevel);
            GenarteDrawCall(drawResources, drawCallResource);
        }

        storeSubmiteVAO.Reset();


        return proxyIndex;


#endif
    }


  

    void StaticeRenderProxys::GenarteDrawCall(const DrawRenderResource& drawResources, const DrawCallResource& drawCallResource)
    {

        const BatchedRenderObjectVAO& batchVAO = drawCallResource.batchVAO;



        Mesh::PerDrawObject drawElement = drawCallResource.perDrawObject;
        drawElement.InstancesCount = batchVAO.curentIndex;

        constexpr uint32_t bindSlotCameraUB = 1;
        constexpr uint32_t bindSlotShadowUB = 2;
        constexpr uint32_t bindSlotMaterielUB = 3;

        constexpr uint32_t bindSlotMaterielTex = 0;
        constexpr uint32_t bindSlotDepthShadowTex = 1;

        RY_CORE_ASSERT(0 < drawElement.InstancesCount);

        const Ref<VertexArray>& vao = batchVAO.vertexArray;
        ShaderDrawResource drawListRef = CreateShaderDrawResource();


        ShaderDrawResource* drawList = &drawListRef;
        drawList->renderMode = drawCallResource.renderMode;
        drawList->indicesCount = drawElement.InstancesCount;
        drawList->drawElement = drawElement;
        drawList->vao = vao;
        drawList->shaderProgramm = drawCallResource.shader;

        drawList->GetBindeUniform().at(bindSlotMaterielUB) = batchVAO.materielUB;
        drawList->GetBindeUniform().at(bindSlotCameraUB) = drawResources.cameraBuffer;
        drawList->GetBindeUniform().at(bindSlotShadowUB) = drawResources.shadowCameraBuffer;

        drawList->GetBindeTextures().at(bindSlotMaterielTex) = drawCallResource.texture;
        drawList->GetBindeTextures().at(bindSlotDepthShadowTex) = drawResources.textureShadow;

        m_DrawCallsVec.emplace_back(drawListRef);

    }


    void StaticeRenderProxys::SubmitDrawList()
    {
        m_ProxyDrawCallGenarterMain.SubmiteShaderDrawListMain();

    }

    uint32_t StaticeRenderProxys::RenderProxysMainBatch(uint32_t proxyIndex, const RenderProxy& first, uint32_t proxyCount)
    {
        return 0;
    }
    


    void StaticeRenderProxys::SetupVertexBufferRenderObject(BatchedRenderObjectVAO& batchedRenderObjectVAO, const Ref<MeshSingle>& mesh, int lodLevel)
    {
        Ref<VertexArray>& vertexArray = batchedRenderObjectVAO.vertexArray;       
        Ref<VertexBuffer>& instanceObjectVAB = batchedRenderObjectVAO.instanceObjectBuffer;

        uint32_t instance = batchedRenderObjectVAO.curentIndex;
        uint32_t byteSize = instance * sizeof(Batch::Render3DMeshObject);

        if (vertexArray == nullptr)
            vertexArray = VertexArray::Create();


        if (instanceObjectVAB == nullptr)
        {
            CreateVertexBufferRenderObject(batchedRenderObjectVAO, mesh, lodLevel);
        }
        else if (instanceObjectVAB->GetByteSize() < byteSize)
        {
            ResizeVertexBufferRenderObject(batchedRenderObjectVAO);
        }
        else if(instanceObjectVAB->GetByteSize() >= byteSize)
        {
            UploadeVertexBufferRenderObject(batchedRenderObjectVAO);
        }
       


    }

    void StaticeRenderProxys::CreateVertexBufferRenderObject(BatchedRenderObjectVAO& batchedRenderObjectVAO, const Ref<MeshSingle>& mesh, int lodLevel)
    {
        constexpr uint32_t instanceIndex = 1u;
        constexpr bool aktive = true;

        Ref<VertexArray>& vertexArray = batchedRenderObjectVAO.vertexArray;
        Ref<VertexBuffer>& instanceObjectVAB = batchedRenderObjectVAO.instanceObjectBuffer;

        uint32_t instance = batchedRenderObjectVAO.curentIndex;
        uint32_t byteSize = instance * sizeof(Batch::Render3DMeshObject);
        Batch::Render3DMeshObject* batchDataPtr = batchedRenderObjectVAO.rendeObjectVec.data();

        BufferLayout layout = BufferLayout({
            { SDT::Float4x4, "a_ModelMarix" },
            { SDT::Float4x4, "a_NormleMatrix" },
            { SDT::Int, "a_EntityID" },
            { SDT::Int3, "a_Empty" }
        }, instanceIndex);
        layout.SetAutoCompress(true);

        instanceObjectVAB = VertexBuffer::Create(batchDataPtr, byteSize, BufferFlag::None, layout);

        const Ref<VertexBuffer>& geometryVAB = mesh->GetVertexBuffer();
        const Ref<IndexBuffer>& geometryIAB = mesh->GetShadeIndexBuffer( /* lodLevel */);

        vertexArray->ClearVertexBuffers();

        vertexArray->AddVertexBuffer(geometryVAB);
        vertexArray->AddVertexBuffer(instanceObjectVAB);
        vertexArray->SetIndexBuffer(geometryIAB);
    }

    void StaticeRenderProxys::ResizeVertexBufferRenderObject(BatchedRenderObjectVAO& batchedRenderObjectVAO)
    {
        uint32_t instance = batchedRenderObjectVAO.curentIndex;
        uint32_t byteSize = instance * sizeof(Batch::Render3DMeshObject);
        Batch::Render3DMeshObject* batchDataPtr = batchedRenderObjectVAO.rendeObjectVec.data();

        Ref<VertexBuffer>& instanceObjectVAB = batchedRenderObjectVAO.instanceObjectBuffer;
        instanceObjectVAB->ResizeBuffer(batchDataPtr, byteSize);
    }

    void StaticeRenderProxys::UploadeVertexBufferRenderObject(BatchedRenderObjectVAO& batchedRenderObjectVAO)
    {
        if (!batchedRenderObjectVAO.needUpdate)
            return;

        uint32_t instance = batchedRenderObjectVAO.curentIndex;
        uint32_t byteSize = instance * sizeof(Batch::Render3DMeshObject);
        Batch::Render3DMeshObject* batchDataPtr = batchedRenderObjectVAO.rendeObjectVec.data();

        Ref<VertexBuffer>& instanceObjectVAB = batchedRenderObjectVAO.instanceObjectBuffer;
        instanceObjectVAB->SetData(batchDataPtr, byteSize);
    }


   bool StaticeRenderProxys::CheckProxyFromIndex(uint32_t proxyIndex, uint32_t proxyCount, ProxyDrawCallGenarter& drawCallGenrater)
   {
        return m_RenderProxysListData.CheckProxyFromIndex(proxyIndex, proxyCount, drawCallGenrater);
   }



}
#include "rypch.h"
#include "RenderPassDrawResoucs.h"
#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/RenderProxy/Proxy.h>

namespace Rynex {

        

        RenderPassDrawResoucs::RenderPassDrawResoucs()
        {
        }

        RenderPassDrawResoucs::~RenderPassDrawResoucs()
        {
        }


        void RenderPassDrawResoucs::SetOutPutLayout(const BufferLayout& outPut)
        {
            m_OutPutLayout = outPut;
            m_LatFrameBatchedRenderObjectsMap.clear();
            m_BatchedRenderObjectsMap.clear();
            RY_CORE_WARN("when out put is set We need to reset the lists!");
        }

        void RenderPassDrawResoucs::SubmiteVisebleObjects(RenderProxyPtr renderProxy, int lodTier)
        {
            const Ref<Material>& materiel = renderProxy->GetMaterial();
            const Ref<MeshSingle>& mesh = renderProxy->GetMesh();
#if 0
            const Ref<Shader>& shader = materiel->GetShaderForDraw(m_OutPutLayout, lodTier);
            int drawSpec = materiel->GetDrawSpecification(m_OutPutLayout, lodTier);
#endif
            Batch3DKey key = Batch3DKey(materiel, mesh, m_OutPutLayout, lodTier);
            Batch3DData& batch = m_BatchedRenderObjectsMap[key];
            SubmiteToList(renderProxy, lodTier, batch);
        }

        void RenderPassDrawResoucs::ClearFrame()
        {
            m_LatFrameBatchedRenderObjectsMap.clear();
            m_LatFrameBatchedRenderObjectsMap = m_BatchedRenderObjectsMap;
            m_BatchedRenderObjectsMap.clear();
        }

        void RenderPassDrawResoucs::GenrateDrawCalls(std::vector<DrawCall>& drawCallList)
        {
        }

        void RenderPassDrawResoucs::SubmiteToList(RenderProxy* proxy, int lodTier, std::vector< std::pair<int, RenderProxy*>>& list)
        {
            list.emplace_back(lodTier, proxy);
        }

        void RenderPassDrawResoucs::DrawCallsFromList(std::vector<DrawCall>& drawCallList, const Batch3DKey& key, std::vector<std::pair<int, RenderProxy*>>& list)
        {
        }

        void RenderPassDrawResoucs::SubmiteToList(RenderProxy* proxy, int lodTier, std::vector< std::pair<int, RenderProxy>>& list)
        {
            list.emplace_back(lodTier, *proxy);
        }

        void RenderPassDrawResoucs::DrawCallsFromList(std::vector<DrawCall>& drawCallList, const Batch3DKey& key, std::vector<std::pair<int, RenderProxy>>& list)
        {
            int drawSpec = key.m_DrawSpec;

            for (auto& [lodLevel, proxy] : list)
            {

            }
        }


    
}



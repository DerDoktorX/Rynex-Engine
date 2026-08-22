#pragma once
#include <Rynex/Renderer/Rendering/Render2D/VertexQuadeTransformBatche.h>

namespace Rynex {

    template<typename T, typename N, int MaxTex, int UpdateDataInstend = 1, int VerticiesQuadMax = 4>
    class VertexQuadeTransformUpdateBatching : public VertexQuadeTransformBatche<T, N, MaxTex, VerticiesQuadMax>
    {
    public:
        VertexQuadeTransformUpdateBatching()
            : VertexQuadeTransformBatche<T, N, MaxTex, VerticiesQuadMax>()
            , m_BeginUpdate(0)
            , m_EndeUpdate(0)
        {
        };

        ~VertexQuadeTransformUpdateBatching()
        {

        };

        void SubmitUpdateDataDarw(const T& data)
        {
            uint32_t curentSize = VertexQuadeTransformBatche<T, N, MaxTex, VerticiesQuadMax>::Batching<T>::Size();
            if (curentSize < m_BeginUpdate)
            {
                m_BeginUpdate = curentSize;
            }
            VertexQuadeTransformBatche<T, N, MaxTex, VerticiesQuadMax>::Batching<T>::Emplace_Back_Push(data);
        }

        void SubmitDarw()
        {
            uint32_t curentSize = VertexQuadeTransformBatche<T, N, MaxTex, VerticiesQuadMax>::Batching<T>::Size();
            uint32_t different = m_EndeUpdate - curentSize;
            if (different < UpdateDataInstend && curentSize )
            {
                
            }
            VertexQuadeTransformBatche<T, N, MaxTex, VerticiesQuadMax>::Batching<T>::MoveElementPtr();
        }
    private:
        uint32_t m_BeginUpdate;
        uint32_t m_EndeUpdate;
    };

}
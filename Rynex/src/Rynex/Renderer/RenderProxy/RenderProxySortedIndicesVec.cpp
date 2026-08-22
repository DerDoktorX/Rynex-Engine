#include "rypch.h"
#include "RenderProxySortedIndicesVec.h"


namespace Rynex {

#pragma region RenderProxySortedIndicesVec

    RenderProxySortedIndicesVec::RenderProxySortedIndicesVec()
    {
    }

    RenderProxySortedIndicesVec::RenderProxySortedIndicesVec(const RenderProxySortedIndicesVec& r)
        : m_FreeListVec(r.m_FreeListVec)
        , m_ProxyVec(r.m_ProxyVec)
        , m_SortedProxyInidicesVec(r.m_SortedProxyInidicesVec)
    {
        uint32_t count = Size();
        uint32_t copyCount = r.Size();
        RY_CORE_ASSERT(count == copyCount, "not same count!");
    }

    RenderProxySortedIndicesVec::~RenderProxySortedIndicesVec()
    {
        Clear();
    }

    uint32_t RenderProxySortedIndicesVec::SubmiteProxy(const RenderProxy& proxy, const glm::mat4& modelMatrix)
    {
        const BoundingVolume& volume = proxy.GetMesh()->GetBoundingVolume();
        glm::mat4 normaleMatrix = glm::inverse(glm::transpose(modelMatrix));

        uint32_t proxyIndex;
        uint32_t count = m_ProxyVec.size();

        proxyIndex = m_FreeListVec.empty() ? count : GetIndexFromFreeList();

        if (count == proxyIndex)
        {
            m_ProxyVec.emplace_back(proxy);
#ifdef RY_SINGLE_VALUE_VEC
            m_BoundingVolumeVec.emplace_back(volume.GetAABB(modelMatrix), volume.GetSphere(modelMatrix));

            m_ModelMatrixVec.emplace_back(modelMatrix);
            m_NormaleMatrixVec.emplace_back(normaleMatrix);
#endif
        }
        else
        {
            m_ProxyVec.at(proxyIndex) = proxy;
#ifdef RY_SINGLE_VALUE_VEC
            m_BoundingVolumeVec.at(proxyIndex) = BoundingVolume(volume.GetAABB(modelMatrix), volume.GetSphere(modelMatrix));
            m_ModelMatrixVec.at(proxyIndex) = modelMatrix;
            m_NormaleMatrixVec.at(proxyIndex) = normaleMatrix;
#endif
        }

        InsertIntoSortedIndices(proxyIndex);
        return proxyIndex;
    }

    void RenderProxySortedIndicesVec::Remove(uint32_t proxyIndex, RenderProxyKey renderProxyKey)
    {
        AddFreeList(proxyIndex);
        RemoveFromSortedIndices(proxyIndex, renderProxyKey);
    }

    bool RenderProxySortedIndicesVec::Empty() const
    {
        RY_CORE_ASSERT(!m_SortedProxyInidicesVec.empty() || (m_SortedProxyInidicesVec.empty() && m_ProxyVec.size() == m_FreeListVec.size()), "if sorted is empty itmplictads that ProxyVec has the same size like the free list!");
        return m_SortedProxyInidicesVec.empty();
    }

    uint32_t RenderProxySortedIndicesVec::Size() const
    {
        uint32_t count = m_SortedProxyInidicesVec.size();
        uint32_t diff = m_ProxyVec.size() - m_FreeListVec.size();

        RY_CORE_ASSERT(diff == count, "if sorted is size same as ProxyVec and freelist sizes differc!");
        return count;
    }

    void RenderProxySortedIndicesVec::Clear()
    {
        m_ProxyVec.clear();
        
        m_FreeListVec.clear();
        m_SortedProxyInidicesVec.clear();
#ifdef RY_SINGLE_VALUE_VEC
        m_BoundingVolumeVec.clear();
        m_ModelMatrixVec.clear();
        m_NormaleMatrixVec.clear();
#endif
    }

    RenderProxy& RenderProxySortedIndicesVec::AtProxy(uint32_t proxyIndex)
    {
        return m_ProxyVec.at(proxyIndex);
    }

    const RenderProxy& RenderProxySortedIndicesVec::AtProxy(uint32_t proxyIndex) const
    {
        return m_ProxyVec.at(proxyIndex);
    }

#ifdef RY_SINGLE_VALUE_VEC
    BoundingVolume& RenderProxySortedIndicesVec::AtBV(uint32_t proxyIndex)
    {
        return m_BoundingVolumeVec.at(proxyIndex);
    }

    const BoundingVolume& RenderProxySortedIndicesVec::AtBV(uint32_t proxyIndex) const
    {
        return m_BoundingVolumeVec.at(proxyIndex);
    }

    glm::mat4& RenderProxySortedIndicesVec::AtModelMatrix(uint32_t proxyIndex)
    {
        return m_ModelMatrixVec.at(proxyIndex);
    }

    const glm::mat4& RenderProxySortedIndicesVec::AtModelMatrix(uint32_t proxyIndex) const
    {
        return m_ModelMatrixVec.at(proxyIndex);
    }

    glm::mat4& RenderProxySortedIndicesVec::AtNormaleMatrix(uint32_t proxyIndex)
    {
        return m_NormaleMatrixVec.at(proxyIndex);
    }

    const glm::mat4& RenderProxySortedIndicesVec::AtNormaleMatrix(uint32_t proxyIndex) const
    {
        return m_NormaleMatrixVec.at(proxyIndex);
    }
#endif


    RenderProxy& RenderProxySortedIndicesVec::AtSortedProxy(uint32_t sortedProxyIndex)
    {
        uint32_t proxyIndex = m_SortedProxyInidicesVec.at(sortedProxyIndex);
        return AtProxy(proxyIndex);
    }

    const RenderProxy& RenderProxySortedIndicesVec::AtSortedProxy(uint32_t sortedProxyIndex) const
    {
        uint32_t proxyIndex = m_SortedProxyInidicesVec.at(sortedProxyIndex);
        return AtProxy(proxyIndex);
    }

#ifdef RY_SINGLE_VALUE_VEC
    BoundingVolume& RenderProxySortedIndicesVec::AtSortedBV(uint32_t sortedProxyIndex)
    {
        uint32_t proxyIndex = m_SortedProxyInidicesVec.at(sortedProxyIndex);
        return AtBV(proxyIndex);
    }

    const BoundingVolume& RenderProxySortedIndicesVec::AtSortedBV(uint32_t sortedProxyIndex) const
    {
        uint32_t proxyIndex = m_SortedProxyInidicesVec.at(sortedProxyIndex);
        return AtBV(proxyIndex);
    }

    glm::mat4& RenderProxySortedIndicesVec::AtSortedModelMatrix(uint32_t sortedProxyIndex)
    {
        uint32_t proxyIndex = m_SortedProxyInidicesVec.at(sortedProxyIndex);
        return AtModelMatrix(proxyIndex);
    }

    const glm::mat4& RenderProxySortedIndicesVec::AtSortedModelMatrix(uint32_t sortedProxyIndex) const
    {
        uint32_t proxyIndex = m_SortedProxyInidicesVec.at(sortedProxyIndex);
        return AtModelMatrix(proxyIndex);
    }

    glm::mat4& RenderProxySortedIndicesVec::AtSortedNormaleMatrix(uint32_t sortedProxyIndex)
    {
        uint32_t proxyIndex = m_SortedProxyInidicesVec.at(sortedProxyIndex);
        return AtNormaleMatrix(proxyIndex);
    }

    const glm::mat4& RenderProxySortedIndicesVec::AtSortedNormaleMatrix(uint32_t sortedProxyIndex) const
    {
        uint32_t proxyIndex = m_SortedProxyInidicesVec.at(sortedProxyIndex);
        return AtNormaleMatrix(proxyIndex);
    }
#endif

    void RenderProxySortedIndicesVec::InsertIntoSortedIndices(uint32_t proxyIndex)
    {
        using SortedIt = std::vector<uint32_t>::iterator;

        const RenderProxy& proxy = m_ProxyVec.at(proxyIndex);
        SortedIt itPos = std::lower_bound(m_SortedProxyInidicesVec.begin(), m_SortedProxyInidicesVec.end(), proxy
#if 0
            , std::bind(&RenderProxySortedIndicesVec::InsertLowerBoundFunc, this, std::placeholders::_1, std::placeholders::_2)
#else
            , [this](uint32_t aIndex, const RenderProxy& bProxy)
            {

                const RenderProxy& aProxy = m_ProxyVec.at(aIndex);
                
                uint64_t keyA = aProxy.GetKey();
                uint64_t keyB = aProxy.GetKey();

                bool result = keyA < keyB;
                return result;
            }
#endif
        );
        m_SortedProxyInidicesVec.insert(itPos, proxyIndex);
    }

    void RenderProxySortedIndicesVec::RemoveFromSortedIndices(uint32_t proxyIndexRemove, RenderProxyKey renderProxyKey)
    {
        using SortedIt = std::vector<uint32_t>::iterator;

        SortedIt itPos = std::find(m_SortedProxyInidicesVec.begin(), m_SortedProxyInidicesVec.end(), proxyIndexRemove);
        if (itPos == m_SortedProxyInidicesVec.end())
        {
            RY_CORE_ERROR("we dont found this index in the sorted map!");
            return;
        }
        m_SortedProxyInidicesVec.erase(itPos);
    }

    void RenderProxySortedIndicesVec::RebuildSortedIndices()
    {
        m_SortedProxyInidicesVec.clear();
        m_SortedProxyInidicesVec.reserve(m_ProxyVec.size());

        for (uint32_t i = 0; i < m_ProxyVec.size(); i++)
        {
            m_SortedProxyInidicesVec.emplace_back(i);
        }

        std::sort(m_SortedProxyInidicesVec.begin(), m_SortedProxyInidicesVec.end(),
#if 0

            std::bind(&RenderProxySortedIndicesVec::RebulidSortFunc, this,
                std::placeholders::_1, std::placeholders::_2
#else
        [this](uint32_t indexA, uint32_t indexB)
        {

            const RenderProxy& aProxy = m_ProxyVec.at(indexA);
            const RenderProxy& bProxy = m_ProxyVec.at(indexB);

            uint64_t keyA = aProxy.GetKey();
            uint64_t keyB = aProxy.GetKey();

            return keyA < keyB;
        }
#endif
        );
    }

    uint32_t RenderProxySortedIndicesVec::GetIndexFromFreeList()
    {
        using ItVec = std::vector<uint32_t>::iterator;
        RY_CORE_ASSERT(!m_FreeListVec.empty());

        uint32_t index = m_FreeListVec.front();
        ItVec itFront = m_FreeListVec.begin();
        m_FreeListVec.erase(itFront);
        return index;
    }

    void RenderProxySortedIndicesVec::AddFreeList(uint32_t index)
    {
        m_FreeListVec.emplace_back(index);
    }

#pragma endregion


}
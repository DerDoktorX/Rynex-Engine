#include "rypch.h"
#include "RenderProxySortedProxyVec.h"
#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/Rendering/Batch3DKey.h>

namespace Rynex {
#pragma region RenderProxySortedProxyVec

    RenderProxySortedProxyVec::RenderProxySortedProxyVec()
    {

    }

    RenderProxySortedProxyVec::~RenderProxySortedProxyVec()
    {
        Clear();
       
    }

    RenderProxySortedProxyVec::RenderProxySortedProxyVec(const RenderProxySortedProxyVec& r)
        : m_FreeListVec(r.m_FreeListVec)
        , m_ProxyInidicesVec(r.m_ProxyInidicesVec)
        , m_SortedProxyVec(r.m_SortedProxyVec)
    {
        uint32_t count = Size();
        uint32_t copyCount = r.Size();
        RY_CORE_ASSERT(count == copyCount, "not same count!");
    }

    uint32_t RenderProxySortedProxyVec::SubmiteProxy(const RenderProxy& proxy, const glm::mat4& modelMatrix)
    {
        uint32_t inserIndex = InsertProxyIntoSortedPorxyVec(proxy, modelMatrix);
        uint32_t proxyIndex = InsertProxyIndex(inserIndex);
        Size();
        // CheckColsionGroupView();
        return proxyIndex;
    }

    void RenderProxySortedProxyVec::Remove(uint32_t proxyAccesIndexRemove, RenderProxyKey renderProxyKey)
    {
        uint32_t proxyIndexRemove = RemoveProxyIndex(proxyAccesIndexRemove);
        RemoveProxyFromSortedPorxyVec(proxyIndexRemove);
        Size();
    }

    bool RenderProxySortedProxyVec::Empty() const
    {
        RY_CORE_ASSERT(!m_SortedProxyVec.empty() || (m_SortedProxyVec.empty() && m_ProxyInidicesVec.size() == m_FreeListVec.size()), "if sorted is empty itmplictads that ProxyVec has the same size like the free list!");
        return m_SortedProxyVec.empty();
    }

    uint32_t RenderProxySortedProxyVec::Size() const
    {
        uint32_t count = m_SortedProxyVec.size();
        uint32_t diff = m_ProxyInidicesVec.size() - m_FreeListVec.size();

        RY_CORE_ASSERT(diff == count, "if sorted is size same as ProxyVec and freelist sizes differc!");
        return count;
    }

    void RenderProxySortedProxyVec::Clear()
    {
        CheckColsionGroupView();

        m_ProxyInidicesVec.clear();
        m_FreeListVec.clear();
        m_SortedProxyVec.clear();
        m_GroupViewsMap.clear();
    }

    RenderProxy& RenderProxySortedProxyVec::AtProxy(RenderProxyKey renderProxyKey, uint32_t proxyIndex)
    {
        uint32_t index = GetProxyIndex(proxyIndex);
        return m_SortedProxyVec.at(index);
    }

    const RenderProxy& RenderProxySortedProxyVec::AtProxy(RenderProxyKey renderProxyKey, uint32_t proxyIndex) const
    {
        uint32_t index = GetProxyIndex(proxyIndex);
        return m_SortedProxyVec.at(index);
    }



    RenderProxy& RenderProxySortedProxyVec::AtSortedProxy(uint32_t sortedProxyIndex)
    {
        return m_SortedProxyVec.at(sortedProxyIndex);
    }

    const RenderProxy& RenderProxySortedProxyVec::AtSortedProxy(uint32_t sortedProxyIndex) const
    {
        return m_SortedProxyVec.at(sortedProxyIndex);
    }


    std::pair<RenderProxyKey, uint32_t> RenderProxySortedProxyVec::FindeProxy(int entity, uint32_t subMesh)
    {
        uint32_t proxyIndex = 0u;
        RenderProxyKey renderProxyKey = -1;
        for (const RenderProxy& proxy : m_SortedProxyVec)
        {
            if (proxy.entity  == entity &&  proxy.subMesh == subMesh)
            {
                renderProxyKey = proxy.GetKey();
                break;
            }
            proxyIndex++;
        }
        uint32_t proxyAccesIndexFind = 0u;

        for (const uint32_t& proxyAccesIndex : m_ProxyInidicesVec)
        {
            if (proxyAccesIndex == proxyIndex)
                break;
            proxyAccesIndexFind++;
        }
        RY_CORE_ASSERT(renderProxyKey != -1);
        return std::pair<RenderProxyKey, uint32_t>{ renderProxyKey, proxyAccesIndexFind };
    }

    uint32_t RenderProxySortedProxyVec::InsertProxyIndex(uint32_t proxyIndexInsert)
    {

        for (uint32_t& proxyAccesIndex : m_ProxyInidicesVec)
        {
            if (MAXUINT32 != proxyAccesIndex && proxyIndexInsert <= proxyAccesIndex)
            {
                proxyAccesIndex++;
            }
        }
        uint32_t proxyIndex;
        if (m_FreeListVec.empty())
        {
            proxyIndex = m_ProxyInidicesVec.size();
            m_ProxyInidicesVec.emplace_back(proxyIndexInsert);


        }
        else
        {
            proxyIndex = GetIndexFromFreeList();
            uint32_t& indexProxy = m_ProxyInidicesVec.at(proxyIndex);
            RY_CORE_ASSERT(UINT32_MAX == indexProxy, "The Stored Value is Vaild!");
            indexProxy = proxyIndexInsert;
        }
#ifdef RY_TEST_CHECK_FOR_IDENTY
        uint32_t count = m_ProxyInidicesVec.size();
        for (uint32_t x = 0; x < count; x++)
        {
            uint32_t proxyAceesX = m_ProxyInidicesVec.at(x);
            if (UINT32_MAX == proxyAceesX)
                continue;


            for (uint32_t y = x + 1; y < count; y++)
            {
                uint32_t proxyAceesY = m_ProxyInidicesVec.at(y);
                RY_CORE_ASSERT(proxyAceesX != proxyAceesY, "All proxy Indicies need to be unique, means only one time present!");
            }
        }
        RY_REMBER_FUNC_CHANGE("Remove the test if not longer needed!");
#endif
        return proxyIndex;
    }

    uint32_t RenderProxySortedProxyVec::RemoveProxyIndex(uint32_t proxyAccesIndexRemove)
    {

        uint32_t removeProxyIndex = m_ProxyInidicesVec.at(proxyAccesIndexRemove);
        m_ProxyInidicesVec.at(proxyAccesIndexRemove) = MAXUINT32;
        
        AddFreeList(proxyAccesIndexRemove);
        constexpr uint32_t maxFreeListSize = 150u;
        if (maxFreeListSize < m_FreeListVec.size())
        {
            RY_CORE_WARN("We have large holes in ProxyInidicesVec! {} removed elments!", m_FreeListVec.size());
        }

        for (uint32_t& proxyAccesIndex : m_ProxyInidicesVec)
        {
            if (MAXUINT32 != proxyAccesIndex && proxyAccesIndex > removeProxyIndex)
            {
                proxyAccesIndex--;
            }
        }

#ifdef RY_TEST_CHECK_FOR_IDENTY
        uint32_t count = m_ProxyInidicesVec.size();
        for (uint32_t x = 0; x < count; x++)
        {
            uint32_t proxyAceesX = m_ProxyInidicesVec.at(x);
            if (UINT32_MAX == proxyAceesX)
                continue;


            for (uint32_t y = x + 1; y < count; y++)
            {
                uint32_t proxyAceesY = m_ProxyInidicesVec.at(y);
                RY_CORE_ASSERT(proxyAceesX != proxyAceesY, "All proxy Indicies need to be unique, means only one time present!");
            }
        }
        RY_REMBER_FUNC_CHANGE("Remove the test if not longer needed!");
#endif

        return removeProxyIndex;
    }





    uint32_t RenderProxySortedProxyVec::InsertProxyIntoSortedPorxyVec(const RenderProxy& proxy, const glm::mat4& modelMatrix)
    {
        using SortedProxyIt = std::vector<RenderProxy>::iterator;
        using SortedBVHIt = std::vector<BoundingVolume>::iterator;
        using SortedMatrixIt = std::vector<glm::mat4>::iterator;

        SortedProxyIt itBeginProxy = m_SortedProxyVec.begin();
        SortedProxyIt itPosProxy = std::lower_bound(itBeginProxy, m_SortedProxyVec.end(), proxy
            , [this](const RenderProxy& aProxy, const RenderProxy& bProxy)
            {

                uint64_t keyA = aProxy.GetKey();
                uint64_t keyB = bProxy.GetKey();

                bool result = keyA < keyB;
                return result;

            }
        );
        uint32_t insertIndex = itPosProxy - itBeginProxy;

        {
            m_SortedProxyVec.insert(itPosProxy, proxy);

            RenderProxyKey renderProxyKey = proxy.GetKey();
            InsertProxyGroupView(insertIndex, renderProxyKey);

        }

        return insertIndex;
    }

    uint32_t RenderProxySortedProxyVec::GetProxyIndex(uint32_t proxyAccesIndex) const
    {
        RY_CORE_ASSERT(proxyAccesIndex < m_ProxyInidicesVec.size(), "Buffer Overflow!");
        uint32_t proxyIndex = m_ProxyInidicesVec.at(proxyAccesIndex);
        RY_CORE_ASSERT(UINT32_MAX != proxyIndex, "Proxy Store Index is Not Vaild!");
        return proxyIndex;
    }

    void RenderProxySortedProxyVec::RemoveProxyFromSortedPorxyVec(uint32_t proxyIndexRemove)
    {
        RY_CORE_ASSERT(proxyIndexRemove < m_SortedProxyVec.size(), "Buffer Overflow!");
        using SortedProxyIt = std::vector<RenderProxy>::iterator;
        using SortedBVHIt = std::vector<BoundingVolume>::iterator;
        using SortedMatrixIt = std::vector<glm::mat4>::iterator;
        
        {
            SortedProxyIt itBeginProxy = m_SortedProxyVec.begin();
            SortedProxyIt itPosProxy = itBeginProxy + proxyIndexRemove;

            RenderProxyKey renderProxyKey = (*itPosProxy).GetKey();
            RemoveProxyGroupView(proxyIndexRemove, renderProxyKey);

            m_SortedProxyVec.erase(itPosProxy);
        }
        
    }

    void RenderProxySortedProxyVec::RebuildSortedIndices()
    {
        RY_CORE_NOT_IMPL();
        RY_REMBER_FUNC_CHANGE("Implemt of func to genrate from scratch!");
    }

    uint32_t RenderProxySortedProxyVec::GetIndexFromFreeList()
    {
        using ItVec = std::vector<uint32_t>::iterator;
        RY_CORE_ASSERT(!m_FreeListVec.empty());

        uint32_t index = m_FreeListVec.back();
        m_FreeListVec.pop_back();

        return index;
    }

    void RenderProxySortedProxyVec::AddFreeList(uint32_t index)
    {
        m_FreeListVec.emplace_back(index);
    }

    void RenderProxySortedProxyVec::InsertProxyGroupView(uint32_t proxyAccesIndexInsert, RenderProxyKey renderProxyKey)
    {
        bool hasKeyAlrady = false;
        for (auto&[key, groupRange] : m_GroupViewsMap)
        {
            if(key == renderProxyKey)
            {
                hasKeyAlrady = true;
                groupRange.second++;
            }
            else if (proxyAccesIndexInsert <= groupRange.first)
            {
                groupRange.first++;
            }
            
        }

        if (hasKeyAlrady)
            return;

        m_GroupViewsMap[renderProxyKey] = { proxyAccesIndexInsert, 1 };
    }

    void RenderProxySortedProxyVec::RemoveProxyGroupView(uint32_t proxyAccesIndexRemove, RenderProxyKey renderProxyKey)
    {
        bool hasZeroMeber = false;
        for (auto& [key, groupRange] : m_GroupViewsMap)
        {
            if (groupRange.first < proxyAccesIndexRemove)
            {
                groupRange.first--;
            }
            else if (key == renderProxyKey)
            {
                groupRange.second--;
                hasZeroMeber = 0 == groupRange.second;
            }
        }

        if (hasZeroMeber)
        {
            m_GroupViewsMap.erase(renderProxyKey);
        }
        
    }

    void RenderProxySortedProxyVec::CheckColsionGroupView()
    {
        uint32_t i = 0;
        for (const RenderProxy& proxy : m_SortedProxyVec)
        {
            uint32_t firstIndex = m_GroupViewsMap.at(proxy.GetKey()).first;
            uint32_t countGroup = m_GroupViewsMap.at(proxy.GetKey()).second;
            uint32_t lastIndex = firstIndex + countGroup;

            RY_CORE_ASSERT(firstIndex <= i && i < lastIndex);
            i++;


        }
        {
            std::map<RenderProxyKey, IndexCountPair> map1;

            BufferLayout renderTragetLayoute = BufferLayout(
                {
                    { SDT::Float4, "Color" },
                    { SDT::Int, "Entity" }
                }
            );

            uint32_t i = 0;
            uint32_t proxyCount = Size();
            while (i < proxyCount)
            {
                uint32_t index = i;
                uint32_t count = 0u;
                Batch3DKey curentBatch3DKey;
                RenderProxyKey key;
                {
                    RenderProxy& proxy = AtSortedProxy(i);
                    constexpr int levelLOD = 0;
                    const Ref<Material>& materiel = proxy.GetMaterial();
                    const Ref<MeshSingle>& mesh = proxy.GetMesh();
                    DrawSpecification::BatchConfig darwSpec = materiel->GetDrawSpecification(renderTragetLayoute, levelLOD);
                    curentBatch3DKey = Batch3DKey(materiel, mesh, renderTragetLayoute, levelLOD);

                    key = proxy.GetKey();
                }

                Batch3DKey proxyBatch3DKey;
                uint64_t curentBatchKeyValue = curentBatch3DKey;
                do
                {
                    {
                        RenderProxy& proxy = AtSortedProxy(i);

                        {
                            constexpr int levelLOD = 0;
                            const Ref<Material>& materiel = proxy.GetMaterial();
                            const Ref<MeshSingle>& mesh = proxy.GetMesh();
                            DrawSpecification::BatchConfig darwSpec = materiel->GetDrawSpecification(renderTragetLayoute, levelLOD);
                            proxyBatch3DKey = Batch3DKey(materiel, mesh, renderTragetLayoute, levelLOD);
                        }
                        RenderProxyKey proxyKey = proxy.GetKey();
                        uint64_t proxyBatchKeyValue = curentBatch3DKey;

                        RY_CORE_ASSERT(proxyBatchKeyValue == curentBatchKeyValue);
                        RY_CORE_ASSERT(key == proxyKey);
                    }
                    i++;
                    count++;

                    if (i < proxyCount)
                    {
                        RenderProxy& nextProxy = AtSortedProxy(i);

                        constexpr int levelLOD = 0;
                        const Ref<Material>& materiel = nextProxy.GetMaterial();
                        const Ref<MeshSingle>& mesh = nextProxy.GetMesh();
                        DrawSpecification::BatchConfig darwSpec = materiel->GetDrawSpecification(renderTragetLayoute, levelLOD);
                        proxyBatch3DKey = Batch3DKey(materiel, mesh, renderTragetLayoute, levelLOD);
                    }
                } while (i < proxyCount && proxyBatch3DKey == curentBatch3DKey);
                
                
                IndexCountPair& pair = map1[key];
                pair.first = index;
                pair.second = count;


                const IndexCountPair& groupRangeClassMap = m_GroupViewsMap.at(key);

                RY_CORE_ASSERT(pair.first == groupRangeClassMap.first, "not same first Index!");
                RY_CORE_ASSERT(pair.second == groupRangeClassMap.second, "not same Count!");

            }
        }
        {
            std::map<RenderProxyKey, IndexCountPair> map2;
            {
                RenderProxyKey key = 0;
                IndexCountPair* ptr = nullptr;
                uint32_t index = 0;
                for (const RenderProxy& proxy : m_SortedProxyVec)
                {
                    if (key != proxy.GetKey())
                    {
                        key = proxy.GetKey();
                        ptr = &(map2[key]);
                        uint32_t& indexRef = (ptr->first);
                        indexRef = index;
                    }
                    uint32_t& countRef = (ptr->second);
                    countRef++;
                    index++;
                }
            }


            for (const auto& [keyClassMap, groupRangeClassMap] : m_GroupViewsMap)
            {
                IndexCountPair& pair = map2.at(keyClassMap);
                RY_CORE_ASSERT(pair.first == groupRangeClassMap.first, "not same first Index!");
                RY_CORE_ASSERT(pair.second == groupRangeClassMap.second, "not same Count!");
            }
        }


        
        
    }

#pragma endregion
   
}
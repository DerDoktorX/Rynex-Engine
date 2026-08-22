#include "rypch.h"
#include "RenderProxyMapProxyVec.h"

namespace Rynex {

    

    RenderProxyGroup::RenderProxyGroup()
        : m_MeshBatch(RenderMeshBatch())
        , m_ProxyVec()
        , m_ProxyInidicesVec()
        , m_FreeListVec()
        , m_ActiveCount(0)
    {
    }

    RenderProxyGroup::RenderProxyGroup(const RenderMeshBatch& renderMeshBatch)
        : m_MeshBatch(renderMeshBatch)
        , m_ProxyVec()
        , m_ProxyInidicesVec()
        , m_FreeListVec()
        , m_ActiveCount(0)
    {
    }

    RenderProxyGroup::~RenderProxyGroup()
    {
    }

    uint32_t RenderProxyGroup::Add(const RenderProxy& proxy)
    {

        uint32_t proxyVecIndex = m_ProxyVec.size();
        m_ProxyVec.emplace_back(proxy);

        uint32_t stableIndex = InsertProxyIndex(proxyVecIndex);

        return stableIndex;
    }

    void RenderProxyGroup::Remove(uint32_t stableIndex)
    {
        uint32_t proxyIndexRemove = RemoveProxyIndex(stableIndex);

        --m_ActiveCount;

#ifdef RY_TEST_CHECK_FOR_IDENTY
        uint32_t freeCount = static_cast<uint32_t>(m_FreeList.size());
        for (uint32_t x = 0; x < freeCount; ++x)
            for (uint32_t y = x + 1; y < freeCount; ++y)
                RY_CORE_ASSERT(m_FreeList[x] != m_FreeList[y],"Doppelter Eintrag in der FreeList!");
#endif
    }

    RenderProxy& RenderProxyGroup::AtProxy(uint32_t stableIndex)
    {
        uint32_t index = GetProxyIndex(stableIndex);
        return AtSortedProxy(stableIndex);
    }

    const RenderProxy& RenderProxyGroup::AtProxy(uint32_t stableIndex) const
    {
        uint32_t index = GetProxyIndex(stableIndex);
        return AtSortedProxy(stableIndex);
    }

    RenderProxy& RenderProxyGroup::AtSortedProxy(uint32_t index)
    {
        RY_CORE_ASSERT(index < static_cast<uint32_t>(m_ProxyVec.size()), "Buffer Overflow!");
        return m_ProxyVec.at(index);
    }

    const RenderProxy& RenderProxyGroup::AtSortedProxy(uint32_t index) const
    {
        RY_CORE_ASSERT(index < static_cast<uint32_t>(m_ProxyVec.size()), "Buffer Overflow!");
        return m_ProxyVec.at(index);
    }

    bool RenderProxyGroup::IsValid(uint32_t stableIndex) const
    {
        if (m_ProxyInidicesVec.size() <= stableIndex)
            return false;

        uint32_t proxyIndex = m_ProxyInidicesVec.at(stableIndex);
        return UINT32_MAX != proxyIndex;
    }

    

    uint32_t RenderProxyGroup::GetProxyIndex(uint32_t stableIndex) const
    {
        RY_CORE_ASSERT(IsValid(stableIndex), "Buffer Overflow!");
        uint32_t proxyIndex = m_ProxyInidicesVec.at(stableIndex);
        RY_CORE_ASSERT(UINT32_MAX != proxyIndex, "Proxy Store Index is Not Vaild!");
        return proxyIndex;
    }

    uint32_t RenderProxyGroup::InsertProxyIndex(uint32_t proxyAccesIndexInsert)
    {
        for (uint32_t& proxyAccesIndex : m_ProxyInidicesVec)
        {
            if (MAXUINT32 != proxyAccesIndex && proxyAccesIndexInsert <= proxyAccesIndex)
            {
                proxyAccesIndex++;
            }
        }
        uint32_t proxyIndex;
        if (m_FreeListVec.empty())
        {
            proxyIndex = m_ProxyInidicesVec.size();
            m_ProxyInidicesVec.emplace_back(proxyAccesIndexInsert);
        }
        else
        {
            proxyIndex = GetIndexFromFreeList();
            uint32_t& indexProxy = m_ProxyInidicesVec.at(proxyIndex);
            RY_CORE_ASSERT(UINT32_MAX == indexProxy, "The Stored Value is Vaild!");
            indexProxy = proxyAccesIndexInsert;
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

    uint32_t RenderProxyGroup::RemoveProxyIndex(uint32_t proxyAccesIndexRemove)
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

    uint32_t RenderProxyGroup::GetIndexFromFreeList()
    {
        using ItVec = std::vector<uint32_t>::iterator;
        RY_CORE_ASSERT(!m_FreeListVec.empty());

        uint32_t index = m_FreeListVec.back();
        m_FreeListVec.pop_back();

        return index;
    }

    void RenderProxyGroup::AddFreeList(uint32_t index)
    {
        m_FreeListVec.emplace_back(index);
    }


#pragma region RenderProxyMapProxyVec

    RenderProxyMapProxyVec::RenderProxyMapProxyVec()
        : m_GroupMap()
    {
    }

    RenderProxyMapProxyVec::RenderProxyMapProxyVec(const RenderProxyMapProxyVec& r)
        : m_GroupMap(r.m_GroupMap)
    {
        uint32_t count = Size();
        uint32_t copyCount = r.Size();
        RY_CORE_ASSERT(count == copyCount, "not same count!");
    }

    RenderProxyMapProxyVec::~RenderProxyMapProxyVec()
    {
        Clear();
    }

    uint32_t RenderProxyMapProxyVec::SubmiteProxy(const RenderProxy& proxy, const glm::mat4& modelMatrix)
    {
        RenderProxyKey key = proxy.GetKey();
        auto it = m_GroupMap.find(key);
        if (it == m_GroupMap.end())
            return it->second.Add(proxy);

        RenderProxyGroup& proxyGroup = m_GroupMap[key] = RenderProxyGroup(proxy.meshBatch);
        return proxyGroup.Add(proxy);
    }

    void RenderProxyMapProxyVec::Remove(uint32_t proxyIndex, RenderProxyKey renderProxyKey)
    {
        Iterator it = m_GroupMap.find(renderProxyKey);
        RY_CORE_ASSERT(it != m_GroupMap.end(), "Keine Gruppe für diesen Key gefunden!");

        it->second.Remove(proxyIndex);

        if (it->second.Empty())
            m_GroupMap.erase(it);

    }

    bool RenderProxyMapProxyVec::Empty() const
    {
        return m_GroupMap.empty();
    }

    uint32_t RenderProxyMapProxyVec::Size() const
    {
        uint32_t count = 0;
        for (const auto& [key, group] : m_GroupMap)
            count += group.ActiveSize();
        return count;
    }

    void RenderProxyMapProxyVec::Clear()
    {
        m_GroupMap.clear();

    }

    RenderProxy& RenderProxyMapProxyVec::AtProxy(RenderProxyKey renderProxyKey, uint32_t proxyIndex)
    {
        Iterator it = m_GroupMap.find(renderProxyKey);
        RY_CORE_ASSERT(it != m_GroupMap.end(), "Proxy Store Key is Not Vaild!");
        return it->second.AtProxy(proxyIndex);
    }

    const RenderProxy& RenderProxyMapProxyVec::AtProxy(RenderProxyKey renderProxyKey, uint32_t proxyIndex) const
    {
        ConstIterator it = m_GroupMap.find(renderProxyKey);
        RY_CORE_ASSERT(it != m_GroupMap.end(), "Proxy Store Key is Not Vaild!");
        return it->second.AtProxy(proxyIndex);
    }



    RenderProxy& RenderProxyMapProxyVec::AtSortedProxy(RenderProxyKey renderProxyKey, uint32_t proxyIndex)
    {
        Iterator it = m_GroupMap.find(renderProxyKey);
        RY_CORE_ASSERT(it != m_GroupMap.end(), "Proxy Store Key is Not Vaild!");
        return it->second.AtSortedProxy(proxyIndex);
    }

    const RenderProxy& RenderProxyMapProxyVec::AtSortedProxy(RenderProxyKey renderProxyKey, uint32_t proxyIndex) const
    {
        ConstIterator it = m_GroupMap.find(renderProxyKey);
        RY_CORE_ASSERT(it != m_GroupMap.end(), "Proxy Store Key is Not Vaild!");
        return it->second.AtSortedProxy(proxyIndex);
    }
   

    RenderProxyGroup* RenderProxyMapProxyVec::GetGroup(RenderProxyKey key)
    {
        Iterator it = m_GroupMap.find(key);
        return (it != m_GroupMap.end()) ? &it->second : nullptr;
    }

    const RenderProxyGroup* RenderProxyMapProxyVec::GetGroup(RenderProxyKey key) const
    {
        ConstIterator it = m_GroupMap.find(key);
        return (it != m_GroupMap.end()) ? &it->second : nullptr;
    }


    std::pair<RenderProxyKey, uint32_t> RenderProxyMapProxyVec::FindeProxy(int entity, uint32_t subMesh) const
    {
        for (const auto& [key, group] : m_GroupMap)
        {
            const ProxyGroupViewConst& proxies = group.GetView();
            for (uint32_t i = 0; i < static_cast<uint32_t>(proxies.Size()); ++i)
            {
                if (!group.IsValid(i))
                    continue;
                if (proxies[i].entity == entity && proxies[i].subMesh == subMesh)
                    return { key, i };
            }
        }
        RY_CORE_ERROR("Not found Proxy! Enity: ({}) / SubMesh: ({})", entity, subMesh);
        return { RenderProxyKey{0}, UINT32_MAX }; 
    }


#pragma endregion


    

}

#include "rypch.h"
#include "RenderProxysListData.h"

namespace Rynex {

    class ExecuteEvent
    {
    public:
        ExecuteEvent(RenderProxysListData& proxy)
            : m_ProxyRef(proxy)
        {

        }

        void operator()(RenderProxysListData::AddFuncArgs funcArgs)
        {
            const RenderProxy& proxy = funcArgs.proxy;
            const glm::mat4& model = funcArgs.model;
            proxy.Check();

            uint32_t proxyIndex = m_ProxyRef.AddProxy(proxy, model);

       
            RY_CORE_ASSERT(!Asset::CurrentOnMainThread(), "Expexted not Main Thread!");
            auto& renderProxyVec = m_ProxyRef.m_RenderProxyArrayVec.at(RenderProxysListData::EventData);


            m_ProxyRef.m_RenderProxyMapEntity.AddProxyIndexToMap(proxy.entity, proxy.subMesh, proxyIndex, proxy.GetKey(), renderProxyVec);

            m_ProxyRef.m_HasChagedMain.at(RenderProxysListData::StateEvent) = true;
            m_ProxyRef.m_HasChagedCurent.at(RenderProxysListData::StateEvent) = true;
            renderProxyVec.Size();

        }

        void operator()(RenderProxysListData::UpdateFuncArgs funcArgs)
        {

            const RenderProxyConectionEntity::IndexSubMesh& indexSubMesh = m_ProxyRef.m_RenderProxyMapEntity.GetIndexSubMeshFromMap(funcArgs.entity, funcArgs.subMesh);

            RY_CORE_ASSERT(!Asset::CurrentOnMainThread(), "Expexted not Main Thread!");


            uint32_t indexProxy = indexSubMesh.proxyIndex;
            RenderProxyKey renderProxyKey = indexSubMesh.renderProxyKey;
            RY_CORE_ASSERT(indexSubMesh.subMesh == funcArgs.subMesh, "not the same sub mesh!");
            m_ProxyRef.UpdateProxyTransform(funcArgs.entity, funcArgs.subMesh, indexProxy, renderProxyKey, funcArgs.model);
            m_ProxyRef.m_HasChagedMain.at(RenderProxysListData::StateEvent) = true;
            m_ProxyRef.m_HasChagedCurent.at(RenderProxysListData::StateEvent) = true;
            m_ProxyRef.m_RenderProxyArrayVec.at(RenderProxysListData::EventData).Size();

        }
        void operator()(RenderProxysListData::RemoveFuncArgs funcArgs)
        {

            RY_CORE_ASSERT(!Asset::CurrentOnMainThread(), "Expexted not Main Thread!");

            auto[proxyIndex, renderProxyKey] = m_ProxyRef.m_RenderProxyMapEntity.RemoveProxyIndexFromMap(funcArgs.entity, funcArgs.subMesh);

            RY_CORE_INFO("Remove Enity {} Submesh {} Index[{}]", funcArgs.entity, funcArgs.subMesh, proxyIndex);
            m_ProxyRef.RemoveProxy(funcArgs.entity, funcArgs.subMesh, proxyIndex, renderProxyKey);


            m_ProxyRef.m_HasChagedMain.at(RenderProxysListData::StateEvent) = true;
            m_ProxyRef.m_HasChagedCurent.at(RenderProxysListData::StateEvent) = true;
            m_ProxyRef.m_RenderProxyArrayVec.at(RenderProxysListData::EventData).Size();
            RY_CORE_WARN("Not test Func");
        }

        void operator()(RenderProxysListData::RemoveFuncArgs2 funcArgs)
        {

            std::vector<RenderProxyConectionEntity::IndexSubMesh> proxyIndexVec = m_ProxyRef.m_RenderProxyMapEntity.RemoveProxyIndexVecFromMap(funcArgs.entity);
            RY_CORE_ASSERT(!Asset::CurrentOnMainThread(), "Expexted not Main Thread!");

            RY_CORE_INFO("Remove all Submesh from Enity {}", funcArgs.entity);
            for (const RenderProxyConectionEntity::IndexSubMesh& indexSubMesh : proxyIndexVec)
            {
                RY_CORE_INFO("Remove Enity {} Submesh {} Index[{}]", funcArgs.entity, indexSubMesh.subMesh, indexSubMesh.proxyIndex);
                m_ProxyRef.RemoveProxy(funcArgs.entity, indexSubMesh.subMesh, indexSubMesh.proxyIndex, indexSubMesh.renderProxyKey);
            }
            m_ProxyRef.m_HasChagedMain.at(RenderProxysListData::StateEvent) = true;
            m_ProxyRef.m_HasChagedCurent.at(RenderProxysListData::StateEvent) = true;
            m_ProxyRef.m_RenderProxyArrayVec.at(RenderProxysListData::EventData).Size();

            
        }
    private:
        RenderProxysListData& m_ProxyRef;
    };


    RenderProxysListData::RenderProxysListData()
        : m_HasChagedMain({ true, true })
        , m_HasChagedCurent({ true, true })
        , m_Stop(false)
        , m_CopySwitch(false)
        , m_CopyEvent(false)
    {
        ResetEventWaitingTimer();
        m_RenderProxyVecPtr = &m_RenderProxyArrayVec.at(RenderingData0);
        CreateThread();
    }

    RenderProxysListData::~RenderProxysListData()
    {
        DestroyThread();
        Clear();
    }

    void RenderProxysListData::Clear()
    {

        std::scoped_lock<std::mutex> eventLock(m_ThreadQueue.GetMutex());
        std::scoped_lock<std::mutex> taskLock(m_EventWorkingTask);

        for (auto& renderProxyVec : m_RenderProxyArrayVec)
        {
            renderProxyVec.Clear();
        }
        m_HasChagedMain.at(StateEvent) = true;
        m_HasChagedCurent.at(StateEvent) = true;
        m_HasChagedMain.at(StateRendering) = true;
        m_HasChagedCurent.at(StateRendering) = true;

        while (!m_ThreadQueue.Empty())
        {
            m_ThreadQueue.PopNoLock();
        }
        m_RenderProxyMapEntity.Clear();


        ResetEventWaitingTimer();


    }

    void RenderProxysListData::Add(int entity, uint32_t subMesh, const Ref<MeshSingle>& mesh, const Ref<Material>& materiel, const glm::mat4& model)
    {

        RenderProxy proxy{
             mesh, materiel, model, entity, subMesh
        };
        RY_CORE_ASSERT(nullptr != mesh, "Not Valid Mesh!");
        RY_CORE_ASSERT(nullptr != materiel, "Not Valid Materiel!");
        RY_CORE_ASSERT(-1 != entity, "Not Valid Entity!");
        RY_CORE_ASSERT(-1 != subMesh, "Not Valid SubMesh!");


        AddFuncArgs addFuncArgs = {
            proxy, model
        };


        {
            std::unique_lock<std::mutex> lock(m_ThreadQueue.GetMutex());
            m_ThreadQueue.PushNoLock(addFuncArgs);
        }
        m_ConditionVariable.notify_one();


    }

    void RenderProxysListData::UpdateTrasform(int entity, uint32_t subMesh, const glm::mat4& model)
    {

        UpdateFuncArgs updateFuncArgs = UpdateFuncArgs{
            entity, subMesh, model
        };

        {
            std::unique_lock<std::mutex> lock(m_ThreadQueue.GetMutex());
            m_ThreadQueue.PushNoLock(updateFuncArgs);
        }
        m_ConditionVariable.notify_one();

    }

    void RenderProxysListData::Remove(int entity)
    {

        RemoveFuncArgs2 removeFuncArgs2 = RemoveFuncArgs2{
            entity
        };



        {
            std::unique_lock<std::mutex> lock(m_ThreadQueue.GetMutex());
            m_ThreadQueue.PushNoLock(removeFuncArgs2);
        }
        m_ConditionVariable.notify_one();
    }

    bool RenderProxysListData::HasEntity(int entity)
    {
        std::scoped_lock<std::mutex> lock(m_EventWorkingTask);
        return m_RenderProxyMapEntity.HasEntity(entity);
    }

    void RenderProxysListData::EventCallback()
    {
        if (!m_CopySwitch)
            return;
        std::scoped_lock lock = std::scoped_lock(m_SwitchTask);
        m_CopySwitch = false;
       
        
        
        m_RenderProxyArrayVec.at(RenderingData1).Size();
        m_RenderProxyArrayVec.at(RenderingData0).Size();
        RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexted Main Thread!");
        if (m_RenderProxyVecPtr == &m_RenderProxyArrayVec.at(RenderingData1))
        {

            m_RenderProxyVecPtr = &m_RenderProxyArrayVec.at(RenderingData0);
        } 
        else if (m_RenderProxyVecPtr == &m_RenderProxyArrayVec.at(RenderingData0))
        {
            m_RenderProxyVecPtr = &m_RenderProxyArrayVec.at(RenderingData1);
        }
        else
        {
            RY_CORE_ASSERT(false, "Not expeted State!");
        }
        m_RenderProxyArrayVec.at(RenderingData1).Size();
        m_RenderProxyArrayVec.at(RenderingData0).Size();

        m_RenderProxyVecPtr->Size();

        

        m_HasChagedMain.at(StateRendering) = true;
        m_HasChagedCurent.at(StateRendering) = true;
        
        m_HasChagedMain.at(StateEvent) = false;
        m_HasChagedCurent.at(StateEvent) = false;

       
    }


    void RenderProxysListData::Remove(int entity, uint32_t subMesh)
    {
        RY_CORE_ASSERT(!Asset::CurrentOnMainThread(), "Expexted not Main Thread!");

        auto[proxyIndex, proxyKey] = m_RenderProxyMapEntity.RemoveProxyIndexFromMap(entity, subMesh);
        RemoveProxy(entity, subMesh, proxyIndex, proxyKey);

        m_HasChagedMain.at(StateRendering) = true;
        m_HasChagedCurent.at(StateRendering) = true;

    }

    uint32_t RenderProxysListData::AddProxy(const RenderProxy& proxy, const glm::mat4& matrix)
    {
       
        RY_CORE_ASSERT(!Asset::CurrentOnMainThread(), "Expexted Not Main Thread!");
        auto& renderProxyVec = m_RenderProxyArrayVec.at(EventData);


        uint32_t index = renderProxyVec.SubmiteProxy(proxy, matrix);
        return index;
    }

    void RenderProxysListData::UpdateProxyTransform(int entity, uint32_t subMesh, uint32_t proxyIndex, RenderProxyKey renderProxyKey, const glm::mat4& matrix)
    {
        RY_CORE_ASSERT(!Asset::CurrentOnMainThread(), "Expexted not Main Thread!");
        auto& renderProxyVec = m_RenderProxyArrayVec.at(EventData);

        RenderProxy& proxy = renderProxyVec.AtProxy(renderProxyKey, proxyIndex);
        RY_CORE_ASSERT(proxy.entity == entity, "We update the whrong Enity in the Proxy!");
        RY_CORE_ASSERT(proxy.subMesh == subMesh, "We update the whrong SubMesh in the Proxy!");
        proxy.Check();

        proxy.model = matrix;
    }

    void RenderProxysListData::RemoveProxy(int entity, uint32_t subMesh, uint32_t proxyIndex, RenderProxyKey renderProxyKey)
    {
        RY_CORE_ASSERT(!Asset::CurrentOnMainThread(), "Expexted not Main Thread!");
        auto& renderProxyVec = m_RenderProxyArrayVec.at(EventData);
        if constexpr (std::is_same_v<std::decay_t<decltype(renderProxyVec)>, RenderProxySortedProxyVec>)
        {
            m_RenderProxyMapEntity.CheckConectionToProxyData(renderProxyVec);

            const RenderProxy& proxy = renderProxyVec.AtProxy(renderProxyKey, proxyIndex);
            proxy.Check();

            if (proxy.entity != entity || proxy.subMesh != subMesh)
            {
                std::pair<RenderProxyKey, uint32_t> pair = renderProxyVec.FindeProxy( entity, subMesh);
                RenderProxyKey key = pair.first;
                uint32_t index = pair.second;
                RY_CORE_ERROR("Whrong index, expextedt index Pos {} but actuely its {}", proxyIndex, index);

                const RenderProxy& proxy2 = renderProxyVec.AtProxy(key, index);
                RY_CORE_ASSERT(proxy2.entity == entity, "We remove the whrong Enity in the Proxy!");
                RY_CORE_ASSERT(proxy2.subMesh == subMesh, "We remove the whrong SubMesh in the Proxy!");
                renderProxyVec.Remove(index, key);

                return;
            }
            renderProxyVec.Remove(proxyIndex, renderProxyKey);


        }
        else if constexpr (std::is_same_v<std::decay_t<decltype(renderProxyVec)>, RenderProxyMapProxyVec>)
        {
            m_RenderProxyMapEntity.CheckConectionToProxyData(renderProxyVec);

            const RenderProxy& proxy = renderProxyVec.AtProxy(renderProxyKey, proxyIndex);
            proxy.Check();

            if (proxy.entity != entity || proxy.subMesh != subMesh)
            {
                std::pair<RenderProxyKey, uint32_t> pair = renderProxyVec.FindeProxy( entity, subMesh);
                RenderProxyKey key = pair.first;
                uint32_t index = pair.second;

                RY_CORE_ERROR("Whrong index, expextedt index Pos {} but actuely its {}", proxyIndex, index);

                const RenderProxy& proxy2 = renderProxyVec.AtProxy(key, index);
                RY_CORE_ASSERT(proxy2.entity == entity, "We remove the whrong Enity in the Proxy!");
                RY_CORE_ASSERT(proxy2.subMesh == subMesh, "We remove the whrong SubMesh in the Proxy!");
                renderProxyVec.Remove(index, key);

                return;
            }
            renderProxyVec.Remove(proxyIndex, renderProxyKey);


        }
        else
        {
            const RenderProxy& proxy = renderProxyVec.AtProxy(renderProxyKey, proxyIndex);
            RY_CORE_ASSERT(proxy.entity == entity, "We remove the whrong Enity in the Proxy!");
            RY_CORE_ASSERT(proxy.subMesh == subMesh, "We remove the whrong SubMesh in the Proxy!");
            proxy.Check();

            renderProxyVec.Remove(proxyIndex, renderProxyKey);
        }
    }

    bool RenderProxysListData::CheckProxyFromIndex(uint32_t proxyIndex, uint32_t proxyCount, ProxyDrawCallGenarter& drawCallGenrater)
    {
#ifdef RY_SORT_KEY
        if (proxyCount <= proxyIndex)
            return false;

        RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexted Main Thread!");
        RenderProxy& proxy = m_RenderProxyVecPtr->AtSortedProxy(proxyIndex);
        return drawCallGenrater.SetProxyData(proxy);
#else
        if (proxyCount <= proxyIndex)
            return false;

        RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexted Main Thread!");
        RenderProxy& proxy = m_RenderProxyVecPtr->AtSortedProxy(proxyIndex);
        return drawCallGenrater.SetProxyData(proxy);
#endif

    }

    void RenderProxysListData::ResetEventWaitingTimer()
    {
        RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexted Main Thread!");
        m_EventWaitingTimer = 0;
    }



    bool RenderProxysListData::HasEvent()
    {
        return !m_ThreadQueue.Empty() || m_Stop;
    }

    void RenderProxysListData::EventThreadFunc()
    {
        uint32_t notSyncEventsCount = 0u;
        constexpr uint32_t maxEvent = 100u;

        while (true)
        {
                        
            RemoveFuncArgs2 removeFuncArgs2 = RemoveFuncArgs2{ -1 };
            std::variant<AddFuncArgs, UpdateFuncArgs, RemoveFuncArgs2> event(removeFuncArgs2);
            {
                std::unique_lock<std::mutex> lock(m_ThreadQueue.GetMutex());
                m_ConditionVariable.wait(lock, RY_BIND_MEMBER_FUNC(RenderProxysListData::HasEvent));

                if (m_Stop)
                {
                    RY_CORE_WARN("We dont finsh the event list!");
                    return;
                }

                if(!m_ThreadQueue.Empty())
                {
                    event = m_ThreadQueue.GetFrontNoLock();
                    m_ThreadQueue.PopNoLock();
                }
                if (m_ThreadQueue.Empty() || maxEvent < notSyncEventsCount)
                    m_CopyEvent = true;
            }
            std::scoped_lock<std::mutex> lock(m_EventWorkingTask);
           

            RemoveFuncArgs2* removeFuncArgs2Ptr = std::get_if<RemoveFuncArgs2>(&event);
            if (nullptr != removeFuncArgs2Ptr && removeFuncArgs2Ptr->entity == -1)
            {
                RY_CORE_TRACE("Skip Event becouse invaild: {}", removeFuncArgs2Ptr->entity);
                CopeyDataEvent();
                notSyncEventsCount = 0;
                continue;
            }
            
            if (m_CopyEvent)
            {
                notSyncEventsCount = 0;
                CopeyDataEvent();
            }
            std::visit(ExecuteEvent{ *this }, event);
            notSyncEventsCount++;

        }
    }

    void RenderProxysListData::CreateThread()
    {
        RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexted Main Thread!");

        std::unique_lock<std::mutex> lock(m_ThreadQueue.GetMutex());
        m_EventProxyProccesThread = std::thread(RY_BIND_MEMBER_FN(RenderProxysListData::EventThreadFunc));
    }

    void RenderProxysListData::DestroyThread()
    {

        RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexted Main Thread!");
        {
            std::unique_lock<std::mutex> lock(m_ThreadQueue.GetMutex());
            m_Stop = true;
        }


        m_ConditionVariable.notify_all();
        m_EventProxyProccesThread.join();
    }

    void RenderProxysListData::CopeyDataEvent()
    {
        std::scoped_lock lock= std::scoped_lock(m_SwitchTask);

        RY_CORE_ASSERT(m_CopyEvent, "no copy Event!");
        if (m_RenderProxyVecPtr == &m_RenderProxyArrayVec.at(RenderingData0))
        {
            m_RenderProxyArrayVec.at(RenderingData1) = m_RenderProxyArrayVec.at(EventData);
        }
        else if (m_RenderProxyVecPtr == &m_RenderProxyArrayVec.at(RenderingData1))
        {
            m_RenderProxyArrayVec.at(RenderingData0) = m_RenderProxyArrayVec.at(EventData);
        }
        else
        {
            RY_CORE_ASSERT(false);
        }
        m_CopyEvent = false;
        m_CopySwitch = true;
    }

}





#pragma once
#include <Rynex/Memory/ThreadQueue.h>
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Renderer/RenderProxy/ProxyDrawCallGenarter.h>
#include <Rynex/Renderer/RenderProxy/RenderProxyConectionEntity.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchManager.h>

// #define RY_PROCESS_ON_THREAD_MAIN
#define RY_THREAD_QUEUE
#define RY_RENDER_PROXY_VEC_CLASS

#if 1
	#define RY_USE_SORTED_PROXY_LIST
#else
	#define RY_USE_MAP_PROXY_LIST
#endif
namespace Rynex {
	class ExecuteEvent;

	class RenderProxysListData
	{
	private:
		struct AddFuncArgs
		{
			AddFuncArgs() = delete;
			AddFuncArgs(AddFuncArgs&&) = delete;
			AddFuncArgs(const AddFuncArgs&) = default;

			RenderProxy proxy;
			glm::mat4 model;

			AddFuncArgs& operator=(const AddFuncArgs& left)
			{
				proxy = left.proxy;
				model = left.model;
				return *this;
			}
		};

		struct UpdateFuncArgs
		{
			int entity;
			uint32_t subMesh;
			glm::mat4 model;

			UpdateFuncArgs() = delete;
			UpdateFuncArgs(UpdateFuncArgs&&) = delete;
			UpdateFuncArgs(const UpdateFuncArgs&) = default;

			UpdateFuncArgs(int entity, uint32_t subMesh, const glm::mat4& model)
				: entity(entity)
				, subMesh(subMesh)
				, model(model)
			{
			}

			UpdateFuncArgs& operator=(const UpdateFuncArgs& left)
			{
				entity = left.entity;
				subMesh = left.subMesh;
				model = left.model;
				return *this;
			}
		};

		struct RemoveFuncArgs
		{
			int entity;
			uint32_t subMesh;
			RemoveFuncArgs() = delete;
			RemoveFuncArgs(RemoveFuncArgs&&) = delete;
			RemoveFuncArgs(const RemoveFuncArgs&) = default;

			RemoveFuncArgs(int entity, uint32_t subMesh)
				: entity(entity)
				, subMesh(subMesh)
			{
			}
		};

		struct RemoveFuncArgs2
		{
			int entity;
			RemoveFuncArgs2() = delete;
			RemoveFuncArgs2(RemoveFuncArgs2&&) = default;
			RemoveFuncArgs2(const RemoveFuncArgs2&) = default;

			RemoveFuncArgs2(int entity)
				: entity(entity)
			{
			}

			RemoveFuncArgs2& operator=(const RemoveFuncArgs2& left)
			{
				entity = left.entity;
				return *this;
			}

		};
#if 0
		struct ThreadProcess
		{
			RenderProxySortedIndicesVec renderProxySortedIndicesVec;
			RenderProxySortedProxyVec renderProxyArrayVec;
			Memory::ThreadQueueVarients<AddFuncArgs, UpdateFuncArgs, RemoveFuncArgs2> m_ThreadQueue;
		};
#endif
		enum StateIndex
		{
			NoneState = -1,
			StateRendering = 0,
			StateEvent = 1

		};

		enum DataIndex
		{
			NoneData = -1,
			EventData = 0,
			RenderingData0 = 1,
			RenderingData1 = 2,

		};
	public:
		RenderProxysListData();
		~RenderProxysListData();

		void Clear();

		void Add(int entity, uint32_t subMesh, const Ref<MeshSingle>& mesh, const Ref<Material>& materiel, const glm::mat4& model);
		void UpdateTrasform(int entity, const glm::mat4& model);
		void UpdateTrasform(int entity, uint32_t subMesh, const glm::mat4& model);

		void Remove(int entity);
		bool HasEntity(int entity);

		void EventCallback();

		bool HasChangedMain() const 
		{ 
			RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexxted Main Thread!");
			return m_HasChagedMain.at(StateRendering);
		}
		bool HasChangedCurent() const 
		{ 
			RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexxted Main Thread!");
			return m_HasChagedCurent.at(StateRendering);
		}
#if defined(RY_PROCESS_ON_THREAD_MAIN) && defined(RY_USE_SORTED_PROXY_LIST)
		RenderProxySortedProxyVec& GetRenderProxyVec() { return m_RenderProxyVec; }
#elif defined(RY_PROCESS_ON_THREAD_MAIN) && !defined(RY_USE_SORTED_PROXY_LIST)
		RenderProxySortedIndicesVec& GetRenderProxyVec() { return m_RenderProxyVec; }
#elif !defined(RY_PROCESS_ON_THREAD_MAIN) && defined(RY_USE_SORTED_PROXY_LIST)
		RenderProxySortedProxyVec& GetRenderProxyVec()
		{
			RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexxted Main Thread!");
			return *m_RenderProxyVecPtr;
		}
#elif !defined(RY_PROCESS_ON_THREAD_MAIN) && defined(RY_USE_MAP_PROXY_LIST)
		RenderProxyMapProxyVec& GetRenderProxyVec()
		{
			RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexxted Main Thread!");
			return *m_RenderProxyVecPtr;
		}
#else	
		RenderProxySortedIndicesVec& GetRenderProxyVec() 
		{ 
			RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexxted Main Thread!");
			return  *m_RenderProxyVecPtr;
		}
#endif
		void ResetChangedMain() 
		{
			RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexxted Main Thread!");
			m_HasChagedMain.at(StateRendering) = false;
		}

		void ResetChangedCurent()
		{ 
			RY_CORE_ASSERT(Asset::CurrentOnMainThread(), "Expexxted Main Thread!");
			m_HasChagedCurent.at(StateRendering) = false;
		}

		bool CheckProxyFromIndex(uint32_t proxyIndex, uint32_t proxyCount, ProxyDrawCallGenarter& drawCallGenrater);
	private:
		void Remove(int entity, uint32_t subMesh);

		uint32_t AddProxy(const RenderProxy& proxy, const glm::mat4& matrix);
		void UpdateProxyTransform(int entity, uint32_t subMesh, uint32_t proxyIndex, RenderProxyKey renderProxyKey, const glm::mat4& matrix);
		void RemoveProxy(int entity, uint32_t subMesh, uint32_t proxyIndex, RenderProxyKey renderProxyKey);


		void ResetEventWaitingTimer();
		bool HasEvent();
		void EventThreadFunc();
		void CreateThread();
		void DestroyThread();
		void CopeyDataEvent();
	private:
		int64_t m_EventWaitingTimer;
#if defined(RY_PROCESS_ON_THREAD_MAIN) && defined(RY_USE_SORTED_PROXY_LIST)
		RenderProxySortedProxyVec m_RenderProxyVec;
#elif defined(RY_PROCESS_ON_THREAD_MAIN) && !defined(RY_USE_SORTED_PROXY_LIST)
		RenderProxySortedIndicesVec m_RenderProxyVec;
#elif !defined(RY_PROCESS_ON_THREAD_MAIN) && defined(RY_USE_SORTED_PROXY_LIST)
		std::array<RenderProxySortedProxyVec, 3> m_RenderProxyArrayVec;
		RenderProxySortedProxyVec* m_RenderProxyVecPtr;

		std::thread m_EventProxyProccesThread;
		std::mutex m_EventWorkingTask;
		std::mutex m_SwitchTask;

		std::condition_variable m_ConditionVariable;
#elif !defined(RY_PROCESS_ON_THREAD_MAIN) && defined(RY_USE_MAP_PROXY_LIST)
		std::array<RenderProxyMapProxyVec, 3> m_RenderProxyArrayVec;
		RenderProxyMapProxyVec* m_RenderProxyVecPtr;

		std::thread m_EventProxyProccesThread;
		std::mutex m_EventWorkingTask;
		std::mutex m_SwitchTask;

		std::condition_variable m_ConditionVariable;
#else	
		std::array<RenderProxySortedIndicesVec, 3> m_RenderProxyArrayVec;
		RenderProxySortedIndicesVec* m_RenderProxyVecPtr;

		std::thread m_EventProxyProccesThread;
		std::mutex m_EventWorkingTask;
		std::mutex m_SwitchTask;
		std::condition_variable m_ConditionVariable;
#endif

		RenderProxyConectionEntity m_RenderProxyMapEntity;
		Memory::ThreadQueueVarients<AddFuncArgs, UpdateFuncArgs, RemoveFuncArgs2> m_ThreadQueue;

		bool m_Stop;
		bool m_CopyEvent;
		bool m_CopySwitch;

		std::array<bool, 2> m_HasChagedMain;
		std::array<bool, 2> m_HasChagedCurent;
	private:
		friend ExecuteEvent;
	};

}
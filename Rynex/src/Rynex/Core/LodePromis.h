#pragma once
#include "rypch.h"

namespace Rynex {
	template<typename N>
	class LodePromis
	{
	public:
		virtual ~LodePromis() {}
		virtual bool IsTransferComplet() const = 0;
		virtual void WaitForLoding() = 0;


		virtual void AddRefObject(const Ref<N>& addObject) = 0;
		virtual void ChangeRefObject(const Ref<N>& from, const Ref<N>& to) = 0;

	};

	template<typename T, typename N, typename ...Args>
	class LodePromisType : public LodePromis<N>
	{
	public:
		using _Func = typename std::function<void(Ref<typename T>, Ref<typename N>, typename Args...)>;
	// --- public member funktion ---------------------------------------------------------------------------------------------

		template<typename ...Vars>
		LodePromisType(const Ref<N>& object, const _Func& func, Vars&& ... funcArgs)
			: m_ObjectVec({
					object
				})
			, m_StaticFunc(func)
			, m_TransferComplet(false)
			, m_FuncArgs(std::forward<Vars>(funcArgs)...)
		{
		}

		LodePromisType(const LodePromisType& promis)
			: m_ObjectVec(promis.m_ObjectVec)
			, m_Mutex(promis.m_Mutex)
			, m_StaticFunc(promis.m_StaticFunc)
			, m_WaitMutex(promis.m_WaitMutex)
			, m_FuncArgs(promis.m_FuncArgs)
			, m_TransferComplet(promis.m_TransferComplet)
			, m_CV(promis.m_CV)
		{
			RY_CORE_WARN("Copy a Promis is not somthing we Recomand doing!");
		}

		LodePromisType()
			: m_ObjectVec()
			, m_Mutex()
			, m_WaitMutex()
			, m_FuncArgs()
			, m_TransferComplet(true)
			, m_CV() 
		{
		}

		~LodePromisType()
		{
			RY_CORE_ASSERT(m_TransferComplet);
		}

		operator bool() const
		{
			return IsTransferComplet();
		}

		virtual bool IsTransferComplet() const override
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			return m_TransferComplet;
		}

		template<typename ...Vars>
		void ChangeFuncArgs(Vars&& ... funcArgs)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_FuncArgs = std::tuple<Args ...>(std::forward<Vars>(funcArgs)...);
		}

		virtual void ChangeRefObject(const Ref<N>& from, const Ref<N>& to)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);

			for (Weak<N>& weakObject : m_ObjectVec)
			{
				if (Ref<N> refObject = weakObject.lock())
				{
					if (from == refObject)
					{
						refObject = to;
						return;
					}
				}
			}

		}

		void ChangeRefObject(const Ref<N>& from, const Ref<N>& to, const _Func& funcTo)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			
			for (Weak<N>& weakObject : m_ObjectVec)
			{
				if (Ref<N> refObject = weakObject.lock())
				{
					if (from == refObject)
					{
						refObject = to;
						m_StaticFunc = funcTo;
						return;
					}
				}
			}
			RY_CORE_ASSERT("didient Found");
		}

		virtual void AddRefObject(const Ref<N>& addObject)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);

			for (Weak<N>& weakObject : m_ObjectVec)
			{
				if (Ref<N> refObject = weakObject.lock())
				{
					if (addObject == refObject)
						return;
				}
			}
			m_ObjectVec.emplace_back(addObject);
		}

		void AddRefObject(const Ref<N>& addObject, const _Func& addFunc)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);

			for (Weak<N>& weakObject : m_ObjectVec)
			{
				if (Ref<N> refObject = weakObject.lock())
				{
					if (addObject == refObject)
					{

						return;
					}
				}
			}
			m_ObjectVec.emplace_back(addObject);
			m_StaticFunc = addFunc;
		}

		void RemoveRefObject(const N* remvoePtrObject)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			uint32_t index = 0;
			for (Weak<typename N>& weakObject : m_ObjectVec)
			{
				if (Ref<typename N> refObject = weakObject.lock())
				{
					const N* ptrObject = refObject.get();

					if (remvoePtrObject == ptrObject)
						break;
				}
				index++;
			}
			uint32_t count = m_ObjectVec.size();
			if (index < count)
			{
				std::vector<typename Weak<typename N>>::const_iterator itBegin = m_ObjectVec.begin();
				std::vector<typename Weak<typename N>>::const_iterator it = itBegin + index;
				m_ObjectVec.erase(it);
			}
		}

		void OnLodingFinisht(Ref<T> asset) 
		{
			Transfering(asset);
			m_CV.notify_one();
		}

		virtual void WaitForLoding() override
		{
			// Difernz ?
			std::unique_lock<std::mutex> lock(m_WaitMutex);
			// std::lock_guard<std::mutex> lock(m_WaitMutex);
			std::function<bool()> func = std::bind(&LodePromisType<T, N, Args...>::LodingFinsht, this);
			m_CV.wait(lock, func);
			RY_CORE_TRACE("Starte Exexute again after now Loding is Finish");
		}
	private:
		void Transfering(Ref<T> asset)
		{
			// Difernz ?
			// std::unique_lock<std::mutex> lock(m_Mutex);
			std::lock_guard<std::mutex> lock(m_Mutex);
			for (Weak<typename N>& weakObject : m_ObjectVec)
			{
				if (Ref<typename N> refObject = weakObject.lock())
				{
					RY_CORE_INFO("Promis Object Still in Place");

					Execute(refObject, asset);
				}
				weakObject.reset();
			}
			m_TransferComplet = true;
		}
		bool LodingFinsht() const
		{
			return m_TransferComplet;
		}
		void Execute(Ref<N> object, Ref<T> asset)
		{
#if 1

			std::apply([object, asset, func = m_StaticFunc](auto&&... args)
				{
					func(asset, object, std::forward<decltype(args)>(args)...);
				}, m_FuncArgs);
#else
			std::apply(
				[object, asset, func](Args&&... args)
				{
					func(asset, object, std::forward<Args>(args)...);
				}, m_FuncArgs);
#endif
		}
		
	// --- private member varibles --------------------------------------------------------------------------------------------
		std::vector<typename Weak<typename N>> m_ObjectVec;
		_Func m_StaticFunc;
		mutable std::mutex m_Mutex;
		std::mutex m_WaitMutex;
		std::tuple<typename Args ...> m_FuncArgs;
		std::condition_variable m_CV;
		bool m_TransferComplet;
	};

}
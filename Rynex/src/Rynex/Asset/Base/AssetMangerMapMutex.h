#pragma once
#include <rypch.h>

#define CHECK_LOCK_STATE_DIRECT_DATA_ACCES(returnFaild) \
	if(!this->IsLockedFromOutsideAny()) \
	{\
		RY_CORE_ASSERT(!m_ScopeGuardLock.expired(), "Curently mutex is not locked! no Scope Lock"); \
		RY_CORE_ASSERT(m_OutSideScope, "Curently mutex is not locked! no Start/Ende Lock");\
		return returnFaild;\
	}\
	RY_CORE_WARN("We give now Direct Data Acces!")

#define CHECK_IS_NOT_LOCK_STATE_LOCKING(returnFaild) \
	if(!this->IsNotLockedFromOutsideCarfull()) \
	{\
		RY_CORE_ASSERT(m_ScopeGuardLock.expired(), "Curently mutex is locked! no Scope Lock"); \
		RY_CORE_ASSERT(!m_OutSideScope, "Curently mutex is locked! no Start/Ende Lock");\
		return returnFaild;\
	}\
	RY_CORE_INFO("We are now able to Lock resurces From outside!")

namespace Rynex {

	template<typename K, typename T>
	class AssetMangerMapMutex
	{
	public:
		using RefScopeLock = Ref<std::unique_lock<std::shared_mutex>>;
		using WeakScopeLock = Weak<std::unique_lock<std::shared_mutex>>;
	public:
		AssetMangerMapMutex()
			: m_Stop(false)
		{
			m_OutSideScope.store(false);
			m_OutSideScopeThreadID.store(0);
		};

		AssetMangerMapMutex(const AssetMangerMapMutex&) = delete;
		AssetMangerMapMutex(AssetMangerMapMutex&&) = default;

		~AssetMangerMapMutex();

		inline T GetCopy(const K& key) const
		{
			return Read(
				[&key](const std::map<K, T>& map)
				{
					return map.at(key);
				}
			);

		};
		
		inline void GetRefLemda(std::function<void(T&)> func, const K& key)
		{
			Write(
				[&key, func](std::map<K, T>& map)
				{
					func(map.at(key));
				}
			);
		};


		// Set Globle Mutex!
		inline T& GetRef(const K& key)
		{
			CHECK_LOCK_STATE_DIRECT_DATA_ACCES(T());			
			return m_AssetMap.at(key);
		};

		inline RefScopeLock GetMutexScopeLock()
		{	
			CHECK_IS_NOT_LOCK_STATE_LOCKING(nullptr);		
			RefScopeLock lockScope = CreateRef<std::unique_lock<std::shared_mutex>>(m_Mutex);
			CHECK_IS_NOT_LOCK_STATE_LOCKING(nullptr);


			uint64_t scopeThreadID = m_OutSideScopeThreadID.load();
			uint64_t currentThreadID = std::hash<std::thread::id>{}(std::this_thread::get_id());

			m_OutSideScopeThreadID.store(currentThreadID);
			m_ScopeGuardLock = WeakScopeLock(lockScope);
			
			return lockScope;
		}

		inline std::map<K, T>* GetMapPtr()
		{
			CHECK_LOCK_STATE_DIRECT_DATA_ACCES(nullptr);

			return &m_AssetMap;

		}

		// Set Globle Mutex!
		inline void GetPtr(const K& key, T* value)
		{
			
			Write(
				[&key, value](std::map<K, T>& map)
				{
					*value = map.at(key);
				}
			);
		};

		template<typename Func>
		inline void WriteValue(const K& key, Func&& func)
		{
			Write(
				[&key, func](std::map<K, T>& map)
				{
					T& value = map.at(key);
					func(value);
				}
			);
		}

		inline void Set(const K& key, const T& value)
		{
			Write(
				[&key, &value](std::map<K, T>& map)
				{
					map.at(key) = value;
				}
			);

		};

		inline void Add(const K& key, const T& value)
		{

			Write(
				[&key, &value](std::map<K, T>& map)
				{
					map[key] = value;
				}
			);
		};

		inline void Remove(const K& key)
		{
			Write(
				[&key](std::map<K, T>& map)
				{
					map.erase(key);
				}
			);
		};

		inline void Change(const K& key, const T& value)
		{
			Write(
				[&key, &value](std::map<K, T>& map)
				{
					map.at(key) = value;
				}
			);
		};

		template<typename Func>
		inline auto ReadValue(const K& key, Func&& func) const
		{
			return Read(
				[&key, func](const std::map<K, T>& map)
				{
					const T& value = map.at(key);
					return func(value);
				}
			);
		}

		inline bool IsFound(const K& key) const
		{
			return Read(
				[&key](const std::map<K, T>& map) 
				{
					return map.find(key) != map.end();
				}
			);
		};

		inline void Clear()
		{
			Write(
				[](std::map<K, T>& map)
				{
					map.clear();
				}
			);		
		}

		template<typename Func>
		auto Read(Func&& func) const
		{
			std::shared_lock lock(m_Mutex);
			RY_CORE_ASSERT(!m_OutSideScope, "Mutex is alrady Set Globle");
			const std::map<K, T>& map = m_AssetMap;
			return func(map);
		}

		template<typename Func>
		void Write(Func&& func)
		{
			std::unique_lock lock(m_Mutex);
			RY_CORE_ASSERT(!m_OutSideScope, "Mutex is alrady Set Globle");
			if (m_Stop)
				return;
			WriteAction(func);
		}
	private:
		template<typename Func>
		void WriteAction(Func&& func)
		{
			std::map<K, T>& map = m_AssetMap;
			func(map);
		}
		inline bool IsLockedFromOutsideAny() const
		{
			if (m_ScopeGuardLock.expired())
			{			
				uint64_t scopeThreadID = m_OutSideScopeThreadID.load();
				if (0 != scopeThreadID)
				{
					scopeThreadID = 0u;
					m_OutSideScopeThreadID.store(scopeThreadID);
				}
				return false;

			}
			else
			{
				uint64_t scopeThreadID = m_OutSideScopeThreadID.load();
				uint64_t currentThreadID = std::hash<std::thread::id>{}(std::this_thread::get_id());

				RY_CORE_ASSERT(0 != scopeThreadID, "error no Thread ID set!");
				RY_CORE_ASSERT(currentThreadID == scopeThreadID, "This thread can't acces data! globel scope lock is set on a other thraed!");

				return true;
			}
			RY_CORE_ASSERT(false);
			return false;
		}

		inline bool IsLockedFromOutsideCarfull() const
		{
			if(m_ScopeGuardLock.expired())
			{
				uint64_t scopeThreadID = m_OutSideScopeThreadID.load();
				if (0 != scopeThreadID)
				{
					scopeThreadID = 0u;
					m_OutSideScopeThreadID.store(scopeThreadID);
				}
				
				return false;
			}
			else
			{
				uint64_t scopeThreadID = m_OutSideScopeThreadID.load();
				uint64_t currentThreadID = std::hash<std::thread::id>{}(std::this_thread::get_id());

				RY_CORE_ASSERT(0 != scopeThreadID, "error no Thread ID set!");
				RY_CORE_ASSERT(currentThreadID == scopeThreadID, "This thread can't acces data! globel scope lock is set on a other thraed!");
				return true;
			}
			RY_CORE_ASSERT(false);
			return false;
		}

		inline bool IsNotLockedFromOutsideAny() const
		{

			if (m_ScopeGuardLock.expired())
			{
				uint64_t scopeThreadID = m_OutSideScopeThreadID.load();
				if (0 != scopeThreadID)
				{
					scopeThreadID = 0u;
					m_OutSideScopeThreadID.store(scopeThreadID);
				}
				return true;
			}
			else
			{
				uint64_t scopeThreadID = m_OutSideScopeThreadID.load();
				uint64_t currentThreadID = std::hash<std::thread::id>{}(std::this_thread::get_id());

				RY_CORE_ASSERT(0 != scopeThreadID, "error no Thread ID set!");
				return scopeThreadID != currentThreadID;
			}

			RY_CORE_ASSERT(false);
			return false;
		}

		inline bool IsNotLockedFromOutsideCarfull() const
		{
			uint32_t threadID = m_OutSideScopeThreadID.load();

			if (m_ScopeGuardLock.expired())
			{
				uint64_t scopeThreadID = m_OutSideScopeThreadID.load();
				if (0 != scopeThreadID)
				{
					scopeThreadID = 0u;
					m_OutSideScopeThreadID.store(scopeThreadID);
				}
				return true;
			}
			else
			{
				uint64_t scopeThreadID = m_OutSideScopeThreadID.load();
				uint64_t currentThreadID = std::hash<std::thread::id>{}(std::this_thread::get_id());

				RY_CORE_ASSERT(0 != scopeThreadID, "error no Thread ID set!");
				return scopeThreadID != currentThreadID;
			}

			RY_CORE_ASSERT(false);
			return false;
		}
	private:
		std::map<K, T> m_AssetMap;
		WeakScopeLock m_ScopeGuardLock;
		mutable std::shared_mutex m_Mutex;
		mutable std::atomic_uint64_t m_OutSideScopeThreadID;

		std::atomic_bool m_OutSideScope;
		bool m_Stop;

	};
	
	
	template<typename K, typename T>
	inline AssetMangerMapMutex<K, T>::~AssetMangerMapMutex()
	{
		std::unique_lock lock(m_Mutex);
		m_Stop = false;
		WriteAction(
			[](std::map<K, T>& map)
			{
				map.clear();
			}
		);
		
	}


}


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

		void Shutdown()
		{
			std::unique_lock lock(m_Mutex);
			m_Stop = true;
			m_AssetMap.clear();
		}

		inline T GetCopy(const K& key) const
		{
			return this->template FindThenRead<T>(key,
				[&key](const std::map<K, T>& map)
				{
					return map.at(key);
				}
			);

		};
		
		inline void GetRefLemda(std::function<void(T&)> func, const K& key)
		{
			FindThenWrite(key,
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

		inline void GetPtr(const K& key, T* value)
		{
			FindThenWrite(key,
				[&key, value](std::map<K, T>& map)
				{
					*value = map.at(key);
				}
			);
		};


		template<typename Func>
		inline void WriteValue(const K& key, Func&& func)
		{
			FindThenWrite(key,
				[&key, func](std::map<K, T>& map)
				{
					T& value = map.at(key);
					func(value);
				}
			);
		}

		/**
		 * The methode calls Write, defines a lambder function that. Get the corresponding value position to the {@param key} and overrides the value withe {@param value}.
		 * @param key find the value in the map.
		 * @param value a protected reference set the value in the map.
		 */
		inline void Set(const K& key, const T& value)
		{
			FindThenWrite(key,
				[&key, &value](std::map<K, T>& map)
				{
					map.at(key) = value;
				}
			);

		};

		/**
		 * The methode calls Write, defines a lambder function that. Can create a new position {@param key} and the position receive the value off {@param value}.
		 * @param key find the value in the map.
		 * @param value a protected reference set the value in the map.
		 */
		inline void Add(const K& key, const T& value)
		{

			Write(
				[&key, &value](std::map<K, T>& map)
				{
					map[key] = value;
				}
			);
		};


		/**
		 * The methode calls Write, defines a lambder function that. Erase the {@param key} and corresponding value and from the map.
		 * @param key find the value in the map.
		 */
		inline void Remove(const K& key)
		{
			FindThenWrite(key,
				[&key](std::map<K, T>& map)
				{
					map.erase(key);
				}
			);
		};



		/**
		 * The methode calls Read withe a lander function to receive the value of the map that matches the {@param key} (no safety check).
		 * After receiving the value it calls the lambder function {@param func}, set the value off the map.
		 * @tparam Func a lambder function that get a argument of type {@T} as param, as a protected reference. Can also have any return value.
		 * @param key argument to the value in the map.
		 * @param func lambda function for that get the a protected reference from the map value.
		 * @return any result you want receive from your own lander.
		 */
		template<typename R, typename Func>
		inline R ReadValue(const K& key, Func&& func) const
		{
			
			return FindThenRead<R>(key,
				[&key, func](const std::map<K, T>& map)
				{
					const T& value = map.at(key);
					return func(value);
				}
			);
		}

		/**
		 * Search for a key, if it is inside off the map or not.
		 * @param key argument to the value in the map.
		 * @return the return value from the Read methode, about the key state.
		 */
		inline bool IsFound(const K& key) const
		{
			return this->template Read<bool>(
				[&key](const std::map<K, T>& map) -> bool
				{
					return map.find(key) != map.end();
				}
			);
		};

		/**
		 * Calls Write methode, with a lambda function that clears the map.
		 */
		inline void Clear()
		{
			Write(
				[](std::map<K, T>& map)
				{
					map.clear();
				}
			);		
		}


		/**
		 * Some read action go throw this methode. The methode lock and unlock the mutex from instance as shared.
		 * Calls the {@param func} lambda function and set the map reference that is protected, from this instance.
		 * Also Controls if the instance is in process off destruction. If that is the case the action can be aborted.
		 * @tparam Func type from the {@param func} lambda function that expect a std::map<K, T> reference that is protected.
		 *				K and T should be the same typs that are used in this instance.
		 * @param func expect a lambder that get read access to the internal map from the instance.
		 * @return if the lamda functions has a return value we return it.
		 */
		template<typename R, typename Func>
		R Read(Func&& func) const
		{
			std::shared_lock lock(m_Mutex);
			RY_CORE_ASSERT(!m_OutSideScope, "Mutex is alrady Set Globle");
			if (m_Stop)
			{
				RY_CORE_ERROR("destructor executed! Abort Read");
				return R{};
			}
			return func(m_AssetMap);
		}

		/**
		 * Some write action go in this methode. The methode lock and unlock the mutex from instance as unique.
		 * Also Controls if the instance is in process off destruction. If that is the case the action can be aborted.
		 * Calls the lambda methode and set the map from this instance as the argument.
		 * @tparam Func type from the {@param} lambda function that expect a std::map<K, T> reference that is not protected.
		 *				K and T should be the same typs that are used in this instance.
		 * @param func expect a lambder that get full write access to the internal map from the instance.
		 */
		template<typename Func>
		void Write(Func&& func)
		{
			std::unique_lock lock(m_Mutex);
			RY_CORE_ASSERT(!m_OutSideScope, "Mutex is already set globe");
			if (m_Stop)
			{
				RY_CORE_ERROR("destructor executed! Abort Write");
				return;
			}
			func(m_AssetMap);
		}

		/**
		 * Some read action go throw this methode. The methode lock and unlock the mutex from instance as shared.
		 * Calls the {@param func} lambda function and set the map reference that is protected, from this instance.
		 * Also Controls if the instance is in process off destruction or the position {@param key} exist. If that is the case the action can be aborted.
		 * @tparam Func type from the {@param func} lambda function that expect a std::map<K, T> reference that is protected.
		 *				K and T should be the same typs that are used in this instance.
		 * @param key expect key value that matches the {@K} of this instance.
		 * @param func expect a lambder that get read access to the internal map from the instance.
		 * @return if the lamda function has a return value we return it.
		 */
		template<typename R, typename Func>
		R FindThenRead(const K& key, Func&& func) const
		{
			if (m_Stop)
			{
				RY_CORE_ERROR("destructor executed! Abort FindThenWrite");
				
				return R{};
			}
			
			std::shared_lock lock(m_Mutex);
			RY_CORE_ASSERT(!m_OutSideScope, "Mutex is already Set Globe");
			if (m_Stop || m_AssetMap.find(key) == m_AssetMap.end())
			{
				RY_CORE_ERROR_IF(m_Stop, "destructor executed! Abort FindThenRead");
				RY_CORE_ERROR_IF(m_AssetMap.find(key) == m_AssetMap.end(), "Key not found! Abort FindThenRead");

				
				return R{};
			}
			return func(m_AssetMap);
		}

		/**
		 * Most write action go in this methode. The methode lock and unlock the mutex from instance as unique.
		 * Also Controls if the instance is in process off or that the key exist. If that is the case the action can be aborted.
		 * Calls the lambda methode and set the map from this instance as the argument.
		 * @tparam Func type from the {@param} lambda function that expect a std::map<K, T> reference that is not protected.
		 *				K and T should be the same typs that are used in this instance.
		 * @param key expect key value that matches the {@K} of this instance.
		 * @param func expect a lambder that get full write access to the internal map from the instance.
		 */
		template<typename Func>
		void FindThenWrite(const K& key, Func&& func)
		{
			if (m_Stop)
			{
				RY_CORE_ERROR("destructor executed! Abort FindThenWrite");
				return;
			}

			std::unique_lock lock(m_Mutex);
			RY_CORE_ASSERT(!m_OutSideScope, "Mutex is already set globe");

			if (m_Stop || m_AssetMap.find(key) == m_AssetMap.end())
			{
				RY_CORE_ERROR_IF(m_Stop, "destructor executed! Abort FindThenWrite");
				RY_CORE_ERROR_IF(m_AssetMap.find(key) == m_AssetMap.end(), "Key not found! Abort FindThenWrite");
				return;
			}
			func(m_AssetMap);
		}

		
	private:

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
		RY_CORE_ASSERT(m_Stop, "Destructor called without prior Shutdown()! Ensure all worker threads are joined before destroying AssetMangerMapMutex.");
	}


}


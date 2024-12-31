#pragma once

namespace Rynex {


	template<typename K, typename T>
	class AssetMangerMapMutex
	{
	public:
		AssetMangerMapMutex()
		{
			m_OutSideScope = false;
		};

		~AssetMangerMapMutex()
		{
			RY_CORE_ASSERT(!m_OutSideScope, "Mutex is alrady Set Globle");
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_AssetMap.empty();
		};

		inline T GetCopy(const K& key) const
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			return m_AssetMap.at(key);
		};

		inline const T GetConstCopy(const K& key) const
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			return m_AssetMap.at(key);
		};
		
		inline void GetRefLemda(std::function<void(T&)> func, const K& key)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			func(m_AssetMap.at(key));
		};

		inline void GetPtrLemda(std::function<void(T*)> func, const K& key)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			func(&m_AssetMap.at(key));
		};

		// Set Globle Mutex!
		inline T& GetRef(const K& key)
		{
			RY_CORE_ASSERT(m_OutSideScope , "Mutex is not Set Outside!");
			if (m_OutSideScope)
				return m_AssetMap.at(key);
			return T();
		};


		inline std::map<K, T>* GetMapPtr()
		{
			RY_CORE_ASSERT(m_OutSideScope, "Mutex is not Set Outside!");
			if (m_OutSideScope)
				return &m_AssetMap;
			return nullptr;
		}

		// Set Globle Mutex!
		inline void GetPtr(const K& key, T* value)
		{
			RY_CORE_ASSERT(m_OutSideScope, "Mutex is not Set Outside!");
			if (m_OutSideScope )
				*value = m_AssetMap.at(key);
			
		};

		// Set The Function Before you want use Value Ptr or Ref!
		inline void GlobleMutexBegin()
		{
			RY_CORE_ASSERT(!m_OutSideScope, "Mutex is alrady Set Globle!");
			if (!m_OutSideScope)
			{
				m_Mutex.lock();
				m_OutSideScope = true;
			}
		};

		// Set The Function After you want use Value Ptr or Ref!
		inline void GlobleMutexEnde()
		{
			RY_CORE_ASSERT(m_OutSideScope, "Mutex is not Set Globle!");
			if (m_OutSideScope )
			{
				m_Mutex.unlock();
				m_OutSideScope = false;
			}
		};

		inline void Set(const K& key, const T& value)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_AssetMap.at(key) = value;
		};

		inline void Add(const K& key, const T& value)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_AssetMap[key] = value;
		};

		inline void Remove(const K& key)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_AssetMap.erase(key);
		};

		inline void Change(const K& key, const T& value)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			m_AssetMap.at(key) = value;
		};

		inline bool IsFound(const K& key) const
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			return m_AssetMap.find(key) != m_AssetMap.end();
		};

		
	private:
		bool m_OutSideScope;
		std::map<K, T> m_AssetMap;
		mutable std::mutex m_Mutex;
	};
	
	
}


#pragma once
#include <rypch.h>

namespace Rynex {

	template<typename _Key1, typename _Key2, typename T>
	class UnorderDoubleMap
	{
	public:
		using StoreValue = std::pair<T, uint32_t>;
		using FirstMapType = std::unordered_map<_Key1, StoreValue>;
		using SecondMapType = std::unordered_map<_Key2, _Key1>;

		constexpr UnorderDoubleMap()
			: m_FirstMap()
			, m_SecoundMap()
		{
		};

		~UnorderDoubleMap()
		{
			Clear();
		};

		bool HasKey1(const _Key1& key) const { return m_FirstMap.find(key) != m_FirstMap.end(); };
		bool HasKey2(const _Key2& key) const { return m_SecoundMap.find(key) != m_SecoundMap.end(); };

		const StoreValue& GetKey1(const _Key1& key) const { return m_FirstMap.at(key); };
		StoreValue& GetKey1(const _Key1& key) { return m_FirstMap.at(key); };

		const StoreValue& GetKey2(const _Key2& key) const
		{
			const _Key1& secoundKey = m_SecoundMap.at(key);
			return GetKey1(secoundKey);
		};

		StoreValue& GetKey2(const _Key2& key)
		{
			const _Key1& secoundKey = GetKey(key);
			return GetKey1(secoundKey);
		};

		StoreValue& Inserte(const _Key1& key1, const _Key2& key2)
		{
			m_SecoundMap[key2] = key1;
			if (HasKey2(key1))
			{
				StoreValue& dataPair = m_FirstMap.at(key1);
				dataPair.second++;
				return dataPair;
			}
			else
			{
				StoreValue& dataPair = m_FirstMap[key1];
				dataPair.second = 1u;
				return dataPair;
			}
			
		};

		uint32_t GetCount(const _Key1& key) const { uint32_t count = Get(key).second; return count; };
		const _Key1& GetKey2Key1(const _Key2& key2) const { return m_SecoundMap.at(key2); };
		
		void Erase(const _Key2& key2)
		{
			const _Key1& key1 = GetKey(key2);
			StoreValue& dataPair = Get(key1);
			dataPair.second--;
			m_SecoundMap.erase(key2);
			if (dataPair.second == 0)
				m_FirstMap.erase(key1);

		};

		void Clear()
		{
			m_FirstMap.clear();
			m_SecoundMap.clear();
		}

		typename FirstMapType::iterator begin() { return m_FirstMap.begin(); }
		typename FirstMapType::iterator end() { return m_FirstMap.end(); }

		typename FirstMapType::const_iterator  begin() const { return m_FirstMap.begin(); }
		typename FirstMapType::const_iterator  end() const { return m_FirstMap.end(); }
	private:
		FirstMapType m_FirstMap;
		SecondMapType m_SecoundMap;
	};


	


}

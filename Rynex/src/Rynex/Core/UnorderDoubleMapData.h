#pragma once
#include <rypch.h>

#define RY_UNORDERED_DOUBLE_MAP_DATA_SAFE_CHECKS 0
#define RY_UNORDERED_DOUBLE_MAP_DATA_NOT_STRATE 1
namespace Rynex {

	template<typename _Key1, typename _Key2>
	class UnorderDoubleMapData
	{
	public:
		

		struct DataInfo2
		{
			uint32_t Index;
			uint32_t Count;
			_Key1 Parent;
			
		};

		struct DataInfo1
		{
			uint32_t BeginIndex;
			uint32_t EndeIndex;
			uint32_t ThisIndex;
			std::vector<typename _Key2> ChilderenKey;
		};
		
		using FirstMapType = typename std::unordered_map<typename _Key1, typename DataInfo1>;
		using SecondMapType = typename std::unordered_map<typename _Key2, typename DataInfo2>;
		using FirstPair = typename FirstMapType::value_type;
		using SecondPair = typename SecondMapType::value_type;

		constexpr UnorderDoubleMapData()
			: m_FirstMap()
			, m_SecoundMap()
			, m_OnClearFunc(nullptr)
			, m_OnEraseInfo1Func(nullptr)
			, m_OnEraseInfo2Func(nullptr)
			, m_OnInserteInfo1Func(nullptr)
			, m_OnInserteInfo2Func(nullptr)
		{
		};

		constexpr UnorderDoubleMapData(
			std::function<void()> onClearFunc

			, std::function<void(const DataInfo1&)> onEraseRangeChangeInfo1Func
			, std::function<void(const DataInfo2&, const _Key2&)> onEraseIndexChangeInfo2Func
			, std::function<void(const DataInfo2&)> onEraseDecreseCountInfo2Func
			, std::function<void(const DataInfo1&)> onEraseRemoveInfo1Func
			, std::function<void(const DataInfo2&, const _Key2&)> onEraseRemoveInfo2Func

			, std::function<void(const DataInfo1&)> onInserteCreateInfo1Func
			, std::function<void(const DataInfo2&, const _Key2&)> onInserteCreateInfo2Func
			, std::function<void(const DataInfo1&)> onInserteRangeChangeInfo1Func
			, std::function<void(const DataInfo2&)> onInserteIndexChangeInfo2Func
			, std::function<void(const DataInfo2&)> onInserteIncresedCountInfo2Func
			
		)
			: m_FirstMap()
			, m_SecoundMap()
			, m_OnClearFunc(onClearFunc)

			, m_OnEraseRangeChangeInfo1Func(onEraseRangeChangeInfo1Func)
			, m_OnEraseIndexChangeInfo2Func(onEraseIndexChangeInfo2Func)
			, m_OnEraseDecreseCountInfo2Func(onEraseDecreseCountInfo2Func)
			, m_OnEraseRemoveInfo1Func(onEraseRemoveInfo1Func)
			, m_OnEraseRemoveInfo2Func(onEraseRemoveInfo2Func)

			, m_OnInserteCreateInfo1Func(onInserteCreateInfo1Func)
			, m_OnInserteCreateInfo2Func(onInserteCreateInfo2Func)
			, m_OnInserteRangeChangeInfo1Func(onInserteRangeChangeInfo1Func)
			, m_OnInserteIndexChangeInfo2Func(onInserteIndexChangeInfo2Func)
			, m_OnInserteIncresedCountInfo2Func(onInserteIncresedCountInfo2Func)
			
		{
		};

		~UnorderDoubleMapData()
		{
			Clear();
		};

		bool HasKey1(const _Key1& key) const { return m_FirstMap.find(key) != m_FirstMap.end(); };
		bool HasKey2(const _Key2& key) const { return m_SecoundMap.find(key) != m_SecoundMap.end(); };

		const DataInfo1& GetInfo1(const _Key1& key1) const 
		{ 
			const DataInfo1& info1 = m_FirstMap.at(key1);
			return info1;
		};

		DataInfo1& GetInfo1(const _Key1& key1)
		{
			DataInfo1& info1 = m_FirstMap.at(key1);
			return info1;
		};

		const DataInfo2& GetInfo2(const _Key2& key2) const
		{
			const DataInfo2& info2 = m_SecoundMap.at(key2);
			return info2;
		};

		DataInfo2& GetInfo2(const _Key2& key2)
		{
			DataInfo2& info2 = m_SecoundMap.at(key2);
			return info2;
		};

		DataInfo2& Inserte(const _Key1& key1, const _Key2& key2)
		{
			RY_PROFILE_FUNCTION()
			bool hasKey1 = HasKey1(key1);
			bool hasKey2 = HasKey2(key2);
			if (hasKey1 && !hasKey2)
			{
				
				DataInfo1& info1 = GetInfo1(key1);
				std::vector<typename _Key2>& child = info1.ChilderenKey;
				child.emplace_back(key2);
				const uint32_t end = info1.EndeIndex;
				info1.EndeIndex++;

				if (m_OnInserteRangeChangeInfo1Func)
					m_OnInserteRangeChangeInfo1Func(info1);

				AddIncreaseAllInfo1Range(end);
				AddIncreaseAllInfo2Index(end);

				DataInfo2& info2 = AddCreateInfo2(key1, key2, end);
				return info2;
			}
			else if(!hasKey1 && !hasKey2)
			{
				uint32_t size = m_SecoundMap.size();
				AddCreateInfo1(key1, key2, size);
				DataInfo2& info2 = AddCreateInfo2(key1, key2, size);
				return info2;
			}
			else if(!hasKey1 && hasKey2)
			{
				RY_CORE_ASSERT(false, "The Same Key Used on Some Parent!")
			}
			else if (hasKey1 && hasKey2)
			{
				DataInfo2& info2 = GetInfo2(key2);
				RY_CORE_ASSERT(info2.Parent == key1, "The Same Key Used on Another Parent!");
				info2.Count++;
				if (m_OnInserteIncresedCountInfo2Func)
					m_OnInserteIncresedCountInfo2Func(info2);
				return info2;
			}
			else
			{
				RY_CORE_ASSERT(false, "Not Expexted Error!");
			}
			return DataInfo2{ static_cast<uint32_t>(-1), 0, _Key1() };
		};

		uint32_t GetSize(const _Key1& key1) const
		{  
			const DataInfo1& info1 = GetInfo1(key1);
			const std::vector<typename _Key2>& data = info1.ChilderenKey;
			uint32_t size = data.size();
			return size;
		};

		uint32_t GetCount(const _Key2& key2) const
		{
			const DataInfo2& info2 = GetInfo2(key2);
			uint32_t count = info2.Count;
			return count;
		};

		const _Key1& GetKey1(const _Key2& key2) const 
		{ 
			const DataInfo2& info2 = GetInfo2(key2);
			const _Key1& key1 = info2.Parent;
			return key1;
		};

		const DataInfo2& GetInfo2Index(const _Key1& key1, uint32_t index) const
		{
			const DataInfo1& info1 = GetInfo1(key1);
			const std::vector<typename _Key2>& data = info1.ChilderenKey;
			const _Key2& key2 = data.at(index);
			const DataInfo2& info2 = GetInfo2(key2);
			return info2;
		};

		DataInfo2 GetInfo2Index(const _Key1& key1, uint32_t index)
		{
			const DataInfo1& info1 = GetInfo1(key1);
			const std::vector<typename _Key2>& data = info1.ChilderenKey;
			const _Key2& key2 = data.at(index);
			DataInfo2 info2 = GetInfo2(key2);
			return info2;
		};
		
		uint32_t GetStoreIndexFromKey1Index(const _Key1& key1, uint32_t index)
		{
			const DataInfo2& info2 = GetInfo2Index(key1, index);
			uint32_t indexStore = info2.Index;
			return indexStore;
		};

		uint32_t GetStoreIndex(const _Key2& key2)
		{
			const DataInfo2& info2 = GetInfo2(key2);
			uint32_t index = info2.Index;
			return index;
		};

		void Erase(const _Key2& key2)
		{
			RY_PROFILE_FUNCTION();
			if (!HasKey2(key2))
			{
				RY_CORE_ERROR("ID Not Found Abourt Erase!");
				return;
			}

			DataInfo2& info2 = GetInfo2(key2);
			const _Key1& key1 = info2.Parent;
			bool haskey1 = HasKey1(key1);
			RY_CORE_ASSERT(haskey1, "Parent Is Not longerer Existing!");
			if(!haskey1)
				return;

			info2.Count--;
			if (info2.Count != 0)
			{
				if (m_OnEraseDecreseCountInfo2Func)
					m_OnEraseDecreseCountInfo2Func(info2);

				return;
			}

			DataInfo1& info1 = GetInfo1(key1);
#if RY_UNORDERED_DOUBLE_MAP_DATA_SAFE_CHECKS
			const DataInfo1 info1Copy = info1;
#endif
			RemoveKey2Info1(key1, key2, info1);
			const uint32_t end = info2.Index;
			const uint32_t index1 = info1.ThisIndex;
			RemoveFromMap(info1, info2, key1,  key2);
			
			RemoveDecreaseAllInfo2Index(end);
			RemoveDecreaseAllInfo1Range(end, index1);
#if RY_UNORDERED_DOUBLE_MAP_DATA_SAFE_CHECKS
			if(info1Copy.ChilderenKey.size() != 1)
			{
				RY_CORE_INFO("Remove One Element Index: {} / Parent Range: {} - {}", end, info1Copy.BeginIndex, info1Copy.EndeIndex);
				RY_CORE_INFO("New Parent Range: {} - {}", info1.BeginIndex, info1.EndeIndex);
			}
			else
			{
				RY_CORE_INFO("Remove One Element Index: {} + Parent Element", end);
			}
#endif
		};

		static uint32_t GetArrayIndexFromVector(const std::vector<_Key2>& vec, const _Key2& target)
		{
			uint32_t index = 0;
			for (const _Key2& id : vec)
			{
				if (id == target)
					break;
				index++;
			}
			uint32_t size = vec.size();
			RY_CORE_ASSERT(index < size, "Not Found In Array!");

			return index;
		}

		uint32_t GetLoclaeParentIndex(const _Key1& key1, const _Key2& key2) const
		{
			const DataInfo1& info1 = this->GetInfo1(key1);
			const DataInfo2& info2 = this->GetInfo2(key2);
			const _Key1& parent = info2.Parent;
			RY_CORE_ASSERT(parent == key1);
			uint32_t loclaeIndex = GetLoclaeParentIndex(key1, key2, info1, info2);
			return loclaeIndex;
		}
		
		uint32_t GetLoclaeParentIndex(const _Key1& key1, const _Key2& key2, const DataInfo1& info1, const DataInfo2& info2) const
		{
			
			uint32_t beginParentChild = info1.BeginIndex;
			uint32_t endParentChild = info1.EndeIndex;
			
#if RY_UNORDERED_DOUBLE_MAP_DATA_NOT_STRATE
			uint32_t localeParenChildSize = endParentChild - beginParentChild;
#if RY_UNORDERED_DOUBLE_MAP_DATA_SAFE_CHECKS
			RY_CORE_INFO("Parent Range: {} - {} / Guss Loclae Size: {}", beginParentChild, endParentChild, localeParenChildSize);
#endif
			uint32_t localeIndex = (info2.Index - beginParentChild);
			uint32_t localeIndex2 = localeIndex + 1;
			uint32_t resultIndex = 0;
			
			const std::vector<_Key2>& childVec = info1.ChilderenKey;
			
			bool isIndex = IsKey2InVecIndexTarget(childVec, key2, localeIndex);
			bool isIndex2 = IsKey2InVecIndexTarget(childVec, key2, localeIndex2);
			if (!isIndex && !isIndex2)
			{
				uint32_t localeIndexFind = GetArrayIndexFromVector(childVec, key2);
				RY_CORE_WARN("Parent Index: (Found/IndexBefor)({} / [{}, {}]", localeIndexFind, localeIndex, localeIndex2);
				resultIndex = localeIndexFind;
			}
			else if (isIndex)
			{
				resultIndex = localeIndex;
				RY_CORE_INFO("Index {} was Korect!", localeIndex);
			}
			else if (isIndex2)
			{
				resultIndex = localeIndex2;
				RY_CORE_INFO("Index2 {} was Korect!", localeIndex2);
			}
			else
			{
				RY_CORE_ASSERT(false);
			}
#else
			uint32_t resultIndex = endParentChild - beginParentChild;
#endif
			return resultIndex;
		}

		void Clear()
		{
			if(m_OnClearFunc)
				m_OnClearFunc();

			m_FirstMap.clear();
			m_SecoundMap.clear();
		}

		

		typename FirstMapType::iterator begin() { return m_FirstMap.begin(); }
		typename FirstMapType::iterator end() { return m_FirstMap.end(); }

		typename FirstMapType::const_iterator begin() const { return m_FirstMap.begin(); }
		typename FirstMapType::const_iterator end() const { return m_FirstMap.end(); }

	private:
		

		static bool IsKey2InVecIndexTarget(const std::vector<_Key2>& vec, const _Key2& key2, uint32_t index)
		{
			uint32_t size = vec.size();
			if (index < size)
			{
				const _Key2& keyChild = vec.at(index);
				return keyChild == key2;
			}
			RY_CORE_WARN("Out Side Off Vec Range!");
			return false;
		}

		

		void AddCreateInfo1(const _Key1& key1, const _Key2& key2, const uint32_t firstIndex)
		{
			uint32_t thisIndex = m_FirstMap.size();
			DataInfo1& info1 = m_FirstMap[key1];
			info1.BeginIndex = firstIndex;
			info1.EndeIndex = firstIndex + 1;
			info1.ThisIndex = thisIndex;
			std::vector<typename _Key2>& child = info1.ChilderenKey;
			child.push_back(key2);

			if (m_OnInserteCreateInfo1Func)
				m_OnInserteCreateInfo1Func(info1);
		}

		DataInfo2& AddCreateInfo2(const _Key1& key1, const _Key2& key2, const uint32_t index)
		{
			DataInfo2& info2 = m_SecoundMap[key2];
			info2.Index = index;
			info2.Parent = key1;
			info2.Count = 1;
			if (m_OnInserteCreateInfo2Func)
				m_OnInserteCreateInfo2Func(info2, key2);
			
			
			return info2;
		}

		void AddIncreaseAllInfo1Range(const uint32_t end)
		{
			for (SecondPair& pairSecound : m_SecoundMap)
			{
				DataInfo2& infoSecound = pairSecound.second;
		
				if (infoSecound.Index >= end)
				{
					infoSecound.Index++;
					if (m_OnInserteIndexChangeInfo2Func)
						m_OnInserteIndexChangeInfo2Func(infoSecound);
				}
			}
		}

		void AddIncreaseAllInfo2Index(const uint32_t end)
		{
			for (FirstPair& pairFirst : m_FirstMap)
			{
				DataInfo1& infoFirst = pairFirst.second;
				if (infoFirst.BeginIndex >= end)
				{
					infoFirst.BeginIndex++;
					infoFirst.EndeIndex++;
					if (m_OnInserteRangeChangeInfo1Func)
						m_OnInserteRangeChangeInfo1Func(infoFirst);
				}
			}
		}

		void RemoveKey2Info1(const _Key1& key1, const _Key2& key2, DataInfo1& info1)
		{
			
			uint32_t localeIndex = GetLoclaeParentIndex(key1, key2);
			std::vector<typename _Key2>& child = info1.ChilderenKey;
			child.erase(child.begin() + localeIndex);
			info1.EndeIndex--;
		}

		static bool CompareFunktionHigest(uint32_t compareIndex, uint32_t index)
		{
			return compareIndex < index;
		}

		static bool CompareFunktionLower(uint32_t compareIndex, uint32_t index)
		{
			return compareIndex > index;
		}

		
		glm::uvec2 GetVecCompareIndex(const std::vector<typename _Key2>& vec,std::function<bool(uint32_t, uint32_t)> func)
		{
			uint32_t compareIndex = func(1, 2) ? 0 : static_cast<uint32_t>(-1);
			uint32_t localeCompareIndex = compareIndex;
			uint32_t i = 0;
			for (const _Key2& key2 : vec)
			{
				const DataInfo2& info2 = GetInfo2(key2);
				uint32_t index = info2.Index;
				if (func(compareIndex, index))
				{
					compareIndex = index;
					localeCompareIndex = i;
				}
				i++;
			}
			return glm::uvec2{ compareIndex, localeCompareIndex };
		}

		void SetHigestChildrenIndex(DataInfo1& info1)
		{
			const std::vector<typename _Key2>& child = info1.ChilderenKey;
			glm::uvec2 higestIndex = GetVecCompareIndex(child, CompareFunktionHigest);
			uint32_t& endIndex = info1.EndeIndex;
			higestIndex.x += 1;
			if(endIndex != higestIndex.x)
			{
				RY_CORE_WARN("Higest Index Was {}, befor it was {} / on locle Index {}", higestIndex.x, endIndex,higestIndex.y);
				endIndex = higestIndex.x;
			}
		}

		void SetLowestChildrenIndex(DataInfo1& info1)
		{
			const std::vector<typename _Key2>& child = info1.ChilderenKey;
			glm::uvec2 higestIndex = GetVecCompareIndex(child, CompareFunktionLower);
			uint32_t& beginIndex = info1.BeginIndex;
			if (beginIndex != higestIndex.x)
			{
				RY_CORE_WARN("Loweste Index Was {}, befor it was {} / on locle Index {}", higestIndex.x, beginIndex,higestIndex.y);
				beginIndex = higestIndex.x;
			}
		}

		void SetRangeChildrenIndex(DataInfo1& info1)
		{
			SetHigestChildrenIndex(info1);
			SetLowestChildrenIndex(info1);
		}

		void RemoveDecreaseAllInfo1Range(const uint32_t end, const uint32_t index1)
		{
			for (FirstPair& pairFirst : m_FirstMap)
			{
				DataInfo1& infoFirst = pairFirst.second;
				if (infoFirst.BeginIndex != 0 && infoFirst.BeginIndex > end)
				{
					RY_CORE_ASSERT(infoFirst.BeginIndex != 0 || infoFirst.EndeIndex != 0);
					infoFirst.BeginIndex--;
					infoFirst.EndeIndex--;
					if(infoFirst.ThisIndex != 0 && index1 < infoFirst.ThisIndex)
						infoFirst.ThisIndex--;
#if RY_UNORDERED_DOUBLE_MAP_DATA_SAFE_CHECKS
					SetRangeChildrenIndex(infoFirst);
#endif
					if (m_OnEraseRangeChangeInfo1Func)
						m_OnEraseRangeChangeInfo1Func(infoFirst);
				}
			}
		}

		void RemoveDecreaseAllInfo2Index(const uint32_t end)
		{
			
			for (SecondPair& pairSecound : m_SecoundMap)
			{
				DataInfo2& infoSecound = pairSecound.second;
				
				if (infoSecound.Index != 0 && infoSecound.Index >= end)
				{
					infoSecound.Index--;
					
					const _Key2& key2 = pairSecound.first;
					if (m_OnEraseIndexChangeInfo2Func)
						m_OnEraseIndexChangeInfo2Func(infoSecound, key2);
				}
			}
		}

		void RemoveFromMap(DataInfo1& info1,DataInfo2& info2, const _Key1& key1, const _Key2& key2)
		{
			_Key1 key1Copy = key1;
			if (m_OnEraseRemoveInfo2Func)
				m_OnEraseRemoveInfo2Func(info2, key2);
			m_SecoundMap.erase(key2);

			std::vector<typename _Key2>& child = info1.ChilderenKey;
			if (child.empty())
			{
				if (m_OnEraseRemoveInfo1Func)
					m_OnEraseRemoveInfo1Func(info1);

				m_FirstMap.erase(key1Copy);
			}
		}

	
	private:
		FirstMapType m_FirstMap;
		SecondMapType m_SecoundMap;


		std::function<void(const DataInfo1&)> m_OnEraseRemoveInfo1Func;
		std::function<void(const DataInfo2&, const _Key2&)> m_OnEraseRemoveInfo2Func;
		std::function<void(const DataInfo1&)> m_OnEraseRangeChangeInfo1Func;
		std::function<void(const DataInfo2&, const _Key2&)> m_OnEraseIndexChangeInfo2Func;
		std::function<void(const DataInfo2&)> m_OnEraseDecreseCountInfo2Func;

		std::function<void(const DataInfo1&)> m_OnInserteCreateInfo1Func;
		std::function<void(const DataInfo2&, const _Key2&)> m_OnInserteCreateInfo2Func;
		std::function<void(const DataInfo1&)> m_OnInserteRangeChangeInfo1Func;
		std::function<void(const DataInfo2&)> m_OnInserteIndexChangeInfo2Func;
		std::function<void(const DataInfo2&)> m_OnInserteIncresedCountInfo2Func;
		

		std::function<void()> m_OnClearFunc;


	};



}

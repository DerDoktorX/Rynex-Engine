#pragma once
#include <rypch.h>

namespace Rynex {
	namespace Memory {

		template<typename T>
		class StaticIndexStoreVec
		{
		public:
			using ValueType = typename T;
			// using ValueType = int;

			using ContainerType = typename std::vector<typename ValueType>;
			using SizeType = typename ContainerType::size_type;
			using DifferenceType = typename ContainerType::difference_type;
		public:
			StaticIndexStoreVec() = default;
			StaticIndexStoreVec(const StaticIndexStoreVec&) = default;
			StaticIndexStoreVec(StaticIndexStoreVec&&) = default;

			~StaticIndexStoreVec()
			{
				Clear();
			}

			SizeType Add(const ValueType& data)
			{
				return Insert(data);
			}

			void Remove(SizeType index)
			{
				RY_CORE_ASSERT(GetIndexInFreeListFromStoreIndex(index) == std::numeric_limits<SizeType>::max());
				
				AddFreeList(index);
			}

			void Clear()
			{
				m_FreeListVec.clear();
				m_IndecStoreVec.clear();
			}

			SizeType StoreElementCount() const
			{
				RY_CORE_ASSERT(m_FreeListVec.size() <= m_IndecStoreVec.size(), "We expexted m_FreeListVec always to be smaler or eqaul with m_IndecStoreVec if equals then this struct has no elements!");
				SizeType diff = m_IndecStoreVec.size() - m_FreeListVec.size();
				return diff;
			}
			
			bool StoreElementEmpty() const
			{
				return StoreElementCount() != 0;
			}

			ValueType& At(SizeType index)
			{
				RY_CORE_ASSERT(IsValidIndex(index), "not vaild Index");
				return m_IndecStoreVec.at(index);
			}

			const ValueType& At(SizeType index) const
			{
				RY_CORE_ASSERT(IsValidIndex(index), "not vaild Index");
				return m_IndecStoreVec.at(index);
			}


			// expexted for the func are 2 inputs, first is your passed value, then then the value to check and need to return bool
			template<typename N, typename Func>
			SizeType FindeDataIndex(const N& valueA, const Func& func, SizeType index = 0) const
			{
				SizeType nextFreeElement = m_FreeListVec.empty() ? std::numeric_limits<SizeType>::max() : m_FreeListVec.front();
				const SizeType freeListCount = m_FreeListVec.size();
				SizeType freeListIndex = 0;
				while (nextFreeElement <= index)
				{
					freeListIndex++;
					if (freeListIndex < freeListCount)
					{
						nextFreeElement = m_FreeListVec.at(freeListIndex);
					}
					else
					{
						nextFreeElement = std::numeric_limits<SizeType>::max();
					}
				}

				const SizeType count = m_IndecStoreVec.size();
				while (index < count)
				{
					const ValueType& valueB = m_IndecStoreVec.at(index);
					if (nextFreeElement == index)
					{
						freeListIndex++;
						if (freeListIndex < freeListCount)
						{
							nextFreeElement = m_FreeListVec.at(freeListIndex);
						}
						else
						{
							nextFreeElement = std::numeric_limits<SizeType>::max();
						}
					}
					else if (func(valueA, valueB))
					{
						return index;
					}
					index++;
				}
				RY_CORE_TRACE("Element Not Found! in Vaild Positions");
				return std::numeric_limits<SizeType>::max();
			}


			SizeType FreeListCount() const
			{
				return m_FreeListVec.size();
			}
			SizeType EndValidIndex() const
			{
				return m_IndecStoreVec.size();
			}

			bool IsValidIndex(SizeType index) const
			{
				return index < m_IndecStoreVec.size() && !IsStoredIndexFreed(index);
			}

			void ReStoreData()
			{
				RY_CORE_WARN("We rebulid this StaticIndecStoreVec, wich means all inices befor are not longer valid!");
				using It = std::vector<ValueType>::iterator;

				for (DifferenceType i = m_FreeListVec.size() - 1; 0 <= i; i--)
				{
					SizeType freeIndex = m_FreeListVec.at(i);
					It pos = m_IndecStoreVec.begin() + freeIndex;
					m_IndecStoreVec.erase(pos);
				}
			}

			void Foreach(const std::function<void(ValueType& data, SizeType index)>& func)
			{
				const SizeType count = m_IndecStoreVec.size();
				SizeType index = 0;
				const SizeType freedCount = m_FreeListVec.size();
				SizeType freedIndex = 0;
				SizeType nextFreedIndex = GetNextFreedIndexFormStoreIndex(index, freedIndex);;
				while (index < count)
				{
					RY_CORE_ASSERT(index <= nextFreedIndex);
					if (nextFreedIndex == index)
					{
						freedIndex = GetNextFreedIndexFormStoreIndex(index, freedIndex);
						nextFreedIndex = freedIndex < freedCount ? m_FreeListVec.at(freedIndex) : std::numeric_limits<SizeType>::max();
					}
					else
					{
						ValueType& data = m_IndecStoreVec.at(index);
						func(data, index);
					}
					index++;
				}
			}

			void Foreach(const std::function<void(const ValueType& data, SizeType index)>& func) const
			{
				const SizeType count = m_IndecStoreVec.size();
				SizeType index = 0;
				const SizeType freedCount = m_FreeListVec.size();
				SizeType freedIndex = 0;
				SizeType nextFreedIndex = GetNextFreedIndexFormStoreIndex(index, freedIndex);;
				while (index < count)
				{
					RY_CORE_ASSERT(index <= nextFreedIndex);
					if(nextFreedIndex == index)
					{
						freedIndex = GetNextFreedIndexFormStoreIndex(index, freedIndex);
						nextFreedIndex = freedIndex < freedCount ? m_FreeListVec.at(freedIndex) : std::numeric_limits<SizeType>::max();
					}
					else
					{
						const ValueType& data = m_IndecStoreVec.at(index);
						func(data, index);
					}
					index++;
				}
			}
			const std::vector<ValueType>& GetDataVec() const { return m_IndecStoreVec; }
		private:
			SizeType Insert(const ValueType& data)
			{
				const SizeType count = m_IndecStoreVec.size();
				SizeType index;
				if (m_FreeListVec.empty())
				{
					index = count;
					m_IndecStoreVec.emplace_back(data);
				}
				else
				{
					index = GetIndexFromFreeList();
					m_IndecStoreVec.at(index) = data;
				}
				return index;
			}
			
			SizeType GetIndexFromFreeList()
			{
				RY_CORE_ASSERT(!m_FreeListVec.empty(), "Free list dont have any size!");
				SizeType freeIndex = m_FreeListVec.front();
				m_FreeListVec.erase(m_FreeListVec.begin());
				return freeIndex;
			}

			SizeType GetNextFreedIndex(SizeType index) const
			{
				using It = std::vector<SizeType>::const_iterator;

				It pos = std::lower_bound(m_FreeListVec.begin(), m_FreeListVec.end(), index);
				if(pos != m_FreeListVec.end())
				{
					SizeType nextFreeIndex = *pos._Ptr;
					return nextFreeIndex;
				}
				return std::numeric_limits<SizeType>::max();
			}

			SizeType GetNextFreedIndexFormStoreIndex(SizeType index, SizeType startFind) const
			{
				using It = std::vector<SizeType>::const_iterator;
				if (startFind == std::numeric_limits<SizeType>::max())
					return std::numeric_limits<SizeType>::max();

				const It begin = m_FreeListVec.begin() + startFind;
				const It end = m_FreeListVec.end();
				if(end <= begin)
					return std::numeric_limits<SizeType>::max();

				const It pos = std::lower_bound(begin, end, index);
				if (pos != m_FreeListVec.end())
				{
					DifferenceType nextFreeIndex = m_FreeListVec.begin() - pos;
					return static_cast<SizeType>(nextFreeIndex);
				}
				return std::numeric_limits<SizeType>::max();
			}

			bool IsStoredIndexFreed(SizeType indexStored) const
			{
				return std::binary_search(m_FreeListVec.begin(), m_FreeListVec.end(), indexStored);
			}

			void AddFreeList(SizeType indexFree)
			{
				using SortedIt = std::vector<SizeType>::iterator;

				SortedIt itPos = std::lower_bound(m_FreeListVec.begin(), m_FreeListVec.end(), indexFree,
					[](SizeType indexA, SizeType freeIndexB)
					{
						return indexA < freeIndexB;
					});

				m_FreeListVec.insert(itPos, indexFree);
			}

			SizeType GetIndexInFreeListFromStoreIndex(SizeType storeIndex)
			{
				SizeType index = 0;
				for (SizeType freeIndex : m_FreeListVec)
				{
					if (storeIndex == freeIndex)
						return index;
					index++;
				}
				return std::numeric_limits<SizeType>::max();
			}
		private:
			ContainerType m_IndecStoreVec;
			std::vector<SizeType> m_FreeListVec;
		};

	}
}
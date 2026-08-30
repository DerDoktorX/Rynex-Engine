#pragma once
#include <rypch.h>
namespace Rynex {
	namespace Memory {

		template<typename T>
		class StoreSubmite
		{
		public:
			using Iterator = typename std::vector<T>::iterator;
			using ConstIterator = typename std::vector<T>::const_iterator;
			using DifferenceType = typename std::vector<T>::difference_type; // type for using store ptr as numbers
			using SizeType = typename std::vector<T>::size_type;

		public:
			StoreSubmite()
				: m_FirstPtr(nullptr)
				, m_LastPtr(nullptr)
				, m_EndPtr(nullptr)
				, m_DataVec()
			{
			};
#if 0
			StoreSubmite(StoreSubmite<T>&& store) noexcept
				: m_FirstPtr(nullptr)
				, m_LastPtr(nullptr)
				, m_EndPtr(nullptr)
				, m_DataVec(std::move(store.m_DataVec))
			{
				DifferenceType diff = store.GetAllocCount();
				DifferenceType pos = store.GetCurentCount();
				m_FirstPtr = m_DataVec.data();
				m_LastPtr = m_FirstPtr + diff;
				m_EndPtr = m_FirstPtr + pos;

				RY_CORE_ASSERT(m_FirstPtr == store.m_FirstPtr, "I expexted to get the same ptr adress as in the origenel data Struct!");
				RY_CORE_ASSERT(m_LastPtr == store.m_LastPtr, "I expexted to get the same ptr adress as in the origenel data Struct!");
				RY_CORE_ASSERT(m_EndPtr == store.m_EndPtr, "I expexted to get the same ptr adress as in the origenel data Struct!");
				store.m_FirstPtr = nullptr;
				store.m_LastPtr = nullptr;
				store.m_EndPtr = nullptr;

			};
#elif 1
			StoreSubmite(const StoreSubmite<T>& store)
				: m_FirstPtr(nullptr)
				, m_LastPtr(nullptr)
				, m_EndPtr(nullptr)
				, m_DataVec(store.m_DataVec)
			{
				SizeType diff = store.m_DataVec.size();
				DifferenceType pos = store.GetCurentCount();
				m_FirstPtr = m_DataVec.data();
				m_LastPtr = m_FirstPtr + diff;
				m_EndPtr = m_FirstPtr + pos;
			};
#endif
			~StoreSubmite() = default;

			void Create(SizeType count)
			{
				m_FirstPtr = nullptr;
				m_LastPtr = nullptr;
				m_EndPtr = nullptr;
				Resize2D(count);
			}

			void Destroy()
			{
				m_FirstPtr = nullptr;
				m_LastPtr = nullptr;
				m_EndPtr = nullptr;
				m_DataVec.clear();
				m_DataVec.shrink_to_fit();
			}

			void Resize2D(SizeType count)
			{
				DifferenceType offsetValue = GetCurentCount();
				m_DataVec.resize(count);
				m_FirstPtr = m_DataVec.data();
				m_LastPtr = m_FirstPtr + count;
				m_EndPtr = m_FirstPtr + offsetValue;

				CheckSize();
				CheckPtr();
			}


			bool IsCurentStoreIndex(SizeType storeIndex) const
			{
				DifferenceType offsetCount = GetCurentCount();
				DifferenceType storeIndexDiff = static_cast<DifferenceType>(storeIndex);
				return offsetCount == storeIndexDiff;
			}

#pragma region Curent_Size

			template<typename Dif = DifferenceType>
			DifferenceType GetCurentCount() const
			{
				DifferenceType curentCount = GetDiferenzBetweenPtrAndFirstPtr(m_EndPtr);
				Dif curentDifByteSize = CastSafeFunc<Dif>(curentCount);
				return curentDifByteSize;
			}

			template<typename Dif = DifferenceType>
			Dif GetCurentByteSize() const
			{
				uint8_t* fristPtr8Bit = reinterpret_cast<uint8_t*>(m_FirstPtr);
				uint8_t* endPtr8Bit = reinterpret_cast<uint8_t*>(m_EndPtr);
				DifferenceType curentByteSize = endPtr8Bit - fristPtr8Bit;
				Dif curentDifByteSize = CastSafeFunc<Dif>(curentByteSize);
				return curentDifByteSize;
			}

#pragma endregion

#pragma region Alloc_Size

			template<typename Dif = DifferenceType>
			DifferenceType GetAllocCount() const
			{
				DifferenceType allocCount = GetDiferenzBetweenPtrAndFirstPtr(m_LastPtr);
				Dif alocDifCount = CastSafeFunc<Dif>(allocCount);
				return alocDifCount;
			}

			DifferenceType GetAlocByteSize() const
			{
				uint8_t* fristPtr8Bit = reinterpret_cast<uint8_t*>(m_FirstPtr);
				uint8_t* lastPtr8Bit = reinterpret_cast<uint8_t*>(m_LastPtr);
				DifferenceType alocByteSize = lastPtr8Bit - fristPtr8Bit;
				DifferenceType alocDifByteSize = CastSafeFunc<DifferenceType>(alocByteSize);
				return alocDifByteSize;
			}

#pragma endregion
			

			T* GetDataEndPtr()
			{
				RY_CORE_ASSERT(IsCurentPosEndVaild(), "We Overflow the Ptr Data or don't even set them!");
				return m_EndPtr;
			}
			
			template<typename N>
			T* GetDataEndPtr(N& storeIndex)
			{
				DifferenceType offsetCount = GetCurentCount();
				DifferenceType storeIndexDiff = CastSafeFunc<DifferenceType>(storeIndex);
				if (offsetCount != storeIndexDiff)
				{
					storeIndex = CastSafeFunc<N>(offsetCount);
				}
				RY_CORE_ASSERT(IsCurentPosEndVaild(), "We Overflow the Ptr Data or don't even set them!");
				return m_EndPtr;
			}

			

			T& GetDataRef()
			{
				RY_CORE_ASSERT(IsCurentPosEndVaild(), "We Overflow the Ptr Data or don't even set them!");
				return (*m_EndPtr);
			}

			template<typename N>
			T& GetDataRef(N& storeIndex)
			{
				DifferenceType offsetCount = GetCurentCount();
				N storeIndexDiff = CastSafeFunc<N>(storeIndex);
				if (offsetCount != storeIndexDiff)
				{
					storeIndex = CastSafeFunc<N>(offsetCount);
				}
				RY_CORE_ASSERT(IsCurentPosEndVaild(), "We Overflow the Ptr Data or don't even set them!");
				return m_EndPtr;
			}


			
			
			void Check()
			{
				if (IsPtrInVaildRange(m_EndPtr) && m_FirstPtr == m_DataVec.data())
				{
					return;
				}
				else if(m_FirstPtr != m_DataVec.data() && !m_DataVec.empty())
				{
					DifferenceType offsetValue = GetCurentCount();
					
					SizeType count = m_DataVec.size();
					if (offsetValue < 0 || count <= offsetValue)
						offsetValue = 0;
					m_FirstPtr = m_DataVec.data();
					m_LastPtr = m_FirstPtr + count;
					m_EndPtr = m_FirstPtr + offsetValue;

					return;
				}

				RY_CORE_ASSERT(m_EndPtr <= m_LastPtr, "We Overflow the Ptr Data more by one!");
				SizeType count = m_DataVec.size();
				SizeType n = 3;
				SizeType lastCountDivadeByN = count / n;
				SizeType lastCountDivadeByNAndAddByOne = lastCountDivadeByN + 1;
				SizeType newCount = count + lastCountDivadeByNAndAddByOne;
				Resize2D(newCount);
			}

			void Reset()
			{
				m_EndPtr = m_FirstPtr;

				CheckSize();
				CheckPtr();
			}

			void Incroment()
			{
				m_EndPtr++;
			}

			void Deincroment()
			{
				m_EndPtr--;
			}

			bool IsCurentPosEndVaild() const
			{
				return IsPtrInsideRange(m_EndPtr);
			}

			bool IsEmpty() const
			{
				return m_FirstPtr == m_EndPtr;
			}

			Iterator begin() noexcept
			{
				Iterator itBeginn = m_DataVec.begin();
				return itBeginn;
			}

			Iterator end() noexcept
			{
				Iterator itBeginn = m_DataVec.begin();
				uint32_t offsetValue = GetCurentCount();
				Iterator itEnde = itBeginn + offsetValue;
				return itEnde;
			}

			ConstIterator begin() const noexcept
			{
				ConstIterator itBeginn = m_DataVec.begin();
				return itBeginn;
			}

			ConstIterator end() const noexcept
			{
				ConstIterator itBeginn = m_DataVec.begin();
				uint32_t offsetValue = GetCurentCount();
				ConstIterator itEnde = itBeginn + offsetValue;
				return itBeginn;
			}

			// returns first Elemment Ptr
			const T* GetDataPtr() const { return m_FirstPtr; }

		protected:
			T* GetEndPtr() { return m_EndPtr; }
			T* GetFirstPtr() { return m_FirstPtr; }
			T* GetLastPtr() { return m_LastPtr; }

			inline bool IsPtrInsideRange(const T *const ptr) const
			{				
				return m_FirstPtr != nullptr && m_LastPtr != nullptr && IsPtrInVaildRange(ptr);
			}

			inline bool IsPtrInVaildRange(const T *const ptr) const
			{
#if 0
				DifferenceType ptrPos = static_cast<DifferenceType>(ptr);
				DifferenceType firstPos = static_cast<DifferenceType>(m_FirstPtr);
				DifferenceType lastPos = static_cast<DifferenceType>(m_LastPtr);

				return m_FirstPtr <= ptr && ptrPos > lastPos;
#else
				return m_FirstPtr <= ptr && ptr < m_LastPtr;
#endif
			}


			// offset = ptr - first
			inline DifferenceType GetDiferenzBetweenPtrAndFirstPtr(const T *const ptr) const
			{
				DifferenceType offset = ptr - m_FirstPtr;
				return offset;
			}


			// offset = last - ptr
			DifferenceType GetDiferenzBetweenLastPtrAndPtr(const T *const ptr) const
			{
				DifferenceType offset = static_cast<DifferenceType>(m_LastPtr - ptr);
				return offset;
			}

			// offset = (last - 1) - ptr
			DifferenceType GetVaildPlaces(const T *const ptr) const
			{
				DifferenceType offset = static_cast<DifferenceType>((m_LastPtr - 1) - ptr);
				return offset;
			}
			
		protected:

			bool CheckSize() const
			{
				DifferenceType allocCount = GetAllocCount();
				DifferenceType sizeByVec = m_DataVec.size();
				bool state = allocCount == sizeByVec;
				RY_CORE_ASSERT(state, "Size Not the same Result!");
				return state;
			}

			bool CheckPtr() const
			{
				const T* vecFirstDataPtr = m_DataVec.data();
				SizeType count = m_DataVec.size();
				const T* vecLastDataPtr = vecFirstDataPtr + count;
				bool state = m_FirstPtr == vecFirstDataPtr && m_LastPtr == vecLastDataPtr && nullptr != m_EndPtr;
				RY_CORE_ASSERT(state, "Size Not the same Result!");
				return state;
			}
		
			
		

			
		private:
			T* m_FirstPtr;
			T* m_LastPtr;
			T* m_EndPtr;
			
			std::vector<T> m_DataVec;
		};

#if 0
		template<typename T>
		class StoreSubmiteCurentPtr : public StoreSubmite<T>
		{
		public:
			using Parent = StoreSubmite<T>;
			using Iterator =  Parent::Iterator;
			using ConstIterator = Parent::ConstIterator;
			using DifferenceType = Parent::DifferenceType; // type for using store ptr as numbers
			using SizeType = Parent::SizeType;

		public:
			StoreSubmiteCurentPtr()
				: StoreSubmite<T>()
			{

			}

			~StoreSubmiteCurentPtr() = default;



			void Create(SizeType count)
			{
				m_UsePtr = nullptr;
				Parent::Create(count);
			}

			void Destroy()
			{
				m_UsePtr = nullptr;
				Parent::Destroy();
			}

			void Check()
			{
				if (IsInVaildRange())
					return;

				SizeType count = this->m_DataVec.size();
				SizeType n = 3;
				SizeType lastCountDivadeByN = count / n;
				SizeType lastCountDivadeByNAndAddByOne = lastCountDivadeByN + 1;
				SizeType newCount = count + lastCountDivadeByNAndAddByOne;
				Resize2D(newCount);

			}

			void Resize2D(SizeType count)
			{
				DifferenceType offsetCurent = Parent::GetDiferenzBetweenPtrAndFirstPtr(m_UsePtr);
				Parent::Resize2D(count);
				SizeType size = static_cast<SizeType>(offsetCurent);
				SetUseToIndex(size);
			}

			inline bool IsInVaildRange() const
			{
				T* endPtr = this->GetEndPtr();
				return Parent::IsPtrInVaildRange(endPtr) && Parent::IsPtrInVaildRange(m_UsePtr);
			}

			bool IsCurentUsePosVaild() const
			{
				return IsPtrInsideRange(m_UsePtr);
			}

			T* GetDataUse()
			{
				RY_CORE_ASSERT(this->IsCurentPosVaild(), "We Overflow the Ptr Data or don't even set them!");
				return m_UsePtr;
			}

			template<typename N>
			T* GetDataUse(N& storeIndex)
			{
				DifferenceType curentCount = this->GetCurentCount();
				DifferenceType storeIndexDiff = CastSafeFunc<DifferenceType>(storeIndex);
				if (offsetCount != storeIndexDiff)
				{
					storeIndex = CastSafeFunc<N>(offsetCount);
				}
				RY_CORE_ASSERT(this->IsUsePosVaild(), "We Overflow the Ptr Data or don't even set them!");
				return m_UsePtr;
			}

			void SetUseToIndex(SizeType size)
			{
				T* fistrPtr = Parent::GetFirstPtr();
				m_UsePtr = fistrPtr + offsetCurent;				
			}
		private:
			T* m_UsePtr;

		};
#endif
		

	}
}
#pragma once
#include <rypch.h>

namespace Rynex {

#define RY_VECTOR_PUSH_ALLECO(currentSize) (currentSize + (currentSize / 2ull));
#define RY_VECTOR_SIMPLE_LOOP 1
#define RY_VECTOR_LOOP 0

	template<typename T = int>
	class Vector
	{
	public:
		using Data = typename T;
	public:
		Vector()
			: m_Begin(nullptr)
			, m_Ende(nullptr)
			, m_CapacitySize(0ull)
		{
		}

		Vector(const Vector& v)
			: m_Begin(nullptr)
			, m_Ende(nullptr)
			, m_CapacitySize(0ull)
		{
			ReAlloced(v.m_CapacitySize);
			v.CopyToPtr(m_Begin, m_CapacitySize);
		}

		Vector(Vector&& v) noexcept
			: m_Begin(v.m_Begin)
			, m_Ende(v.m_Ende)
			, m_CapacitySize(v.m_CapacitySize)
		{
			v.m_Begin = nullptr;
			v.m_Ende = nullptr;
			v.m_CapacitySize = 0ull;
		}

		~Vector()
		{
			Destroy();
		}

		bool IsEmpty()
		{
			return ((nullptr == m_Begin) && (nullptr == m_Ende) && (0ull == m_CapacitySize));
		}

		void Reserve(uint64_t size)
		{
			ReAlloced(size);
		}

		void Resize2D(uint64_t size)
		{

		}

		void Clear()
		{
#if RY_VECTOR_SIMPLE_LOOP
			uint64_t size = Size();

			for (uint64_t i = 0; i < size; i++)
			{
				RY_CORE_ASSERT(i < size, "Buffer Overfolw!");
				m_Begin[i].~Data();
			}

#elif RY_VECTOR_LOOP 
#elif RY_VECTOR_CLEAR_EDVENC
#endif

		}

		uint64_t ByteSize()const
		{
			uint64_t size = m_Ende - m_Begin;
			return size * sizeof(Data);
		}

		uint64_t Size()const
		{
			uint64_t size = m_Ende - m_Begin;
			return size;
		}

		uint64_t AllocSize() const
		{
			return m_CapacitySize;
		}
	private:
		void Destroy()
		{
			if (nullptr != m_Begin)
			{
				delete[] m_Begin;
				m_Begin = nullptr;
				m_Ende = nullptr;
				m_CapacitySize = 0ull;
			}
			RY_CORE_ASSERT(IsEmpty(), "Vector is not empty");
		}

		void ReAlloced(uint64_t newCapacity)
		{
			Data* ptrBegin = new Data[newCapacity];
			uint64_t size = Size();

			Data* ptrEnde = nullptr;

			ptrEnde = ptrBegin + newCapacity;
			size = ptrEnde - ptrBegin;

			RY_CORE_ASSERT(newCapacity < size, "size is great then capcity");
			if (newCapacity >= size)
			{
				CopyToPtr(ptrBegin, newCapacity);
				SetNewData(ptrBegin, ptrEnde, newCapacity);
			}


		}

		void CopyToPtr(Data* beginPtr, uint64_t capcitySize) const
		{
#if RY_VECTOR_SIMPLE_LOOP
			uint64_t curentCapacity = Size();
			uint32_t size = capcitySize < curentCapacity ? capcitySize : curentCapacity;

			for (uint64_t i = 0; i < size; i++)
			{
				RY_CORE_ASSERT(i < capcitySize && i < curentCapacity);
				beginPtr[i] = m_Begin[i];
			}
#elif RY_VECTOR_LOOP 
#elif RY_VECTOR_COPYEDVENC
#endif
		}

		void MoveToPtr(Data* beginPtr, uint64_t capcitySize)
		{
#if RY_VECTOR_SIMPLE_LOOP
			uint64_t curentCapacity = Size();
			uint32_t size = capcitySize < curentCapacity ? capcitySize : curentCapacity;

			for (uint64_t i = 0; i < size; i++)
			{
				RY_CORE_ASSERT(i < capcitySize && i < curentCapacity);
				beginPtr[i] = std::move(m_Begin[i]);
			}
#elif RY_VECTOR_LOOP 
#elif RY_VECTOR_MOVE_EDVENC
#endif
		}

		void SetNewData(Data* beginPtr, Data* endePtr, uint64_t capcitySize)
		{
			Destroy();
			m_Begin = beginPtr;
			m_Ende = endePtr;
			m_CapacitySize = capcitySize;
		}
	private:
		Data* m_Begin;
		Data* m_Ende;
		uint64_t m_CapacitySize;
	};

}



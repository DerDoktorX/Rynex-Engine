#pragma once
#include<rypch.h>

namespace Rynex {
	namespace Memory {
		template<typename T>
		class Allocater : public std::allocator<T>
		{
		public:
			using size_type = uint32_t;

		// --- public member funktion -----------------------------------------------------------------------------------------
			T* allocate(const size_t count)
			{
				T* memoryPtr = new T[count];
				return memoryPtr;
			}


#if 0
			Allocater CopyHeapMemoryOneToOne();
			void MoveHeapMemoryOneToOne(Allocater< T, _SizeType >& target);
			void SwapOneToOne(Allocater<T, _SizeType>& a);

			T GetCopy(_SizeType index) const;
			T& GetRef(_SizeType index);
			const T& GetCopyRef(_SizeType index) const;

			template<typename _As_Type>
			_As_Type GetCopyAs(_SizeType index) const;
			
			T& SetValue(_SizeType index, T value);
			T& SetRef(_SizeType index, T& value);
			T& SetCopyRef(_SizeType index, const T& value);
			T& SetMove(_SizeType index, T&& value);
			
			T* FirstDataPtr();
			const T* FirstProtectedDataPtr();
			T *const FirstDataProtectedPtr();
			const T *const FirstProtectedDataAndPtr() const;
			
			T* LastDataPtr();
			const T* LastProtectedDataPtr();
			T *const LastDataProtectedPtr();
			const T *const LastProtectedDataAndPtr() const;

			T* EndDataPtr();
			const T* EndProtectedDataPtr();
			T *const EndDataProtectedPtr();
			const T *const EndProtectedDataAndPtr() const;
#endif

		};

		
	}
}
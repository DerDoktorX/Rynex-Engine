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

		};

		
	}
}
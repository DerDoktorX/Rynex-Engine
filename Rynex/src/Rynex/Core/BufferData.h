#pragma once
#include "Rynex/Core/Base.h"

#include <cstdint>
#include <cstring>


namespace Rynex {

	struct BufferData
	{
		uint8_t* Data = nullptr;
		uint64_t Size = 0;

		BufferData() = default;

		BufferData(const BufferData&) = default;

		BufferData(uint64_t size)
		{
			Allocate(size);
		}

		static BufferData Copy(BufferData other)
		{
			BufferData result(other.Size);
			memcpy(result.Data, other.Data, other.Size);
			return result;
		}

		void Allocate(uint64_t size)
		{
			Release();

			Data = (uint8_t*)malloc(size);
			Size = size;
		}

		void Release()
		{
			free(Data);
			Data = nullptr;
			Size = 0;
		}

		template<typename T>
		T* As()
		{
			RY_CORE_ASSERT(Data, "is nulptr");
			return (T*)Data;
		}
		operator bool() const
		{
			return (bool)Data;
		}
	};

}
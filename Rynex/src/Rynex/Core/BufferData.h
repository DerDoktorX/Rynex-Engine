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

	class BufferDataRuntimeSingleType
	{
	public:

		BufferDataRuntimeSingleType()
			: m_DataType(ShaderDataType::None)
			, m_Count(0u)
			, m_Buffer()
		{
		}


		BufferDataRuntimeSingleType(const ShaderDataType dataType, uint32_t count)
			: m_DataType(dataType)
			, m_Count(count)
			, m_Buffer()
		{
			if (m_DataType == ShaderDataType::None)
				return;
			uint32_t byteSize = ShaderDataTypeSize(m_DataType) * m_Count;
			RY_CORE_ASSERT(byteSize != 0, "A Buffer withe only 0 byte is not Vaild!");
			uint64_t byteSize64 = static_cast<uint64_t>(byteSize);
			m_Buffer.Allocate(byteSize64);
		}

		BufferDataRuntimeSingleType(const BufferDataRuntimeSingleType& buffer)
			: m_DataType(buffer.m_DataType)
			, m_Count(buffer.m_Count)
			, m_Buffer(BufferData::Copy(buffer.m_Buffer))
		{
		}

		~BufferDataRuntimeSingleType()
		{
			m_Buffer.Release();
		}

		const ShaderDataType GetDataType() const
		{
			return m_DataType;
		}

		const uint32_t GetCount() const
		{
			return m_Count;
		}

		template<typename T>
		T& As() const
		{
			RY_CORE_ASSERT(0 != m_Buffer.Size);
			return *m_Buffer.As<T>();
		}

		template<typename T>
		T& As(uint32_t index) const
		{
			RY_CORE_ASSERT(0 != m_Buffer.Size);
			RY_CORE_ASSERT(index < m_Count);
			return *m_Buffer.As<T>();
		}
	private:
		const ShaderDataType m_DataType = SDT::None;
		const uint32_t m_Count = 0u;
		BufferData m_Buffer;
	};

}
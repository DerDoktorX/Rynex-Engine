#include"rypch.h"
#include"MemorBuffer.h"


namespace Rynex {



	MemoryBuffer::MemoryBuffer()
	{
	}

	MemoryBuffer::~MemoryBuffer()
	{
	}

	uint32_t MemoryBuffer::PushData(const uint8_t* bytePtr, uint32_t byteSize)
	{
		uint32_t nextByteSize = m_ByteSize + byteSize;
		if (m_DataVec.size() < (m_ByteSize + byteSize))
		{
			m_DataVec.resize(nextByteSize);
		}
		uint8_t* offsetByte = m_DataVec.data() + m_ByteSize;
		std::memcpy(offsetByte, bytePtr, byteSize);
		m_ByteSize = nextByteSize;
		return m_PushCount++;
	}

	uint8_t* MemoryBuffer::GetData()
	{
		return m_DataVec.data();
	}

	const uint8_t* MemoryBuffer::GetData() const
	{
		return m_DataVec.data();
	}

	uint8_t MemoryBuffer::GetByteSize() const
	{
		return m_ByteSize;
	}

	uint32_t MemoryBuffer::GetPushCount() const
	{
		return m_PushCount;
	}

}
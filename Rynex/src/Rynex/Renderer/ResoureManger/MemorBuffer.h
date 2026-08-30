#pragma once


namespace Rynex {

	class MemoryBuffer
	{
	public:
		MemoryBuffer();
		~MemoryBuffer();

	
		uint32_t PushData(const uint8_t* bytePtr, uint32_t byteSize);

		uint8_t* GetData();
		const uint8_t* GetData() const;
		
		uint8_t GetByteSize() const;
		uint32_t GetPushCount() const;

		template<typename T>
		uint32_t Push(const T& value)
		{
			return PushData(&value, sizeof(T));
		}

		template<typename T>
		T* Get(uint32_t index = 0u)
		{
			uint32_t beginByteSize = index * sizeof(T);
			uint32_t endeByteSize = beginByteSize + sizeof(T);
			RY_CORE_ASSERT(beginByteSize < m_ByteSize);
			const uint8_t* bytePtr = m_DataVec.data() + beginByteSize;
			const T* valuePtr = reinterpret_cast<uint8_t*>(bytePtr);
			return valuePtr;
		}

		template<typename T>
		const T* Get(uint32_t index = 0u) const
		{
			uint32_t beginByteSize = index * sizeof(T);
			uint32_t endeByteSize = beginByteSize + sizeof(T);
			RY_CORE_ASSERT(endeByteSize <= m_ByteSize, "Buffer Overflow!");
			const uint8_t* bytePtr = m_DataVec.data() + beginByteSize;
			const T* valuePtr = reinterpret_cast<uint8_t*>(bytePtr);
			return valuePtr;
		}
	private:
		std::vector<uint8_t> m_DataVec;
		uint32_t m_ByteSize;
		uint32_t m_PushCount;
	};

	

}
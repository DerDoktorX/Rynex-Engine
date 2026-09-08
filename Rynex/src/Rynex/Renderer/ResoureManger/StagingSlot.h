#pragma once


namespace Rynex {

	class IStagingSlot
	{
	public:
		virtual uint32_t Add(const void* val, uint32_t byteSize) = 0;
		virtual uint32_t Set(const void* val, uint32_t byteSize, uint64_t key) = 0;
		virtual void Reset() = 0;
		virtual void* DataPtr() = 0;
		virtual const void* DataPtr() const = 0;
		virtual uint32_t ByteSize() const = 0;
		virtual uint32_t Count() const = 0;
		virtual uint32_t StrideByteSize() const = 0;

	// --- public static funktion ---------------------------------------------------------------------------------------------

		template<typename T>
		static constexpr uint32_t Add(Ref<IStagingSlot>& stagingSlot, const T& value)
		{
			return stagingSlot->Add(&value, sizeof(T));
		}

		template<typename T>
		static constexpr uint32_t Set(Ref<IStagingSlot>& stagingSlot, const T& value, uint64_t key)
		{
			return stagingSlot->Set(&value, sizeof(T), key);
		}
	};

	class StagingSlotAppendByte : public IStagingSlot
	{
	public:
		explicit StagingSlotAppendByte(uint32_t strideByteSize, uint32_t reserve = 2048u)
			: m_StrideByteSize(strideByteSize)
		{
			m_ElementVec.reserve(reserve);
		}


		virtual uint32_t Add(const void* dataPtr, uint32_t byteSize) override
		{
			RY_CORE_ASSERT(byteSize == m_StrideByteSize, "not same Bytesize!");
			
			uint32_t nextVecByteSize = NextPushByteSize();
			uint32_t offsetByteSize = ByteSize();
			uint32_t allocSize = m_ElementVec.capacity();
			m_ElementVec.resize(nextVecByteSize + m_StrideByteSize);
			uint8_t* offsetPtr = m_ElementVec.data() + offsetByteSize;
			std::memcpy(offsetPtr, dataPtr, byteSize);
			RY_CORE_WARN_IF(allocSize != m_ElementVec.capacity(), "Resize action by {} Bytes new Alloc Bytesize: {}!", (static_cast<uint32_t>(m_ElementVec.capacity()) - allocSize), m_ElementVec.capacity());
			return offsetByteSize;
		}

		virtual uint32_t Set(const void* dataPtr, uint32_t byteSize, uint64_t key) override
		{
			RY_CORE_WARN_IF(0 != key, "Set funktion is expeted Map not StagingSlotAppendByte! key dont required: {}!", key);
			return Add(dataPtr, byteSize);
		}

		virtual void Reset() override { m_ElementVec.clear(); }
		virtual void* DataPtr() override { return m_ElementVec.data(); }
		virtual const void* DataPtr() const override { return m_ElementVec.data(); }
		virtual uint32_t ByteSize() const override { return static_cast<uint32_t>(m_ElementVec.size()); }
		virtual uint32_t Count() const override 
		{ 
			uint32_t byteSize = ByteSize();
			uint32_t offset = byteSize % m_StrideByteSize;

			RY_CORE_ASSERT(0 == offset, "we have a not full strides!");

			uint32_t count = byteSize / m_StrideByteSize;

			return count;
		}
		virtual uint32_t StrideByteSize() const override { return m_StrideByteSize; }

		
	private:
		uint32_t NextPushByteSize() const
		{
			uint32_t byteSize = static_cast<uint32_t>(m_ElementVec.size());
			byteSize += m_StrideByteSize;
			return byteSize;
		}
	// --- private member varibles --------------------------------------------------------------------------------------------
		std::vector<uint8_t> m_ElementVec;
		uint32_t m_StrideByteSize;
	};

	class StagingSlotAppendByteWrite : public IStagingSlot
	{
	public:
		explicit StagingSlotAppendByteWrite(uint32_t strideByteSize, uint32_t writePos = 0, uint32_t reserve = 2048u)
			: m_StrideByteSize(strideByteSize)
			, m_WriteBytePos(writePos)
		{
			m_ElementVec.resize(reserve);
		}


		virtual uint32_t Add(const void* dataPtr, uint32_t byteSize) override
		{
			uint32_t nextWhritePos = m_WriteBytePos + byteSize;
			uint32_t allocSize = m_ElementVec.capacity();
			uint32_t offsetStrideCurent = GetStrideByteOffset();
			uint32_t offsetStrideNext = GetStrideByteOffset(nextWhritePos);
			RY_CORE_ASSERT(offsetStrideCurent < offsetStrideNext || offsetStrideNext == 0, "unexpted size, write from midle strid a in to midle stride B!");


			if (m_ElementVec.size() <= nextWhritePos)
			{
				m_ElementVec.resize(nextWhritePos + m_StrideByteSize);
			}
			RY_CORE_ASSERT(nextWhritePos < m_ElementVec.size(), "exted next whrite pos is to high!");


			uint8_t* offsetPtr = m_ElementVec.data();
			offsetPtr += m_WriteBytePos;
			std::memcpy(offsetPtr, dataPtr, byteSize);

			RY_CORE_WARN_IF(allocSize != m_ElementVec.capacity(), "Resize action by {} Bytes new Alloc Bytesize: {}!", (static_cast<uint32_t>(m_ElementVec.capacity()) - allocSize), m_ElementVec.capacity());
			
			uint32_t oldWhritePos = m_WriteBytePos;
			m_WriteBytePos += byteSize;
			return oldWhritePos;
		}

		virtual uint32_t Set(const void* dataPtr, uint32_t byteSize, uint64_t key) override
		{
			RY_CORE_WARN_IF(0 != key, "Set funktion is expeted Map not StagingSlotAppendByteWrite! key dont required: {}!", key);
			return Add(dataPtr, byteSize);
		}

		virtual void Reset() override { m_WriteBytePos = 0u; }
		virtual void* DataPtr() override { return m_ElementVec.data(); }
		virtual const void* DataPtr() const override { return m_ElementVec.data(); }
		virtual uint32_t ByteSize() const override { return m_WriteBytePos; }
		virtual uint32_t Count() const override 
		{ 
			uint32_t offset = GetStrideByteOffset();
			RY_CORE_WARN_IF(0ull == offset, "we have not full strides! {} Bytes more", offset);

			uint32_t count = m_WriteBytePos / m_StrideByteSize;
			return count;
		}
		virtual uint32_t StrideByteSize() const override { return m_StrideByteSize; }
	private:
		uint32_t GetStrideByteOffset() const
		{
			uint32_t offset = GetStrideByteOffset(m_WriteBytePos);
			return offset;
		}

		uint32_t GetStrideByteOffset(uint32_t pos) const
		{
			uint32_t offset = pos % m_StrideByteSize;
			return offset;
		}
	// --- private member varibles --------------------------------------------------------------------------------------------
		std::vector<uint8_t> m_ElementVec;
		uint32_t m_StrideByteSize;
		uint32_t m_WriteBytePos;
	};

	template<typename T>
	class StagingSlotAppend : public IStagingSlot
	{
	public:
		explicit StagingSlotAppend(uint32_t reserve = 64u)
		{
			m_ElementVec.reserve(reserve);
		}


		virtual uint32_t Add(const void* dataPtr, uint32_t byteSize) override
		{
			RY_CORE_ASSERT(byteSize == sizeof(T), "not same Bytesize!");

			const T* typeDataPtr = reinterpret_cast<const T*>(dataPtr);
			uint32_t pos = Push(typeDataPtr);
			return pos;
		}


		uint32_t Push(const T* dataPtr)
		{
			return Push(*dataPtr);
		}

		uint32_t Push(const T& ellment)
		{
			uint32_t pos = Count();
			uint32_t allocSize = m_ElementVec.capacity();

			m_ElementVec.emplace_back(ellment);
			RY_CORE_WARN_IF(allocSize != m_ElementVec.capacity(), "Resize action by {} Bytes new Alloc Bytesize: {}!", (static_cast<uint32_t>(m_ElementVec.capacity()) - allocSize), m_ElementVec.capacity());
			return pos;
		}

		virtual uint32_t Set(const void* dataPtr, uint32_t byteSize, uint64_t key) override
		{
			RY_CORE_WARN_IF(0ull != key, "Set funktion is expeted Map not StagingSlotAppend! key dont required: {}!", key);
			return Add(dataPtr, byteSize);
		}

		virtual void Reset() override { m_ElementVec.clear(); }
		virtual void* DataPtr() override { return m_ElementVec.data(); }
		virtual const void* DataPtr() const override { return m_ElementVec.data(); }
		virtual uint32_t ByteSize() const override { return static_cast<uint32_t>(m_ElementVec.size() * sizeof(T)); }
		virtual uint32_t Count() const override { return static_cast<uint32_t>(m_ElementVec.size()); }
		virtual uint32_t StrideByteSize() const override { return static_cast<uint32_t>(sizeof(T)); }
	private:
		uint32_t NextPushSize() const
		{
			uint32_t elementCount = static_cast<uint32_t>(m_ElementVec.size());
			elementCount += 1u;
			return elementCount;
		}
		// --- private member varibles --------------------------------------------------------------------------------------------
		std::vector<T> m_ElementVec;
	};

	template<typename T>
	class StagingSlotAppendWrite : public IStagingSlot
	{
	public:
		explicit StagingSlotAppendWrite(uint32_t writePos = 0u, uint32_t reserve = 64u)
			: m_WriteBytePos(writePos)
		{
			m_ElementVec.resize(reserve);
		}


		virtual uint32_t Add(const void* dataPtr, uint32_t byteSize) override
		{
			return PushBytes(dataPtr, byteSize);;
		}


		

		uint32_t PushBytes(const void* dataPtr, uint32_t byteSize)
		{
			RY_CORE_ASSERT(byteSize <= sizeof(T), "to high Byte Size!");
			uint32_t allocSize = m_ElementVec.capacity();
			uint32_t byteSizeVec = m_ElementVec.size() * sizeof(T);
			
			uint32_t nextWhritePos = m_WriteBytePos + byteSize;
			uint32_t offsetStrideCurent = GetStrideByteOffset();
			uint32_t offsetStrideNext = GetStrideByteOffset(nextWhritePos);
			RY_CORE_ASSERT(offsetStrideCurent < offsetStrideNext || offsetStrideNext == 0, "unexpted size, write from midle strid a in to midle stride B!");

			
			if (byteSizeVec <= nextWhritePos)
			{
				m_ElementVec.emplace_back();
				byteSizeVec = m_ElementVec.size() * sizeof(T);
			}
			RY_CORE_ASSERT(nextWhritePos <= byteSizeVec, "exted next whrite pos is to high!");

			uint8_t* offsetPtr = reinterpret_cast<uint8_t*>(m_ElementVec.data());
			offsetPtr += m_WriteBytePos;

			RY_CORE_WARN_IF(allocSize != m_ElementVec.capacity(), "Resize action by {} Bytes new Alloc Bytesize: {}!", (static_cast<uint32_t>(m_ElementVec.capacity()) - allocSize), m_ElementVec.capacity());

			std::memcpy(offsetPtr, dataPtr, byteSize);
			uint32_t oldWhritePos = m_WriteBytePos;
			m_WriteBytePos += byteSize;
			return oldWhritePos;
		}

		virtual uint32_t Set(const void* dataPtr, uint32_t byteSize, uint64_t key) override
		{
			RY_CORE_WARN_IF(0ull != key, "Set funktion is expeted Map not StagingSlotAppendWrite! key dont required: {}!", key);
			return Add(dataPtr, byteSize);
		}

		virtual void Reset() override { m_WriteBytePos = 0ull; }
		virtual void* DataPtr() override { return m_ElementVec.data(); }
		virtual const void* DataPtr() const override { return m_ElementVec.data(); }
		virtual uint32_t ByteSize() const override { return m_WriteBytePos; }
		virtual uint32_t Count() const override
		{
			uint32_t offset = GetStrideByteOffset();

			RY_CORE_WARN_IF(0ull == offset, "we have not full strides! {} Bytes more", offset);

			uint32_t count = m_WriteBytePos / sizeof(T);
			return count;
		}

		virtual uint32_t StrideByteSize() const override { return static_cast<uint32_t>(sizeof(T)); }
	private:
		uint32_t GetStrideByteOffset() const
		{
			uint32_t offset = GetStrideByteOffset(m_WriteBytePos);
			return offset;
		}

		uint32_t GetStrideByteOffset(uint32_t pos) const
		{
			uint32_t offset = pos % sizeof(T);
			return offset;
		}
	// --- private member varibles --------------------------------------------------------------------------------------------
		std::vector<T> m_ElementVec;
		uint32_t m_WriteBytePos;
	};

}
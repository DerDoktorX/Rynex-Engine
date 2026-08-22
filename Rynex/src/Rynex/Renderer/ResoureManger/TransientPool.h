#pragma once
#include <rypch.h>
#include <Rynex/Renderer/API/Buffer.h>

namespace Rynex {
	template<typename T>
	class TransientPool
	{
	private:
		using BufferVec = std::vector<Ref<T>>;
		using BuketKey = uint32_t;
		using Map = HashMapFlat<BuketKey, BufferVec>;
		using MapIt = typename Map::iterator;
		using MapConstIt = typename Map::const_iterator;
	public:
		TransientPool()
		{ 
		}
		~TransientPool()
		{ 
			Destroy();
		}

		Ref<T> GetBuffer(uint32_t minByteSize, BufferFlagGPU flags)
		{
			BuketKey bucketKey = GetBucketKey(minByteSize);

			Ref<T> buffer = nullptr;

			BufferVec& freeBufferVec = m_FreeBuffer[bucketKey];

			if (freeBufferVec.empty())
			{
				buffer = T::Create(nullptr, bucketKey, flags);
			}
			else
			{
				buffer = std::move(freeBufferVec.back());
				freeBufferVec.pop_back();
			}
			RY_CORE_ASSERT(buffer != nullptr, "Empty Buffer!");
			RY_CORE_ASSERT(minByteSize <= buffer->GetByteSize() , "to smal!");
			m_InuseBufferVec.emplace_back(buffer);
			return buffer;
		}

		size_t FreePoolCount() const 
		{
			size_t n = 0;
			for (const auto& [bucketKey, freeBufferVec] : m_FreeBuffer)
			{
				n += freeBufferVec.size();
			}
			return n;
		}

		size_t InusePoolByteSize() const
		{
			size_t byteSize = 0;
			for (const Ref<T>& buffer : m_InuseBufferVec)
			{
				byteSize += buffer->GetByteSize();
			}
			return byteSize;
		}

		void Reset()
		{
			for (Ref<T>& buffer : m_InuseBufferVec) 
			{
				uint32_t bucket = buffer->GetByteSize();
				BuketKey bucketKey = GetBucketKey(minByteSize);
				RY_CORE_ASSERT(bucket == bucketKey, "Not ByteSize Of Power 2!");

				BufferVec& freeBufferVec = m_FreeBuffer[bucketKey];
				freeBufferVec.emplace_back(std::move(buffer));
			}
			m_InuseBufferVec.clear();
		}

		void Destroy()
		{
			m_FreeBuffer.clear();
			m_InuseBufferVec.clear();
		}
	private:
	// --- private constexpr member funktion ----------------------------------------------------------------------------------
		constexpr BuketKey GetBucketKey(uint32_t size)
		{
			if (size == 0)
				return 1;
			size--;
			size |= size >> 1; 
			size |= size >> 2;
			size |= size >> 4; 
			size |= size >> 8; 
			size |= size >> 16;
			size++;
			return size;
		}
	// --- private member varibles --------------------------------------------------------------------------------------------
		Map m_FreeBuffer;
		BufferVec m_InuseBufferVec;
	};
}
#pragma once
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Renderer/Rendering/StoreSubmite.h>
namespace Rynex {

	namespace BatchedRender {
		static void SetBufferData(Ref<VertexBuffer>& buffer, const void *const dataPtr, uint32_t byteSize);
		static void SetBufferData(Ref<IndexBuffer>& buffer, const uint32_t *const dataPtr, uint32_t byteSize);
		static void SetBufferData(Ref<IndexBuffer>& buffer, const uint16_t *const dataPtr, uint32_t byteSize);
		static void SetBufferData(Ref<UniformBuffer>& buffer, const void *const dataPtr, uint32_t byteSize);
		static void SetBufferData(Ref<IndirectBuffer>& buffer, const void *const dataPtr, uint32_t byteSize);
		static void SetBufferData(Ref<StorageBuffer>& buffer, const void *const dataPtr, uint32_t byteSize);


		static void ResizeBufferData(Ref<VertexBuffer>& buffer, const void *const dataPtr, uint32_t byteSize);
		static void ResizeBufferData(Ref<UniformBuffer>& buffer, const void *const dataPtr, uint32_t byteSize);
		static void ResizeBufferData(Ref<IndirectBuffer>& buffer, const void *const dataPtr, uint32_t byteSize);
		static void ResizeBufferData(Ref<StorageBuffer>& buffer, const void *const dataPtr, uint32_t byteSize);

	}

	template<typename T>
	class BatchedRenderData
	{
	public:
		BatchedRenderData();
		~BatchedRenderData();

		void SetMaxStoreData(uint32_t count);
		bool SetObject(int entity, uint32_t& storeIndex, const T& data);
		bool ForceObject(int entity, uint32_t& storeIndex, const T& data);

		void Reset();
		void Clear();

		uint32_t GetCounter() const;
		uint32_t GetByteSize() const;

		const T *const GetDataPtr() const;

		bool HasChaged() const;
		bool SetUpdated();

		template<typename N>
		bool UpdateBuffer(Ref<N>& buffer);

	private:
		Memory::StoreSubmite<T> m_DataStore;
		
		bool m_Changed;
	};

#pragma region BatchedRenderDataFuncDefintion


	template<typename T>
	inline BatchedRenderData<T>::BatchedRenderData()
	{

	}

	template<typename T>
	inline BatchedRenderData<T>::~BatchedRenderData()
	{
		Clear();
	}

	template<typename T>
	inline void BatchedRenderData<T>::SetMaxStoreData(uint32_t count)
	{
		uint32_t allocCount = m_DataStore.GetAllocCount<uint32_t>();
		if (allocCount != count)
		{
			m_DataStore.Resize2D(count);
		}

	}

	template<typename T>
	inline bool BatchedRenderData<T>::SetObject(int entity, uint32_t& storeIndex, const T& data)
	{
		if(!m_DataStore.IsCurentPosEndVaild())
			return false;

		T& storeData = m_DataStore.GetDataRef<uint32_t>(storeIndex);
		if (data != storeData)
		{
			storeData = data;
			m_Changed = true;
		}
		m_DataStore.Incroment();
		return true;
	}
	template<typename T>
	inline bool BatchedRenderData<T>::ForceObject(int entity, uint32_t& storeIndex, const T& data)
	{
		if (!m_DataStore.IsCurentPosEndVaild())
			return false;

		T& storeData = m_DataStore.GetDataRef<uint32_t>(storeIndex);
		storeData = data;
		m_Changed = true;
		m_DataStore.Incroment();

		return true;
	}

	

	

	template<typename T>
	inline uint32_t BatchedRenderData<T>::GetCounter() const
	{
		uint32_t count = m_DataStore.GetCurentCount<uint32_t>()
		return count;
	}
	
	template<typename T>
	inline uint32_t BatchedRenderData<T>::GetByteSize() const
	{		
		constexpr uint32_t elmentByteSize = sizeof(T);

		uint32_t byteSize = m_DataStore.GetCurentByteSize<uint32_t>();

		uint32_t count = m_DataStore.GetCurentCount<uint32_t>()
		uint32_t byteSize2 = count * elmentByteSize;
		RY_CORE_ASSERT(byteSize == byteSize2, "difernt ByteSize!");

		return byteSize;
	}
	
	template<typename T>
	inline void Reset()
	{
		m_DataStore.Reset();
	}


	template<typename T>
	inline void BatchedRenderData<T>::Clear()
	{
		RY_CORE_WARN("StoreSubmite can't not be Cleared! but we will reset the range");
		Reset();
	}

	
	

	template<typename T>
	inline const T *const BatchedRenderData<T>::GetDataPtr() const
	{
		const T* dataPtr = m_DataStore.GetDataPtr();
		return dataPtr;
	}

	template<typename T>
	inline bool BatchedRenderData<T>::HasChaged() const
	{
		return m_Changed;
	}

	template<typename T>
	inline bool BatchedRenderData<T>::SetUpdated()
	{
		m_Changed = true;
	}



	template<typename T>
	template<typename N>
	inline bool BatchedRenderData<T>::UpdateBuffer(Ref<N>& buffer)
	{
		if (nullptr == buffer)
			return false;
		uint32_t bufferByteSize = buffer->GetByteSize();
		uint32_t byteSize = GetByteSize();
		if constexpr (!std::is_same_v<N, IndexBuffer>)
		{
			if (bufferByteSize < byteSize)
			{
				BatchedRender::ResizeBufferData(buffer, dataPtr, byteSize);
			}
			else
			{
				const T* dataPtr = GetDataPtr();
				BatchedRender::SetBufferData(buffer, dataPtr, byteSize);
			}
		}
		else
		{
			if (bufferByteSize < byteSize)
			{
				RY_CORE_ASSERT("We can't resize a Index Buffer curently!");
				return false;
			}
			else
			{
				const T* dataPtr = GetDataPtr();
				BatchedRender::SetBufferData(buffer, dataPtr, byteSize);
			}
		}
		return true;
	}

#pragma endregion
}



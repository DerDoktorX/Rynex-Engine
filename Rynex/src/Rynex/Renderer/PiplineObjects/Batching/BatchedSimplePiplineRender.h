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


		// static void ResizeBufferData(Ref<IndriectBuffer>& buffer, const uint16_t *const dataPtr, uint32_t byteSize);
		// static void ResizeBufferData(Ref<IndriectBuffer>& buffer, const uint32_t *const dataPtr, uint32_t byteSize);
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
#if RY_DATA_VEC
		std::vector<T> m_DataVec;
		uint32_t m_StoreObjectCount;
		uint32_t m_MaxStoredObjects;
#else
		Memory::StoreSubmite<T> m_DataStore;
#endif
		
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
			
#if RY_DATA_VEC
			m_DataVec.resize(m_MaxStoredObjects);
#else
			m_DataStore.Resize2D(count);
#endif
		}

	}

	template<typename T>
	inline bool BatchedRenderData<T>::SetObject(int entity, uint32_t& storeIndex, const T& data)
	{
#if RY_DATA_VEC
		if (m_MaxStoredObjects <= m_StoreObjectCount)
			return false;

		if (m_StoreObjectCount != storeIndex)
			storeIndex = m_StoreObjectCount;
		T& storeData = m_DataVec.at(m_StoreObjectCount);

		if (data != storeData)
		{
			storeData = data;
			m_Changed = true;
		}

		m_StoreObjectCount++;
#else
		if(!m_DataStore.IsCurentPosEndVaild())
			return false;

		T& storeData = m_DataStore.GetDataRef<uint32_t>(storeIndex);
		if (data != storeData)
		{
			storeData = data;
			m_Changed = true;
		}
		m_DataStore.Incroment();
#endif
		return true;
	}
	template<typename T>
	inline bool BatchedRenderData<T>::ForceObject(int entity, uint32_t& storeIndex, const T& data)
	{
#if RY_DATA_VEC
		if (m_MaxStoredObjects <= m_StoreObjectCount)
			return false;

		if (m_StoreObjectCount != storeIndex)
			storeIndex = m_StoreObjectCount;

		T& storeData = m_DataVec.at(m_StoreObjectCount);
		storeData = data;
		m_Changed = true;
		m_StoreObjectCount++;
#else
		if (!m_DataStore.IsCurentPosEndVaild())
			return false;

		T& storeData = m_DataStore.GetDataRef<uint32_t>(storeIndex);
		storeData = data;
		m_Changed = true;
		m_DataStore.Incroment();
#endif

		return true;
	}

	

	

	template<typename T>
	inline uint32_t BatchedRenderData<T>::GetCounter() const
	{
#if RY_DATA_VEC
		uint32_t count = m_StoreObjectCount;
#else
		uint32_t count = m_DataStore.GetCurentCount<uint32_t>()
#endif
		return count;
	}
	
	template<typename T>
	inline uint32_t BatchedRenderData<T>::GetByteSize() const
	{		
		constexpr uint32_t elmentByteSize = sizeof(T);

#if RY_DATA_VEC
		uint32_t count = m_StoreObjectCount;
		uint32_t byteSize = count * elmentByteSize;
#else
		uint32_t byteSize = m_DataStore.GetCurentByteSize<uint32_t>();

		uint32_t count = m_DataStore.GetCurentCount<uint32_t>()
		uint32_t byteSize2 = count * elmentByteSize;
		RY_CORE_ASSERT(byteSize == byteSize2, "difernt ByteSize!");
#endif

		return byteSize;
	}
	
	template<typename T>
	inline void Reset()
	{
#if RY_DATA_VEC
		m_StoreObjectCount = 0u;
#else
		m_DataStore.Reset();
#endif
	}


	template<typename T>
	inline void BatchedRenderData<T>::Clear()
	{
#if RY_DATA_VEC
		m_DataVec.clear();
#else
		RY_CORE_WARN("StoreSubmite can't not be Cleared! but we will reset the range");
#endif
		Reset();
	}

	
	

	template<typename T>
	inline const T *const BatchedRenderData<T>::GetDataPtr() const
	{
#if RY_DATA_VEC
		const T *const dataPtr = m_DataVec.data();
#else
		const T* dataPtr = m_DataStore.GetDataPtr();
#endif
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



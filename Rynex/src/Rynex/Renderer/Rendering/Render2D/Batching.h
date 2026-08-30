#pragma once

namespace Rynex {

	template<typename T>
	class Batching
	{
	public:

		Batching()
			: m_Size(0u)
			, m_AlecSize(0u)
			, m_PushSize(1u)
			, m_Data(nullptr)
			, m_Ptr(nullptr)
			, m_Begin(nullptr)
			, m_Ende(nullptr)
			, m_Changed(true)
		{
		};

		Batching(uint32_t size)
			: m_Size(0u)
			, m_AlecSize(0u)
			, m_PushSize(1u)
			, m_Data(nullptr)
			, m_Ptr(nullptr)
			, m_Begin(nullptr)
			, m_Ende(nullptr)
			, m_Changed(true)
		{
			Reserve(size);
		};

		virtual ~Batching()
		{
			Destroy();
		};
		

		virtual void Create(uint32_t count)
		{
			Reserve(count);
		};

		virtual void Destroy()
		{
			RestData();
			m_PushSize = 0u;
			m_Changed = true;
		};

		virtual void Resize2D(uint32_t size)
		{
			if (size > m_AlecSize)
			{
				AllocNewMemory(size);
				m_Size = m_AlecSize;
				m_Ptr = m_Ende;
			}
		};

		virtual void Reserve(uint32_t size)
		{
			uint32_t allocSize = m_Size + size;
			if (allocSize > m_AlecSize)
			{
				AllocNewMemory(allocSize);
			}
		};

		virtual uint32_t Size() const { return m_Size; }

		virtual uint32_t AlecSize() const { return m_AlecSize; }
		virtual bool HasChanaged() const {return m_Changed;}

		virtual T* Data() { return m_Data; }

		virtual T& Emplace_Back(T& element)
		{
			if(m_Ptr == m_Ende)
			{
				RY_CORE_ASSERT(false, "out of bounds!");
				return element;
			}

			T& elementBuffer = *m_Ptr;
			if (element != elementBuffer)
			{
				elementBuffer = element;
				m_Changed = true;
			}
			m_Ptr++;
			m_Size++;
			return element;
		}

		virtual bool Emplace_Back_IsFill(const T& element)
		{
			if (m_Ptr == m_Ende)
				return true;

			T& elementBuffer = *m_Ptr;
			if (element != elementBuffer)
			{
				elementBuffer = element;
				m_Changed = true;
			}
			m_Ptr++;
			m_Size++;
			return false;
		}

		virtual bool Emplace_Back_Push(const T& element)
		{
			
			bool needPush = m_Ptr == m_Ende;
			if (needPush && m_PushSize == 0)
				return true;

			else if (needPush)
				this->Reserve(m_PushSize);

			T& elementBuffer = *m_Ptr;
			elementBuffer = element;
			m_Changed = true;
			m_Ptr++;
			m_Size++;
			return needPush;
		}



		virtual bool MoveElementPtr(uint32_t size = 1)
		{
			bool needPush = m_Ptr == m_Ende;
			if (needPush && m_PushSize == 0)
				return true;

			else if (needPush)
				this->Reserve(m_PushSize);
			m_Ptr++;
			m_Size++;
			return needPush;
		}

		virtual void Clear()
		{ 
			m_Changed = false;
			m_Size = 0u; 
			m_Ptr = m_Begin; 
		}
		
		virtual void Clear(const T& target)
		{
			for (T* it = m_Begin; it != m_Ende; it++)
			{
				*it = target;
			}
			Clear();
		}

		virtual T& At(uint32_t index) { return m_Data[index]; }

		virtual void PushSize(uint32_t pushSize) { m_PushSize = pushSize; }
		virtual uint32_t GetPushSize() const { return m_PushSize; }
	private:
		void RestData()
		{
			if(m_Data != nullptr)
				delete[] m_Data;

			m_Data = nullptr;
			m_Begin = nullptr;
			m_Ende = nullptr;
			m_Ptr = nullptr;
			m_AlecSize = 0u;
			m_Size = 0u;
		}

		void SetData(T* data, uint32_t size, uint32_t offset)
		{
			RY_CORE_ASSERT(data != nullptr && size != 0, "No Ptr Adress!");
			RY_CORE_ASSERT(
				m_Data == nullptr 
				&& m_Begin == nullptr 
				&& m_Ende == nullptr 
				&& m_Ptr == nullptr 
				&& m_Size == 0 
				&& m_AlecSize == 0, 
				"Ptr Adress Set! Data: {}", (int64_t)m_Data);

			m_Data = data;
			m_Begin = m_Data;
			m_Ende = m_Begin + size;
			m_Ptr = m_Begin + offset;
			m_Size = offset;
			m_AlecSize = size;

			RY_CORE_ASSERT(
				m_Data != nullptr
				&& m_Begin != nullptr
				&& m_Ende != nullptr
				&& m_Ptr != nullptr
				&& m_AlecSize != 0,
				"Some Ptr Adress is not Set! Data: {}, Begin: {}, Ende: {}, Ptr: {}, Offset: {}",
				(int64_t)m_Data);
		}

		void CopyData(T* data)
		{
			std::memcpy(data, m_Data, m_Size * sizeof(T));
		}

		void AllocNewMemory(uint32_t size)
		{
			uint32_t offsetPtr = 0u;
			T* alloc = new T[size];
			if (m_AlecSize != 0u)
			{
				
				offsetPtr = m_Ptr - m_Begin;
				if (offsetPtr < size)
				{
					CopyData(alloc);
				}
				else if(offsetPtr > size)
				{
					offsetPtr = size - 1u;
				}
				RestData();
			}
			m_Changed = true;
			SetData(alloc, size, offsetPtr);
		}

	private:
		T* m_Data;
		T* m_Ptr;
		T* m_Begin;
		T* m_Ende;
		uint32_t m_Size;
		uint32_t m_AlecSize;
		uint32_t m_PushSize;
		bool m_Changed;
	};
	
	

}

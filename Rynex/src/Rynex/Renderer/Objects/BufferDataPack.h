#pragma once
#include <rypch.h>
#include <Rynex/Core/Range.h>
#include <Rynex/Core/MapVector.h>
#include <Rynex/Core/VectorMapElementRef.h>
namespace Rynex {

#define RY_BUFFER_PACKEGE_WARN_BLOCK 0

	template<typename T, typename N>
	class BufferDataPackage
	{
	public:
		using SizeType = typename uint32_t;
		using BindSlotType = typename uint32_t;

	public:
		BufferDataPackage(const BufferDataPackage& p)
			: m_Buffer(p.m_Buffer)
			, m_Update(true)
			, m_Data(p.m_Data)
		{
		}

		BufferDataPackage()
			: m_Buffer(nullptr)
			, m_Update(true)
			, m_Data(N())
		{
		}

		BufferDataPackage(const Ref<T>& buffer)
			: m_Buffer(buffer)
			, m_Update(true)
			, m_Data(N())
		{
		}


		BufferDataPackage(const Ref<T>& buffer, const N& data)
			: m_Buffer(buffer)
			, m_Update(true)
			, m_Data(data)
		{
		}

		BufferDataPackage(const N& data)
			: m_Buffer(nullptr)
			, m_Update(true)
			, m_Data(data)
		{
		}

		~BufferDataPackage()
		{
			DestroyPackege();
		}
		
		bool NeedUpdated() const { return m_Update; }
		bool NeedNotUpdated() const { return !m_Update; }
		bool HasBuffer() const { return nullptr != m_Buffer; }
		bool HasNoBuffer() const { return nullptr == m_Buffer; }
		bool IsRady() const { return (NeedNotUpdated() && HasBuffer()); }
		bool IsNotRady() const { return (NeedUpdated() || HasNoBuffer()); }
		operator bool() const { return !IsRady(); }
		
		const Ref<T>& GetBuffer()const { return m_Buffer; }
		const N& GetData()const { return m_Data; }

		void SetBuffer(const Ref<T>& buffer) { m_Update = true; m_Buffer = buffer; }
		void SetData(const N& data) { m_Update = true; m_Data = data; }
		
		template<typename U>
		void SetData(SizeType offsetByteSize, const U& data)
		{
			this->SetData(&data, offsetByteSize, sizeof(U));
		}


		void SetData(const void* dataPtr, SizeType offsetByteSize, SizeType byteSize)
		{
			SizeType bufferSizeExpectSize = offsetByteSize + byteSize;

			int differenz =  sizeof(N)-bufferSizeExpectSize;
			RY_CORE_ASSERT(sizeof(N) >= bufferSizeExpectSize, "Buffer Overfolwe by {} Bytes too large", (-differenz));
			SizeType copyByteSize = byteSize - offsetByteSize;

			m_Update = true;
			const void* offsetDataPtr = dataPtr + offset;
			N* offsetMemeberDataPtr = &m_Data;

			std::memcpy(offsetMemeberDataPtr, offsetDataPtr, copyByteSize);
		}

		N& ConfigData() { m_Update = true;  return m_Data; }

		void BindBuffer(BindSlotType slot)
		{ 
			if (m_Update)
				LoadeDataUp(); 
			m_Buffer->Bind(slot); 
		}

		void UnBind()
		{
			m_Buffer->UnBind();
		}

		void DestroyPackege()
		{
			RY_DESTROY_REF(m_Buffer);
			m_Update = true;
		}

		void ForceLoadeAllDataUpNeeded()
		{
			if (m_Update)
				LoadeDataUp();
		}

		void ForceLoadeAllDataUp()
		{
			LoadeDataUp();
		}

		void ForceStateDataUptoData()
		{
			m_Update = false;
		}
	private:
		void LoadeDataUp()
		{
			m_Buffer->SetData(&m_Data, sizeof(N));
			m_Update = false;
		}

	private:
		Ref<T> m_Buffer;
		N m_Data;
		bool m_Update;

	};


	template<typename T, typename N /*, typename Array = std::vector<N>*/ >
	class BufferArrayPackage
	{
	public:
		using _T = typename T;
		using _N = typename N;

		using SizeType = typename uint32_t;
		using DifferenceType = typename int32_t;

		using Array = typename std::vector<typename N>;
		using ArrayIterator = typename Array::iterator;
		using ArrayConstIterator = typename Array::const_iterator;
	public:
		BufferArrayPackage(const BufferArrayPackage& p)
			: m_Buffer(p.m_Buffer)
			, m_ArrayData(p.m_ArrayData)
			, m_Range(p.m_Range)
			, m_Update(p.m_Update)
		{
		}

		BufferArrayPackage()
			: m_Buffer(nullptr)
			, m_ArrayData()
			, m_Range(MaxRange())
			, m_Update(true)
		{
		}

		BufferArrayPackage(const Ref<_T>& buffer)
			: m_Buffer(buffer)
			, m_ArrayData()
			, m_Range(MaxRange())
			, m_Update(true)
		{
		}


		BufferArrayPackage(const Ref<_T>& buffer, const Array& arrayData)
			: m_Buffer(buffer)
			, m_ArrayData(arrayData)
			, m_Range(Range::MaxRange())
			, m_Update(true)
		{
			ArrayRange();
		}

		BufferArrayPackage(const Ref<_T>& buffer, const Array& arrayData, bool update)
			: m_Buffer(buffer)
			, m_ArrayData(arrayData)
			, m_Range(MaxRange())
			, m_Update(update)
		{
			if(m_Update)
				ArrayRange();
		}

		BufferArrayPackage(const Array& arrayData)
			: m_Buffer(nullptr)
			, m_ArrayData(arrayData)
			, m_Range(MaxRange())
			, m_Update(true)
		{
			ArrayRange();
		}

		~BufferArrayPackage()
		{
			DestroyPackege();
		}

		bool NeedUpdated() const { return m_Range != MaxRange(); }
		bool NeedNotUpdated() const { return m_Range == MaxRange(); }
		bool HasBuffer() const { return nullptr != m_Buffer; }
		bool HasNoBuffer() const { return nullptr == m_Buffer; }
		bool IsRady() const { return (NeedNotUpdated() && HasBuffer()); }
		bool IsNotRady() const { return (NeedUpdated() || HasNoBuffer()); }
		bool EmptyArray() const { return m_ArrayData.empty(); }
		operator bool() const { return !IsRady(); }

		typename const Ref<typename _T>& GetBuffer() const { return m_Buffer; }
		const Array& GetArrayData() const { return m_ArrayData; }

		const N& GetElementData(SizeType index) const { return m_ArrayData.at(index); }
		N& GetElementData(SizeType index) { SetRange(index);  return m_ArrayData.at(index); }

		void SetBuffer(const Ref<_T>& buffer) { SetRange(0u, m_ArrayData.size()); m_Buffer = buffer; }
		void SetData(const Array& arrayData) { SetRange(0u, arrayData.size()); m_ArrayData = arrayData; }
		SizeType CountElements() const { return m_ArrayData.size(); }
		SizeType GetByteSize() const { return m_ArrayData.size() * sizeof(N); }
		template<typename U, typename ArrayU>
		void SetData(SizeType offsetCount, const ArrayU& arrayData)
		{
			SizeType ellementByteSize = sizeof(U);
			SizeType offsetByteSize = ellementByteSize * offsetCount;
			SizeType count = arrayData.size();
			SizeType byteSize = ellementByteSize * count;

			const void* dataPtr = arrayData.data();
			this->SetData(dataPtr, offsetByteSize, byteSize);
		}


		void SetData(const void* dataPtr, SizeType offsetByteSize, SizeType byteSize)
		{
			int bufferSizeExpectSize = offsetByteSize + byteSize;

			SizeType elmentByteSize = sizeof(N);
			SizeType count = m_ArrayData.size();

			SizeType arrayByteSize = elmentByteSize * count;

			DifferenceType differenz = arrayByteSize - bufferSizeExpectSize;
			RY_CORE_ASSERT(elmentByteSize >= bufferSizeExpectSize, "Buffer Overfolwe by {} Bytes too large", (-differenz));
			SizeType copyByteSize = byteSize - offsetByteSize;

			const void* offsetDataPtr = dataPtr + offsetByteSize;
			void* offsetMemeberDataPtr = m_ArrayData.data();

			SizeType ellmentOffset = ByteSizeInElementCount(offsetByteSize);
			SizeType ellmentCount = ByteSizeInElementCount(byteSize);

			SizeType ellmentCopyBegin = ellmentOffset;
			SizeType ellmentCopyEnde = ellmentOffset + ellmentCount;
			
			this->SetRange(ellmentCopyBegin, ellmentCopyEnde);

			std::memcpy(offsetMemeberDataPtr, offsetDataPtr, copyByteSize);
		}

		void SetData(const N& elementData, SizeType index)
		{
			SizeType count = m_ArrayData.size();
			DifferenceType ellmentCountDifferz = count - static_cast<int>(index);
			RY_CORE_ASSERT(index < count, "Overflow buffer size by {} elements", -(ellmentCountDifferz));
			N& arrayElement = m_ArrayData.at(index);
			arrayElement = elementData;
			this->SetRange(index);
			m_Update = true;
		}

		Array& ConfigData() { m_Update = true;  return m_ArrayData; }

		void Resize2D(SizeType size)
		{
			SizeType oldSize = m_ArrayData.size();
			m_ArrayData.resize(size);
			SizeType newSize = m_ArrayData.size();

			SetRange(oldSize, newSize);
		}

		void Erase(SizeType index)
		{
			ArrayIterator it = m_ArrayData.begin() + index;

			m_ArrayData.erase(it);
			SizeType size = m_ArrayData.size();

			SetRange(index, size);
		}

		void Erase(SizeType begin, SizeType ende)
		{
			ArrayIterator itBegin = m_ArrayData.begin() + begin;
			ArrayIterator itEnde = m_ArrayData.begin() + ende;
			m_ArrayData.erase(itBegin, itEnde);
			SizeType size = m_ArrayData.size();

			SetRange(begin, size);
		}

		void BindBuffer(SizeType slot)
		{
			if (m_Update)
				LoadeDataUpAll();

			m_Buffer->Bind(slot);
		}

		void BindBufferUpdateRange(SizeType slot)
		{
			if (m_Update)
				LoadeDataUpOffset();

			m_Buffer->Bind(slot);
		}

		void UnBind()
		{
			m_Buffer->UnBind();
		}

		void DestroyPackege()
		{
			RY_DESTROY_REF(m_Buffer);
			ResetRange();
			m_ArrayData.clear();
			m_ArrayData.shrink_to_fit();
		}

		void ForceLoadeAllDataUpNeeded()
		{
			if (m_Update)
				LoadeDataUpAll();
		}

		void ForceLoadeRangeDataUpNeeded()
		{
			if (m_Update)
				LoadeDataUpOffset();
		}
		

		void ForceLoadeAllDataUp()
		{
			RY_CORE_WARN("We Imediedt Push The Date ({}) to Buffer ({}) withe oute Binding!", s_DataNameStr, s_BufferNameStr);
			LoadeDataUpAll();
		}

		void ForceLoadeRangeDataUp()
		{
			RY_CORE_WARN("We Imediedt Push The Date ({}) to Buffer ({}) withe oute Binding!", s_DataNameStr, s_BufferNameStr);
			LoadeDataUpOffset();
		}

		void ForceStateDataUptoData()
		{
			RY_CORE_WARN("We Imediedt Skip The Data ({}) to be Uploadedt tu Buffer ({}) Untile the Data change another Time!", s_DataNameStr, s_BufferNameStr);
			ResetRange();
		}

	private:
		SizeType ByteSizeInElementCount(SizeType byteSize)
		{
			SizeType elmentByteSize = sizeof(N);
			SizeType v = elmentByteSize / byteSize;
			return v;
		}

		template<typename U, typename ArrayU>
		static SizeType GetBytesSizeArra(const ArrayU& arrayData)
		{
			SizeType elementSize = sizeof(U);
			SizeType arrayCount = arrayData.size();
			SizeType v = elementSize * arrayCount;
			return v;
		}

		static SizeType CopyOffsetByteSize(SizeType byteOffset, SizeType byteSize)
		{
			SizeType v = byteOffset;
			return v;
		}

		static SizeType CopyByteSize(SizeType byteOffset, SizeType byteSize)
		{
			SizeType v = byteSize;
			return v;
		}

		static SizeType CopyBeginByte(SizeType byteOffset, SizeType byteSize)
		{
			SizeType v = byteOffset;
			return v;
		}

		static SizeType CopyEndeByte(SizeType byteOffset, SizeType byteSize)
		{
			SizeType v = byteOffset + byteSize;
			return v;
		}

		void CheckCountSizeDiffernz(SizeType size, SizeType offset = 0)
		{
			SizeType rangeCount = size + offset;
			SizeType arrayCount = m_ArrayData.size();
			DifferenceType ellmentCountDifferz = arrayCount - rangeCount;
			
			RY_CORE_ASSERT(rangeCount < arrayCount, "Overflow on Index Size withe {}", rangeCount);
			RY_CORE_ASSERT(ellmentCountDifferz <= 1, "Overflow buffer size by {} elements", -(ellmentCountDifferz));
		}

		void CheckByteSizeDiffernz(SizeType byteSize, SizeType byteOffset = 0)
		{
			SizeType byteSizeEllemnt = sizeof(N);
			SizeType arrayCount = m_ArrayData.size();

			SizeType byteRange = byteSize + byteOffset;
			SizeType byteArraySize = arrayCount * byteSizeEllemnt;
			DifferenceType ellmentByteSizeDifferz = byteArraySize - byteRange;

			RY_CORE_ASSERT(byteRange < byteArraySize, "Overflow on Index Size with {}", index);
			RY_CORE_ASSERT(ellmentByteSizeDifferz <= 1, "Overflow buffer size by {} elements", -(ellmentByteSizeDifferz));
		}

		void SetRange(SizeType index)
		{
			m_Update = true;
			Range(m_Range, index);
		}

		void SetRange(SizeType begin, SizeType ende)
		{
			m_Update = true;
			m_Range = Range(begin, ende);
		}

		template<typename ArrayU>
		void SetFromRange(const ArrayU& arrayData, SizeType offset = 0u)
		{
			SizeType size = arrayData.size();
			SetRange(offset, size);
		}

		void ResetRange()
		{
			SizeType begin = 0u;
			SizeType ende = m_ArrayData.size();
			m_Range = Range(ende, begin);
			m_Update = false;
		}

		void ArrayRange()
		{
			SizeType begin = 0u;
			SizeType ende = m_ArrayData.size();
			SetRange(begin, ende);
		}

		void LoadeDataUpOffset()
		{
			const void* dataPtr = m_ArrayData.data();
			RY_CORE_ASSERT(m_Range.y >= m_ArrayData.size(), "Greter Range Than Data!");

			SizeType elmentByteSize = sizeof(N);
			SizeType completByteCopySize = m_Range.y * elmentByteSize;

			SizeType bufferByteSize = m_Buffer->GetByteSize();
			if (completByteCopySize <= bufferByteSize)
			{
				SizeType copyCount = m_Range.x - m_Range.y;
				SizeType byteCopySizeOffset = m_Range.x * elmentByteSize;
				SizeType byteCopySize = copyCount * elmentByteSize;

				m_Buffer->SetData(dataPtr, byteCopySizeOffset, byteCopySize);
			}
			else
			{
				SizeType arrayCount = m_ArrayData.size();
				SizeType copyByteSize = arrayCount * elmentByteSize;

				m_Buffer->Resize2D(dataPtr, byteCopySize);
			}
			
			ResetRange();
		}

		void LoadeDataUpAll()
		{
			const void* dataPtr = m_ArrayData.data();

			SizeType elmentByteSize = sizeof(N);
			SizeType count = m_ArrayData.size();

			SizeType byteCopySize = count * elmentByteSize;
			SizeType bufferByteSize = m_Buffer->GetByteSize();

			if(byteCopySize == bufferByteSize)
				m_Buffer->SetData(dataPtr, byteCopySize);
			else
				m_Buffer->Resize2D(dataPtr, byteCopySize);
			ResetRange();
		}

				
	private:
		bool m_Update;
		Ref<T> m_Buffer;
		Array m_ArrayData;
		Range m_Range;

	};


	template<typename T, typename _Key,typename N /*, typename Array = std::vector<N>*/ >
	class BufferArrayMapPackage
	{
	public:
		using ArrayMap = typename MapVector<typename _Key, typename N>;
		using ArrayMapIterator = typename ArrayMap::VectorValueTypeIterartor;
		using ArrayMapConstIterator = typename ArrayMap::VectorValueTypeIterartor_Const;
	public:
		BufferArrayMapPackage(const BufferArrayMapPackage& p)
			: m_Buffer(p.m_Buffer)
			, m_ArrayData(p.m_ArrayData)
			, m_Range(p.m_Range)
			, m_Update(p.m_Update)
		{
		}

		BufferArrayMapPackage()
			: m_Buffer(nullptr)
			, m_ArrayData()
			, m_Range(MaxRange())
			, m_Update(true)
		{
		}

		BufferArrayMapPackage(const Ref<T>& buffer)
			: m_Buffer(buffer)
			, m_ArrayData()
			, m_Range(MaxRange())
			, m_Update(true)
		{
		}


		BufferArrayMapPackage(const Ref<T>& buffer, const ArrayMap& arrayData)
			: m_Buffer(buffer)
			, m_ArrayData(arrayData)
			, m_Range(MaxRange())
			, m_Update(true)
		{
			ArrayRange();
		}

		BufferArrayMapPackage(const Ref<T>& buffer, const ArrayMap& arrayData, bool update)
			: m_Buffer(buffer)
			, m_ArrayData(arrayData)
			, m_Range(MaxRange())
			, m_Update(update)
		{
			if (m_Update)
				ArrayRange();
		}

		BufferArrayMapPackage(const ArrayMap& arrayData)
			: m_Buffer(nullptr)
			, m_ArrayData(arrayData)
			, m_Range(MaxRange())
			, m_Update(true)
		{
			ArrayRange();
		}

		~BufferArrayMapPackage()
		{
			DestroyPackege();
		}

		bool NeedUpdated() const { return m_Range != MaxRange(); }
		bool NeedNotUpdated() const { return m_Range == MaxRange(); }
		bool HasBuffer() const { return nullptr != m_Buffer; }
		bool HasNoBuffer() const { return nullptr == m_Buffer; }
		bool IsRady() const { return (NeedNotUpdated() && HasBuffer()); }
		bool IsNotRady() const { return (NeedUpdated() || HasNoBuffer()); }
		bool EmptyArray() const { return m_ArrayData.Empty(); }

		bool HasKey(const _Key& key) const { return m_ArrayData.HasKey(key); }

		operator bool() const { return !IsRady(); }

		typename const Ref<typename T>& GetBuffer() const { return m_Buffer; }
		const ArrayMap& GetArrayData() const { return m_ArrayData; }

		const N& GetElementDataKey(const _Key& key) const
		{
			return m_ArrayData.GetDataFromKey(key);
		}
		
		N& GetElementDataKey(const _Key& key) 
		{   
			uint32_t index = m_ArrayData.GetIndex(key);
			SetRange(index);  
			return m_ArrayData.GetDataFromIndex(index);
		}

		const N& GetElementDataIndex(uint32_t index) const
		{
			return m_ArrayData.GetDataFromIndex(index);
		}

		N& GetElementDataIndex(uint32_t index)
		{
			SetRange(index);
			return m_ArrayData.GetDataFromIndex(index);
		}

		const _Key& GetElementKey(uint32_t index) const
		{
			return m_ArrayData.GetKey(index);
		}

		
		uint32_t GetKeyElementIndex(const _Key& key) const
		{
			return m_ArrayData.GetIndex(key);
		}
		

		N& AddElementData(const _Key& key, const N& data)
		{
			RY_CORE_ASSERT(!m_ArrayData.HasKey(key));
			uint32_t index = m_ArrayData.AddDataIndex(key, data);
			SetRange(index);
			return m_ArrayData.GetDataFromKey(key);
		}

		N& AddElementData(const _Key& key, N& data)
		{
			RY_CORE_ASSERT(!m_ArrayData.HasKey(key));
			uint32_t index = m_ArrayData.AddDataIndex(key, data);
			SetRange(index);
			return m_ArrayData.GetDataFromKey(key);
		}

		uint32_t AddElementDataIndex(const _Key& key, const N& data)
		{
			RY_CORE_ASSERT(!m_ArrayData.HasKey(key));
			uint32_t index = m_ArrayData.AddDataIndex(key, data);
			SetRange(index);
			return index;
		}

		uint32_t AddElementDataIndex(const _Key& key, N& data)
		{
			RY_CORE_ASSERT(!m_ArrayData.HasKey(key));
			uint32_t index = m_ArrayData.AddDataIndex(key, data);
			SetRange(index);
			return index;
		}

		void SetBuffer(const Ref<T>& buffer) 
		{ 
			SetRange(0u, m_ArrayData.Size()); 
			m_Buffer = buffer;
		}

		void SetData(const ArrayMap& arrayData) 
		{
			SetRange(0u, arrayData.Size()); 
			m_ArrayData = arrayData; 
		}

		void ChangeKey(const _Key& fromKey, const _Key& toKey)
		{
			m_ArrayData.ChangeKey(fromKey, toKey);
		}

		uint32_t CountElements() const
		{
			return m_ArrayData.Size(); 
		}

		uint32_t GetByteSize() const 
		{ 
			return m_ArrayData.Size() * sizeof(N); 
		}

		template<typename U, typename ArrayU>
		void SetData(uint32_t offsetCount, const ArrayU& arrayData)
		{
			uint32_t ellementByteSize = sizeof(U);
			uint32_t offsetByteSize = ellementByteSize * offsetCount;
			uint32_t count = arrayData.size();
			uint32_t byteSize = ellementByteSize * count;

			const void* dataPtr = arrayData.data();
			this->SetData(dataPtr, offsetByteSize, byteSize);
		}


		void SetData(const void* dataPtr, uint32_t offsetByteSize, uint32_t byteSize)
		{
			int bufferSizeExpectSize = offsetByteSize + byteSize;

			uint32_t elmentByteSize = sizeof(N);
			uint32_t count = m_ArrayData.Size();

			int arrayByteSize = elmentByteSize * count;

			int differenz = arrayByteSize - bufferSizeExpectSize;
			RY_CORE_ASSERT(elmentByteSize >= bufferSizeExpectSize, "Buffer Overfolwe by {} Bytes too large", (-differenz));
			uint32_t copyByteSize = byteSize - offsetByteSize;
			std::vector<N>& vecData = m_ArrayData.GetVector();
			const void* offsetDataPtr = dataPtr + offsetByteSize;
			void* offsetMemeberDataPtr = vecData.data();

			uint32_t ellmentOffset = ByteSizeInElementCount(offsetByteSize);
			uint32_t ellmentCount = ByteSizeInElementCount(byteSize);

			uint32_t ellmentCopyBegin = ellmentOffset;
			uint32_t ellmentCopyEnde = ellmentOffset + ellmentCount;

			this->SetRange(ellmentCopyBegin, ellmentCopyEnde);

			std::memcpy(offsetMemeberDataPtr, offsetDataPtr, copyByteSize);
		}

		void SetData(const N& elementData, const _Key& key)
		{
			if (!m_ArrayData.HasKey(key))
				return;

			N& arrayElement = m_ArrayData.GetDataFromKey(key);
			arrayElement = elementData;
			this->SetRange(index);
			m_Update = true;
		}

		ArrayMap& ConfigData() { m_Update = true;   return m_ArrayData; }

		const N* GetDataPtr() const { return m_ArrayData.Data(); }

		void EraseIndex(uint32_t index)
		{
			m_ArrayData.EraseFromIndex(index);
			uint32_t size = m_ArrayData.Size();
			SetRange(index, size);
		}

		void EraseKey(const _Key& key)
		{
			uint32_t index = m_ArrayData.GetIndex(key);
			m_ArrayData.EraseFromKey(key);
			uint32_t size = m_ArrayData.Size();
			SetRange(index, size);
			
		}

		

		void BindBuffer(uint32_t slot)
		{
			if (m_Update)
				LoadeDataUpAll();

			m_Buffer->Bind(slot);
		}

		void BindBufferUpdateRange(uint32_t slot)
		{
			if (m_Update)
				LoadeDataUpOffset();

			m_Buffer->Bind(slot);
		}

		void UnBind()
		{
			m_Buffer->UnBind();
		}

		void DestroyPackege()
		{
			RY_DESTROY_REF(m_Buffer);
			ResetRange();
			m_ArrayData.Clear();
		}

		void ForceLoadeAllDataUpNeeded()
		{
			if (m_Update)
				LoadeDataUpAll();
		}

		void ForceLoadeRangeDataUpNeeded()
		{
			if (m_Update)
				LoadeDataUpOffset();
		}


		void ForceLoadeAllDataUp()
		{
			RY_CORE_WARN("We Imediedt Push The Date ({}) to Buffer ({}) withe oute Binding!", s_DataNameStr, s_BufferNameStr);
			LoadeDataUpAll();
		}

		void ForceLoadeRangeDataUp()
		{
			RY_CORE_WARN("We Imediedt Push The Date ({}) to Buffer ({}) withe oute Binding!", s_DataNameStr, s_BufferNameStr);
			LoadeDataUpOffset();
		}

		void ForceStateDataUptoData()
		{
			RY_CORE_WARN("We Imediedt Skip The Data ({}) to be Uploadedt to Buffer ({}) Untile the Data change another Time!", s_DataNameStr, s_BufferNameStr);
			ResetRange();
		}

		static glm::uvec2 MaxRange()
		{
			glm::uvec2 rangeMax = glm::uvec2{ static_cast<uint32_t>(-1), 0u };
			return rangeMax;
		}

		static void Range(glm::uvec2& range, uint32_t index)
		{
			uint32_t x = range.x > index ? index : range.x;
			uint32_t iAdd = index + 1u;
			uint32_t y = range.y < (iAdd) ? (iAdd) : range.y;
			range = glm::uvec2{ x,  y };
		}

	private:
		uint32_t ByteSizeInElementCount(uint32_t byteSize)
		{
			uint32_t elmentByteSize = sizeof(N);
			uint32_t v = elmentByteSize / byteSize;
			return v;
		}

		template<typename U, typename ArrayU>
		static uint32_t GetBytesSizeArra(const ArrayU& arrayData)
		{
			uint32_t elementSize = sizeof(U);
			uint32_t arrayCount = arrayData.Size();
			uint32_t v = elementSize * arrayCount;
			return v;
		}

		static uint32_t CopyOffsetByteSize(uint32_t byteOffset, uint32_t byteSize)
		{
			uint32_t v = byteOffset;
			return v;
		}

		static uint32_t CopyByteSize(uint32_t byteOffset, uint32_t byteSize)
		{
			uint32_t v = byteSize;
			return v;
		}

		static uint32_t CopyBeginByte(uint32_t byteOffset, uint32_t byteSize)
		{
			uint32_t v = byteOffset;
			return v;
		}

		static uint32_t CopyEndeByte(uint32_t byteOffset, uint32_t byteSize)
		{
			uint32_t v = byteOffset + byteSize;
			return v;
		}

		void CheckCountSizeDiffernz(uint32_t size, uint32_t offset = 0)
		{
			int rangeCount = static_cast<int>(size + offset);
			int arrayCount = m_ArrayData.Size();
			int ellmentCountDifferz = arrayCount - rangeCount;

			RY_CORE_ASSERT(rangeCount < arrayCount, "Overflow on Index Size withe {}", rangeCount);
			RY_CORE_ASSERT(ellmentCountDifferz <= 1, "Overflow buffer size by {} elements", -(ellmentCountDifferz));
		}

		void CheckByteSizeDiffernz(uint32_t byteSize, uint32_t byteOffset = 0)
		{
			uint32_t byteSizeEllemnt = sizeof(N);
			uint32_t arrayCount = m_ArrayData.Size();

			int byteRange = static_cast<int>(byteSize + byteOffset);
			int byteArraySize = arrayCount * byteSizeEllemnt;
			int ellmentByteSizeDifferz = byteArraySize - byteRange;

			RY_CORE_ASSERT(byteRange < byteArraySize, "Overflow on Index Size with {}", index);
			RY_CORE_ASSERT(ellmentByteSizeDifferz <= 1, "Overflow buffer size by {} elements", -(ellmentByteSizeDifferz));
		}

		void SetRange(uint32_t index)
		{
			m_Update = true;
			Range(m_Range, index);
		}

		void SetRange(uint32_t begin, uint32_t ende)
		{
			m_Update = true;
			Range(m_Range, begin);
			Range(m_Range, ende);
		}

		template<typename ArrayU>
		void SetFromRange(const ArrayU& arrayData, uint32_t offset = 0u)
		{
			uint32_t size = arrayData.Size();
			SetRange(offset, size);
		}

		void ResetRange()
		{
			m_Range = MaxRange();
			m_Update = false;
		}

		void ArrayRange()
		{
			uint32_t begin = 0u;
			uint32_t ende = m_ArrayData.Size();
			SetRange(begin, ende);
		}

		void LoadeDataUpOffset()
		{
			
			std::vector<N>& vecData = m_ArrayData.GetVector();
			RY_CORE_ASSERT(m_Range.y >= vecData.size(), "Greter Range Than Data!");
			const void* dataPtr = vecData.data();

			uint32_t elmentByteSize = sizeof(N);

			uint32_t completByteCopySize = m_Range.y * elmentByteSize;
			uint32_t bufferByteSize = m_Buffer->GetByteSize();

			if (completByteCopySize <= bufferByteSize)
			{
				uint32_t copyCount = m_Range.x - m_Range.y;
				uint32_t byteCopySizeOffset = m_Range.x * elmentByteSize;
				uint32_t byteCopySize = copyCount * elmentByteSize;

				m_Buffer->SetData(dataPtr, byteCopySizeOffset, byteCopySize);
			}
			else
			{
				uint32_t arrayCount = m_ArrayData.size();
				uint32_t copyByteSize = arrayCount * elmentByteSize;

				m_Buffer->Resize2D(dataPtr, byteCopySize);
			}

			ResetRange();
		}

		void LoadeDataUpAll()
		{
			std::vector<N>& vecData = m_ArrayData.GetVector();
			const void* dataPtr = vecData.data();

			uint32_t elmentByteSize = sizeof(N);
			uint32_t count = m_ArrayData.Size();
			uint32_t byteCopySize = count * elmentByteSize;
			uint32_t bufferByteSize = m_Buffer->GetByteSize();

			if (byteCopySize == bufferByteSize)
				m_Buffer->SetData(dataPtr, byteCopySize);
			else
				m_Buffer->Resize2D(dataPtr, byteCopySize);

			ResetRange();
		}

		constexpr static std::string GetBufferName()
		{
			std::string str = "Ref<";
			std::string typeName = typeid(T).name();
			typeName.erase(typeName.begin(), typeName.begin() + 6);
			str += typeName;
			str += ">";
			return str;
		}

		constexpr static std::string GetDataName()
		{
			std::string str = typeid(N).name();
			if (str.size() > 7)
				str.erase(str.begin(), str.begin() + 7);
			return str;
		}
	private:
		bool m_Update;
		Ref<T> m_Buffer;
		ArrayMap m_ArrayData;
		glm::uvec2 m_Range;

		inline static std::string s_BufferNameStr = GetBufferName();
		inline static std::string s_DataNameStr = GetDataName();
	};
	
	


	template<typename T, typename _Key, typename N /*, typename Array = std::vector<N> */>
	class BufferArrayMapElementPtrPackage
	{
	public:
		using ArrayMap = typename MapVectorElementPtr<typename _Key, typename N>;
		using ArrayRefPtrValue = typename Ref<typename ElementVecPtr<typename N>>;
		using ArrayPtrValue = typename ElementVecPtr<typename N>;

		using ArrayMapIterator = typename ArrayMap::VectorValueTypeIterartor;
		using ArrayMapConstIterator = typename ArrayMap::VectorValueTypeIterartor_Const;
	public:
		BufferArrayMapElementPtrPackage(const BufferArrayMapElementPtrPackage& p)
			: m_Buffer(p.m_Buffer)
			, m_ArrayData(p.m_ArrayData)
			, m_Range(p.m_Range)
			, m_Update(p.m_Update)
		{
		}

		BufferArrayMapElementPtrPackage()
			: m_Buffer(nullptr)
			, m_ArrayData()
			, m_Range(MaxRange())
			, m_Update(true)
		{
		}

		BufferArrayMapElementPtrPackage(const Ref<T>& buffer)
			: m_Buffer(buffer)
			, m_ArrayData()
			, m_Range(MaxRange())
			, m_Update(true)
		{
		}


		BufferArrayMapElementPtrPackage(const Ref<T>& buffer, const ArrayMap& arrayData)
			: m_Buffer(buffer)
			, m_ArrayData(arrayData)
			, m_Range(MaxRange())
			, m_Update(true)
		{
			ArrayRange();
		}

		BufferArrayMapElementPtrPackage(const Ref<T>& buffer, const ArrayMap& arrayData, bool update)
			: m_Buffer(buffer)
			, m_ArrayData(arrayData)
			, m_Range(MaxRange())
			, m_Update(update)
		{
			if (m_Update)
				ArrayRange();
		}

		BufferArrayMapElementPtrPackage(const ArrayMap& arrayData)
			: m_Buffer(nullptr)
			, m_ArrayData(arrayData)
			, m_Range(MaxRange())
			, m_Update(true)
		{
			ArrayRange();
		}

		~BufferArrayMapElementPtrPackage()
		{
			DestroyPackege();
		}

		bool NeedUpdated() const { return m_Range != MaxRange(); }
		bool NeedNotUpdated() const { return m_Range == MaxRange(); }
		bool HasBuffer() const { return nullptr != m_Buffer; }
		bool HasNoBuffer() const { return nullptr == m_Buffer; }
		bool IsRady() const { return (NeedNotUpdated() && HasBuffer()); }
		bool IsNotRady() const { return (NeedUpdated() || HasNoBuffer()); }
		bool EmptyArray() const { return m_ArrayData.Empty(); }

		bool HasKey(const _Key& key) const { return m_ArrayData.HasKey(key); }

		operator bool() const { return !IsRady(); }

		typename const Ref<typename T>& GetBuffer() const { return m_Buffer; }
		const ArrayMap& GetArrayData() const { return m_ArrayData; }

		const N& GetElementDataKey(const _Key& key) const
		{
			return m_ArrayData.GetDataFromKey(key);
		}

		N& GetElementDataKey(const _Key& key)
		{
			uint32_t index = m_ArrayData.GetElementRef(key);
			SetRangeIndex(index);
			return m_ArrayData.GetDataFromIndex(index);
		}

		const N& GetElementDataIndex(uint32_t index) const
		{
			return m_ArrayData.GetDataFromIndex(index);
		}

		N& GetElementDataIndex(uint32_t index)
		{
			SetRangeIndex(index);
			return m_ArrayData.GetDataFromIndex(index);
		}

		const _Key& GetElementKey(uint32_t index) const
		{
			return m_ArrayData.GetKey(index);
		}


		uint32_t GetKeyElementIndex(const _Key& key) const
		{
			return m_ArrayData.GetIndex(key);
		}


		Ref<ArrayPtrValue> AddElementData(const _Key& key, const N& data)
		{
			RY_CORE_ASSERT(!m_ArrayData.HasKey(key));
			Ref<ArrayPtrValue> valuePtr = m_ArrayData.AddData(key, data);
			
			uint32_t index = valuePtr->GetIndex();
			SetRangeIndex(index);
			std::function<void(uint32_t index)> func = std::bind(&BufferArrayMapElementPtrPackage<T, _Key, N>::SetRangeIndex, this, std::placeholders::_1);
			valuePtr->AddChangeFunc(func);
			return valuePtr;
		}


		void SetBuffer(const Ref<T>& buffer)
		{
			SetRange(0u, m_ArrayData.Size());
			m_Buffer = buffer;
		}

		void SetData(const ArrayMap& arrayData)
		{
			SetRange(0u, arrayData.Size());
			m_ArrayData = arrayData;
		}

		void ChangeKey(const _Key& fromKey, const _Key& toKey)
		{
			m_ArrayData.ChangeKey(fromKey, toKey);
		}

		uint32_t CountElements() const
		{
			return m_ArrayData.Size();
		}

		uint32_t GetByteSize() const
		{
			return m_ArrayData.Size() * sizeof(N);
		}

		template<typename U, typename ArrayU>
		void SetData(uint32_t offsetCount, const ArrayU& arrayData)
		{
			uint32_t ellementByteSize = sizeof(U);
			uint32_t offsetByteSize = ellementByteSize * offsetCount;
			uint32_t count = arrayData.size();
			uint32_t byteSize = ellementByteSize * count;

			const void* dataPtr = arrayData.data();
			this->SetData(dataPtr, offsetByteSize, byteSize);
		}


		void SetData(const void* dataPtr, uint32_t offsetByteSize, uint32_t byteSize)
		{
			int bufferSizeExpectSize = offsetByteSize + byteSize;

			uint32_t elmentByteSize = sizeof(N);
			uint32_t count = m_ArrayData.Size();

			int arrayByteSize = elmentByteSize * count;

			int differenz = arrayByteSize - bufferSizeExpectSize;
			RY_CORE_ASSERT(elmentByteSize >= bufferSizeExpectSize, "Buffer Overfolwe by {} Bytes too large", (-differenz));
			uint32_t copyByteSize = byteSize - offsetByteSize;
			std::vector<N>& vecData = m_ArrayData.GetVector();
			const void* offsetDataPtr = dataPtr + offsetByteSize;
			void* offsetMemeberDataPtr = vecData.data();

			uint32_t ellmentOffset = ByteSizeInElementCount(offsetByteSize);
			uint32_t ellmentCount = ByteSizeInElementCount(byteSize);

			uint32_t ellmentCopyBegin = ellmentOffset;
			uint32_t ellmentCopyEnde = ellmentOffset + ellmentCount;

			this->SetRange(ellmentCopyBegin, ellmentCopyEnde);

			std::memcpy(offsetMemeberDataPtr, offsetDataPtr, copyByteSize);
		}

		void SetData(const N& elementData, const _Key& key)
		{
			if (!m_ArrayData.HasKey(key))
				return;

			N& arrayElement = m_ArrayData.GetDataFromKey(key);
			arrayElement = elementData;
			this->SetRangeIndex(index);
			m_Update = true;
		}

		ArrayMap& ConfigData() { m_Update = true; return m_ArrayData; }

		const N* GetDataPtr() const { return m_ArrayData.Data(); }

		void EraseIndex(uint32_t index)
		{
			m_ArrayData.EraseFromIndex(index);
			uint32_t size = m_ArrayData.Size();
			SetRange(index, size);
		}

		void EraseKey(const _Key& key)
		{
			uint32_t index = m_ArrayData.GetIndex(key);
			m_ArrayData.EraseFromKey(key);
			uint32_t size = m_ArrayData.Size();
			SetRange(index, size);

		}



		void BindBuffer(uint32_t slot)
		{
			if (m_Update)
				LoadeDataUpAll();

			m_Buffer->Bind(slot);
		}

		void BindBufferUpdateRange(uint32_t slot)
		{
			if (m_Update)
				LoadeDataUpOffset();

			m_Buffer->Bind(slot);
		}

		void UnBind()
		{
			m_Buffer->UnBind();
		}

		void DestroyPackege()
		{
			RY_DESTROY_REF(m_Buffer);
			ResetRange();
			m_ArrayData.Clear();
		}

		void ForceLoadeAllDataUpNeeded()
		{
			if (m_Update)
				LoadeDataUpAll();
		}

		void ForceLoadeRangeDataUpNeeded()
		{
			if (m_Update)
				LoadeDataUpOffset();
		}


		void ForceLoadeAllDataUp()
		{
			RY_CORE_WARN("We Imediedt Push The Date ({}) to Buffer ({}) withe oute Binding!", s_DataNameStr, s_BufferNameStr);
			LoadeDataUpAll();
		}

		void ForceLoadeRangeDataUp()
		{
			RY_CORE_WARN("We Imediedt Push The Date ({}) to Buffer ({}) withe oute Binding!", s_DataNameStr, s_BufferNameStr);
			LoadeDataUpOffset();
		}

		void ForceStateDataUptoData()
		{
			RY_CORE_WARN("We Imediedt Skip The Data ({}) to be Uploadedt to Buffer ({}) Untile the Data change another Time!", s_DataNameStr, s_BufferNameStr);
			ResetRange();
		}

		static glm::uvec2 MaxRange()
		{
			glm::uvec2 rangeMax = glm::uvec2{ static_cast<uint32_t>(-1), 0u };
			return rangeMax;
		}

		static void Range(glm::uvec2& range, uint32_t index)
		{
			uint32_t x = range.x > index ? index : range.x;
			uint32_t iAdd = index + 1u;
			uint32_t y = range.y < (iAdd) ? (iAdd) : range.y;
			range = glm::uvec2{ x,  y };
		}

	private:
		uint32_t ByteSizeInElementCount(uint32_t byteSize)
		{
			uint32_t elmentByteSize = sizeof(N);
			uint32_t v = elmentByteSize / byteSize;
			return v;
		}

		template<typename U, typename ArrayU>
		static uint32_t GetBytesSizeArra(const ArrayU& arrayData)
		{
			uint32_t elementSize = sizeof(U);
			uint32_t arrayCount = arrayData.Size();
			uint32_t v = elementSize * arrayCount;
			return v;
		}

		static uint32_t CopyOffsetByteSize(uint32_t byteOffset, uint32_t byteSize)
		{
			uint32_t v = byteOffset;
			return v;
		}

		static uint32_t CopyByteSize(uint32_t byteOffset, uint32_t byteSize)
		{
			uint32_t v = byteSize;
			return v;
		}

		static uint32_t CopyBeginByte(uint32_t byteOffset, uint32_t byteSize)
		{
			uint32_t v = byteOffset;
			return v;
		}

		static uint32_t CopyEndeByte(uint32_t byteOffset, uint32_t byteSize)
		{
			uint32_t v = byteOffset + byteSize;
			return v;
		}

		void CheckCountSizeDiffernz(uint32_t size, uint32_t offset = 0)
		{
			int rangeCount = static_cast<int>(size + offset);
			int arrayCount = m_ArrayData.Size();
			int ellmentCountDifferz = arrayCount - rangeCount;

			RY_CORE_ASSERT(rangeCount < arrayCount, "Overflow on Index Size withe {}", rangeCount);
			RY_CORE_ASSERT(ellmentCountDifferz <= 1, "Overflow buffer size by {} elements", -(ellmentCountDifferz));
		}

		void CheckByteSizeDiffernz(uint32_t byteSize, uint32_t byteOffset = 0)
		{
			uint32_t byteSizeEllemnt = sizeof(N);
			uint32_t arrayCount = m_ArrayData.Size();

			int byteRange = static_cast<int>(byteSize + byteOffset);
			int byteArraySize = arrayCount * byteSizeEllemnt;
			int ellmentByteSizeDifferz = byteArraySize - byteRange;

			RY_CORE_ASSERT(byteRange < byteArraySize, "Overflow on Index Size with {}", index);
			RY_CORE_ASSERT(ellmentByteSizeDifferz <= 1, "Overflow buffer size by {} elements", -(ellmentByteSizeDifferz));
		}

		void SetRangeIndex(uint32_t index)
		{
			m_Update = true;
			Range(m_Range, index);
		}

		void SetRange(uint32_t begin, uint32_t ende)
		{
			m_Update = true;
			Range(m_Range, begin);
			Range(m_Range, ende);
		}

		template<typename ArrayU>
		void SetFromRange(const ArrayU& arrayData, uint32_t offset = 0u)
		{
			uint32_t size = arrayData.Size();
			SetRange(offset, size);
		}

		void ResetRange()
		{
			m_Range = MaxRange();
			m_Update = false;
		}

		void ArrayRange()
		{
			uint32_t begin = 0u;
			uint32_t ende = m_ArrayData.Size();
			SetRange(begin, ende);
		}

		void LoadeDataUpOffset()
		{

			std::vector<N>& vecData = m_ArrayData.GetVector();
			RY_CORE_ASSERT(m_Range.y >= vecData.size(), "Greter Range Than Data!");
			const void* dataPtr = vecData.data();

			uint32_t elmentByteSize = sizeof(N);

			uint32_t completByteCopySize = m_Range.y * elmentByteSize;
			uint32_t bufferByteSize = m_Buffer->GetByteSize();

			if (completByteCopySize <= bufferByteSize)
			{
				uint32_t copyCount = m_Range.x - m_Range.y;
				uint32_t byteCopySizeOffset = m_Range.x * elmentByteSize;
				uint32_t byteCopySize = copyCount * elmentByteSize;

				m_Buffer->SetData(dataPtr, byteCopySizeOffset, byteCopySize);
			}
			else
			{
				uint32_t arrayCount = m_ArrayData.size();
				uint32_t copyByteSize = arrayCount * elmentByteSize;

				m_Buffer->Resize2D(dataPtr, byteCopySize);
			}

			ResetRange();
		}

		void LoadeDataUpAll()
		{
			std::vector<N>& vecData = m_ArrayData.GetVector();
			const void* dataPtr = vecData.data();

			uint32_t elmentByteSize = sizeof(N);
			uint32_t count = m_ArrayData.Size();
			uint32_t byteCopySize = count * elmentByteSize;
			uint32_t bufferByteSize = m_Buffer->GetByteSize();

			if (byteCopySize == bufferByteSize)
				m_Buffer->SetData(dataPtr, byteCopySize);
			else
				m_Buffer->Resize2D(dataPtr, byteCopySize);

			ResetRange();
		}

		constexpr static std::string GetBufferName()
		{
			std::string str = "Ref<";
			std::string typeName = typeid(T).name();
			typeName.erase(typeName.begin(), typeName.begin() + 6);
			str += typeName;
			str += ">";
			return str;
		}

		constexpr static std::string GetDataName()
		{
			std::string str = typeid(N).name();
			if (str.size() > 7)
				str.erase(str.begin(), str.begin() + 7);
			return str;
		}
	private:
		bool m_Update;
		Ref<T> m_Buffer;
		ArrayMap m_ArrayData;
		glm::uvec2 m_Range;

		inline static std::string s_BufferNameStr = GetBufferName();
		inline static std::string s_DataNameStr = GetDataName();
	};

	

	template<typename T>
	using UniformArrayPackage = typename BufferArrayPackage<UniformBuffer, T>;

	template<typename T>
	using StorageArrayPackage = typename BufferArrayPackage<StorageBuffer, T>;

	template<typename T>
	using IndriectArrayPackage = typename BufferArrayPackage<IndirectBuffer, T>;

	template<typename T>
	using IndexArrayPackage = typename BufferArrayPackage<IndexBuffer, T>;


	template<typename T>
	using UniformDataPackage = typename BufferDataPackage<UniformBuffer, T>;

	template<typename T>
	using StorageDataPackage = typename BufferDataPackage<StorageBuffer, T>;

	template<typename T>
	using IndriectDataPackage = typename BufferDataPackage<IndirectBuffer, T>;

	template<typename T>
	using IndexDataPackage = typename BufferDataPackage<IndexBuffer, T>;
	


	template< typename _Key, typename T>
	using StorageMapPackage = typename BufferArrayMapPackage<StorageBuffer, _Key, T>;
	
	
	template< typename _Key, typename T>
	using UniformArrayElementPtrPackage = typename BufferArrayMapElementPtrPackage<UniformBuffer, _Key, T>;

	template< typename _Key, typename T>
	using StorageArrayElementPtrPackage = typename BufferArrayMapElementPtrPackage<StorageBuffer, _Key, T>;

	template< typename _Key, typename T>
	using IndriectArrayElementPtrPackage = typename BufferArrayMapElementPtrPackage<IndirectBuffer, _Key, T>;

	template< typename _Key, typename T>
	using IndexArrayElementPtrPackage = typename BufferArrayMapElementPtrPackage<IndexBuffer, _Key, T>;
}

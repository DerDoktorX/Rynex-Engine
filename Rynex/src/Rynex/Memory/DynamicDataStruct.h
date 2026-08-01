#pragma once

#include <Rynex/Renderer/API/Buffer.h>
// #define RY_LOG_DYNAMIC_STRUCT_DATA_TRANSFARE
namespace Rynex {

	namespace Memory {

		class DynamicDataStruct 
		{
		public:
			using ContainerValueType = uint32_t;
			using ContainerValueTypePtr = ContainerValueType*;
			using ContainerConstValueTypePtr = const ContainerValueType*;
			using ContainerValueTypeRef = ContainerValueType&;
			using ContainerConstValueTypeRef = const ContainerValueType&;


			using ContainerType = std::vector<ContainerValueType>;
			using SizeType = typename ContainerType::size_type;
			using DifferenceType = typename ContainerType::difference_type;
		public:
			DynamicDataStruct();
			explicit DynamicDataStruct(const BufferLayout& layout, SizeType size = 1);

			DynamicDataStruct(const DynamicDataStruct& dynamicDataStruct);
			DynamicDataStruct(DynamicDataStruct&& dynamicDataStruct) noexcept;

			~DynamicDataStruct()noexcept;

			DynamicDataStruct& operator=(const DynamicDataStruct& rigth);

			DynamicDataStruct& operator=(DynamicDataStruct&& rigth) noexcept;

#pragma region templateFunktionName

			template<typename T>
			void Set(const std::string& name, const T& value)
			{
				DynamicDataStruct::Set(name, 0, value);
			}

			template<typename T>
			void Set(const std::string& name, SizeType index, const T& value)
			{
				SizeType offsetPos = DynamicDataStruct::GetBytesSizeOffset<T>(name, index);
				DynamicDataStruct::CopyValueToData<T>(value, offsetPos);
			}

			template<typename T>
			T* const GetPtr(const std::string& name)
			{
				return DynamicDataStruct::GetPtr<T>(name, 0);
			}

			template<typename T>
			T* const GetPtr(const std::string& name, SizeType index)
			{
				SizeType offsetPos = DynamicDataStruct::GetBytesSizeOffset<T>(name, index);
				ContainerValueTypePtr valuePtr = GetOffsetDataPosPtr<T>(offsetPos);
				T* valuePtr = reinterpret_cast<T*>(valuePtr);
				T* const valuePtrConst = const_cast<T* const>(valuePtr);
				return valuePtrConst;
			}
			
			template<typename T>
			T Get(const std::string& name) const
			{
				return DynamicDataStruct::Get<T>(name, 0);
			}

			template<typename T>
			T Get(const std::string& name, SizeType index) const
			{
				SizeType offsetPos = DynamicDataStruct::GetBytesSizeOffset<T>(name, index);
				T value;
				DynamicDataStruct::CopyDataToValuePtr<T>(&value, offsetPos);
				return value;
			}

#pragma endregion

#pragma region templateFunktionElementIndex
			
			template<typename T>
			void Set(SizeType elementIndex, const T& value)
			{
				DynamicDataStruct::Set<T>(elementIndex, 0, value);
			}

			template<typename T>
			void Set(SizeType elementIndex, SizeType index, const T& value)
			{
				SizeType offsetPos = DynamicDataStruct::GetBytesSizeOffset<T>(elementIndex, index);
				DynamicDataStruct::CopyValueToData<T>(value, offsetPos);
			}

			template<typename T>
			const T* const GetPtr(SizeType elementIndex) const
			{
				return DynamicDataStruct::GetPtr<T>(elementIndex, 0);
			}

			template<typename T>
			const T* const GetPtr(SizeType elementIndex, SizeType arrayIndex) const
			{
				SizeType offsetPos = DynamicDataStruct::GetBytesSizeOffset<T>(elementIndex, arrayIndex);
				ContainerValueTypePtr valuePtr = DynamicDataStruct::GetOffsetDataPosPtr<T>(offsetPos);
				T* valuePtr = reinterpret_cast<T*>(valuePtr);
				const T* const constValuePtrConst = const_cast<const T* const>(valuePtr);

				return constValuePtrConst;
			}
			
			template<typename T>
			T Get(SizeType elementIndex) const
			{
				return DynamicDataStruct::Get<T>(elementIndex, 0);
			}
			template<typename T>
			T Get(SizeType elementIndex, SizeType arrayIndex) const
			{
				SizeType offsetPos = DynamicDataStruct::GetBytesSizeOffset<T>(elementIndex, arrayIndex);
				T value;
				DynamicDataStruct::CopyDataToValuePtr<T>(&value, offsetPos);
				return value;
			}

#pragma endregion
			

			ContainerValueTypePtr Data() { return m_Data.data(); }
			ContainerConstValueTypePtr Data() const { return m_Data.data(); }

			SizeType Size() const { return m_Count; }
			SizeType LayoutElementCount() const { return m_Layout.GetLength(); }
			SizeType LayouteBytesSize() const { return m_Layout.GetStride(); }
			SizeType BufferByteSize() const 
			{
				SizeType dataByteSize = GetDataByteSize();
				SizeType dataByteCount = m_Data.size() * s_DividedBasicTypeByteUinte;
				if (dataByteSize != dataByteCount)
					RY_CORE_ERROR("{} == {}", dataByteSize, dataByteCount);
				RY_CORE_ASSERT(dataByteSize == dataByteCount);

				return dataByteCount;
			}

			bool Empty() const { return (m_Data.empty() || m_Layout.Empty()) && BufferByteSize() == 0; }

			void Resize(SizeType count);
			void ClearData();

			const BufferLayout& GetLayout()const { return m_Layout; }
		private:
			template<typename T>
			SizeType GetBytesSizeOffset(const std::string& name, SizeType indexArray) const
			{
				const BufferElement* elementPtr = FindElement(name);


				SizeType offsetByteSize = DynamicDataStruct::GetBytesSizeOffset<T>(elementPtr, indexArray);
				return offsetByteSize;
			}

			template<typename T>
			SizeType GetBytesSizeOffset(SizeType indexElement, SizeType indexArray) const
			{
				SizeType layoutCount = m_Layout.GetLength();
				RY_CORE_ASSERT(indexElement < layoutCount, "index Element not insigth Element Count layout");
				
				const BufferElement* elementPtr = &m_Layout[indexElement];
				SizeType offsetByteSize = DynamicDataStruct::GetBytesSizeOffset<T>(elementPtr, indexArray);
				return offsetByteSize;
			}

			template<typename T>
			SizeType GetBytesSizeOffset(const BufferElement* elementPtr, SizeType indexArray) const
			{
				SizeType elementSize = elementPtr->size;
				RY_CORE_ASSERT(sizeof(T) == elementSize, "Type size mismatch");
				RY_CORE_ASSERT(indexArray < m_Count, "Array Overflow");


				SizeType offsetBytesSizeLayoute = GetLayouteByteSizeByIndex(indexArray);
				SizeType offsetByteSizeElement = elementPtr->offset;
				SizeType beginBytesSizeElementPos = offsetBytesSizeLayoute + offsetByteSizeElement;

				SizeType dataByteSize = m_Data.size() * s_DividedBasicTypeByteUinte;
				RY_CORE_ASSERT(beginBytesSizeElementPos < dataByteSize, "Element offset Pos Overflow data!");
				SizeType endeBytesSizeElementPos = beginBytesSizeElementPos;
				RY_CORE_ASSERT(endeBytesSizeElementPos <= dataByteSize, "Element end or (offset+size) pos Overflow data!");
				return beginBytesSizeElementPos;

			}


			template<typename T>
			ContainerValueTypePtr GetOffsetDataPosPtr(SizeType offsetBytesSize) const
			{
				ContainerConstValueTypePtr  beginMemoryContainerDataPtr = m_Data.data();
				const SizeType byteSize = m_Data.size();
				ContainerConstValueTypePtr endMemoryContainerDataPtr = beginMemoryContainerDataPtr + byteSize;

				
				const SizeType byteElementCountOffset = offsetBytesSize / s_DividedBasicTypeByteUinte;

				ContainerConstValueTypePtr beginValueDataPosPtr = beginMemoryContainerDataPtr + byteElementCountOffset;

				{
					bool beginElementPosVaild;
					beginElementPosVaild = beginMemoryContainerDataPtr <= beginValueDataPosPtr;
					beginElementPosVaild = beginValueDataPosPtr < endMemoryContainerDataPtr && beginElementPosVaild;

					RY_CORE_ASSERT(beginElementPosVaild, "begin Ptr Pos in Not In Buffer Range!");
				}

				{
					const SizeType elementSize = sizeof(T);
					const SizeType byteElementCount = elementSize / s_DividedBasicTypeByteUinte;

					ContainerConstValueTypePtr endValueDataPosPtr = beginValueDataPosPtr + byteElementCount;

					bool endElementPosVaild;
					endElementPosVaild = endValueDataPosPtr <= endMemoryContainerDataPtr;
					endElementPosVaild = beginMemoryContainerDataPtr < endValueDataPosPtr && endElementPosVaild;
					endElementPosVaild = beginValueDataPosPtr < endValueDataPosPtr && endElementPosVaild;

					RY_CORE_ASSERT(endElementPosVaild, "end Ptr Pos in Not In Buffer Range!");
				}
				ContainerValueTypePtr offsetElementPosPtr = const_cast<ContainerValueTypePtr>(beginValueDataPosPtr);
				return offsetElementPosPtr;
			}

			template<typename T>
			void CopyValueToData(const T& value, SizeType offsetBytesSize)
			{
				ContainerValueTypePtr beginValueDataPosPtr = DynamicDataStruct::GetOffsetDataPosPtr<T>(offsetBytesSize);
				const T* valuePtr = &value;
				SizeType byteSize = sizeof(T);
#ifdef RY_LOG_DYNAMIC_STRUCT_DATA_TRANSFARE
				DifferenceType offsetIndex = beginValueDataPosPtr - m_Data.data();
				DifferenceType lastIndex = offsetIndex + byteSize;
				RY_CORE_INFO("Set Value {} withe {} bytes in value Range ({}, {})[{}]", typeid(T).name(), sizeof(T), offsetIndex, lastIndex, m_Data.size());
#endif
				std::memcpy(beginValueDataPosPtr, valuePtr, byteSize);
			}

			template<typename T>
			void CopyDataToValuePtr(T* valuePtr, SizeType offsetBytesSize) const
			{
				ContainerValueTypePtr beginValueDataPosPtr = DynamicDataStruct::GetOffsetDataPosPtr<T>(offsetBytesSize);
				SizeType byteSize = sizeof(T);

#ifdef RY_LOG_DYNAMIC_STRUCT_DATA_TRANSFARE
				DifferenceType offsetIndex = beginValueDataPosPtr - m_Data.data();
				DifferenceType lastIndex = offsetIndex + byteSize;
				RY_CORE_INFO("Get Value {} withe {} bytes in value Range ({}, {})[{}]", typeid(T).name(), sizeof(T), offsetIndex, lastIndex, m_Data.size());
#endif
				std::memcpy(valuePtr, beginValueDataPosPtr, byteSize);
			}

			const BufferElement* FindElement(const std::string& name) const;
			
			SizeType GetLayouteByteSizeByIndex(SizeType index) const;

			SizeType GetDataByteSize() const;
			SizeType GetContainerCount() const;

			SizeType GetArrayCountFromBytesSize(SizeType byteSize) const;
		private:
			BufferLayout m_Layout;
			ContainerType m_Data;
			SizeType m_Count;
		private:
			inline static constexpr SizeType s_DividedBasicTypeByteUinte = sizeof(ContainerValueType);
		};

	}
}
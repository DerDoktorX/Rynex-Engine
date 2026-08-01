#include "rypch.h"
#include "DynamicDataStruct.h"

namespace Rynex {

	namespace Memory {

		DynamicDataStruct::DynamicDataStruct()
			: m_Layout()
			, m_Count(0)
			, m_Data()
		{
		}

		DynamicDataStruct::DynamicDataStruct(const BufferLayout& layout, SizeType size)
			: m_Layout(layout)
			, m_Count(size)
		{
			RY_CORE_ASSERT(!m_Layout.Empty(), "empty layout is not vaild!");

			Resize(size);
		}

		DynamicDataStruct::DynamicDataStruct(const DynamicDataStruct& dynamicDataStruct)
			: m_Layout(dynamicDataStruct.m_Layout)
			, m_Count(dynamicDataStruct.m_Count)
			, m_Data(dynamicDataStruct.m_Data)
		{
			if(m_Layout.Empty())
				RY_CORE_WARN("empty layout is not vaild! DynamicDataStruct is empty but copyed");
			BufferByteSize();
		}

		DynamicDataStruct::DynamicDataStruct(DynamicDataStruct&& dynamicDataStruct) noexcept
			: m_Layout(std::move(dynamicDataStruct.m_Layout))
			, m_Count(dynamicDataStruct.m_Count)
			, m_Data(std::move(dynamicDataStruct.m_Data))
		{
			if (m_Layout.Empty())
				RY_CORE_WARN("empty layout is not vaild! DynamicDataStruct is empty but moved");
			BufferByteSize();

			dynamicDataStruct.m_Layout = BufferLayout();
			dynamicDataStruct.m_Count = 0;
			dynamicDataStruct.m_Data.clear();
		}

		DynamicDataStruct::~DynamicDataStruct() noexcept
		{
		}

		DynamicDataStruct& DynamicDataStruct::operator=(const DynamicDataStruct& rigth)
		{
			if(this != &rigth)
			{
				m_Layout = rigth.m_Layout;
				m_Count = rigth.m_Count;
				m_Data = rigth.m_Data;
				BufferByteSize();
			}
			return *this;
		}

		DynamicDataStruct& DynamicDataStruct::operator=(DynamicDataStruct&& rigth) noexcept 
		{
			if (this != &rigth)
			{
				m_Layout = std::move(rigth.m_Layout);
				m_Count = rigth.m_Count;
				m_Data = std::move(rigth.m_Data);
				BufferByteSize();
			}
			return *this;
		}

		const BufferElement* DynamicDataStruct::FindElement(const std::string& name) const
		{
			for (const BufferElement& elem : m_Layout) 
			{
				if (elem.name == name)
					return &elem;
			}
			return nullptr;
		}

		DynamicDataStruct::SizeType DynamicDataStruct::GetLayouteByteSizeByIndex(SizeType index) const
		{
			SizeType bytesSizeDataStruct = m_Layout.GetStride();
			SizeType offsetBytesSize = bytesSizeDataStruct * index;
			return offsetBytesSize;
		}

		DynamicDataStruct::SizeType DynamicDataStruct::GetDataByteSize() const
		{
			SizeType bytesSize = GetLayouteByteSizeByIndex(m_Count);
			return bytesSize;
		}

		DynamicDataStruct::SizeType DynamicDataStruct::GetContainerCount() const
		{
			SizeType bytesSize = GetDataByteSize();
			SizeType containerCount = bytesSize / s_DividedBasicTypeByteUinte;
			return containerCount;
		}

		DynamicDataStruct::SizeType DynamicDataStruct::GetArrayCountFromBytesSize(SizeType byteSize) const
		{
			SizeType bytesSizeDataStruct = m_Layout.GetStride();
			SizeType bytesSizemoduleLayouteByteSize = byteSize % bytesSizeDataStruct;
			RY_CORE_ASSERT("Data Overfolw", 0 == bytesSizemoduleLayouteByteSize);

			SizeType count = byteSize / bytesSizeDataStruct;
			return count;
		}

		void DynamicDataStruct::Resize(SizeType count)
		{
			m_Count = count;
			

			SizeType containerCount = GetContainerCount();
			m_Data.resize(containerCount);
			BufferByteSize();
		}

		void DynamicDataStruct::ClearData()
		{
			BufferByteSize();
			m_Data.clear();
			m_Count = 0;

			BufferByteSize();
		}
	}
}
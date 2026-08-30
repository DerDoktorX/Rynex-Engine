#pragma once
#include <rypch.h>
namespace Rynex {

	namespace Memory {

		template<typename T>
		class VectorData2D;


		template<typename T>
		class VectorData
		{
		public:
			using ValueType = T;
			using Container = typename VectorData2D<ValueType>::Container;
			
			using SizeType = typename VectorData2D<ValueType>::SizeType;
			using DifferenceType = typename VectorData2D<ValueType>::DifferenceType;
		public:
			VectorData(VectorData2D<ValueType>& refData, SizeType dymension)
				: m_DataRef(refData)
				, m_Dimension(dymension)
			{
			}
			VectorData(const VectorData&) = delete;
			VectorData(VectorData&&) = delete;

			~VectorData()
			{
			}

			const ValueType& At(SizeType indexY) const
			{
				return m_DataRef.AtAt(m_Dimension, indexY);
			}

			ValueType& At(SizeType indexY)
			{
				return m_DataRef.AtAt(m_Dimension, indexY);
			}

			const ValueType& operator[](SizeType indexY) const
			{
				return m_DataRef[m_Dimension][indexY];
			}

			ValueType& operator[](SizeType indexY)
			{
				return m_DataRef[m_Dimension][indexY];
			}

			SizeType Size() const
			{
				return m_DataRef.SizeDY();
			}

			void Push(SizeType pushCount = 1)
			{
				m_DataRef.PushDY(pushCount);
			}

			void Resize2D(SizeType sizeY)
			{
				m_DataRef.ResizeY(sizeY);
			}

			ValueType* GetPtr() { return m_DataRef.GetPtr(); }
			const ValueType* GetPtr() const { return m_DataRef.GetPtr(); }

			ValueType& Get() { return m_DataRef.Get(); }
			const ValueType& Get() const { return m_DataRef.Get(); }


			void SetValueToIndex(SizeType indexY)
			{
				m_DataRef.SetValueToIndex(m_Dimension, indexY);
			}

			void ValueBegin()
			{
				SetValueToIndex(0);
			}

			void Increase()
			{
				m_DataRef.Increase();
			}

			void Decrease()
			{
				m_DataRef.Decrease();
			}

		private:
			VectorData2D<ValueType>& m_DataRef;
			const SizeType m_Dimension;
		};

		template<typename T>
		class VectorData2D
		{
		public:
			using ValueType = T;
			using Container = std::vector<ValueType>;
			using SizeType = typename Container::size_type;
			using DifferenceType = typename Container::difference_type;
		public:
			VectorData2D()
				: m_CurentPtr(nullptr)
				, m_SizeX(0)
				, m_SizeY(0)
				, m_Vector()
			{
			}

			~VectorData2D()
			{
				m_Vector.clear();
				m_Vector.shrink_to_fit();
			}

			void Resize2D(SizeType sizeX, SizeType sizeY)
			{
				DifferenceType pos = 0;
				if (IsValue())
				{
					pos = m_CurentPtr - m_Vector.data();
					auto[xCount, yCount] = GetIndexDimensions(pos);
					SizeType xIndex = xCount % sizeX;
					SizeType yIndex = yCount % sizeY;
					pos = yIndex + (yIndex * xIndex);
				}

				
				

				Container tempVec = m_Vector;
				SizeType sizeTempX = sizeX < m_SizeX ? sizeX : m_SizeX;
				SizeType sizeTempY = sizeY < m_SizeY ? sizeY : m_SizeY;

				m_SizeX = sizeX;
				m_SizeY = sizeY;
				SizeType count = Size();
				m_Vector.resize(count);

				if(!tempVec.empty())
				{

					for (SizeType iX = 0; iX < sizeTempX; iX++)
					{
						for (SizeType iY = 0; iY < sizeTempY; iY++)
						{
							SizeType indexTemp = iY + (iY * iX);
							SizeType index = Index(iX, iY);
							m_Vector.at(index) = tempVec.at(indexTemp);
						}
					}
				}




				m_CurentPtr = m_Vector.data() + pos;
			}

			void ResizeY(SizeType sizeY)
			{
				SizeType sizeX = SizeDX();
				Resize2D(sizeX, sizeY);
			}

			void ResizeX(SizeType sizeX)
			{
				SizeType sizeY = SizeDY();
				Resize2D(sizeX, sizeY);
			}

			SizeType Size() const
			{
				SizeType count = m_SizeX * m_SizeY;
				SizeType countVec = m_Vector.size();
				return count;
			}

			SizeType SizeDX() const
			{
				return m_SizeX;
			}

			SizeType SizeDY() const
			{
				return m_SizeY;
			}

			std::pair<SizeType, SizeType> SizeD() const
			{
				return std::pair<SizeType, SizeType>(m_SizeX, m_SizeY);
			}

			void Clear()
			{
				m_Vector.clear();
				m_CurentPtr = nullptr;
				m_SizeX = 0;
				m_SizeY = 0;
			}

			bool Empty() const
			{
				return m_Vector.empty();
			}

			const ValueType& AtAt(SizeType iX, SizeType iY) const
			{
				SizeType index = Index(iX, iY);
				return m_Vector.at(index);
			}

			ValueType& AtAt(SizeType iX, SizeType iY)
			{
				SizeType index = Index(iX, iY);
				return m_Vector.at(index);
			}

			SizeType GetIndexDimensionsX(SizeType index) const
			{
				uint32_t countIndexX = index / m_SizeY;
				return countIndexX;
			}

			SizeType GetIndexDimensionsY(SizeType index) const
			{
				uint32_t countIndexY = index % m_SizeY;
				return countIndexY;
			}

			std::pair<SizeType, SizeType> GetIndexDimensions(SizeType index) const
			{
				uint32_t countIndexY = GetIndexDimensionsY(index);
				uint32_t countIndexX = GetIndexDimensionsX(index);

				return std::pair<SizeType, SizeType>(countIndexX, countIndexY);
			}

			const VectorData<ValueType> At(SizeType indexX) const
			{
				RY_CORE_ASSERT(indexX < m_SizeX, "X dimension offerflow!");
				return VectorData<ValueType>(*this, indexX);
			}

			VectorData<ValueType> At(SizeType indexX)
			{
				RY_CORE_ASSERT(indexX < m_SizeX, "X dimension offerflow!");
				return VectorData<ValueType>(*this, indexX);
			}

			const VectorData<ValueType> operator[](SizeType indexX) const
			{
				return VectorData<ValueType>(*this, indexX);
			}

			VectorData<ValueType> operator[](SizeType indexX)
			{
				return VectorData<ValueType>(*this, indexX);
			}


			SizeType Index(SizeType iX, SizeType iY) const
			{				
				RY_CORE_ASSERT(iX < m_SizeX && iY < m_SizeY, "Dimension out of Bounds");
				return iY + (iY * iX);
			}

			ValueType* GetPtr()
			{
				return m_CurentPtr;
			}

			const ValueType* const GetPtr() const
			{
				return m_CurentPtr;
			}

			ValueType& Get()
			{
				return *m_CurentPtr;
			}

			const ValueType& Get() const
			{
				return *m_CurentPtr;
			}

			bool IsValue() const
			{
				return m_CurentPtr != nullptr;
			}
			
			bool InRange() const
			{
				const ValueType* begin = m_Vector.data();
				const ValueType* end = begin + m_Vector.size();
				return begin <= m_CurentPtr && m_CurentPtr < end;
			}

			void SetValueToIndex(SizeType iX, SizeType iY)
			{
				SizeType index = Index(iX, iY);
				ValueType* begin = m_Vector.data();
				m_CurentPtr = begin + index;
			}

			void Increase()
			{
				m_CurentPtr++;
			}

			void Decrease()
			{
				m_CurentPtr--;
			}

			void PushDX(SizeType pushCount = 1)
			{
				Resize2D(m_SizeX + pushCount, m_SizeY);
			}

			void PushDY(SizeType pushCount = 1)
			{
				Resize2D(m_SizeX, m_SizeY + pushCount);
			}

		private:
			Container m_Vector;
			ValueType* m_CurentPtr;
			SizeType m_SizeX;
			SizeType m_SizeY;
		};

	

	}

}
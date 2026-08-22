#pragma once
#include <rypch.h>

namespace Ryenx{
	namespace Memory {

		class VectorDataBool
		{
		public:
			using InteraleValueType = typename int;
			using ValueType = typename bool;

			using Alloc = typename std::vector<typename InteraleValueType>;
			using SizeType = typename Alloc::size_type;
			using DifferenceType = typename Alloc::difference_type;

		public:
			VectorDataBool()
			{
				RY_CORE_NOT_IMPL();
				RY_REMBER_FUNC_CHANGE("Fisish this class");
			}

			~VectorDataBool()
			{
				RY_CORE_NOT_IMPL();
				RY_REMBER_FUNC_CHANGE("Fisish this class");
			}

			ValueType At(SizeType globleIndex) const
			{
				SizeType indexVector = GetVectorIndex(globleIndex);
				SizeType indexElement = GetElementIndex(globleIndex);

				const InteraleValueType& value = m_BoolVec.at(indexVector);
				bool result = BIT_EQUAL(value, BIT(indexElement)) != 0;
				return result;
			}

			ValueType operator[](SizeType globleIndex) const
			{
				SizeType indexVector = GetVectorIndex(globleIndex);
				SizeType indexElement = GetElementIndex(globleIndex);

				const InteraleValueType& value = m_BoolVec.at(indexVector);
				bool result = BIT_EQUAL(value, BIT(indexElement)) != 0;

				return result;
			}			

			SizeType Size() const
			{
				return m_BoolVec.size();
			}

		private:
			SizeType VectorDataBool::GetVectorIndex(SizeType globleIndex) const
			{
				SizeType indexVector = globleIndex / s_BoolValuesPerIntValue;
				return indexVector;
			}

			SizeType VectorDataBool::GetElementIndex(SizeType globleIndex) const
			{
				SizeType indexElement = globleIndex % s_BoolValuesPerIntValue;
				return indexElement;
			}
		private:
			Alloc m_BoolVec;
			SizeType m_LastEllementUse = 0;
		private:
			static constexpr const SizeType s_BitsPerByte = 8ull;
			static constexpr const SizeType s_BoolValuesPerIntValue = sizeof(InteraleValueType) * s_BitsPerByte;
		};


		
	}
}
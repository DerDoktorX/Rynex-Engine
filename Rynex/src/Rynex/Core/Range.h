#pragma once
#include<rypch.h>

namespace Rynex {

	struct Range
	{
		union { int First, x; };
		union { int Last, y; };
		Range() = default;
		Range(const Range&) = default;
		Range(int first, int last)
			: First(first)
			, Last(last)
		{
		}


		int GetCount() const
		{
			int count = Last - First;
			return count;
		}

		bool IsInsideValueRange(int i) const
		{
			return First <= i && i < Last;
		}

		bool IsColiding(const Range& rigth) const
		{
			return IsInsideValueRange(rigth.First) || IsInsideValueRange(rigth.Last - 1);
		}

		int operator[](int i) const
		{
			RY_CORE_ASSERT(i < 2, "Not Vaild index Range!");
			switch (i)
			{
			default:
			case 0:
				return First;
			case 1:
				return Last;
			}

		}

		int& operator[](int i)
		{
			RY_CORE_ASSERT(i < 2, "Not Vaild index Range!");
			switch (i)
			{
			default:
			case 0:
				return First;
			case 1:
				return Last;
			}

		}

		static Range MaxRange()
		{
			constexpr int max = std::numeric_limits<int>::max();
			constexpr int min = std::numeric_limits<int>::min();
			return Range{ max, min };
		}
	};

	bool operator==(const Range& left, const Range& rigth);
	bool operator!=(const Range& left, const Range& rigth);
	bool operator<(const Range& left, const Range& rigth);
	bool operator<=(const Range& left, const Range& rigth);
	bool operator>(const Range& left, const Range& rigth);
	bool operator>=(const Range& left, const Range& rigth);
}
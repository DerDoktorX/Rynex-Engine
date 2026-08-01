#include "rypch.h"
#include "Range.h"

namespace Rynex {
	bool operator==(const Range& left, const Range& rigth)
	{
		return left.First == rigth.First
			&& left.Last == rigth.Last;

	}

	bool operator!=(const Range& left, const Range& rigth)
	{
		return left.First != rigth.First
			|| left.Last != rigth.Last;
	}


	bool operator<(const Range& left, const Range& rigth)
	{
		return left.Last < rigth.First
			&& left.Last < rigth.Last
			&& left.First < rigth.First
			&& left.First < rigth.Last;
	}

	bool operator<=(const Range& left, const Range& rigth)
	{
		return left.Last <= rigth.First
			&& left.Last < rigth.Last
			&& left.First < rigth.First
			&& left.First < rigth.Last;
	}

	bool operator>(const Range& left, const Range& rigth)
	{
		return left.Last > rigth.First
			&& left.Last > rigth.Last
			&& left.First > rigth.First
			&& left.First > rigth.Last;
	}

	bool operator>=(const Range& left, const Range& rigth)
	{
		return left.Last > rigth.First
			&& left.Last > rigth.Last
			&& left.First > rigth.First
			&& left.First >= rigth.Last;
	}
}

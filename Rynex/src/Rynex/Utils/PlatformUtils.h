#pragma once

#include <string>

namespace Rynex {

	class RYNEX_API FileDialoges
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

}



#pragma once

#include <string>

namespace Rynex {

	class RYNEX_API FileDialoges
	{
	public:
		static std::string OpenFile(const char* filter, const char* beginDir = "");
		static std::string SaveFile(const char* filter);

		static std::string SelectFolder();
	};

	class RYNEX_API LaunchFile 
	{
	public:
		static void OpenFileInDefaultApp(const std::filesystem::path& filpath, const std::filesystem::path& workingDirectory = "");

	};
}



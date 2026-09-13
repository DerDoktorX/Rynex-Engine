#include "rypch.h"

#include<Rynex/Utils/PlatformUtils.h>
#include<Rynex/Core/Application.h>

#include <GLFW/glfw3.h>

#if defined(RY_PLATFORM_WINDOWS) && RY_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>



#include <ShlObj.h>
#include <commdlg.h>
#else

#endif
namespace Rynex {
#if defined(RY_PLATFORM_WINDOWS) && RY_PLATFORM_WINDOWS
	std::string FileDialoges::OpenFile(const char* filter, const char* beginDir)
	{
		constexpr uint32_t MAC_PATH_CHAR_COUNT = MAX_PATH;
		OPENFILENAMEA ofn;
		CHAR szFile[MAC_PATH_CHAR_COUNT] = "";
		ZeroMemory(&ofn, sizeof(OPENFILENAME));


		ofn.lStructSize = sizeof(OPENFILENAME);
		Weak<Window> windowWeak = Application::Get().GetWindowWeak();
		GLFWwindow* windowPtr = Window::GetWindowPtr<GLFWwindow>(windowWeak);
		ofn.hwndOwner = glfwGetWin32Window(windowPtr);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);

		std::string begindirView(beginDir);
		if(!begindirView.empty())
		{
			char* filePathCharPtr = &begindirView[0];
			if (GetCurrentDirectoryA(MAC_PATH_CHAR_COUNT, filePathCharPtr))
				ofn.lpstrInitialDir = beginDir;
		}

		// Sets the default extension by extracting it from the filter
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		std::string fileSellected;
		if (GetOpenFileNameA(&ofn))
		{
			if(nullptr == ofn.lpstrFile)
				return fileSellected;

			fileSellected = ofn.lpstrFile;
			RY_CORE_INFO("Fished OpenFile Success!");
		}
		
		return fileSellected;
	}

	std::string FileDialoges::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		Weak<Window> windowWeak = Application::Get().GetWindowWeak();
		GLFWwindow* windowPtr = Window::GetWindowPtr<GLFWwindow>(windowWeak);
		ofn.hwndOwner = glfwGetWin32Window(windowPtr);
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);

		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;
		std::string result;
		if (GetSaveFileNameA(&ofn))
			result = ofn.lpstrFile;

		return result;
	}

	std::string FileDialoges::SelectFolder()
	{
		BROWSEINFOA bi;
		ZeroMemory(&bi, sizeof(BROWSEINFOA));
		CHAR szPath[MAX_PATH] = { 0 };

		Weak<Window> windowWeak = Application::Get().GetWindowWeak();
		GLFWwindow* windowPtr = Window::GetWindowPtr<GLFWwindow>(windowWeak);

		bi.hwndOwner = glfwGetWin32Window(windowPtr);
		bi.pszDisplayName = szPath;
		
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		
		LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
		if (pidl != nullptr)
		{
			if (SHGetPathFromIDListA(pidl, szPath))
			{
				CoTaskMemFree(pidl);
				return std::string(szPath);
			}
			CoTaskMemFree(pidl);
		}

		return std::string();
	}

	void LaunchFile::OpenFileInDefaultApp(const std::filesystem::path& filpath, const std::filesystem::path& workingDirectory)
	{
		RY_CORE_ASSERT(filpath.has_filename(), "No File Name Found");
		bool workingDir = workingDirectory.empty();
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

#if UNICODE
		std::wstring pathWstr = filpath.wstring();
		std::wstring workingDirWstr = filpath.wstring();
		wchar_t* filePathCharPtr = reinterpret_cast<wchar_t*>(pathWstr.data());
		const wchar_t* workingDirCharPtr = reinterpret_cast<const wchar_t*>(workingDir ? workingDirWstr.data() : NULL);
#else
		std::string pathStr = filpath.string();
		std::string workingDirStr = filpath.string();

		char* filePathCharPtr = reinterpret_cast<char*>(pathStr.data());
		const char* workingDirCharPtr = reinterpret_cast<const char*>(workingDir ? workingDirStr.data() : NULL);
#endif

		if (CreateProcess(NULL, filePathCharPtr, NULL, NULL, FALSE, 0, NULL, workingDirCharPtr, &si, &pi))
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		else
		{
			RY_CORE_ASSERT(false, "Error: On try To Start a other APP!");
		}
		RY_CORE_INFO("Programm: '{}' started on {}", filpath.filename().string().c_str(), workingDir ? workingDirectory.string().c_str() : "Default Working Dir");
	}
#else
	void LaunchFile::OpenFileInDefaultApp(const std::filesystem::path& filpath, const std::filesystem::path& workingDirectory)
	{
		std::string filePathSize;
		filePathSize.resize(512);
		char* charPtr = filePathSize.data();
		RY_CORE_INFO("OpenFileInDefaultApp");
		scanf("%s", charPtr);("%s", charPtr);

	}

	std::string FileDialoges::OpenFile(const char* filter, const char* beginDir)
	{
		
		RY_CORE_INFO("OpenFile");
		std::string filePathSize;
		filePathSize.resize(512);
		char* charPtr = filePathSize.data();

		scanf("%s", charPtr);
		return filePathSize;

	}

	std::string FileDialoges::SaveFile(const char* filter)
	{
		RY_CORE_INFO("SaveFile");
		std::string filePathSize;
		filePathSize.resize(256);
		char* charPtr = filePathSize.data();

		scanf("%s", charPtr);
		return filePathSize;

	}

	std::string FileDialoges::SelectFolder()
	{
		RY_CORE_INFO("SelectFolder");
		std::string filePathSize;
		filePathSize.resize(256);
		char* charPtr = filePathSize.data();

		scanf("%s", charPtr);
		return filePathSize;

	}
#endif
}
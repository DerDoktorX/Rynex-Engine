#include "rypch.h"

#include "Rynex/Utils/PlatformUtils.h"
#include "Rynex/Core/Application.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <ShlObj.h>


namespace Rynex {

	std::string FileDialoges::OpenFile(const char* filter, const char* beginDir)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };
		//CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if(beginDir!="")
		{
			if (GetCurrentDirectoryA(256, (char*)beginDir))
				ofn.lpstrInitialDir = beginDir;
		}
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile ? ofn.lpstrFile : std::string("");
		
		return std::string("");
	}

	std::string FileDialoges::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };
		//CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		//if (GetCurrentDirectoryA(256, currentDir))
		//	ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}

	std::string FileDialoges::SelectFolder()
	{
		BROWSEINFOA bi;
		ZeroMemory(&bi, sizeof(BROWSEINFOA));
		CHAR szPath[MAX_PATH] = { 0 };

		bi.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
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
		bool workingDir = workingDirectory != "";
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		if (CreateProcess(NULL, filpath.wstring().data(), NULL, NULL, FALSE, 0, NULL, workingDir ? workingDirectory.wstring().data() : NULL, &si, &pi))
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
}
#pragma once

#include "Rynex/Renderer/Texture.h"

#include <filesystem>

namespace Rynex {

	enum class FileFormats
	{
		FOLDER,		// Standerd folder
		PNG,		// Image
		RYNEXSCENS,	// Scene
		GLSL,		// Shader
		TXT,		// Text
					// Binary
	};

	enum class FileStats 
	{
		NONE,
		ERORO,
		UPDATE,
		CHANGE,
		READY,
		WAHRNING,
	};

	class ContentBrowserPannel
	{
	public:
		ContentBrowserPannel();

		void OnImGuiRender();

		

	private:
		
		void FileFormate(std::filesystem::directory_entry directoryEntry, FileFormats formate = FileFormats::RYNEXSCENS);

	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};
}



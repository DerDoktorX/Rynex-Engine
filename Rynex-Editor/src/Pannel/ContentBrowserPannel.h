#pragma once

#include "Rynex/Renderer/Texture.h"

#include <filesystem>

namespace Rynex {

	enum class FileFormats
	{
		none = 0,	// Not defined
		folder,		// Standerd folder
		png,		// Image
		rynexscene,	// Scene
		glsl,		// Shader
		txt,		// Text
		// Binary
	};

	enum class FileStats
	{
		None = 0,
		Error,
		Update,
		Change,
		Ready,
		Wahrning,
	};

	class ContentBrowserPannel
	{
	public:
		ContentBrowserPannel();

		void OnImGuiRender();



	private:

		void FileFormate(const std::filesystem::directory_entry& directoryEntry, FileFormats formate = FileFormats::none) const;

	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D>	m_DirectoryIcon;
		//Files
		Ref<Texture2D>	m_FileIconDefault;
		Ref<Texture2D>	m_FileIconError;
		Ref<Texture2D> 	m_FileIconScene;
		Ref<Texture2D>	m_FileIconShader;
		Ref<Texture2D>	m_FileIconTexture;

	};				   
}					   



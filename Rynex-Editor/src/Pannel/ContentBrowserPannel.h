#pragma once

#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Project/Project.h>
#include <FileWatch.h>

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
		OnDisc,
		OnRam,
		LostConection,
		Update,
		Change,
		Ready,
		Wahrning,
	};

	struct DealteAsset
	{
		std::string AssetFileName;
		std::filesystem::path AssetFileParentPath;
	};

	class ContentBrowserPannel
	{
	public:
		ContentBrowserPannel();

		void OnImGuiRender();
		void OnAtache();

		static void AssetFileWatcher();
	private:
		void BrowserPannel();
		void AssetPannel();
		void AssetRegestriyPannel();

		void GetFileList(const std::filesystem::path& curentPath);
		void SetAssetRegestriy(const std::filesystem::path& curentPath);

		void FileFormate(const std::filesystem::directory_entry& directoryEntry, FileFormats formate = FileFormats::none) const;
		void FileAsste(const std::filesystem::directory_entry& directoryEntry, AssetHandle handle) const;

		void DelateListeAsset(DealteAsset dealeteAsset);
		void DelateAsset();

		static void InitAssetFileWatcher();
	private:
		Ref<Project> m_Project;

		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D>	m_DirectoryIcon;
		//Files
		Ref<Texture2D>	m_FileIconDefault;
		Ref<Texture2D>	m_FileIconError;
		Ref<Texture2D> 	m_FileIconScene;
		Ref<Texture2D>	m_FileIconShader;
		Ref<Texture2D>	m_FileIconTexture;


		bool m_CompletAssetDirectory = false;
		
		// AssetDirectory m_AssetDirectorys;
		Ref<EditorAssetManager> m_AssetManger;

		std::vector<DealteAsset> m_DealeteAssetList;
	};				   
}					   



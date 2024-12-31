#pragma once
#include "PopUp/FrambufferWindow.h"

#include <Rynex/Renderer/API/Framebuffer.h>
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

	enum class SettingsPopUpWindow : uint8_t
	{
		None = 0,
		CeateFarmbufferSettings,
		CeateVertexArraySettings
	};

	struct DealteAsset
	{
		std::string AssetFileName;
		std::filesystem::path AssetFileParentPath;
	};

	struct CreateFrambuffer 
	{
		char Buffer[64] = "FrameBufferDefaulte";
		std::filesystem::path Path = "";
		FramebufferSpecification FrambufferSpec = FramebufferSpecification();

		CreateFrambuffer() = default;
	};

	class ContentBrowserPannel
	{
	public:
		ContentBrowserPannel();

		void OnImGuiRender();
		void OnAtache();
		void OpenAssetPannel();
		void OpenRegestriyPannel();

		static void AssetFileWatcher();
#if RY_EDITOR_ASSETMANGER_THREADE-1
		static void SetAssetRegestriy(const std::filesystem::path& curentPath, Ref<EditorAssetManager>& assetManager);
#endif

	private:
		void BrowserPannel();
		void AssetPannel();
		void AssetRegestriyPannel();

		void GetFileList(const std::filesystem::path& curentPath);

		void CreateAsset(const std::filesystem::path& assetPath, AssetMetadata metadata);
		void FileFormate(const std::filesystem::directory_entry& directoryEntry, FileFormats formate = FileFormats::none) const;
		void FileAsste(const std::filesystem::directory_entry& directoryEntry, AssetHandle handle) const;

		void CreateFrambufferAsset(std::filesystem::path& path);

		void NewMenue();
		void NewAsset();
		void NewFile();

		void DelateListeAsset(DealteAsset dealeteAsset);
		void DelateAsset();

		void DelateListeFolder(const std::filesystem::path& folderPath);
		void DelateFolder();

		static void InitAssetFileWatcher();
		
		void OnloadeAssetsList();
		void OnLoadeAsset(AssetHandle handle);

	private:
		Ref<Project> m_Project;
		Ref<FrambufferWindow> m_FrambufferWindow;
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture>	m_DirectoryIcon;
		//Files
		Ref<Texture>	m_FileIconDefault;
		Ref<Texture>	m_FileIconError;
		Ref<Texture> 	m_FileIconScene;
		Ref<Texture>	m_FileIconShader;
		Ref<Texture>	m_FileIconTexture;


		bool m_CompletAssetDirectory = false;
		bool m_WindowAssetPannelOpen = true;
		bool m_WindowRegestriyPannellOpen = true;
		SettingsPopUpWindow m_OppenWindow = SettingsPopUpWindow::None;
		
		// AssetDirectory m_AssetDirectorys;
#if RY_EDITOR_ASSETMANGER_THREADE
		Ref<EditorAssetManegerThreade> m_AssetManger;
#else
		Ref<EditorAssetManager> m_AssetManger;
#endif
		CreateFrambuffer m_CreateFrambuffer = CreateFrambuffer();
		std::vector<DealteAsset> m_DealeteAssetList;
		std::vector<AssetHandle> m_OnLoadeAsset;
		std::vector<std::filesystem::path> m_DealeteFolderList;
#if RY_EDITOR_ASSETMANGER_THREADE
		RegisterItemesThreade m_RegisterItemes;
		ContentBrowserItemesThreade m_FileItemes;
#else
		ContentBrowserItemes m_FileItemes;
#endif
	};				   
}					   



#include "rypch.h"
#include "ContentBrowserPannel.h"

#include "Rynex/Core/Application.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Asset/Import/TextureImporter.h"
#include "Rynex/Asset/Import/ShaderImporter.h"
#include "Rynex/Asset/Import/SceneImporter.h"

#include <imgui/imgui.h>


namespace Rynex {

	extern const std::filesystem::path g_AssetsPath = "Assets";
	const int s_MaxFileLength = 12;

#if RY_ASSETMANGER_FILE_WATCHER
	struct AssetMangerFileWatcherData
	{
		Scope<filewatch::FileWatch<std::string>> AssetFileWatcher;
		std::map<filewatch::Event, std::string> FileWatcherEventMap = {
			{ filewatch::Event::added, "Event::added"},
			{ filewatch::Event::modified, "Event::modified"},
			{ filewatch::Event::removed, "Event::removed"},
			{ filewatch::Event::renamed_new, "Event::renamed_new"},
			{ filewatch::Event::renamed_old, "Event::renamed_old"},
		};
	};
	
	static AssetMangerFileWatcherData* s_Data = nullptr;
#endif


	ContentBrowserPannel::ContentBrowserPannel()
		: m_Project(Project::GetActive())
		 , m_BaseDirectory(g_AssetsPath)
		 , m_CurrentDirectory(m_BaseDirectory)
	{
	}
	
	void ContentBrowserPannel::OnAtache()
	{
		RY_PROFILE_FUNCTION();
		m_DirectoryIcon		= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		
		//Files
		m_FileIconDefault	= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIconDefault.png");
		m_FileIconError		= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIconError.png");
		m_FileIconScene		= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIconScene.png");
		m_FileIconShader	= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIconShader.png");
		m_FileIconTexture	= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIconTexture.png");
		m_AssetManger		= m_Project->GetEditorAssetManger();
		
		m_AssetManger->SerialzeAssetRegestriy();
		SetAssetRegestriy(m_BaseDirectory);
		InitAssetFileWatcher();
		
	}

	void ContentBrowserPannel::OnImGuiRender()
	{
		//BrowserPannel();
		AssetPannel();
		AssetRegestriyPannel();
	}

	void ContentBrowserPannel::GetFileList(const std::filesystem::path& curentPath)
	{
		for (auto& p : std::filesystem::directory_iterator(curentPath))
		{
			const auto& path = p.path();
			auto realtivPath = std::filesystem::relative(path, curentPath);

			if (p.is_directory())
			{
				GetFileList(path);
			}
			else
			{
				ImGui::Text("%s", realtivPath.string().c_str());
			}
		}
	}

	void ContentBrowserPannel::BrowserPannel()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		
		static float thumbernailSize = 80.0f;// 128.0f
		static float padding = 16.0f;


		float cellSize = thumbernailSize + padding;

		float pannelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(pannelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{

			const auto& path = directoryEntry.path();
			auto realtivPath = std::filesystem::relative(path, g_AssetsPath);
			std::string fileNameString = path.filename().string();


			ImGui::PushID(fileNameString.c_str());
			FileFormate(directoryEntry);


			if (ImGui::BeginDragDropSource())
			{
				//std::filesystem::path realtivPath(path);
				const wchar_t* itemPath = realtivPath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();

			}

			//RY_CORE_INFO("after drag drop ContentBrowserPannel");
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			ImGui::TextWrapped(fileNameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();


		}

		ImGui::End();
	}

	static ImVec4 GetAssetStateColor(AssetState state)
	{
		switch (state)
		{
		case AssetState::Error:			return ImVec4(0.85, 0.05, 0.1, 1.0);
		case AssetState::Updateing:		return ImVec4(0.3, 0.5, 0.7, 1.0);
		case AssetState::LostConection:	return ImVec4(0.2, 0.5, 0.8, 1.0);
		case AssetState::Ready:			return ImVec4(0.2, 0.8, 0.3, 1.0);
		case AssetState::NotLoaded:		return ImVec4(0.8, 0.7, 0.1, 1.0);
		case AssetState::None:			return ImVec4(1.0, 1.0, 0.5, 1.0);
		default:
			break;
		}
		RY_CORE_ASSERT(false, "Error not defined FileStats: ContentBrowserPannel::SetFileStateColor!");
		return ImVec4(0, 0, 0, 1);
	}

	void ContentBrowserPannel::AssetPannel()
	{
		ImGui::Begin("Asset Content");
		
		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float thumbernailSize = 80.0f;// 128.0f
		static float padding = 16.0f;


		float cellSize = thumbernailSize + padding;

		float pannelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(pannelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);
#if 0
		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{

			const auto& path = directoryEntry.path();
			auto realtivPath = std::filesystem::relative(path, g_AssetsPath);
			std::string fileNameString = path.filename().string();

			ImGui::PushID(fileNameString.c_str());

			FileAsste(directoryEntry, m_AssetManger->GetAssetHandle(path));

			if (ImGui::BeginDragDropSource())
			{
				//std::filesystem::path realtivPath(path);
				//const wchar_t* itemPath = realtivPath.c_str();
				const AssetHandle* handle = &m_AssetManger->GetAssetHandle(path);
				ImGui::SetDragDropPayload("ASSET_BROWSER_ITEM", handle, sizeof(AssetHandle));
				ImGui::EndDragDropSource();
			}

			//RY_CORE_INFO("after drag drop ContentBrowserPannel");
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			ImGui::TextWrapped(fileNameString.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}
#else
		//AssetFileDirectory assetFileDirectory = m_AssetDirectorys[m_CurrentDirectory];
		AssetFileDirectory& assetFileDirectory = m_AssetManger->GetAssetFileDirectory(m_CurrentDirectory);
		const std::vector<std::filesystem::path>& foldersPath = assetFileDirectory.Folders;
		for (const std::filesystem::path& folderPath : foldersPath)
		{
			std::string fileNameString = folderPath.filename().string();
			ImGui::PushID(fileNameString.c_str());
			Ref<Texture2D> icon = m_DirectoryIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.15f, 0.75f, 0.2f, 0.15f), ImVec4(1, 1, 1, 1));
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_CurrentDirectory /= folderPath.filename();
			}
			ImGui::TextWrapped(fileNameString.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
			
		}

		std::vector<AssetHandle> handles = assetFileDirectory.Files;
		for (AssetHandle handle : handles)
		{
			const AssetMetadata& metaData = m_AssetManger->GetMetadata(handle);
			const std::filesystem::path& path = metaData.FilePath;
			std::filesystem::path realtivPath = std::filesystem::relative(path, g_AssetsPath);
			std::string fileNameString = path.filename().string();

			ImGui::PushID(fileNameString.c_str());

			Ref<Texture2D> icon;
			switch (GetAssetTypeFromFilePath(path.filename()))
			{
				case AssetType::Texture2D:
				{
					//AssetManager::GetAsset<Texture2D>(handle);
					//icon = AssetManager::GetAsset<Texture2D>(handle);
					icon = m_FileIconTexture;
					break;
				}
				case AssetType::Shader:
				{
					icon = m_FileIconShader;
					break;
				}
				case AssetType::Scene:
				{
					icon = m_FileIconScene;
					break;
				}
				default:
				{
					icon = m_FileIconDefault;
					break;
				}
			}
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton(
				(ImTextureID)icon->GetRenderID(), 
				{ thumbernailSize , thumbernailSize }, 
				{ 0, 1 }, 
				{ 1, 0 },
				-1, 
				ImVec4(0.15f, 0.85f, 0.2f, 0.1f), 
				GetAssetStateColor(m_AssetManger->GetMetadata(handle).State));


			if (ImGui::BeginDragDropSource())
			{
				//std::filesystem::path realtivPath(path);
				//const wchar_t* itemPath = realtivPath.c_str();
				const AssetHandle* handle = &m_AssetManger->GetAssetHandle(path);
				// TODO: Make Sperate DragDrop Type Names
				ImGui::SetDragDropPayload("ASSET_BROWSER_ITEM", handle, sizeof(AssetHandle)); 
				ImGui::EndDragDropSource();
			}

			//RY_CORE_INFO("after drag drop ContentBrowserPannel");
			ImGui::PopStyleColor();
			

			ImGui::TextWrapped(fileNameString.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}

		
#endif
		ImGui::End();
	}

	void ContentBrowserPannel::AssetRegestriyPannel()
	{
		ImGui::Begin("Asset Regestriy");
		const auto& assetRegestriey  = m_Project->GetEditorAssetManger()->GetAssetRegistry();
		
		for (const auto& [handle, metadata] : assetRegestriey)
		{
			ImGui::Text("AssetHandle(UUID): (%i), Realtiv FilePath: %s", handle, metadata.FilePath.string().c_str());
		}
			
		ImGui::End();
	}

	

	static ImVec4 GetFileStateColor(FileStats state)
	{
		switch (state)
		{
			case FileStats::Error:		return ImVec4(0.85, 0.05, 0.1, 1.0);
			case FileStats::Update:		return ImVec4(0.3, 0.5, 0.7, 1.0);
			case FileStats::Change:		return ImVec4(0.2, 0.5, 0.8, 1.0);
			case FileStats::Ready:		return ImVec4(0.2, 0.8, 0.3, 1.0);
			case FileStats::Wahrning:	return ImVec4(0.8, 0.7, 0.1, 1.0);
			case FileStats::None:		return ImVec4(1.0, 1.0, 0.5, 1.0);
			default:
				break;
		}
		RY_CORE_ASSERT(false, "Error not defined FileStats: ContentBrowserPannel::SetFileStateColor!");
		return ImVec4(0, 0, 0, 1);
	}

	static FileFormats GetFileFormate(const std::filesystem::path& filePath)
	{
		std::filesystem::path extension = filePath.filename().extension();

		if (extension == ".png") return FileFormats::png;
		if (extension == ".txt") return FileFormats::txt;
		if (extension == ".glsl") return FileFormats::glsl;
		if (extension == ".rynexscene") return FileFormats::rynexscene;

		return FileFormats::none;
	}


	

#if 0
	static Ref<Texture2D> GetFileIconeTexture(FileFormats format)
	{
		switch (format)
		{
		case FileFormats::png:		return;
		case FileFormats::txt:		return;
		}


		return Ref<Texture2D>();
	}
#endif // TODO: Make Separte Funktion for Textur

	void ContentBrowserPannel::SetAssetRegestriy(const std::filesystem::path& curentPath)
	{
		m_AssetManger->CreateDirektoryRegestriy(curentPath);
		for (auto& p : std::filesystem::directory_iterator(curentPath))
		{
			const auto& path = p.path();
			if (p.is_directory())
			{
				m_AssetManger->AddDirektoryToDirektory(path, curentPath);
				SetAssetRegestriy(path);
			}
			else
			{
				// assetDirectory.Files.push_back(m_AssetManger->AddFileToRegistry(path));
				m_AssetManger->AddAssetToDirektory(path, curentPath);
			}
		}
		
	}

	void ContentBrowserPannel::FileFormate(const std::filesystem::directory_entry& directoryEntry, FileFormats formate) const
	{
		FileStats state = FileStats::None;
		static float padding = 16.0f;
		static float thumbernailSize = 80.0f;
		Ref<Texture2D> icon;
		if (directoryEntry.is_directory())
		{
			icon = m_DirectoryIcon;
		}
		else
		{
			const auto& path = directoryEntry.path();
			switch (GetFileFormate(path))
			{
				case FileFormats::png: 
			{
				icon = m_FileIconTexture; 
				break;
			}
				case FileFormats::glsl:
			{
				icon = m_FileIconShader;
				break;
			}
				case FileFormats::rynexscene:
			{
				icon = m_FileIconScene;
				break;
			}
				default:
				{
					icon = m_FileIconDefault;
					break;
				}
			}
		}
		
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::ImageButton((ImTextureID)icon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 },-1, ImVec4(0, 0, 0, 0), directoryEntry.is_directory() ? ImVec4(1, 1, 1, 1) : GetFileStateColor(state));
	}
	
#if 0
	void ContentBrowserPannel::FileAsste(const std::filesystem::directory_entry& directoryEntry, AssetHandle handle) const
	{
		AssetState state = m_AssetManger->GetMetadata(handle).State;
		static float padding = 16.0f;
		static float thumbernailSize = 90.0f;
		Ref<Texture2D> icon;
		const auto& path = directoryEntry.path();
		if (directoryEntry.is_directory())
		{
			icon = m_DirectoryIcon;
			if (m_AssetDirectorys.find(path) != m_AssetDirectorys.end())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton((ImTextureID)icon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.15f, 0.75f, 0.2f, 0.15f), ImVec4(1, 1, 1, 1) );
			} 
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton((ImTextureID)icon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.85f, 0.2f, 0.15f, 0.225f), ImVec4(1, 1, 1, 1) );
			}
		}
		else
		{
			
			switch (GetAssetTypeFromFilePath(path.filename()))
			{
				case AssetType::Texture2D:
				{
					//AssetManager::GetAsset<Texture2D>(handle);
					//icon = AssetManager::GetAsset<Texture2D>(handle);
					icon = m_FileIconTexture;
					break;
				}
				case AssetType::Shader:
				{
					icon = m_FileIconShader;
					break;
				}
				case AssetType::Scene:
				{
					icon = m_FileIconScene;
					break;
				}
				default:
				{
					icon = m_FileIconDefault;
					break;
				}
			}
			std::filesystem::path path2 = path.parent_path();
			auto& files = m_AssetDirectorys.at(path2).Files;
			bool inRegestry = false;
			if (files.find(path) != files.end())
			{
				inRegestry = m_AssetManger->IsAssetHandleValid(files.at(path));
			}

			if (m_AssetDirectorys.find(path2) != m_AssetDirectorys.end() && inRegestry)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton((ImTextureID)icon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.15f, 0.85f, 0.2f, 0.1f),  GetAssetStateColor(state));
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4( 0, 0, 0, 0));
				ImGui::ImageButton((ImTextureID)icon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.85f, 0.2f, 0.15f, 0.2f), GetAssetStateColor(state));
			}
		}
		
		
		
	}
#endif

	

#if RY_ASSETMANGER_FILE_WATCHER

	static std::string GetFileWatcherEventString(filewatch::Event change_type)
	{
		return s_Data->FileWatcherEventMap[change_type];
	}

	// TODO: ADD File Watcher
	static void OnFileSystemEvent(const std::filesystem::path& path, const filewatch::Event change_type)
	{
		switch (change_type)
		{
		case filewatch::Event::modified: 
		{
			Application::Get().SubmiteToMainThreedQueue([]()
			{
				Ref<EditorAssetManager> assetManger = Project::GetActive()->GetEditorAssetManger();
				AssetHandle handle = assetManger->GetAssetHandle("Assets");
				if (assetManger->IsAssetLoaded(handle))
				{
					assetManger->ReLoadeAsset(handle);
					RY_CORE_INFO("Thread ReLoadeAsset!");
				}
				else
				{
					RY_CORE_WARN("Thread Not ReLoadeAsset!");
				}
			});  
			return; 
		}
		default:
			break;
		}
		
	};
	
	void ContentBrowserPannel::InitAssetFileWatcher()
	{
		s_Data = new AssetMangerFileWatcherData();
		//for(auto&[folderPath, assetFile] : m_AssetDirectorys)
		{
			//AssetFileDirectory& asset = m_AssetDirectorys["Assets"];
			AssetFileWatcher();
		}
		
	}

	void ContentBrowserPannel::AssetFileWatcher()
	{
		s_Data->AssetFileWatcher = CreateScope<filewatch::FileWatch<std::string>>("Assets", OnFileSystemEvent);
		RY_CORE_INFO("New Thread Raedy!");
	}

#endif
}


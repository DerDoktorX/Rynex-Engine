#include <rypch.h>
#include "ContentBrowserPannel.h"

#include <Rynex/Core/Application.h>

#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Asset/Base/AssetImporter.h>
#include <Rynex/Asset/Import/TextureImporter.h>
#include <Rynex/Asset/Import/ShaderImporter.h>
#include <Rynex/Asset/Import/SceneImporter.h>


#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Utils/EnumString.h>


#include <imgui.h>
#include <imgui_internal.h>

#include <future>

namespace Rynex {

#define RY_FRAMBUFFER_WINDOW 1
#if RY_PATH_IN_LINE
	// extern const std::filesystem::path g_AssetsPath = "Assets";
#endif

	const int s_MaxFileLength = 12;

#if RY_ASSETMANGER_FILE_WATCHER
	struct AssetMangerFileWatcherData
	{
		Scope<filewatch::FileWatch<std::string>> ProjectAssetFileWatcher;
		Scope<filewatch::FileWatch<std::string>> EditorAssetFileWatcher;
		

		std::map<filewatch::Event, std::string> FileWatcherEventMap = {
			{ filewatch::Event::added,			"Event::added"},
			{ filewatch::Event::modified,		"Event::modified"},
			{ filewatch::Event::removed,		"Event::removed"},
			{ filewatch::Event::renamed_new,	"Event::renamed_new"},
			{ filewatch::Event::renamed_old,	"Event::renamed_old"},
		};

		~AssetMangerFileWatcherData()
		{
			ProjectAssetFileWatcher.reset();
			EditorAssetFileWatcher.reset();
		}
	};
	
	static AssetMangerFileWatcherData* s_Data = nullptr;
#endif


	

	
	ContentBrowserPannel::ContentBrowserPannel()
		: m_Project(nullptr)
		 , m_BaseDirectory("")
		 , m_CurrentDirectory(m_BaseDirectory)
	{
	}
	
	void ContentBrowserPannel::OnAtache()
	{
		m_Project = Project::GetActive();
		RY_CORE_INFO("ContentBrowserPannel::OnAtache Start!");
		m_DirectoryIcon		= TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/DirectoryIcon.png");
		
		//Files
		Ref<Texture> defaultIcon = TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/FileIconDefault.png");
#if 0
		AssetManager::CreatLocaleAsset<Texture>(defaultIcon);
		m_FileIconDefault	= AssetManager::GetAsset<Texture>(defaultIcon->Handle);
#else
		m_FileIconDefault = defaultIcon;
#endif
		m_FileIconError		= TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/FileIconError.png");
		m_FileIconScene		= TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/FileIconScene.png");
		m_FileIconShader	= TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/FileIconShader.png");
		m_FileIconTexture	= TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/FileIconTexture.png");
		m_AssetManger		= m_Project->GetEditorAssetManger();

		m_AssetManger->SerialzeAssetRegistry();
		RY_CORE_INFO("Fished SerialzeAssetRegistry!");
		m_BaseDirectory = Project::GetActiveAssetDirectory();
		m_CurrentDirectory = m_BaseDirectory;
		InitAssetFileWatcher();


		RY_CORE_INFO("ContentBrowserPannel::OnAtache Finished!");
	}

	void ContentBrowserPannel::OnDetache()
	{
		DestroyAssetFileWatcher();

		RY_DESTROY_REF(m_Project);
		m_BaseDirectory.clear();
		m_CurrentDirectory.clear();

		RY_DESTROY_REF(m_DirectoryIcon);
		RY_DESTROY_REF(m_FileIconDefault);
		RY_DESTROY_REF(m_FileIconError);
		RY_DESTROY_REF(m_FileIconScene);
		RY_DESTROY_REF(m_FileIconShader);
		RY_DESTROY_REF(m_FileIconTexture);

		RY_DESTROY_REF(m_AssetManger);

		m_RegisterItemes.clear();
		m_RegisterItemes.shrink_to_fit();
		m_FileItemes.clear();
		m_FileItemes.shrink_to_fit();
	}

	void ContentBrowserPannel::OpenAssetPannel()
	{
		m_WindowAssetPannelOpen = true;
	}

	void ContentBrowserPannel::OpenRegestriyPannel()
	{
		m_WindowRegestriyPannellOpen = true;
	}

	void ContentBrowserPannel::OnImGuiRender()
	{
		AssetRegestriyPannel();

		AssetPannel();
		DelateAsset();
		OnloadeAssetsList();
		
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

	static ImVec4 GetAssetStateColor(AssetState state)
	{
		switch (state)
		{
			case AssetState::Error:			return ImVec4(0.95, 0.05, 0.1, 1.0);
			case AssetState::Updateing:		return ImVec4(0.3, 0.5, 0.7, 1.0);
			case AssetState::LostConection:	return ImVec4(0.75, 0.1, 0.05, 1.0);
			case AssetState::Loading:		return ImVec4(0.85, 0.275, 0.05, 1.0);
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
		if(m_WindowAssetPannelOpen)
		{
			ImGui::Begin("Asset Content", &m_WindowAssetPannelOpen, ImGuiWindowFlags_None);
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered())
			{
				ImGui::OpenPopup("Empty-Space-Menu");
			}

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

			float pannelWidth1 = ImGui::GetContentRegionAvail().x;
			float pannelWidth2 = ImGui::GetWindowWidth();
			pannelWidth2 -= 30.0f;
			int columnCount = (int)(pannelWidth2 / cellSize);
			if (columnCount < 1)
				columnCount = 1;

			ImGui::Columns(columnCount, 0, false);


			if (m_AssetManger->IsCurentAssetState(m_CurrentDirectory))
			{
				m_FileItemes = m_AssetManger->GetCurentAssetInformation(m_CurrentDirectory);
				RY_CORE_WARN("New Asset Items Ordert");
			}

			for (AssetBrowserDataThreade& data : m_FileItemes)
			{
				if (data.IsAssset)
				{
					ImGuiAssetFile(data, thumbernailSize);
				}
				else if (data.IsFolder)
				{
					ImGui::PushID(data.Name.c_str());
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					if (data.Path != m_BaseDirectory / "Loadead (NotAssetFiles)" && data.Path != m_BaseDirectory / "Unknown File Types")
					{
						uint32_t textureID = m_DirectoryIcon->GetRenderID();
						ImTextureID imTextureID = reinterpret_cast<ImTextureID>(&textureID);
						ImVec2 textureSize{ thumbernailSize , thumbernailSize };
						ImVec2 uv1{ 0, 1 };
						ImVec2 uv2{ 1, 0 };
						ImVec4 bacgroundColor{ 0.15f, 0.85f, 0.2f, 0.1f };
						ImVec4 iconColor{ 1, 1, 1, 1 };

						ImGui::ImageButton(
							"",
							imTextureID,
							textureSize,
							uv1,
							uv2,
							bacgroundColor,
							iconColor
						);
					}
					else
					{
						uint32_t textureID = m_DirectoryIcon->GetRenderID();
						ImTextureID imTextureID = reinterpret_cast<ImTextureID>(&textureID);
						
						ImVec2 textureSize{ thumbernailSize , thumbernailSize };
						ImVec2 uv1{ 0, 1 };
						ImVec2 uv2{ 1, 0 };
						ImVec4 bacgroundColor{ 0.15f, 0.85f, 0.2f, 0.1f };
						ImVec4 iconColor{ 1, 1, 1, 1 };

						ImGui::ImageButton(
							"",
							imTextureID, 
							textureSize,
							uv1,
							uv2,
							bacgroundColor,
							iconColor
						);
					}

					ImGui::PopStyleColor();
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_CurrentDirectory /= data.Name;
					}

					if (ImGui::BeginPopupContextItem(data.Name.c_str()))
					{
						if (ImGui::MenuItem("Delete Folder"))
						{
							RY_CORE_WARN("Delete Folder {} form path {0}", data.Name.c_str(), data.PathString.c_str());
						}

						ImGui::EndPopup();
					}
					ImGui::TextWrapped(data.Name.c_str());
					ImGui::NextColumn();
					ImGui::PopID();
				}
				else
				{
					RY_CORE_ASSERT(false);
				}
			}

			if (ImGui::BeginPopup("Empty-Space-Menu"))
			{
				if (ImGui::BeginMenu("New"))
				{
					NewMenue();
				}
				if (ImGui::MenuItem("Scane Directory"))
				{

				}
				if (ImGui::MenuItem("Onlode All Assets Form Register"))
				{
					m_AssetManger->ClearLodeadAssetList();
				}


				ImGui::EndPopup();
			}

			ImGui::End();
		}
	
			
	}

	void ContentBrowserPannel::ImGuiAssetFile(AssetBrowserDataThreade& data, float thumbernailSize)
	{
		std::filesystem::path& relativProjectPath = data.RelativProjectPath;
		std::filesystem::path& path = data.Path;
		AssetHandle& handle = data.Handle;
		AssetType& type = data.Type;

		if (relativProjectPath == "")
			relativProjectPath = std::filesystem::relative(path, m_BaseDirectory);

		std::string& name = data.Name;
		std::string& typeString = data.TypeString;
		ImGui::PushID(name.c_str());

		Ref<Texture>& texture = data.Texture;
		if (!texture)
		{
			
			switch (type)
			{
			case AssetType::Texture2D:
			{
				texture = m_FileIconTexture;
				break;
			}
			case AssetType::Shader:
			{
				texture = m_FileIconShader;
				break;
			}
			case AssetType::Scene:
			{
				texture = m_FileIconScene;
				break;
			}
			case AssetType::Framebuffer:
			{
				// TODO: Creat Framebuffer Image
				texture = m_FileIconDefault;
				break;
			}
			case AssetType::Model:
			{
				// TODO: Creat Model Image
				texture = m_FileIconDefault;
				break;
			}
			case AssetType::MeshSource:
			{
				// TODO: Creat Model Image
				texture = m_FileIconDefault;
				break;
			}
			case AssetType::MeshStatic:
			{
				// TODO: Creat Model Image
				texture = m_FileIconDefault;
				break;
			}
			case AssetType::Mesh:
			{
				// TODO: Creat Mesh Image
				texture = m_FileIconDefault;
				break;
			}
			default:
			{
				texture = m_FileIconDefault;
				break;
			}
			}
		}

		AssetState& state = data.State;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		uint32_t renderID = texture->GetRenderID();
		ImTextureID imTextureID = ImTextureID(renderID);
		ImVec2 textureSize = ImVec2{ thumbernailSize , thumbernailSize };
		ImVec2 uv1 = ImVec2{ 0, 1 };
		ImVec2 uv2 = ImVec2{ 1, 0 };
		ImVec4 bacgroundColor = ImVec4{ 0.15f, 0.85f, 0.2f, 0.1f };
		ImVec4 iconColor = GetAssetStateColor(state);
		ImGui::ImageButton(
			"",
			imTextureID,
			textureSize,
			uv1,
			uv2,
			bacgroundColor,
			iconColor
		);

		if (state == AssetState::LostConection || state == AssetState::Error)
		{
			// ImGui::BeginDragDropSource();
			// const AssetHandle* handleE = &m_AssetManger->GetAssetHandle(path);
			// ImGui::SetDragDropPayload(GetAssetTypeMoveAssetInfosName(type).c_str(), handleE, sizeof(AssetHandle));
			// ImGui::EndDragDropSource();
		}
		else
		{
			if (ImGui::BeginDragDropTarget())
			{
				
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeString.c_str()))
				{
					
					(AssetHandle*)payload->Data;
					AssetHandle handleO = AssetHandle(handle);

				}
				ImGui::EndDragDropTarget();

			}
		}

		if (ImGui::BeginPopupContextItem(name.c_str()))
		{
			if (ImGui::MenuItem("Open"));
			if (ImGui::MenuItem("Rename"));
			// if (ImGui::MenuItem("Delete Asset"));
			// DelateListeAsset({ fileNameString, m_CurrentDirectory });
			if (ImGui::MenuItem("Delete Asset + File"));
			if (ImGui::MenuItem("OnLoade Asset"))
				OnLoadeAsset(handle);
			if (ImGui::MenuItem("Details"));
			ImGui::EndPopup();
		}

		if (ImGui::BeginDragDropSource())
		{
			const AssetHandle handle = m_AssetManger->GetAssetHandle(path);
			const AssetHandle* handlePtr = &handle;
			ImGui::SetDragDropPayload(typeString.c_str(), handlePtr, sizeof(AssetHandle));
			ImGui::EndDragDropSource();
		}

		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			switch (type)
			{
			case AssetType::Framebuffer:
			{

				m_CurrentDirectory /= name;
				break;
			}
			default:
			{
				break;
			}
			}
		}


		ImGui::TextWrapped(name.c_str());
		ImGui::NextColumn();
		ImGui::PopID();
		
	}

	void ContentBrowserPannel::AssetRegestriyPannel()
	{
		if(m_WindowRegestriyPannellOpen)
		{
			ImGui::Begin("Asset Regestriy", &m_WindowRegestriyPannellOpen, ImGuiWindowFlags_None);
			
			if (m_AssetManger->IsCurentRegistryAssetChandge())
			{
				m_RegisterItemes = m_AssetManger->GetCurentAssetRegistry();
			}
			
			for (const auto& [handle, metadata, filePath] : m_RegisterItemes)
			{
				if(filePath != "")
					ImGui::Text("AssetHandle(UUID): (%ull), Realtiv FilePath: %s", handle, filePath.c_str());
				else
					ImGui::Text("AssetHandle(UUID): (%ull), Data Type: %i", handle, (int)metadata.Type);
			}
			ImGui::End();
		}
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


	void ContentBrowserPannel::NewMenue()
	{
		if (ImGui::BeginMenu("Asset")) 
			NewAsset();
		if (ImGui::MenuItem("File"));

		ImGui::EndMenu();
	}

	void ContentBrowserPannel::NewAsset()
	{
		if (ImGui::MenuItem("FrameBuffer"))
		{
			m_OppenWindow = SettingsPopUpWindow::CeateFarmbufferSettings;
		}
		if (ImGui::MenuItem("VertexArray"))
		{
			m_OppenWindow = SettingsPopUpWindow::CeateVertexArraySettings;
		}
		ImGui::EndMenu();
	}

	void ContentBrowserPannel::NewFile()
	{
		 if (ImGui::BeginMenu("Asset"))		
			 NewAsset();
		 if (ImGui::MenuItem("File"));
		 ImGui::EndMenu();
	}


	void ContentBrowserPannel::DelateListeAsset(DealteAsset deleateAsset)
	{
		m_DealeteAssetList.emplace_back(deleateAsset);
	}

	void ContentBrowserPannel::DelateAsset()
	{
		
		for (auto& assetDealte :  m_DealeteAssetList)
		{
			RY_CORE_ASSERT(false, "Asset Dealt System Not Finished!");

		}
	}

	void ContentBrowserPannel::DelateListeFolder(const std::filesystem::path& folderPath)
	{
		m_DealeteFolderList.emplace_back(folderPath);
	}

	void ContentBrowserPannel::DelateFolder()
	{
		for (auto& folderDealte : m_DealeteFolderList)
		{
			RY_CORE_ASSERT(false, "Folder Dealte System Not Finished!");
			
		}
	}

	

#if RY_ASSETMANGER_FILE_WATCHER

	static std::string GetFileWatcherEventString(filewatch::Event change_type)
	{
		return s_Data->FileWatcherEventMap[change_type];
	}

	

	static void OnFileSystemEventProjectDirectory(std::string filepath, const filewatch::Event change_type)
	{
		RY_CORE_TRACE("Event File Ptah {}", filepath);
		std::filesystem::path filePath = (Project::GetActiveAssetDirectory() / std::filesystem::path(filepath)).generic_string();

		
		RY_CORE_TRACE("FileWatcher Project Event Info : {}!", filePath.string().c_str());
		switch (change_type)
		{
			case filewatch::Event::modified:
			{

				Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
				assetManger->EventAsyncModified(filePath);

				break;
			}
			case filewatch::Event::added:
			{
				Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
				assetManger->EventAsyncAdded(filePath);
				break;

			}
			case filewatch::Event::removed:
			{

				Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
				assetManger->EventAsyncRemoved(filePath);
				break;
			}
			case filewatch::Event::renamed_new:
			{
				Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
				assetManger->EventAsyncRenamedNew(filePath);
				break;
			}
			case filewatch::Event::renamed_old:
			{
				Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
				assetManger->EventAsyncRenamedOld(filePath);
				break;
			}
			default:
				RY_CORE_FATAL("Thread Not found Event! {0}", filePath.string().c_str());
				break;
		}
		
	};

	static void OnFileSystemEventEditorAssetDirectory(std::string filepath, const filewatch::Event change_type)
	{
		RY_CORE_TRACE("Event File Ptah {}", filepath);
		std::filesystem::path filePath = ("..\\Rynex-Editor\\Editor-Assets" / std::filesystem::path(filepath)).generic_string();

		std::string event = GetFileWatcherEventString(change_type);
		RY_CORE_TRACE("FileWatcher Editor Event: ({}) Info: ({})!", event, filePath.string().c_str());
		switch (change_type)
		{
		case filewatch::Event::modified:
		{

			Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
			assetManger->EventAsyncModified(filePath);

			break;
		}
		case filewatch::Event::added:
		{
			Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
			assetManger->EventAsyncAdded(filePath);
			break;

		}
		case filewatch::Event::removed:
		{

			Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
			assetManger->EventAsyncRemoved(filePath);
			break;
		}
		case filewatch::Event::renamed_new:
		{
			Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
			assetManger->EventAsyncRenamedNew(filePath);
			break;
		}
		case filewatch::Event::renamed_old:
		{
			Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
			assetManger->EventAsyncRenamedOld(filePath);
			break;
		}
		default:
			RY_CORE_FATAL("Thread Not found Event! {0}", filePath.string().c_str());
			break;
		}

	};


	void ContentBrowserPannel::InitAssetFileWatcher()
	{
		RY_CORE_INFO("Init Asset FileWatcher");

		s_Data = new AssetMangerFileWatcherData();
		{
			AssetFileWatcher();
		}
		
	}

	void ContentBrowserPannel::DestroyAssetFileWatcher()
	{
		
		if (nullptr == s_Data)
			return;

		delete s_Data;
		s_Data = nullptr;

		RY_CORE_INFO("Destroy Asset FileWatcher");
	}

	void ContentBrowserPannel::OnloadeAssetsList()
	{
		for (AssetHandle assetonLoade : m_OnLoadeAsset)
		{
			Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
			assetManger->UnLoadeFileAsset(assetonLoade);
		}
		m_OnLoadeAsset.clear();
	}

	void ContentBrowserPannel::OnLoadeAsset(AssetHandle handle)
	{
		m_OnLoadeAsset.emplace_back(handle);
		
	}

	void ContentBrowserPannel::AssetFileWatcher()
	{
		std::string projectFilePathStr = Project::GetActiveAssetDirectory().string();
		RY_CORE_TRACE("Project-Asset: Watch File Path ({})", projectFilePathStr);
		s_Data->ProjectAssetFileWatcher = CreateScope<filewatch::FileWatch<std::string>>(projectFilePathStr, OnFileSystemEventProjectDirectory);
#if RY_EDITOR_ASSET_FILEWATCHER
		std::string editorFilePathStr = "..\\Rynex-Editor\\Editor-Assets";
		RY_CORE_TRACE("Editor-Asset: Watch File Path ({})", projectFilePathStr);
		s_Data->EditorAssetFileWatcher = CreateScope<filewatch::FileWatch<std::string>>(editorFilePathStr, OnFileSystemEventEditorAssetDirectory);
#else
		s_Data->EditorAssetFileWatcher = nullptr;
#endif
		RY_CORE_INFO("New Thread Raedy!");
	}

#endif
}


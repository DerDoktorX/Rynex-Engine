#include <rypch.h>
#include "ContentBrowserPannel.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Core/Application.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Asset/Base/AssetImporter.h>
#include <Rynex/Asset/Import/TextureImporter.h>
#include <Rynex/Asset/Import/ShaderImporter.h>
#include <Rynex/Asset/Import/SceneImporter.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/Rendering/Renderer2D.h>

#include <imgui/imgui.h>


namespace Rynex {

#if RY_PATH_IN_LINE
	extern const std::filesystem::path g_AssetsPath = "Assets";
#endif

	const int s_MaxFileLength = 12;

#if RY_ASSETMANGER_FILE_WATCHER
	struct AssetMangerFileWatcherData
	{
		Scope<filewatch::FileWatch<std::string>> AssetFileWatcher;
		std::map<filewatch::Event, std::string> FileWatcherEventMap = {
			{ filewatch::Event::added,			"Event::added"},
			{ filewatch::Event::modified,		"Event::modified"},
			{ filewatch::Event::removed,		"Event::removed"},
			{ filewatch::Event::renamed_new,	"Event::renamed_new"},
			{ filewatch::Event::renamed_old,	"Event::renamed_old"},
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
#if RY_PATH_IN_LINE
		m_DirectoryIcon		= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		
		//Files
		m_FileIconDefault	= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIconDefault.png");
		m_FileIconError		= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIconError.png");
		m_FileIconScene		= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIconScene.png");
		m_FileIconShader	= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIconShader.png");
		m_FileIconTexture	= TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIconTexture.png");
		m_AssetManger		= m_Project->GetEditorAssetManger();
#endif
		m_AssetManger->SerialzeAsseRegistry();

		SetAssetRegestriy(m_BaseDirectory);		
		InitAssetFileWatcher();
		
	}

	void ContentBrowserPannel::OnImGuiRender()
	{
		RY_PROFILE_FUNCTION();
		AssetRegestriyPannel();
		AssetPannel();
		DelateAsset();
	}

	void ContentBrowserPannel::GetFileList(const std::filesystem::path& curentPath)
	{
		RY_PROFILE_FUNCTION();
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
		RY_PROFILE_FUNCTION();
		switch (state)
		{
			case AssetState::Error:			return ImVec4(0.85, 0.05, 0.1, 1.0);
			case AssetState::Updateing:		return ImVec4(0.3, 0.5, 0.7, 1.0);
			case AssetState::LostConection:	return ImVec4(0.95, 0.1, 0.05, 1.0);
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
		RY_PROFILE_FUNCTION();
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

		const AssetFileDirectory& assetFileDirectory = m_AssetManger->GetAssetFileDirectory(m_CurrentDirectory);
		const std::vector<std::filesystem::path>& foldersPath = assetFileDirectory.Folders;
		// Folders
		for (const std::filesystem::path& folderPath : foldersPath)
		{
			std::string fileNameString = folderPath.filename().string();
			ImGui::PushID(fileNameString.c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			if(folderPath != m_BaseDirectory / "Loadead (NotAssetFiles)" && folderPath != m_BaseDirectory / "Unknown File Types")
				ImGui::ImageButton((ImTextureID)m_DirectoryIcon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.15f, 0.75f, 0.2f, 0.15f), ImVec4(1, 1, 1, 1));
			else
				ImGui::ImageButton((ImTextureID)m_DirectoryIcon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.875f, 0.875f, 0.35f, 0.5f), ImVec4(1, 1, 1, 1));

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
		// Files
		for (AssetHandle handle : handles)
		{
			
			const AssetMetadata& metaData = m_AssetManger->GetMetadata(handle);
			const std::filesystem::path& path = metaData.FilePath;


			std::filesystem::path realtivPath = std::filesystem::relative(path, g_AssetsPath);
			std::string fileNameString = path.filename().string();

			ImGui::PushID(fileNameString.c_str());

			Ref<Texture2D> icon;
			AssetType assetType = GetAssetTypeFromFilePath(path.filename());
			switch (assetType)
			{
				case AssetType::Texture2D:
				{
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

			if (ImGui::BeginPopupContextItem(fileNameString.c_str()))
			{
				if (ImGui::MenuItem("Delete Asset"))
					DelateListeAsset({ fileNameString, m_CurrentDirectory });

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropSource())
			{
			
				const AssetHandle* handle = &m_AssetManger->GetAssetHandle(path);
				ImGui::SetDragDropPayload( GetAssetTypeDragAndDropName(assetType).c_str(), handle, sizeof(AssetHandle) );
				ImGui::EndDragDropSource();
			}

			//RY_CORE_INFO("after drag drop ContentBrowserPannel");
			ImGui::PopStyleColor();


			ImGui::TextWrapped(fileNameString.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}
		

		

		ImGui::End();
	}

	void ContentBrowserPannel::AssetRegestriyPannel()
	{
		RY_PROFILE_FUNCTION();
		ImGui::Begin("Asset Regestriy");
		const auto& assetRegestriey  = m_Project->GetEditorAssetManger()->GetHandleRegistry();
		
		for (const auto& [handle, metadata] : assetRegestriey)
		{
			ImGui::Text("AssetHandle(UUID): (%i), Realtiv FilePath: %s", handle, metadata.FilePath.string().c_str());
		}
			
		ImGui::End();
	}

	

	static ImVec4 GetFileStateColor(FileStats state)
	{
		RY_PROFILE_FUNCTION();
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
		RY_PROFILE_FUNCTION();
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
		RY_PROFILE_FUNCTION();
		//m_AssetManger->CreateDirektoryRegestriy(curentPath);
		for (auto& p : std::filesystem::directory_iterator(curentPath))
		{
			const auto& path = p.path();
			if (p.is_directory())
			{
				m_AssetManger->AddDirectoryToParent(path);
				SetAssetRegestriy(path);
			}
			else
			{
				// assetDirectory.Files.push_back(m_AssetManger->AddFileToRegistry(path));
				if(GetAssetTypeFromFilePath(path) != AssetType::None)
				{
					m_AssetManger->CreateAsset(path);
				}
				
			}
		}
		
	}

	void ContentBrowserPannel::FileFormate(const std::filesystem::directory_entry& directoryEntry, FileFormats formate) const
	{
		RY_PROFILE_FUNCTION();
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
	
	void ContentBrowserPannel::DelateListeAsset(DealteAsset deleateAsset)
	{
		m_DealeteAssetList.push_back(deleateAsset);
	}

	void ContentBrowserPannel::DelateAsset()
	{
		
		for (auto& assetDealte :  m_DealeteAssetList)
		{
			RY_CORE_ASSERT(false, "Asset Dealt System Not Finished!");
			m_AssetManger->GetAssetFileDirectory(assetDealte.AssetFileParentPath);
		}
	}

	

#if RY_ASSETMANGER_FILE_WATCHER

	static std::string GetFileWatcherEventString(filewatch::Event change_type)
	{
		RY_PROFILE_FUNCTION();
		return s_Data->FileWatcherEventMap[change_type];
	}

	// TODO: ADD File Watcher
	static void OnFileSystemEvent(std::string filepath, const filewatch::Event change_type)
	{
		std::filesystem::path filePath = ("Assets" / std::filesystem::path(filepath)).generic_string();
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(500ms);
		
		switch (change_type)
		{
			case filewatch::Event::modified:
			{
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
					RY_CORE_INFO("Thread ReLoadeAsset! Begine");
					Ref<EditorAssetManager> assetManger = Project::GetActive()->GetEditorAssetManger();
					AssetHandle handle = assetManger->GetAssetHandle(filePath);
					if (assetManger->IsAssetLoaded(handle))
					{
						AssetImporter::ReLoadeAsset(handle, assetManger->GetMetadata(handle));
						RY_CORE_INFO("Thread ReLoadeAsset! {0}", filePath.string().c_str());
						//Renderer2D::Shutdown();
						//Renderer::Init();
						
					}
					else
					{
						RY_CORE_WARN("Thread Not ReLoadeAsset! Finished {0}", filePath.string().c_str());
					}

				});
				break;
			}
			case filewatch::Event::added:
			{
				RY_CORE_INFO("Thread Added! Begine");
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
				});
				break;

			}
			case filewatch::Event::removed:
			{
				RY_CORE_INFO("Thread Removed! Begine");
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
				});
				break;
			}
			case filewatch::Event::renamed_new:
			{
				RY_CORE_INFO("Thread Renamed New! Begine");
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
				});
				break;
			}
			case filewatch::Event::renamed_old:
			{
				RY_CORE_INFO("Thread Renamed Old! Begine");
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
				});
				break;
			}
			default:
				RY_CORE_WARN("Thread Not found Event!");
				break;
		}
		RY_CORE_INFO("Thread Path! -> {0} Now Fished", filePath.string().c_str());
	};

	void ContentBrowserPannel::InitAssetFileWatcher()
	{
		RY_PROFILE_FUNCTION();
		s_Data = new AssetMangerFileWatcherData();
		//for(auto&[folderPath, assetFile] : m_AssetDirectorys)
		{
			//AssetFileDirectory& asset = m_AssetDirectorys["Assets"];
			AssetFileWatcher();
		}
		
	}

	void ContentBrowserPannel::AssetFileWatcher()
	{
		RY_PROFILE_FUNCTION();

		s_Data->AssetFileWatcher = CreateScope<filewatch::FileWatch<std::string>>("Assets", OnFileSystemEvent);
		RY_CORE_INFO("New Thread Raedy!");
	}

#endif
}


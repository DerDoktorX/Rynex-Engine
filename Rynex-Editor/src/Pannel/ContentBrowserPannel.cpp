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

#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Serializers/FrambufferSerializer.h>


#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "future"

namespace Rynex {

#define RY_FRAMBUFFER_WINDOW 1
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
#if RY_PATH_IN_LINE
		RY_CORE_INFO("ContentBrowserPannel::OnAtache Start!");
		m_DirectoryIcon		= TextureImporter::LoadTexture("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		
		//Files
		Ref<Texture> defaultIcon = TextureImporter::LoadTexture("Resources/Icons/ContentBrowser/FileIconDefault.png");
		AssetManager::CreatLocaleAsset<Texture>(defaultIcon);
		m_FileIconDefault	= AssetManager::GetAsset<Texture>(defaultIcon->Handle);
		m_FileIconError		= TextureImporter::LoadTexture("Resources/Icons/ContentBrowser/FileIconError.png");
		m_FileIconScene		= TextureImporter::LoadTexture("Resources/Icons/ContentBrowser/FileIconScene.png");
		m_FileIconShader	= TextureImporter::LoadTexture("Resources/Icons/ContentBrowser/FileIconShader.png");
		m_FileIconTexture	= TextureImporter::LoadTexture("Resources/Icons/ContentBrowser/FileIconTexture.png");
		m_AssetManger		= m_Project->GetEditorAssetManger();
#endif
		m_AssetManger->SerialzeAssetRegistry();

		SetAssetRegestriy(m_BaseDirectory, m_AssetManger);
		InitAssetFileWatcher();

#if RY_FRAMBUFFER_WINDOW
		m_FrambufferWindow = CreateRef<FrambufferWindow>();
		m_FrambufferWindow->OnAtache();
#endif

		RY_CORE_INFO("ContentBrowserPannel::OnAtache Finished!");
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

				if (folderPath != m_BaseDirectory / "Loadead (NotAssetFiles)" && folderPath != m_BaseDirectory / "Unknown File Types")
					ImGui::ImageButton((ImTextureID)m_DirectoryIcon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.15f, 0.75f, 0.2f, 0.15f), ImVec4(1, 1, 1, 1));
				else
					ImGui::ImageButton((ImTextureID)m_DirectoryIcon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.875f, 0.875f, 0.35f, 0.5f), ImVec4(1, 1, 1, 1));

				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					m_CurrentDirectory /= folderPath.filename();
				}

				if (ImGui::BeginPopupContextItem(fileNameString.c_str()))
				{
					const AssetFileDirectory& folder = m_AssetManger->GetAssetFileDirectory(folderPath);
					if ((!folder.Files.size()) && ImGui::MenuItem("Delete Folder"))
					{
						RY_CORE_WARN("Delete Folder {0}", fileNameString.c_str());
					}

					ImGui::EndPopup();
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

				Ref<Texture> icon;
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
				case AssetType::Framebuffer:
				{
					// TODO: Creat Framebuffer Image
					icon = m_FileIconDefault;
					break;
				}
				case AssetType::Model:
				{
					// TODO: Creat Model Image
					icon = m_FileIconDefault;
					break;
				}
				case AssetType::Mesh:
				{
					// TODO: Creat Mesh Image
					icon = m_FileIconDefault;
					break;
				}
				default:
				{
					icon = m_FileIconDefault;
					break;
				}
				}
				AssetState state = m_AssetManger->GetMetadata(handle).State;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton(
					(ImTextureID)icon->GetRenderID(),
					{ thumbernailSize , thumbernailSize },
					{ 0, 1 },
					{ 1, 0 },
					-1,
					ImVec4(0.15f, 0.85f, 0.2f, 0.1f),
					GetAssetStateColor(state));

				if (state == AssetState::LostConection || state == AssetState::Error)
				{
					ImGui::BeginDragDropSource();
					const AssetHandle* handleE = &m_AssetManger->GetAssetHandle(path);
					ImGui::SetDragDropPayload(GetAssetTypeMoveAssetInfosName(assetType).c_str(), handleE, sizeof(AssetHandle));
					ImGui::EndDragDropSource();
				}
				else
				{
					if (ImGui::BeginDragDropTarget())
					{

						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetAssetTypeMoveAssetInfosName(assetType).c_str()))
						{
							(AssetHandle*)payload->Data;
							AssetHandle handleO = m_AssetManger->GetAssetHandle(path);

						}
						ImGui::EndDragDropTarget();

					}
				}

				if (ImGui::BeginPopupContextItem(fileNameString.c_str()))
				{
					if (ImGui::MenuItem("Open"));
					if (ImGui::MenuItem("Rename"));
					if (ImGui::MenuItem("Delete Asset"));
					//DelateListeAsset({ fileNameString, m_CurrentDirectory });
					if (ImGui::MenuItem("Delete Asset + File"));
					if (ImGui::MenuItem("Details"));
					ImGui::EndPopup();
				}

				if (ImGui::BeginDragDropSource())
				{
					const AssetHandle* handle = &m_AssetManger->GetAssetHandle(path);
					ImGui::SetDragDropPayload(GetAssetTypeDragAndDropName(assetType).c_str(), handle, sizeof(AssetHandle));
					ImGui::EndDragDropSource();
				}

				//RY_CORE_INFO("after drag drop ContentBrowserPannel");
				ImGui::PopStyleColor();
				switch (assetType)
				{
				case AssetType::Framebuffer:
				{
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_CurrentDirectory /= path.filename();
					}
					break;
				}
				default:
				{
					break;
				}
				}


				ImGui::TextWrapped(fileNameString.c_str());
				ImGui::NextColumn();
				ImGui::PopID();
			}

			if (ImGui::BeginPopup("Empty-Space-Menu"))
			{
				if (ImGui::BeginMenu("New"))
				{
					NewMenue();
				}
				if (ImGui::MenuItem("Scane Directory"));
				ImGui::EndPopup();
			}



#if RY_FRAMBUFFER_WINDOW

			if (m_OppenWindow == SettingsPopUpWindow::CeateFarmbufferSettings)
			{
				ImGui::OpenPopup("Settings-FrameBuffer");
				m_OppenWindow = SettingsPopUpWindow::None;
				m_CreateFrambuffer = CreateFrambuffer();
				m_FrambufferWindow->OnOpen(m_CurrentDirectory);
			}
			m_FrambufferWindow->OnImGuiRender();
#else
			CreateFrambufferAsset(m_CurrentDirectory);

#endif
			ImGui::End();
		}
	}

	void ContentBrowserPannel::AssetRegestriyPannel()
	{
		if(m_WindowRegestriyPannellOpen)
		{
			ImGui::Begin("Asset Regestriy", &m_WindowRegestriyPannellOpen, ImGuiWindowFlags_None);
			const auto& assetRegestriey = m_Project->GetEditorAssetManger()->GetHandleRegistry();

			for (const auto& [handle, metadata] : assetRegestriey)
			{
				ImGui::Text("AssetHandle(UUID): (%i), Realtiv FilePath: %s", handle, metadata.FilePath.string().c_str());
			}

			ImGui::End();
		}
	}

	void ContentBrowserPannel::CreateFrambufferAsset(std::filesystem::path& path)
	{
		if (m_OppenWindow == SettingsPopUpWindow::CeateFarmbufferSettings)
		{
			ImGui::OpenPopup("Settings-FrameBuffer");
			m_OppenWindow = SettingsPopUpWindow::None;
			m_CreateFrambuffer = CreateFrambuffer();
		}

		if (ImGui::BeginPopupModal("Settings-FrameBuffer", NULL, ImGuiWindowFlags_MenuBar))
		{
			ImVec2 windowSize = ImGui::GetWindowSize();
			{
				ImGui::PushID("FrameBuffer Path");
				ImGui::Columns(1, "##FrameBuffer Path", true);
				ImGui::Text("Path: %s", path.string().c_str());
				ImGui::SameLine();
				ImGui::InputText("##FileName", m_CreateFrambuffer.Buffer, 64);
				ImGui::SameLine();
				ImGui::Text(".ryframe");
				ImGui::NextColumn();
				ImGui::Columns(1);
				ImGui::PopID();
			}
			
			auto& spec = m_CreateFrambuffer.FrambufferSpec;
			{
				ImGui::PushID("FrameBuffer Size");
				{
					ImGui::NewLine();
					ImGui::Columns(2, "##FrameBuffer Size", true);						
					float columWithe = windowSize.y / 3.25;
					ImGui::SetColumnWidth(0, columWithe);
					ImGui::SetColumnWidth(1, columWithe);
					ImGui::InputInt("Width", (int*)&spec.Width);							
					

					ImGui::NextColumn();
					ImGui::InputInt("Height", (int*)&spec.Height);
					if ((int)spec.Height < 1)
						spec.Height = 1;
					if ((int)spec.Width < 1)
						spec.Width = 1;
					ImGui::NextColumn();
					ImGui::Columns(1);
				}
				ImGui::PopID();
			}
			

			{
				char* textureFormatChar[] = {
					"None",
					"RGBA8",
					"RED_INTEGER",
					"DEPTH24STENCIL8",
				};
				uint32_t textureFormatLength = 4; 
				char* textureWarpingChar[] = {
					"None",
					"Repeate",
					"MirrorRepeate",
					"ClampEdge",
					"ClampBorder",
					"MirrorClampEdge",
				};
				uint32_t textureWarpingLength = 6;
				char* texWarpingDimenChar[] = { "S","T","R" };
				uint32_t texWarpingDimenLength = 3;
				char* textureFilteringChar[] = {
					"None",
					"Linear",
					"Nearest",
				};
				uint32_t textureFilteringLength = 3;
				ImGui::NewLine();
				ImGui::Text("Texture Attachments:");
				ImGui::NewLine();
#if 0
				ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
				flags |= ImGuiTreeNodeFlags_SpanAvailWidth;


				bool opende = ImGui::TreeNodeEx(&id, flags, tag.c_str());
#endif
				uint32_t index = 0;
				int removeTex = -1;
				//ImGuiTreeNodeFlags flags = (false ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
				//flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
				std::string tagNme = "Texture: ";
				std::string idName = "FramTexAtach: ";
				const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap;
				for (FramebufferTextureSpecification& framTexSpec : spec.Attachments.Attachments)
				{
					ImVec2 contenRegionAvablie = ImGui::GetContentRegionAvail();
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
					float linHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
					ImGui::Separator();
					bool open = ImGui::TreeNodeEx((idName + std::to_string(index)).c_str(), treeNodeFlags, (tagNme + std::to_string(index)).c_str());
					ImGui::PopStyleVar();
					ImGui::SameLine(contenRegionAvablie.x - linHeigth * 0.5f);
					
					if (ImGui::Button("-", ImVec2{ linHeigth, linHeigth }))
					{
						removeTex = index;
						RY_CORE_ASSERT(spec.Attachments.Attachments[removeTex] == framTexSpec, "Not the same Texture!");
					}

					if(open)
					{

						//ImGui::Text("Texture: %i", index);
						if (ImGui::BeginCombo("TextureFormat: ", textureFormatChar[(int)framTexSpec.TextureFormat], ImGuiComboFlags_None))
						{
							for (uint32_t i = 1; i < textureFormatLength; i++)
							{
								if (ImGui::MenuItem(textureFormatChar[i]))
								{
									framTexSpec.TextureFormat = (FramebufferTextureFormat)i;
									ImGui::CloseCurrentPopup();
								}
							}
							ImGui::EndCombo();

						}
						auto& warping = framTexSpec.TextureWrapping;
						ImGui::Columns(3, "##Texture Atachments", true);
						float columWithe = windowSize.y / (texWarpingDimenLength + 1);

						for (int texDimeIndex = 1; texDimeIndex < texWarpingDimenLength; texDimeIndex++)
						{
							ImGui::SetColumnWidth(texDimeIndex, columWithe);
							if (ImGui::BeginCombo(texWarpingDimenChar[texDimeIndex], textureWarpingChar[(int)warping[texDimeIndex]], ImGuiComboFlags_None))
							{
								for (uint32_t i = 1; i < textureWarpingLength; i++)
								{
									if (ImGui::MenuItem(textureWarpingChar[i]))
									{
										warping[texDimeIndex] = TextureWrappingMode(i);
										ImGui::CloseCurrentPopup();
									}
								}
								ImGui::EndCombo();
							}
							ImGui::NextColumn();
						}

						ImGui::Columns(1, "##Texture-Filtering", true);

						if (ImGui::BeginCombo("Texture-Filtering: ", textureFilteringChar[(int)framTexSpec.TextureFiltering], ImGuiComboFlags_None))
						{
							for (uint32_t i = 1; i < textureFilteringLength; i++)
							{
								if (ImGui::MenuItem(textureFilteringChar[i]))
								{
									framTexSpec.TextureFiltering = (TextureFilteringMode)i;
									ImGui::CloseCurrentPopup();
								}
							}
							ImGui::EndCombo();
						}
						ImGui::Columns(1, "##Add Butten", true);
						ImGui::NewLine();
						ImGui::TreePop();
					}
					index++;
				}

				if (ImGui::Button("+"))
				{
					FramebufferTextureSpecification ftspec = FramebufferTextureSpecification();
					ftspec.TextureFiltering = TextureFilteringMode::Nearest;
					ftspec.TextureFormat = FramebufferTextureFormat::RGBA8;
					ftspec.TextureWrapping.S = TextureWrappingMode::ClampEdge;
					ftspec.TextureWrapping.T = TextureWrappingMode::ClampEdge;
					ftspec.TextureWrapping.R = TextureWrappingMode::ClampEdge;
					spec.Attachments.Attachments.push_back(ftspec);
				}
				
				if (removeTex != -1)
				{
					spec.Attachments.Attachments.erase(spec.Attachments.Attachments.begin() + removeTex);
				}
			}
			

			ImGui::PushID("Finish Window");
			if (ImGui::Button("ok"))
			{
				
				m_OppenWindow = SettingsPopUpWindow::None;
				std::string fileExtension = ".ryframe";
				std::string fileName = m_CreateFrambuffer.Buffer + fileExtension;
				m_CreateFrambuffer.Path = path / fileName;
				Ref<Framebuffer> frambuffer = Framebuffer::Create(spec);

				AssetMetadata metadata;
				metadata.Type = frambuffer->GetType();
				metadata.FilePath = m_CreateFrambuffer.Path;
				int i = 0;
				while (m_AssetManger->IsAssetHandleValid(metadata.FilePath))
				{
					fileName = m_CreateFrambuffer.Buffer + std::to_string(i) + fileExtension;
					m_CreateFrambuffer.Path = path / fileName;
					metadata.FilePath = m_CreateFrambuffer.Path;
					i++;
				}
				m_AssetManger->CreateAsset(metadata.FilePath, (Ref<Asset>)frambuffer, metadata);
				
				if(FrambufferSerializer::Serilze(metadata.FilePath, frambuffer))
					ImGui::CloseCurrentPopup();
				
			}
			ImGui::SameLine();
			if (ImGui::Button("chancle"))
			{
				ImGui::CloseCurrentPopup();
				m_OppenWindow = SettingsPopUpWindow::None;
			}
			ImGui::PopID();
			ImGui::EndPopup();

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
#if 0
	static FileFormats GetFileFormate(const std::filesystem::path& filePath)
	{
		std::filesystem::path extension = filePath.filename().extension();

		if (extension == ".png") return FileFormats::png;
		if (extension == ".txt") return FileFormats::txt;
		if (extension == ".glsl") return FileFormats::glsl;
		if (extension == ".rynexscene") return FileFormats::rynexscene;

		return FileFormats::none;
	}
#endif

	

#if 0
	static Ref<Texture> GetFileIconeTexture(FileFormats format)
	{
		switch (format)
		{
		case FileFormats::png:		return;
		case FileFormats::txt:		return;
		}


		return Ref<Texture>();
	}
#endif // TODO: Make Separte Funktion for Textur

	void ContentBrowserPannel::SetAssetRegestriy(const std::filesystem::path& curentPath, Ref<EditorAssetManager>& assetManger)
	{
		//m_AssetManger->CreateDirektoryRegestriy(curentPath);
		for (auto& p : std::filesystem::directory_iterator(curentPath))
		{
			const auto& path = p.path();
			if (p.is_directory())
			{
				assetManger->AddDirectoryToParent(path);
				SetAssetRegestriy(path, assetManger);
			}
			else
			{
				// assetDirectory.Files.push_back(m_AssetManger->AddFileToRegistry(path));
				if(GetAssetTypeFromFilePath(path) != AssetType::None)
				{
					assetManger->CreateAsset(path);
				}
				
			}
		}
		
	}
#if 0
	void ContentBrowserPannel::FileFormate(const std::filesystem::directory_entry& directoryEntry, FileFormats formate) const
	{
		FileStats state = FileStats::None;
		static float padding = 16.0f;
		static float thumbernailSize = 80.0f;
		Ref<Texture> icon;
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
#endif 
	void ContentBrowserPannel::NewMenue()
	{
		if (ImGui::BeginMenu("Asset")) NewAsset();
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
		 if (ImGui::BeginMenu("Asset"))		NewAsset();
		 if (ImGui::MenuItem("File"));
		 ImGui::EndMenu();
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

	void ContentBrowserPannel::DelateListeFolder(std::filesystem::path& folderPath)
	{
		m_DealeteFolderList.push_back(folderPath);
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
		

					
					assetManger->ReLoadeAsset(filePath);
					RY_CORE_INFO("Thread ReLoadeAsset! {0}", filePath.string().c_str());
						//Renderer2D::Shutdown();
						//Renderer::Init();
						
					
					if(assetManger->IsAssetLoaded(filePath))
					{
					
						RY_CORE_WARN("Thread Not ReLoadeAsset! Finished {0}", filePath.string().c_str());
						
					}

				});
				break;
			}
			case filewatch::Event::added:
			{
				RY_CORE_INFO("Thread Added! Begine! {0}", filePath.string().c_str());
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
					Ref<EditorAssetManager> assetManger = Project::GetActive()->GetEditorAssetManger();
					std::filesystem::path filepath = std::filesystem::path(filePath);
					
					ContentBrowserPannel::SetAssetRegestriy(filepath.parent_path().generic_string(), assetManger);
				});
				break;

			}
			case filewatch::Event::removed:
			{
				RY_CORE_INFO("Thread Removed! Begine! {0}", filePath.string().c_str());
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
				});
				break;
			}
			case filewatch::Event::renamed_new:
			{
				RY_CORE_INFO("Thread Renamed New! {0}", filePath.string().c_str());
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
					Ref<EditorAssetManager> assetManger = Project::GetActive()->GetEditorAssetManger();
					std::filesystem::path filepath = std::filesystem::path(filePath);

					ContentBrowserPannel::SetAssetRegestriy(filepath.parent_path().generic_string(), assetManger);
				});
				break;
			}
			case filewatch::Event::renamed_old:
			{
				RY_CORE_INFO("Thread Renamed Old! Begine! {0}", filePath.string().c_str());
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
				});
				break;
			}
			default:
				RY_CORE_WARN("Thread Not found Event! {0}", filePath.string().c_str());
				break;
		}
		RY_CORE_INFO("Thread Path! -> {0} Now Fished", filePath.string().c_str());
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


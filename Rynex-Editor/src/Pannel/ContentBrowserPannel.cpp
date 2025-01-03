#include <rypch.h>
#include "ContentBrowserPannel.h"

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
#include <Rynex/Utils/EnumString.h>


#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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
		 , m_BaseDirectory("")
		 , m_CurrentDirectory(m_BaseDirectory)
	{
	}
	
	void ContentBrowserPannel::OnAtache()
	{
#if RY_PATH_IN_LINE
		RY_CORE_INFO("ContentBrowserPannel::OnAtache Start!");
		m_DirectoryIcon		= TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/DirectoryIcon.png", false);
		
		//Files
		Ref<Texture> defaultIcon = TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/FileIconDefault.png", false);
		AssetManager::CreatLocaleAsset<Texture>(defaultIcon);
		m_FileIconDefault	= AssetManager::GetAsset<Texture>(defaultIcon->Handle);
		m_FileIconError		= TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/FileIconError.png", false);
		m_FileIconScene		= TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/FileIconScene.png", false);
		m_FileIconShader	= TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/FileIconShader.png", false);
		m_FileIconTexture	= TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ContentBrowser/FileIconTexture.png", false);
		m_AssetManger		= m_Project->GetEditorAssetManger();
#endif
		m_AssetManger->SerialzeAssetRegistry();
		RY_CORE_INFO("Fished SerialzeAssetRegistry!");
		m_BaseDirectory = Project::GetActiveAssetDirectory();
		m_CurrentDirectory = m_BaseDirectory;
#if RY_EDITOR_ASSETMANGER_THREADE ? 0 : 1 
		SetAssetRegestriy(m_BaseDirectory, m_AssetManger);
#endif
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

			float pannelWidth = ImGui::GetContentRegionAvail().x;
			int columnCount = (int)(pannelWidth / cellSize);
			if (columnCount < 1)
				columnCount = 1;

			ImGui::Columns(columnCount, 0, false);
#if 0
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


				std::filesystem::path realtivPath = std::filesystem::relative(path, m_BaseDirectory);
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
#else

			if (m_AssetManger->IsCurentAssetState(m_CurrentDirectory))
			{
				m_FileItemes = m_AssetManger->GetCurentAssetInformation(m_CurrentDirectory);
				RY_CORE_WARN("New Asset Items Ordert");
			}

			for (auto& [isAsset, isFolder, handle, metadata, type, typeString, texture, state, name, path, relativPath, pathString] : m_FileItemes)
			{
				if (isAsset)
				{

					if(relativPath == "")
						relativPath = std::filesystem::relative(path, m_BaseDirectory);
					

					ImGui::PushID(name.c_str());

					if(!texture)
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

					
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::ImageButton(
						(ImTextureID)texture->GetRenderID(),
						{ thumbernailSize , thumbernailSize },
						{ 0, 1 },
						{ 1, 0 },
						-1,
						ImVec4(0.15f, 0.85f, 0.2f, 0.1f),
						GetAssetStateColor(state));

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
						const AssetHandle* handle = &m_AssetManger->GetAssetHandle(path);
						ImGui::SetDragDropPayload(typeString.c_str(), handle, sizeof(AssetHandle));
						ImGui::EndDragDropSource();
					}

					//RY_CORE_INFO("after drag drop ContentBrowserPannel");
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
				else if (isFolder)
				{
					ImGui::PushID(name.c_str());
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					if (path != m_BaseDirectory / "Loadead (NotAssetFiles)" && path != m_BaseDirectory / "Unknown File Types")
						ImGui::ImageButton((ImTextureID)m_DirectoryIcon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.15f, 0.75f, 0.2f, 0.15f), ImVec4(1, 1, 1, 1));
					else
						ImGui::ImageButton((ImTextureID)m_DirectoryIcon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 }, -1, ImVec4(0.875f, 0.875f, 0.35f, 0.5f), ImVec4(1, 1, 1, 1));

					ImGui::PopStyleColor();
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						m_CurrentDirectory /= name;
					}

					if (ImGui::BeginPopupContextItem(name.c_str()))
					{
						if (ImGui::MenuItem("Delete Folder"))
						{
							RY_CORE_WARN("Delete Folder {} form path {0}", name.c_str(), pathString.c_str());
						}

						ImGui::EndPopup();
					}
					ImGui::TextWrapped(name.c_str());
					ImGui::NextColumn();
					ImGui::PopID();
				}
				else
				{
					RY_CORE_ASSERT(false);
				}
			}

#endif
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

#if RY_EDITOR_ASSETMANGER_THREADE
			
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
#else
			const auto& assetRegestriey = m_Project->GetEditorAssetManger()->GetHandleRegistry();

			for (const auto& [handle, metadata] : assetRegestriey)
			{
				ImGui::Text("AssetHandle(UUID): (%i), Realtiv FilePath: %s", handle, metadata.FilePath.string().c_str());
			}

#endif
			ImGui::End();
		}
	}

#if 0
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
						if (ImGui::BeginCombo("TextureFormat: ", EnumString::GetStringFromEnum<TextureFormat>(framTexSpec.TextureFormat).data(), ImGuiComboFlags_None))
						{
							for (uint32_t i = 1; i < 12; i++)
							{
								TextureFormat format = (TextureFormat)i;
								if (ImGui::MenuItem(EnumString::GetStringFromEnum<TextureFormat>(format).data()))
								{
									framTexSpec.TextureFormat = format;
									ImGui::CloseCurrentPopup();
								}
							}
							ImGui::EndCombo();

						}

						uint32_t textureWarpingLength = 6;
						char* texWarpingDimenChar[] = { "R","S","T" };

						TextureWrappingSpecification& warping = framTexSpec.TextureWrapping;
						ImGui::Columns(3, "##Texture Atachments", true);
						float columWithe = windowSize.y / (3 + 1);

						for (int texDimeIndex = 1; texDimeIndex < 3; texDimeIndex++)
						{
							ImGui::SetColumnWidth(texDimeIndex, columWithe);
							if (ImGui::BeginCombo(texWarpingDimenChar[texDimeIndex], EnumString::GetStringFromEnum<TextureWrappingMode>(warping[texDimeIndex]).data(), ImGuiComboFlags_None))
							{
								for (uint32_t i = 1; i < textureWarpingLength; i++)
								{
									TextureWrappingMode texWarpMode = (TextureWrappingMode)i;
									if (ImGui::MenuItem(EnumString::GetStringFromEnum<TextureWrappingMode>(texWarpMode).data()))
									{
										warping[texDimeIndex] = texWarpMode;
										ImGui::CloseCurrentPopup();
									}
								}
								ImGui::EndCombo();
							}
							ImGui::NextColumn();
						}

						ImGui::Columns(1, "##Texture-Filtering", true);

						if (ImGui::BeginCombo("Texture-Filtering: ", EnumString::GetStringFromEnum<TextureFilteringMode>(framTexSpec.TextureFiltering).data(), ImGuiComboFlags_None))
						{
							for (uint32_t i = 1; i < 2; i++)
							{
								TextureFilteringMode texFilteringMode = TextureFilteringMode(i);
								if (ImGui::MenuItem(EnumString::GetStringFromEnum<TextureFilteringMode>(texFilteringMode).data()))
								{
									framTexSpec.TextureFiltering = texFilteringMode;
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
					ftspec.TextureFormat = TextureFormat::RGBA8;
					ftspec.TextureWrapping.S = TextureWrappingMode::ClampEdge;
					ftspec.TextureWrapping.T = TextureWrappingMode::ClampEdge;
					ftspec.TextureWrapping.R = TextureWrappingMode::ClampEdge;
					spec.Attachments.push_back(ftspec);
				}
				
				if (removeTex != -1)
				{
					spec.Attachments.erase(spec.Attachments.begin() + removeTex);
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
#if RY_EDITOR_ASSETMANGER_THREADE
#else
				m_AssetManger->CreateAsset(metadata.FilePath, (Ref<Asset>)frambuffer, metadata);
#endif
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
#endif

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

#if RY_EDITOR_ASSETMANGER_THREADE ? 0 : 1

	void ContentBrowserPannel::SetAssetRegestriy(const std::filesystem::path& curentPath, Ref<EditorAssetManager>& assetManger)
	{
		//m_AssetManger->CreateDirektoryRegestriy(curentPath);
		if (curentPath.empty())
			return;
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

#endif


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
#if RY_EDITOR_ASSETMANGER_THREADE
#else
			m_AssetManger->GetAssetFileDirectory(assetDealte.AssetFileParentPath);
#endif
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

	// TODO: ADD File Watcher
	static void OnFileSystemEvent(std::string filepath, const filewatch::Event change_type)
	{
		std::filesystem::path filePath = (Project::GetActiveAssetDirectory() / std::filesystem::path(filepath)).generic_string();
		// using namespace std::chrono_literals;
		// std::this_thread::sleep_for(10ms);

		RY_CORE_TRACE("FileWatcher Event Info : {}!", filePath.string().c_str());
		switch (change_type)
		{
			case filewatch::Event::modified:
			{

#if RY_EDITOR_ASSETMANGER_THREADE
				Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
				assetManger->EventAsyncModified(filePath);
#else
				RY_CORE_INFO("Thread triger Modified! Submit! {0}", filePath.string().c_str());
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
					RY_CORE_INFO("Thread Execut Call -> ReLoadeAsset");
					Ref<EditorAssetManager> assetManger = Project::GetActive()->GetEditorAssetManger();
					
					assetManger->ReLoadeAsset(filePath);
					RY_CORE_INFO("Thread ReLoadeAsset! {0}", filePath.string().c_str());

					if(assetManger->IsAssetLoaded(filePath))
					{
						RY_CORE_WARN("Thread Not ReLoadeAsset! Finished {0}", filePath.string().c_str());
					}
				});
#endif
				break;
			}
			case filewatch::Event::added:
			{
#if RY_EDITOR_ASSETMANGER_THREADE
				Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
				assetManger->EventAsyncAdded(filePath);
#else
				RY_CORE_INFO("Thread triger Added! Submit! {0}", filePath.string().c_str());
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
					RY_CORE_INFO("Thread Assets! Exexut! {0}", filePath.string().c_str());
					Ref<EditorAssetManager> assetManger = Project::GetActive()->GetEditorAssetManger();
					std::filesystem::path filepath = std::filesystem::path(filePath);
					
					ContentBrowserPannel::SetAssetRegestriy(filepath.parent_path().generic_string(), assetManger);
				});
#endif
				break;

			}
			case filewatch::Event::removed:
			{

#if RY_EDITOR_ASSETMANGER_THREADE
				Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
				assetManger->EventAsyncRemoved(filePath);
#else
				RY_CORE_INFO("Thread Removed! Submit! {0}", filePath.string().c_str());
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
					RY_CORE_INFO("Thread Assets! Exexut! {0}", filePath.string().c_str());
					RY_CORE_WARN("Thread is nothing doing! TODO!");
				});
#endif
				break;
			}
			case filewatch::Event::renamed_new:
			{
#if RY_EDITOR_ASSETMANGER_THREADE
				Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
				assetManger->EventAsyncRenamedNew(filePath);
#else				
				RY_CORE_INFO("Thread Renamed New! {0}", filePath.string().c_str());
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
					RY_CORE_INFO("Thread Assets! Exexut! {0}", filePath.string().c_str());
					Ref<EditorAssetManager> assetManger = Project::GetActive()->GetEditorAssetManger();
					std::filesystem::path filepath = std::filesystem::path(filePath);

					ContentBrowserPannel::SetAssetRegestriy(filepath.parent_path().generic_string(), assetManger);
				});
#endif
				break;
			}
			case filewatch::Event::renamed_old:
			{
#if RY_EDITOR_ASSETMANGER_THREADE
				Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
				assetManger->EventAsyncRenamedOld(filePath);
#else
				RY_CORE_INFO("Thread Renamed Old! Begine! {0}", filePath.string().c_str());
				Application::Get().SubmiteToMainThreedQueueAssetFileWatcher([filePath]()
				{
						RY_CORE_INFO("Thread Renamed Old! Execut {0}", filePath.string().c_str());
						RY_CORE_WARN("Thread is nothing doing! TODO!");

					RY_CORE_INFO("Thread Assets! Exexut! {0}", filePath.string().c_str());
					RY_CORE_WARN("Thread is nothing doing! TODO!");

				});
#endif
				break;
			}
			default:
				RY_CORE_FATAL("Thread Not found Event! {0}", filePath.string().c_str());
				break;
		}
		
	};

	void ContentBrowserPannel::InitAssetFileWatcher()
	{
		s_Data = new AssetMangerFileWatcherData();
		{
			AssetFileWatcher();
		}
		
	}

	void ContentBrowserPannel::OnloadeAssetsList()
	{
		for (AssetHandle assetunLoade : m_OnLoadeAsset)
		{
#if RY_EDITOR_ASSETMANGER_THREADE
			Ref<EditorAssetManegerThreade> assetManger = Project::GetActive()->GetEditorAssetManger();
			assetManger->UnLoadeFileAsset(assetunLoade);
#else
			m_AssetManger->OnLoadeAsset(assetonLoade);
#endif
		}
		m_OnLoadeAsset.clear();
	}

	void ContentBrowserPannel::OnLoadeAsset(AssetHandle handle)
	{
		m_OnLoadeAsset.emplace_back(handle);
		
	}

	void ContentBrowserPannel::AssetFileWatcher()
	{
		s_Data->AssetFileWatcher = CreateScope<filewatch::FileWatch<std::string>>(Project::GetActiveAssetDirectory().string(), OnFileSystemEvent);
		RY_CORE_INFO("New Thread Raedy!");
	}

#endif
}


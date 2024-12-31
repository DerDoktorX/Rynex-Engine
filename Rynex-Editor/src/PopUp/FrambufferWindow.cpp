#include <rypch.h>
#include "FrambufferWindow.h"

#include <Rynex/Project/Project.h>
#include <Rynex/Serializers/FrambufferSerializer.h>

#include <imgui/imgui.cpp>

namespace Rynex {

	FrambufferWindow::FrambufferWindow()
	{
	}

	FrambufferWindow::~FrambufferWindow()
	{
	}

	void FrambufferWindow::OnAtache()
	{
		m_AssetManger = Project::GetActive()->GetEditorAssetManger();
	}

	void FrambufferWindow::OnDettache()
	{
	}

	void FrambufferWindow::OnUpdate()
	{
	}

	void FrambufferWindow::OnOpen(const std::filesystem::path& path)
	{
		m_Open = true;
		m_Path = path;
	}

	void FrambufferWindow::OnClose()
	{
		m_Open = false;
	}

	void FrambufferWindow::OnImGuiRender()
	{
		if (!m_Open) 
			return;

		if (ImGui::BeginPopupModal("Settings-FrameBuffer", &m_Open, ImGuiWindowFlags_MenuBar))
		{
			ImVec2 windowSize = ImGui::GetWindowSize();
			{
				ImGui::PushID("FrameBuffer Path");
				ImGui::Columns(1, "##FrameBuffer Path", true);
				ImGui::Text("Path: %s", m_Path.string().c_str());
				ImGui::SameLine();
				ImGui::InputText("##FileName", m_Name, 64);
				ImGui::SameLine();
				ImGui::Text(".ryframe");
				ImGui::NextColumn();
				ImGui::Columns(1);
				ImGui::PopID();
			}

			{
				ImGui::PushID("FrameBuffer Size");
				{
					ImGui::NewLine();
					ImGui::Columns(2, "##FrameBuffer Size", true);
					float columWithe = windowSize.y / 3.25;
					ImGui::SetColumnWidth(0, columWithe);
					ImGui::SetColumnWidth(1, columWithe);
					ImGui::InputInt("Width", (int*)&m_FBspec.Width);


					ImGui::NextColumn();
					ImGui::InputInt("Height", (int*)&m_FBspec.Height);
					if ((int)m_FBspec.Height < 1)
						m_FBspec.Height = 1;
					if ((int)m_FBspec.Width < 1)
						m_FBspec.Width = 1;
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
					"Depth24Stencil8",
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
				for (FramebufferTextureSpecification& framTexSpec : m_FBspec.Attachments.Attachments)
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
						RY_CORE_ASSERT(m_FBspec.Attachments.Attachments[removeTex] == framTexSpec, "Not the same Texture!");
					}

					if (open)
					{

						//ImGui::Text("Texture: %i", index);
						if (ImGui::BeginCombo("TextureFormat: ", textureFormatChar[(int)framTexSpec.TextureFormat], ImGuiComboFlags_None))
						{
							for (uint32_t i = 1; i < textureFormatLength; i++)
							{
								if (ImGui::MenuItem(textureFormatChar[i]))
								{
									framTexSpec.TextureFormat = (TextureFormat)i;
									ImGui::CloseCurrentPopup();
								}
							}
							ImGui::EndCombo();

						}
						TextureWrappingSpecification& warping = framTexSpec.TextureWrapping;
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
					ftspec.TextureFormat = TextureFormat::RGBA8;
					ftspec.TextureWrapping.S = TextureWrappingMode::ClampEdge;
					ftspec.TextureWrapping.T = TextureWrappingMode::ClampEdge;
					ftspec.TextureWrapping.R = TextureWrappingMode::ClampEdge;
					m_FBspec.Attachments.Attachments.push_back(ftspec);
				}

				if (removeTex != -1)
				{
					m_FBspec.Attachments.Attachments.erase(m_FBspec.Attachments.Attachments.begin() + removeTex);
				}
			}


			ImGui::PushID("Finish Window");
			if (ImGui::Button("ok"))
			{

				
				std::string fileExtension = ".ryframe";
				std::string fileName = m_Name + fileExtension;
				m_Path = m_Path / fileName;
				Ref<Framebuffer> frambuffer = Framebuffer::Create(m_FBspec);

				AssetMetadata metadata;
				metadata.Type = frambuffer->GetType();
				metadata.FilePath = m_Path;
				int i = 0;
				while (m_AssetManger->IsAssetHandleValid(metadata.FilePath))
				{
					fileName = m_Name + std::to_string(i) + fileExtension;
					m_Path = m_Path / fileName;
					metadata.FilePath = m_Path;
					i++;
				}
#if RY_EDITOR_ASSETMANGER_THREADE
#else
				m_AssetManger->CreateAsset(metadata.FilePath, (Ref<Asset>)frambuffer, metadata);
#endif
				if (FrambufferSerializer::Serilze(metadata.FilePath, frambuffer))
					ImGui::CloseCurrentPopup();
				OnClose();

			}
			ImGui::SameLine();
			if (ImGui::Button("chancle"))
			{
				ImGui::CloseCurrentPopup();
				OnClose();
			}
			ImGui::PopID();
			ImGui::EndPopup();

		}
	}

}
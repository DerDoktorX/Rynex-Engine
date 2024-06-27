#include "SceneHierachyPannel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "Rynex/Scene/Components.h"
#include <cstring>

#include <filesystem>

#define ImGuiDemoWindow 0

namespace Rynex {

	extern const std::filesystem::path g_AssetsPath;

	SceneHierachyPannel::SceneHierachyPannel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierachyPannel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierachyPannel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opende = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entiytDeleted = false;
		if (ImGui::BeginPopupContextItem(tag.c_str()))
		{
			if (ImGui::MenuItem("Delete Entity"))
				entiytDeleted = true;


			ImGui::EndPopup();
		}

		if (opende)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			opende = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opende)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entiytDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static void DrawVec3Controler(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columeWith = 80.0f)
	{

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columeWith);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{0.0f , 0.0f});

		float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 butenSize = { lineHeigth + 3.0f, lineHeigth };



		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", butenSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f" );
		ImGui::PopItemWidth();
		ImGui::SameLine();
		

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", butenSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", butenSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}


	template<typename T, typename UIFuction>
	static void DrawComponent(const std::string& name, Entity entity, UIFuction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap;
		if (entity.HasComponent<T>())
		{
			
			auto& component = entity.GetComponent<T>();
			ImVec2 contenRegionAvablie = ImGui::GetContentRegionAvail();
			
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float linHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(

			);
			ImGui::SameLine(contenRegionAvablie.x - linHeigth * 0.5f);

			if (ImGui::Button("+", ImVec2{ linHeigth, linHeigth }))
				ImGui::OpenPopup("ComponentSettings");

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				
				//ImGui::ColorEdit4("Color", glm::value_ptr(color), 0.1f);
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierachyPannel::DrawComponents(Entity entity)
	{

		// TagComponent
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-0.5f);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				auto& tag = entity.GetComponent<TagComponent>().Tag;
				if (tag == "Empty Entity")
				{
					tag = "Camera Entity";
				}
				auto& camer = m_SelectionContext.AddComponent<CameraComponent>().Camera;
				camer.SetPerspectiv(glm::degrees(45.0f), 0.1f, 1000);
				camer.SetOrthoGrafic(10, -1.0f, 1.0f);
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer"))
			{
				auto& tag = entity.GetComponent<TagComponent>().Tag;
				if (tag == "Empty Entity")
				{
					tag = "Sprite Entity";
				}
				m_SelectionContext.AddComponent<SpriteRendererComponent>();
				ImGui::CloseCurrentPopup();
			}



			ImGui::EndPopup();
		}

		// Transform
		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3Controler("Translation", component.Transaltion);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Controler("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DrawVec3Controler("Scale", component.Scale, 1.0f);
		});

		// Camera
		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			auto& camera = component.Camera;
			const char* projectioneTypeStrings[] = { "Perspective", "Othographic" };
			const char* currentProjectioneTypeStrings = projectioneTypeStrings[(int)camera.GetProjectionType()];

			if (ImGui::BeginCombo("Projektion", currentProjectioneTypeStrings))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelelcted = currentProjectioneTypeStrings == projectioneTypeStrings[i];
						if (ImGui::Selectable(projectioneTypeStrings[i], isSelelcted))
						{
							currentProjectioneTypeStrings = projectioneTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelelcted)
						{
							ImGui::SetItemDefaultFocus();
						}

					}

					ImGui::EndCombo();
				}




			switch (camera.GetProjectionType())
				{
					case SceneCamera::ProjectionType::Orthographic:
					{
						float sizeOrth = camera.GetOrthographicSize();
						if (ImGui::DragFloat("Size", &sizeOrth))
							camera.SetOrthograficSize(sizeOrth);

						float nearOrth = camera.GetOrthographicNearClipe();
						if (ImGui::DragFloat("Near", &nearOrth))
							camera.SetOrthograficNearClipe(nearOrth);

						float farOrth = camera.GetOrthographicFarClipe();
						if (ImGui::DragFloat("Far", &farOrth))
							camera.SetOrthograficFarClipe(farOrth);
						ImGui::Checkbox("Fixxed Aspect Rotation", &component.FixedAspectRotaion);
						break;
					}
					case SceneCamera::ProjectionType::Perspectiv:
					{
						float sizePers = glm::degrees(camera.GetPerspectivVerticleFOV());
						if (ImGui::DragFloat("FOV", &sizePers))
							camera.SetPerspectivVerticleFOV(glm::radians(sizePers));

						float nearPers = camera.GetPerspectivNearClipe();
						if (ImGui::DragFloat("Near", &nearPers))
							camera.SetPerspectivNearClipe(nearPers);

						float farPers = camera.GetPerspectivFarClipe();
						if (ImGui::DragFloat("Far", &farPers))
							camera.SetPerspectivFarClipe(farPers);
						break;
					}
				};
		});

		// SpriteRenderer
		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color), 0.1f);
			// Texture
			ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texPath = (g_AssetsPath) / path;
					component.Texture = Texture2D::Create(texPath.string());
				}
				ImGui::EndDragDropTarget();
			}
			if (component.Texture != nullptr)
			{
				ImGui::SameLine();
				if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
					component.Texture = nullptr;
			}


			ImGui::Checkbox("Render Single", &component.RenderSingle);
#if 0
			ImGui::DragFloat("TalingFloat", &component.Tali..., 0.0f, 0.0f, 100.0f);
#endif		// TODO: Tailing Factor

		});

	}


	void SceneHierachyPannel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierachy");
		
		//loop thorw all Entity
		m_Context->m_Registery.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			});


		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_Context->CreateEntity("Empty Entiy");
			}

			ImGui::EndPopup();
		}


		ImGui::End();

		ImGui::Begin("Properties");

		DrawProperties();


#if ImGuiDemoWindow
		ImGui::ShowDemoWindow();
#endif
		ImGui::End();
	}

	void SceneHierachyPannel::DrawProperties()
	{
		if (m_SelectionContext)
			DrawComponents(m_SelectionContext);
	}

}
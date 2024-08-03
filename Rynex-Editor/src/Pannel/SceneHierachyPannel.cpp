#include "rypch.h"
#include "SceneHierachyPannel.h"
#include <Rynex/Scene/Components.h>
#include <Rynex/Scripting/ScriptingEngine.h>
#include <Rynex/Asset/Base/AssetManager.h>

#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>

namespace Rynex {

	extern const std::filesystem::path g_AssetsPath;

	SceneHierachyPannel::SceneHierachyPannel(const Ref<Scene>& context)
	{
		RY_PROFILE_FUNCTION();
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
		auto& id = entity.GetComponent<IDComponent>().ID;
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

#if RY_HRACHIE_STATE
		if(entity.GetState() == Entity::State::Error)
			ImGui::PushStyleColor(ImGuiCol_Header & ImGuiCol_TitleBg & ImGuiCol_TextDisabled & ImGuiCol_MenuBarBg, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));
#endif

		bool opende = ImGui::TreeNodeEx(&id, flags, tag.c_str());

#if RY_HRACHIE_STATE
		if (entity.GetState() == Entity::State::Error)
			ImGui::PopStyleColor();


		if (m_CheckErrors) 
		{
			CheckEnttiyForError(entity);
		}

#endif

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

#if RY_HRACHIE_STATE
			if (entity.GetState() == Entity::State::Error)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));
#endif

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			opende = ImGui::TreeNodeEx( &id, flags, tag.c_str());
			if (opende)
			{
				ImGui::TreePop();
			}
			ImGui::TreePop();

#if RY_HRACHIE_STATE
			if (entity.GetState() == Entity::State::Error)
				ImGui::PopStyleColor();
#endif

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
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<ScriptComponent>("Script");
			DisplayAddComponentEntry<CameraComponent>("Camera");
			if(!m_SelectionContext.HasComponent<MaterialComponent>() && !m_SelectionContext.HasComponent<GeomtryComponent>())
				DisplayAddComponentEntry<SpriteRendererComponent>("Sprite");

			if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
			{
				DisplayAddComponentEntry<MaterialComponent>("Material");
				DisplayAddComponentEntry<GeomtryComponent>("Geomtry");
			}
#if 0
			if (ImGui::MenuItem("Script"))
			{
				auto& tag = entity.GetComponent<TagComponent>().Tag;
				if (tag == "Empty Entity")
				{
					tag = "Script Entity";
				}
				m_SelectionContext.AddComponent<ScriptComponent>();
				ImGui::CloseCurrentPopup();
			}

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
#endif
			

			ImGui::EndPopup();
		}

		// Transform
		DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
		{
			DrawVec3Controler("Translation", component.Transaltion);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Controler("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DrawVec3Controler("Scale", component.Scale, 1.0f);
		});

		// Camera
		DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
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
		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color), 0.1f);
			// Texture
			ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
#if RY_OLD_DRAG_AND_DROP
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texPath = (g_AssetsPath) / path;
					component.Texture = Texture2D::Create(texPath.string());
				}
				ImGui::EndDragDropTarget();
#else
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM"))
				{
					component.Texture = AssetManager::GetAsset<Texture2D>(*(AssetHandle*)payload->Data);
				}
				ImGui::EndDragDropTarget();
#endif
			}
			if (component.Texture != nullptr)
			{
				ImGui::SameLine();
				if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
					component.Texture = nullptr;
			}


			ImGui::Checkbox("Render Single", &component.RenderSingle);
#if RY_TODO_TAILING_FACTOR
			ImGui::DragFloat("TalingFloat", &component.Tali..., 0.0f, 0.0f, 100.0f);
#endif		// TODO: Tailing Factor

		});

		//Material
		DrawComponent<MaterialComponent>("Material", entity, [](MaterialComponent& component)
			{

				ImGui::ColorEdit3("Color", glm::value_ptr(component.Color), 0.1f);

				// Texture
				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM"))
					{
						component.Texture = AssetManager::GetAsset<Texture2D>(*(AssetHandle*)payload->Data);
					}
					ImGui::EndDragDropTarget();
				}

				if (component.Texture != nullptr)
				{
					ImGui::SameLine();
					if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
						component.Texture = nullptr;
				}

				ImGui::Button("Shader", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_BROWSER_ITEM"))
					{
						component.Shader = AssetManager::GetAsset<Shader>(*(AssetHandle*)payload->Data);
					}
					ImGui::EndDragDropTarget();
				}

				if (component.Shader != nullptr)
				{
					ImGui::SameLine();
					if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
						component.Shader = nullptr;
				}

				

				

				

				//if(ImGui::Checkbox("Z-Buffer:", (int)component.Flags))
#if RY_TODO_MATERILA_FLAGS
				ImGui::Checkbox("Render Single", &component.);
#endif
			});

		//Geomtry
		DrawComponent<GeomtryComponent>("Geometry", entity, [](GeomtryComponent& component)
			{
				//const char* primtiv = component.Geometry->GetPrimitvChar();
				//ImGui::BeginCombo("Primtiv", primtiv);
				if (ImGui::Button("Geomtry", ImVec2(100.0f, 0.0f)))
				{

				}
				
			});

		// Script
		DrawComponent<ScriptComponent>("Script", entity, [](auto& component)
			{
				bool sricptClassExist = ScriptingEngine::ClassExists(component.Name);

				static char buffer[64];
				strcpy(buffer, component.Name.c_str());

				if (!sricptClassExist)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));
					
				}
#if 1
#if RY_OLD_TYPE_SCRIPT_NAME
				if (ImGui::InputText("Script: Class", buffer, sizeof(buffer)) )
				{
					//ImGui::ItemAdd("")
					
					component.Name = buffer;
					
					
				}
#endif

				if (ImGui::BeginCombo("Script: Class", buffer, ImGuiComboFlags_None))
				{
					uint32_t length = ScriptingEngine::GetClassLength();
					for (uint32_t i = 0; i < length; i++)
					{
						std::string& temp = ScriptingEngine::GetListClassName(i);
						if (ImGui::MenuItem(temp.c_str()))
						{
							if (temp.c_str() != "None")
								component.Name = temp;
							else
								component.Name = std::string();
							ImGui::CloseCurrentPopup();
						}
					}
					ImGui::EndCombo();
				}

				
				
				
				//ImGui::OpenPopup("ScriptClass");
#if RY_OLD_TYPE_SCRIPT_NAME
				if (ImGui::BeginPopup("ScriptClass"))
				{
					uint32_t length = ScriptingEngine::GetClassLength();
					for (uint32_t i = 0; i < length; i++)
					{
						std::string& temp = ScriptingEngine::GetListClassName(i);
						if (ImGui::MenuItem(temp.c_str()))
						{
							component.Name = temp;
							ImGui::CloseCurrentPopup();
						}
					}
					ImGui::EndPopup();
				}
#endif
				
#else
				
				

	

				if (ImGui::BeginCombo("Class", buffer))
				{
					uint32_t length = ScriptingEngine::GetClassLength();
					for (uint32_t i = 0; i < length; i++)
					{
						//bool is_selected = (component.selectedScript == i);
						if (ImGui::Selectable(ScriptingEngine::GetListClassName(i), false))
							component.selectedScript = i;

						//if (is_selected)
							//ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				
#endif 
				if (!sricptClassExist)
				{
					ImGui::PopStyleColor();
				}

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

#if CHECK_FOR_ERRORS
		if (m_CheckErrors)
		{
			m_CheckErrors = false;
			RY_INFO("Checked For Error on Enttiy!");
		}
#endif

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				
				CreateEntity("Empty Entity");
			}

			ImGui::EndPopup();
		}


		ImGui::End();

		ImGui::Begin("Properties");

		DrawProperties();

#if RY_IMGUI_DEMO_WINDOW
		ImGui::ShowDemoWindow();
#endif // RY_IMGUI_DEMO_WINDOW

		ImGui::End();
	}

	void SceneHierachyPannel::CreateEntity(const std::string& name, uint32_t entityNumber)
	{
		// TODO: Whrite a System how Make a counter up
		if (!m_Context->IsTagInScene(name))
		{
			if(entityNumber)
				m_Context->CreateEntity(name + std::to_string(entityNumber));
			else
				m_Context->CreateEntity("Empty Entiy");
		}
		else
		{
			CreateEntity(name + std::to_string(entityNumber), entityNumber++);
			
		}
	}

	void SceneHierachyPannel::DrawProperties()
	{
		if (m_SelectionContext)
			DrawComponents(m_SelectionContext);
	}

	void SceneHierachyPannel::CheckEnttiyForError(Entity& entity)
	{
		if (entity.HasComponent<ScriptComponent>())
		{
			auto component = entity.GetComponent<ScriptComponent>();
			if (!ScriptingEngine::ClassExists(component.Name))
				entity.SetState(Entity::State::Error);
			else
				entity.SetState(Entity::State::None);
		}
	}

	template<typename T>
	void SceneHierachyPannel::DisplayAddComponentEntry(const std::string& name)
	{
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(name.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	
}
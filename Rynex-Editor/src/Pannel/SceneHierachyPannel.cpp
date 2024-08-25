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


		bool opende = ImGui::TreeNodeEx(&id, flags, tag.c_str());


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
			opende = ImGui::TreeNodeEx( &id, flags, tag.c_str());
			if (opende)
			{
				ImGui::TreePop();
			}
			ImGui::TreePop();


		}
		
		if (entiytDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static void DrawVec4Controler(const std::string& label, glm::vec4& values, float resetValue = 0.0f, float columeWith = 80.0f)
	{

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columeWith);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f , 0.0f });

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
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
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

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.85f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.8f, 0.95f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.85f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("W", butenSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##W", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
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

	static void DrawVec2Controler(const std::string & label, glm::vec2& values, float resetValue = 0.0f, float columeWith = 80.0f)
	{

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columeWith);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f , 0.0f });

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
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
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

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	static void DrawFloatControler(const std::string& label, float* values, float resetValue = 0.0f, float columeWith = 80.0f)
	{

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columeWith);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());


		ImGui::SameLine();
		ImGui::DragFloat("##F", values, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

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

	// This Funktion Add A Butte and You Can Drag and Drop Items From your AssetManger in there, and Add A Dealte For Removing The Asset
	// The Secound Arge is the not Ref<...> only ... 
	// This Funktion Works only Withe UUID Assets
	template<typename T>
	static void DragDropButtenForAsset(Ref<T> *compontenItem, const char* buttenName, AssetType assetType)
	{
		ImGui::Button(buttenName, ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( GetAssetTypeDragAndDropName(assetType).c_str() ))
			{
				*compontenItem = AssetManager::GetAsset<T>(*(AssetHandle*)payload->Data);
			}
			ImGui::EndDragDropTarget();

		}
		if (*compontenItem != nullptr)
		{
			ImGui::SameLine();
			if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
				*compontenItem = nullptr;
		}
	}

	static void DargDropTextureButten(Ref<Texture2D>* compontenItem)
	{
		DragDropButtenForAsset<Texture2D>(compontenItem, "Texture2D", AssetType::Texture2D);
	}

	static void DargDropShaderButten(Ref<Shader>* compontenItem)
	{
		DragDropButtenForAsset<Shader>(compontenItem, "Shader", AssetType::Shader);
	}

	static void DargDropShaderButten(MaterialComponent& material)
	{
		ImGui::Button("Shader", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( GetAssetTypeDragAndDropName(AssetType::Shader).c_str() ))
			{
				material.Shader = AssetManager::GetAsset<Shader>(*(AssetHandle*)payload->Data);
				auto& layoute = material.Shader->GetUniformLayoute();
				material.UniformLayoute.clear();
				for (auto& [name, type] : layoute)
				{
					UniformElement ellement;
					ellement.Name = name;
					ellement.Type = GetShaderDataTypeFromString(type);
					material.UniformLayoute.push_back(ellement);
				}
			}
			ImGui::EndDragDropTarget();

		}
		if (material.Shader != nullptr)
		{
			ImGui::SameLine();
			if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
			{
				material.Shader = nullptr;
				material.UniformLayoute.clear();
			}
			
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
			DisplayAddComponentEntry<FrameBufferComponent>("FrameBuffer");

			DisplayAddComponentEntry<SceneComponent>("Scene");
	

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
			DargDropTextureButten(&component.Texture);
			ImGui::Checkbox("Render Single", &component.RenderSingle);
		});

		//Material
		DrawComponent<MaterialComponent>("Material", entity, [](MaterialComponent& component)
			{

				ImGui::ColorEdit3("Color", glm::value_ptr(component.Color), 0.1f);

				// Texture

				DargDropTextureButten(&component.Texture);


				DargDropShaderButten(component);

				if (component.Shader != nullptr)
				{
					if (component.UniformLayoute.size() == 0)
					{
						component.UniformLayoute.clear();
						auto& layoute = component.Shader->GetUniformLayoute();
						for (auto& [name, type] : layoute)
						{
							UniformElement ellement;
							ellement.Name = name;
							ellement.Type = GetShaderDataTypeFromString(type);

							if (name.find("Model") != name.size() && ellement.Type == ShaderDataType::Float4x4)
							{
								ellement.ShaderResourceType = ShaderResourceType::LocalModel;
							}
							else if (name.find("ViewProj") != name.size() && ellement.Type == ShaderDataType::Float4x4)
							{
								ellement.ShaderResourceType = ShaderResourceType::MainCameraViewProjectionMatrix;
							}
							else if(name.find("Color") != name.size() && ellement.Type == ShaderDataType::Float3)
							{
								ellement.ShaderResourceType = ShaderResourceType::LocalColor;
							}

							component.UniformLayoute.push_back(ellement);
						}
					}

					for(auto& element :  component.UniformLayoute)
					{
						
						ImGuiIO& io = ImGui::GetIO();
						auto boldFont = io.Fonts->Fonts[0];

						ImGui::PushID(element.Name.c_str());
						ImGui::Columns(2, "##Material Uniform",false);
						
						ImGui::SetColumnWidth(0, 85);
						
						ImGui::Text(element.Name.c_str());
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(GetStringFromShaderData(element.Type).c_str());

						ImGui::NextColumn();
						ImGui::PushMultiItemsWidths( 1, ImGui::CalcItemWidth());

						
						ImGui::Checkbox("##Globle Resurce", &element.GloblelResurce);
						
						if(element.GloblelResurce)
						{
							ImGui::SameLine();
							char* otherResources[] = {
								"Nono",
								"Local Model",
								"Local Color",
								"Main Camer Pos",
								"Main Camera View",
								"Main Camer Projection",
								"Main Camera ViewProjection",
								"Globle Resource"
							};
						
							if (ImGui::BeginCombo("##", otherResources[(uint8_t)element.ShaderResourceType], ImGuiComboFlags_None))
							{
								
								for (uint8_t i = 0; i < 8; i++)
								{

									if (ImGui::MenuItem(otherResources[i]))
									{
										element.ShaderResourceType = (ShaderResourceType)i;
										ImGui::CloseCurrentPopup();
									}
								}
								ImGui::EndCombo();
							}
						}
						else
						{
							if (element.LocalResurce == nullptr)
							{
								element.LocalResurce = new void*[ShaderDataTypeSize(element.Type)];
								switch (element.Type)
								{
									case ShaderDataType::Float:
									{
										float* value = static_cast<float*>(element.LocalResurce);
										ImGui::DragFloat("##Float", value);
										break;
									}
									case ShaderDataType::Float2:
									{

										ImGui::DragFloat2("##Float2", glm::value_ptr(*(glm::vec<2, float>*)element.LocalResurce));
										break;
									}
									case ShaderDataType::Float3:
									{
										glm::vec<3, float>* value = static_cast<glm::vec<3, float>*>(element.LocalResurce);
										*value = { 0.0f, 0.0f,0.0f };
										break;
									}
									case ShaderDataType::Float4:
									{
										glm::vec<4, float>* value = static_cast<glm::vec<4, float>*>(element.LocalResurce);
										*value = { 0.0f, 0.0f,0.0f, 0.0f };
										break;
									}
									case ShaderDataType::Float3x3:
									{
										glm::mat3* value = static_cast<glm::mat3*>(element.LocalResurce);
										*value = { 
											1.0f, 0.0f, 0.0f, 
											0.0f, 1.0f, 0.0f,
											0.0f, 0.0f, 1.0f
										};
										break;
									}
									case ShaderDataType::Float4x4:
									{
										glm::mat4* value = static_cast<glm::mat4*>(element.LocalResurce);
										*value = {
											1.0f, 0.0f, 0.0f, 0.0f,
											0.0f, 1.0f, 0.0f, 0.0f,
											0.0f, 0.0f, 1.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 1.0f
										};
										break;
									}
									case ShaderDataType::Int:
									{
										break;
									}
									case ShaderDataType::Uint:
									{
										break;
									}
									default:
										break;
								}
							}
							switch (element.Type)
							{
								case ShaderDataType::Float:
								{
									ImGui::SameLine();
									float* value = static_cast<float*>(element.LocalResurce);
									ImGui::DragFloat("##Float", value, 0.1f, 0.0f, 0.0f, "%.1f");
									break;
								}
								case ShaderDataType::Float2:
								{
									ImGui::SameLine();
									float* value = static_cast<float*>(element.LocalResurce);
									ImGui::DragFloat2("##Float2", value, 0.1f, 0.0f, 0.0f, "%.1f");
									break;
								}
								case ShaderDataType::Float3:
								{
									ImGui::SameLine();
									float* value = static_cast<float*>(element.LocalResurce);
									ImGui::DragFloat3("##Float3", value, 0.1f, 0.0f, 0.0f, "%.1f");
									break;
								}
								case ShaderDataType::Float4:
								{
									ImGui::SameLine();
									float* value = static_cast<float*>(element.LocalResurce);
									ImGui::DragFloat4("##Float4", value, 0.1f, 0.0f, 0.0f, "%.1f");
									break;
								}
								case ShaderDataType::Float3x3:
								{
									break;
								}
								case ShaderDataType::Float4x4: 
								{
									float* value = static_cast<float*>(element.LocalResurce);
									ImGui::DragFloat4("##0", &value[0], 0.1, 0.0f, 0.0f, "%.1f");
									ImGui::DragFloat4("##4", &value[4], 0.1f, 0.0f, 0.0f, "%.1f");
									ImGui::DragFloat4("##8", &value[8], 0.1f, 0.0f, 0.0f, "%.1f");
									ImGui::DragFloat4("##12", &value[12], 0.1f, 0.0f, 0.0f, "%.1f");
									break;
								}
								case ShaderDataType::Int:
								{
									ImGui::SameLine();
									int* value = static_cast<int*>(element.LocalResurce);
									ImGui::DragInt("##Int", value, 1);
									break;
								}
								
							default:
								break;
							}
						}
						
											
						ImGui::NextColumn();
						ImGui::SameLine();
						
						ImGui::PopItemWidth();
						ImGui::Columns(1);
							
						ImGui::PopID();
						
					}

				}
				
				bool aBuffer = (component.AlgorithmFlags & (1 << Shader::Algorithm::A_Buffer)) != 0,
					blend = (component.AlgorithmFlags & (1 << Shader::Algorithm::Blend)) != 0,
					clockWise = (component.AlgorithmFlags & (1 << Shader::Algorithm::ClockWise)) != 0,
					depthBuffer = (component.AlgorithmFlags & (1 << Shader::Algorithm::Depth_Buffer)) != 0,
					dobbleSide = (component.AlgorithmFlags & (1 << Shader::Algorithm::DobbleSide)) != 0;
					// frontSide = (component.AlgorithmFlags & (1 << Shader::Algorithm::)) != 0,
					// backSide = (component.AlgorithmFlags & (1 << Shader::Algorithm::A_Buffer)) != 0;


			

				if (ImGui::Checkbox("A-Buffer", &aBuffer)||
					ImGui::Checkbox("Blend", &blend)||
					ImGui::Checkbox("ClockWise", &clockWise)||
					ImGui::Checkbox("Depth-Buffer", &depthBuffer)||
					ImGui::RadioButton("Dobble-Side", &dobbleSide))
					component.AlgorithmFlags = (aBuffer << Shader::Algorithm::A_Buffer) | 
												(blend << Shader::Algorithm::Blend) | 
												(clockWise << Shader::Algorithm::ClockWise) | 
												(depthBuffer << Shader::Algorithm::Depth_Buffer) | 
												(dobbleSide << Shader::Algorithm::DobbleSide);

			});

		//Geomtry
		DrawComponent<GeomtryComponent>("Geometry", entity, [](GeomtryComponent& component)
			{
				if (ImGui::Button("Geomtry", ImVec2(100.0f, 0.0f)))
				{

				}
			});

		// Script
		DrawComponent<ScriptComponent>("Script", entity, [](ScriptComponent& component)
			{
				bool sricptClassExist = ScriptingEngine::ClassExists(component.Name);

				static char buffer[64];
				strcpy(buffer, component.Name.c_str());

				if (!sricptClassExist)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));
					
				}


				ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
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
				ImGui::PopItemWidth();

				if (!sricptClassExist)
				{
					ImGui::PopStyleColor();
				}

			});

		DrawComponent<FrameBufferComponent>("FrameBuffer", entity, [](FrameBufferComponent& component)
		{
				auto& frameBuffer = component.FrameBuffer;
					
				if (ImGui::Button("Create Framebuffer", ImVec2(0.0f, 0.0f)))
				{
					component.FrameBuffer = Framebuffer::Create(component.FramebufferSpecification);
#if 0
					component.FrameBuffer->AsstHandle = AsstHandle();
#endif
				}
				if(frameBuffer)
				{
					ImGui::SameLine();
					bool dealteFrambuffer = ImGui::Button("Delete", ImVec2(100.0f, 0.0f));
					auto& attachments = component.FramebufferSpecification.Attachments.Attachments;
					auto& fbspec = component.FramebufferSpecification;

					int delateNumber = -1;
					int colorAtachments = 0;
					for (size_t i = 0; i < attachments.size(); i++)
					{
						auto texFormat = attachments[i].TextureFormat;
						std::string format = GetStringFromFramTexFormat(texFormat);
						ImGui::PushID((format + std::to_string(i)).c_str());
						ImGui::Columns(2, "##FrameBuffer Textures", false);

						
						uint32_t textureID = 0;

						if (texFormat != FramebufferTextureFormat::Depth) textureID = component.FrameBuffer->GetColorAttachmentRendererID(colorAtachments);
						else textureID = component.FrameBuffer->GetDeathAttachmentRendererID();

						ImVec2 windowSize = ImGui::GetWindowSize();
						windowSize.x = windowSize.x * 0.92 - 110;
						ImGui::SetColumnWidth(0, windowSize.x+5);
						
						
						ImGui::SetColumnWidth(1,100);
						windowSize.y = windowSize.x / ((float)fbspec.Width / fbspec.Height);
						ImGui::Image((ImTextureID)textureID,  windowSize, ImVec2(0, 1), ImVec2(1, 0));
						
						ImGui::NextColumn();
						
						if (ImGui::Button(format.c_str(), { 100.0f , 0.0f}))
						{
						}



						//ImGui::SameLine();
						if (ImGui::Button("Delete", ImVec2(100.0f, 0.0f)))
							delateNumber = i;						
						ImGui::NextColumn();
						ImGui::SameLine();
						ImGui::Columns(1);
						if (texFormat != FramebufferTextureFormat::Depth)
							ImGui::Text("On Color Attachment: %i", colorAtachments++);
						else
							ImGui::Text("On Death Attachment");
						ImGui::PopID();
					}

					if (delateNumber != -1)
					{
						attachments.erase(attachments.begin(), attachments.begin() + delateNumber);
#if 0
						AsstHandle uuid = frameBuffer->AsstHandle;
#endif
						component.FrameBuffer = Framebuffer::Create(component.FramebufferSpecification);
#if 0
						component.FrameBuffer->AsstHandle = uuid;
#endif
					}
						
					if (ImGui::Button("Add Texture", ImVec2(100.0f, 0.0f)))
					{
						// TODO: Add Textur, list.
#if 0
						attachments.push_back(texType);
#if 0
						AsstHandle uuid = frameBuffer->AsstHandle;
#endif
						component.FrameBuffer = Framebuffer::Create(component.FramebufferSpecification);
#if 0
						component.FrameBuffer->AsstHandle = uuid;
#endif
#endif
					}

					if(dealteFrambuffer)
						component.FrameBuffer = nullptr;
					
					

					//ImGui::PopItemWidth();
					

					
				}
		});

		DrawComponent<MainViewPortComponent>("MainViewPort", entity, [](MainViewPortComponent& component)
		{
			
			auto& framBuffer = component.FrameBuffer;
			if(ImGui::Button("Reset Camer"))
			{
				component.EditorCamera = CreateRef<EditorCamera>(30.0f, 1.778f, 0.1, 1000.0f);
			}
			auto& camera = component.EditorCamera;
			glm::vec3 cameraPostion = camera->GetPosition();

			ImGui::Text("Position { x: %f, y: %f, z: %f }", cameraPostion.x, cameraPostion.y, cameraPostion.z);
			ImGui::Text("Distance { %f }", camera->GetDistance());
		});

		DrawComponent<SceneComponent>("Scene", entity, [](SceneComponent& component)
			{
				


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
				CreateEntity("Empty Entity");
			}

			ImGui::EndPopup();
		}


		ImGui::End();

		ImGui::Begin("Properties");

		DrawProperties();



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
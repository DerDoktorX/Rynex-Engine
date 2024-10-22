#include "rypch.h"
#include "SceneHierachyPannel.h"

#include <Rynex/Scene/Components.h>
#if RY_SCRIPTING_HAZEL
#include <Rynex/Scripting/HazelScripting/ScriptEngine.h>
#else
#include <Rynex/Scripting/ScriptingEngine.h>
#endif
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/Rendering/Renderer.h>

#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace Rynex {

	extern const std::filesystem::path g_AssetsPath;

	namespace Utils {

		static Entity AddChilrenToEntity(Entity parentEntity, Ref<Scene>& context, const std::string& childeName = "Empty Childern Entiy")
		{
			UUID parent = parentEntity.GetComponent<RealtionShipComponent>().FirstID;

			Entity cildeEntity = context->CreateEntity(childeName);

			RealtionShipComponent& childeRealtionshipC = cildeEntity.GetComponent<RealtionShipComponent>();
			RealtionShipComponent& parentRealtionshipC = parentEntity.GetComponent<RealtionShipComponent>();

			if (!parent)
			{
				parentRealtionshipC.FirstID = cildeEntity.GetUUID();
				childeRealtionshipC.ParentID = parentEntity.GetUUID();
			}
			else
			{
				Entity otherChildsEntity = context->GetEntitiyByUUID(parent);
				UUID nextChildren = otherChildsEntity.GetComponent<RealtionShipComponent>().NextID;

				while (nextChildren)
				{
					otherChildsEntity = context->GetEntitiyByUUID(nextChildren);
					nextChildren = otherChildsEntity.GetComponent<RealtionShipComponent>().NextID;
				}

				otherChildsEntity.GetComponent<RealtionShipComponent>().NextID = cildeEntity.GetUUID();
				cildeEntity.GetComponent<RealtionShipComponent>().PreviusID = otherChildsEntity.GetUUID();
				childeRealtionshipC.ParentID = parentEntity.GetUUID();

			}
			return cildeEntity;
		}

		static void DealteAllChilrenFromEntity(Entity parentEntity, Ref<Scene>& context, Entity *selectionContext)
		{
			RealtionShipComponent& parentRealtionshipC = parentEntity.GetComponent<RealtionShipComponent>();
			UUID nextCild = parentRealtionshipC.FirstID;
			while (nextCild)
			{
				Entity childEntity = context->GetEntitiyByUUID(nextCild);

				RealtionShipComponent& childeRealtionshipC = childEntity.GetComponent<RealtionShipComponent>();

				RY_CORE_ASSERT(childeRealtionshipC.ParentID == parentEntity.GetUUID(), "is not SameParent!");
				
				UUID first = childeRealtionshipC.FirstID;
				if (first)
					DealteAllChilrenFromEntity(context->GetEntitiyByUUID(first), context, selectionContext);
				nextCild = childeRealtionshipC.NextID;
				if (childEntity == *selectionContext)
					*selectionContext = {};
				context->DestroyEntity(childEntity);

			}
			parentRealtionshipC.FirstID = 0;
		}

		static void UpdateChilrenMatrixeCompentens(Entity parentEntity, Ref<Scene>& context)
		{
			RealtionShipComponent& parentRealtionshipC = parentEntity.GetComponent<RealtionShipComponent>();
			Matrix4x4Component& parentMat4C = parentEntity.GetComponent<Matrix4x4Component>();
			TransformComponent& parentTransfromC = parentEntity.GetComponent<TransformComponent>();
			parentMat4C.Matrix4x4 = parentTransfromC.GetTransform();
			if (parentRealtionshipC.ParentID)
			{
				parentMat4C.GlobleMatrix4x4 = context->GetEntitiyByUUID(parentRealtionshipC.ParentID).GetComponent<Matrix4x4Component>().GlobleMatrix4x4 * parentMat4C.Matrix4x4;
			}
			else
			{
				parentMat4C.GlobleMatrix4x4 = parentMat4C.Matrix4x4;
			}

			UUID parentIntEntiy = parentEntity.GetUUID();
			UUID nextCild = parentRealtionshipC.FirstID;
			while (nextCild)
			{
				Entity childEntity = context->GetEntitiyByUUID(nextCild);

				RealtionShipComponent& childeRealtionshipC = childEntity.GetComponent<RealtionShipComponent>();
				if (childeRealtionshipC.ParentID == parentIntEntiy)
				{
					Matrix4x4Component& cildernMat4C = childEntity.GetComponent<Matrix4x4Component>();
					TransformComponent& cildernTransfromC = childEntity.GetComponent<TransformComponent>();
					cildernMat4C.Matrix4x4 = cildernTransfromC.GetTransform();
					cildernMat4C.GlobleMatrix4x4 = parentMat4C.GlobleMatrix4x4 * cildernMat4C.Matrix4x4;
					if (childeRealtionshipC.FirstID)
						UpdateChilrenMatrixeCompentens(childEntity, context);
					nextCild = childeRealtionshipC.NextID;
				}
			}
		}

		static bool DrawVec4Controler(const std::string& label, glm::vec4& values, float resetValue = 0.0f, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columeWith);
			ImGui::Text(label.c_str());
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(BufferAPI::GetStringFromShaderData(ShaderDataType::Float4).c_str());
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
			{
				values.x = resetValue;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1f", resetValue);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f") || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", butenSize))
			{
				values.y = resetValue;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1f", resetValue);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f") || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Z", butenSize))
			{
				values.z = resetValue;
				changes = true;
			}

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1f", resetValue);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f") || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.85f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.8f, 0.95f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.85f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if ( ImGui::Button("W", butenSize) )
			{
				values.z = resetValue;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1ff", resetValue);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##W", &values.z, 0.1f, 0.0f, 0.0f, "%.2f") || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PopStyleVar();

			ImGui::Columns(1);
			ImGui::PopID();
			return changes;
		}

		static bool DrawVec3Controler(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());
			ImGui::Columns(2, label.c_str(), false);
			ImGui::SetColumnWidth(0, columeWith);
			ImGui::Text(label.c_str());
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(BufferAPI::GetStringFromShaderData(ShaderDataType::Float3).c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{0.0f , 0.0f});

			float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 butenSize = { lineHeigth - 2.0f, lineHeigth };



			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", butenSize))
			{
				values.x = resetValue;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value  %.1f", resetValue);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f" ) || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();
			

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", butenSize))
			{
				values.y = resetValue;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value  %.1f", resetValue);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f") || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Z", butenSize))
			{
				values.z = resetValue;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value  %.1f", resetValue);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f") || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PopStyleVar();

			ImGui::Columns(1);
			ImGui::PopID();
			return changes;
		}

		static bool DrawVec2Controler(const std::string & label, glm::vec2& values, float resetValue = 0.0f, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columeWith);
			ImGui::Text(label.c_str());
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(BufferAPI::GetStringFromShaderData(ShaderDataType::Float2).c_str());
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
			{
				values.x = resetValue;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1f", resetValue);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f") || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", butenSize))
			{
				values.y = resetValue;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1f", resetValue);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f") || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PopStyleVar();

			ImGui::Columns(1);
			ImGui::PopID();
			return changes;
		}

		static bool DrawFloatControler(const std::string& label, float* values, float resetValue = 0.0f, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columeWith);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());


			ImGui::SameLine();
			changes = ImGui::DragFloat("##F", values, 0.1f, 0.0f, 0.0f, "%.2f") || changes;
			ImGui::PopItemWidth();

			ImGui::Columns(1);
			ImGui::PopID();
			return changes;
		}
		
		static void LoadeMesh()
		{

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

		static void DargDropTextureButten(Ref<Texture>* compontenItem)
		{
			DragDropButtenForAsset<Texture>(compontenItem, "Texture", AssetType::Texture2D);
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
						ellement.Type = BufferAPI::GetShaderDataTypeFromString(type);
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
	}

	SceneHierachyPannel::SceneHierachyPannel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierachyPannel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierachyPannel::DrawEntityNode(Entity entity, bool normale)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		auto& id = entity.GetComponent<IDComponent>().ID;
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		auto& realtionship = entity.GetComponent<RealtionShipComponent>();
		bool parent = realtionship.FirstID, 
			cildern = realtionship.ParentID;
		if (((cildern && !parent) || (cildern && parent)) && normale)
			return;

		if (!parent)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opende = ImGui::TreeNodeEx(&id,flags, tag.c_str());


		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entiytDeleted = false, entiytChildrenDeleted = false,enitityCildeAdd = false, createCopyEnitity = false;
		if (ImGui::BeginPopupContextItem(tag.c_str()))
		{
			
			if (ImGui::MenuItem("Delete Entity"))
				entiytDeleted = true;
			if (ImGui::MenuItem("Delete Entity + Children"))
				entiytDeleted = true;
			
			if (ImGui::MenuItem("Add Childe Entity"))
				enitityCildeAdd = true;
#if 0
			if (ImGui::MenuItem("Create Copy"))
				createCopyEnitity = true;
#endif	// TODO: Create Copy Entity
			ImGui::EndPopup();
		}

		if (opende)
		{
			if(parent)
			{
				Entity enitytCilds = m_Context->GetEntitiyByUUID(realtionship.FirstID);
				DrawEntityNode(enitytCilds,false);
				UUID nextChild = enitytCilds.GetComponent<RealtionShipComponent>().NextID;
				UUID trueParentChild = entity.GetUUID();
				while (nextChild)
				{
					enitytCilds = m_Context->GetEntitiyByUUID(nextChild);
					DrawEntityNode(enitytCilds,false);
					nextChild = enitytCilds.GetComponent<RealtionShipComponent>().NextID;

				}

				ImGui::TreePop();

			}
			
		}
		if(enitityCildeAdd)
		{
			Utils::AddChilrenToEntity(entity, m_Context, "Empty Childern Entiy");
			Utils::UpdateChilrenMatrixeCompentens(entity, m_Context);
		}
#if 0
		if (createCopyEnitity)
		{

		}
#endif	// TODO: Create Copy Entity
		if (entiytDeleted || entiytChildrenDeleted)
		{
			DeleteEntity(entity, entiytChildrenDeleted);
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

				if(!m_SelectionContext.HasComponent<MeshComponent>() )
					DisplayAddComponentEntry<GeomtryComponent>("Geomtry");

				if (!m_SelectionContext.HasComponent<GeomtryComponent>() )
					DisplayAddComponentEntry<MeshComponent>("Mesh");

				
			}
			DisplayAddComponentEntry<FrameBufferComponent>("FrameBuffer");

			if (!m_SelectionContext.HasComponent<DrirektionleLigthComponent>() && 
				!m_SelectionContext.HasComponent<PointLigthComponent>() &&
				!m_SelectionContext.HasComponent<SpotLigthComponent>())
				DisplayAddComponentEntry<AmbientLigthComponent>("Ambient-Ligth");

			if (!m_SelectionContext.HasComponent<AmbientLigthComponent>() &&
				!m_SelectionContext.HasComponent<PointLigthComponent>() &&
				!m_SelectionContext.HasComponent<SpotLigthComponent>())
			DisplayAddComponentEntry<DrirektionleLigthComponent>("Drirektionle-Ligth");

			if (!m_SelectionContext.HasComponent<AmbientLigthComponent>() &&
				!m_SelectionContext.HasComponent<DrirektionleLigthComponent>() &&
				!m_SelectionContext.HasComponent<SpotLigthComponent>())
			DisplayAddComponentEntry<PointLigthComponent>("Point-Ligth");

			if (!m_SelectionContext.HasComponent<AmbientLigthComponent>() &&
				!m_SelectionContext.HasComponent<PointLigthComponent>() &&
				!m_SelectionContext.HasComponent<DrirektionleLigthComponent>())
			DisplayAddComponentEntry<SpotLigthComponent>("Spot-Ligth");

			ImGui::EndPopup();
		}

		// Transform
		Utils::DrawComponent<TransformComponent>("Transform", entity, [&](TransformComponent& component)
		{
			bool use = false;
			use = Utils::DrawVec3Controler("Translation", component.Transaltion) || use;
			glm::vec3 rotation = glm::degrees(component.Rotation);
			use = Utils::DrawVec3Controler("Rotation", rotation) || use;
			component.Rotation = glm::radians(rotation);
			use = Utils::DrawVec3Controler("Scale", component.Scale, 1.0f) || use;
			if(use)
			{

				entity.UpdateMatrix();
#if 0
				Utils::UpdateChilrenMatrixeCompentens(entity, m_Context);
				if (entity.HasComponent<StaticMeshComponent>())
				{
					glm::mat4& mat4C = entity.GetComponent<Matrix4x4Component>().GlobleMatrix4x4;
					StaticMeshComponent& staticMesh = entity.GetComponent<StaticMeshComponent>();
					uint32_t index = 0;
#if 1
					for (auto& meshData : staticMesh.LocaleMeshMatrix)
					{
						staticMesh.GlobleMeshMatrix[index] = mat4C * meshData;
						index++;
					}
#else
					const std::vector<Ref<Node>>& dataNode = staticMesh.ModelR->GetNodeDatas();
					for (auto& meshData : dataNode)
					{
						int parent = meshData->ParentIndex;
						if(parent!=-1)
						{
							meshData->GlobleMatrix = dataNode[parent]->LocaleMatrix * meshData->LocaleMatrix;
						}
						else
						{
							meshData->GlobleMatrix = mat4C * meshData->LocaleMatrix;
						}
						index++;
					}
#endif
				}
#endif
			}
		});

		// Camera
		Utils::DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
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
						ImGui::Checkbox("Fixxed Aspect Rotation", &component.FixedAspectRotaion);
						break;
					}
				};
			ImGui::Checkbox("Primary Camer", &component.Primary);
		});

		// SpriteRenderer
		Utils::DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color), 0.1f);
			// Texture
			Utils::DargDropTextureButten(&component.Texture);
			ImGui::Checkbox("Render Single", &component.RenderSingle);
		});

		// Material
		Utils::DrawComponent<MaterialComponent>("Material", entity, [](MaterialComponent& component)
		{

			ImGui::ColorEdit3("Color", glm::value_ptr(component.Color), 0.1f);

			// Texture

			Utils::DargDropTextureButten(&component.Texture);
			Utils::DargDropShaderButten(component);

			if (component.Shader != nullptr)
			{
				if (component.UniformLayoute.size() == 0)
				{
					component.UniformLayoute.clear();
					
					const auto& layoute = component.Shader->GetUniformLayoute();
					component.UniformLayoute.reserve(layoute.size());

					for (const auto& [name, type] : layoute)
						component.UniformLayoute.emplace_back(name, BufferAPI::GetShaderDataTypeFromString(type), ShaderResourceType::None);
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
						ImGui::SetTooltip(BufferAPI::GetStringFromShaderData(element.Type).c_str());

					ImGui::NextColumn();
					ImGui::PushMultiItemsWidths( 1, ImGui::CalcItemWidth());

					
					ImGui::Checkbox("##Globle Resurce", &element.GloblelResurce);
					
					if(element.GloblelResurce)
					{
						ImGui::SameLine();
						std::map<ShaderResourceType,std::string>& otherResources = BufferAPI::GetShaderResourceTypeStringMap();
					
						if (ImGui::BeginCombo("##", otherResources[element.ShResourceType].c_str(), ImGuiComboFlags_None))
						{
							ShaderResourceType name;
							uint8_t size = otherResources.size();
							for (uint8_t i = 0; i < size; i++)
							{
								name = (ShaderResourceType)i;
								if (ImGui::MenuItem(otherResources[name].c_str()))
								{
									element.ShResourceType = name;
									ImGui::CloseCurrentPopup();
								}
							}
							ImGui::EndCombo();
						}
					}
					else
					{
						if (!element.LocalResurce.size())
						{
							element.LocalResurce.resize(ShaderDataTypeSize(element.Type));
							switch (element.Type)
							{
								case ShaderDataType::Float:
								{
									float* value = (float*)element.LocalResurce.data();
									ImGui::DragFloat("##Float", value);
									break;
								}
								case ShaderDataType::Float2:
								{
									glm::vec<2, float>* value = (glm::vec<2, float>*)element.LocalResurce.data();
									*value = { 0.0f, 0.0f };
									break;
								}
								case ShaderDataType::Float3:
								{
									glm::vec<3, float>* value = (glm::vec<3, float>*)element.LocalResurce.data();
									*value = { 0.0f, 0.0f,0.0f };
									break;
								}
								case ShaderDataType::Float4:
								{
									glm::vec<4, float>* value = (glm::vec<4, float>*)element.LocalResurce.data();
									*value = { 0.0f, 0.0f,0.0f, 0.0f };
									break;
								}
								case ShaderDataType::Float3x3:
								{
									glm::mat3* value = (glm::mat3*)element.LocalResurce.data();
									*value = { 
										1.0f, 0.0f, 0.0f, 
										0.0f, 1.0f, 0.0f,
										0.0f, 0.0f, 1.0f
									};
									break;
								}
								case ShaderDataType::Float4x4:
								{
									glm::mat4* value = (glm::mat4*)element.LocalResurce.data();
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
								float* value = (float*)element.LocalResurce.data();
								ImGui::DragFloat("##Float", value, 0.1f, 0.0f, 0.0f, "%.1f");
								break;
							}
							case ShaderDataType::Float2:
							{
								ImGui::SameLine();
								float* value = (float*)element.LocalResurce.data();
								ImGui::DragFloat2("##Float2", value, 0.1f, 0.0f, 0.0f, "%.1f");
								break;
							}
							case ShaderDataType::Float3:
							{
								ImGui::SameLine();
								float* value = (float*)element.LocalResurce.data();
								ImGui::DragFloat3("##Float3", value, 0.1f, 0.0f, 0.0f, "%.1f");
								break;
							}
							case ShaderDataType::Float4:
							{
								ImGui::SameLine();
								float* value = (float*)element.LocalResurce.data();
								ImGui::DragFloat4("##Float4", value, 0.1f, 0.0f, 0.0f, "%.1f");
								break;
							}
							case ShaderDataType::Float3x3:
							{
								break;
							}
							case ShaderDataType::Float4x4: 
							{
								float* value = (float*)element.LocalResurce.data();
								ImGui::DragFloat4("##0", &value[0], 0.1, 0.0f, 0.0f, "%.1f");
								ImGui::DragFloat4("##4", &value[4], 0.1f, 0.0f, 0.0f, "%.1f");
								ImGui::DragFloat4("##8", &value[8], 0.1f, 0.0f, 0.0f, "%.1f");
								ImGui::DragFloat4("##12", &value[12], 0.1f, 0.0f, 0.0f, "%.1f");
								break;
							}
							case ShaderDataType::Int:
							{
								ImGui::SameLine();
								int* value = (int*)element.LocalResurce.data();
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
			
			int entityRendererMode = component.AlgorithmFlags;

			bool wireFrame = BIT_EQUAL(entityRendererMode, Renderer::WireFrame),
				a_Buffer = BIT_EQUAL(entityRendererMode, Renderer::A_Buffer),
				death_Buffer = BIT_EQUAL(entityRendererMode, Renderer::Death_Buffer),
				callFace_Nono = BIT_EQUAL(entityRendererMode, Renderer::CallFace_Nono),
				callFace_Front = BIT_EQUAL(entityRendererMode, Renderer::CallFace_Front),
				callFace_Back = BIT_EQUAL(entityRendererMode, Renderer::CallFace_Back),
				callFace_FrontBack = BIT_EQUAL(entityRendererMode, Renderer::CallFace_FrontBack);

			if (ImGui::Checkbox("Wirframe mode:", &wireFrame))
			{
				component.AlgorithmFlags = BIT_SET_ON(wireFrame, Renderer::WireFrame, entityRendererMode);

			}
			if (ImGui::Checkbox("A-Buffer mode:", &a_Buffer))
			{
				component.AlgorithmFlags = BIT_SET_ON(a_Buffer, Renderer::A_Buffer, entityRendererMode);
			}
			if (ImGui::Checkbox("Death-Buffer mode:", &death_Buffer))
			{
				component.AlgorithmFlags = BIT_SET_ON(death_Buffer, Renderer::Death_Buffer, entityRendererMode);
			}

			if (ImGui::RadioButton("None", callFace_Nono))
			{
				entityRendererMode = BIT_SET_ON(1, Renderer::CallFace_Nono, entityRendererMode);
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_Front, entityRendererMode);
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_Back, entityRendererMode);
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_FrontBack, entityRendererMode);
				component.AlgorithmFlags = entityRendererMode;
			}
			if (ImGui::RadioButton("Front", callFace_Front))
			{
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_Nono, entityRendererMode);
				entityRendererMode = BIT_SET_ON(1, Renderer::CallFace_Front, entityRendererMode);
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_Back, entityRendererMode);
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_FrontBack, entityRendererMode);
				component.AlgorithmFlags = entityRendererMode;
			}
			if (ImGui::RadioButton("Back", callFace_Back))
			{
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_Nono, entityRendererMode);
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_Front, entityRendererMode);
				entityRendererMode = BIT_SET_ON(1, Renderer::CallFace_Back, entityRendererMode);
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_FrontBack, entityRendererMode);
				component.AlgorithmFlags = entityRendererMode;
			}
			if (ImGui::RadioButton("Front Back", callFace_FrontBack))
			{
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_Nono, entityRendererMode);
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_Front, entityRendererMode);
				entityRendererMode = BIT_SET_ON(0, Renderer::CallFace_Back, entityRendererMode);
				entityRendererMode = BIT_SET_ON(1, Renderer::CallFace_FrontBack, entityRendererMode);
				component.AlgorithmFlags = entityRendererMode;
			}



		



		});

		// Geomtry
		Utils::DrawComponent<GeomtryComponent>("Geometry", entity, [](GeomtryComponent& component)
		{
			Utils::DragDropButtenForAsset<VertexArray>(&component.Geometry, "VertexArray", AssetType::VertexArray);
			auto& vertexArray = component.Geometry;
			if (vertexArray)
			{

			}
		});

		// Script
		Utils::DrawComponent<ScriptComponent>("Script", entity, [](ScriptComponent& component)
			{
#if RY_SCRIPTING_HAZEL
				std::unordered_map<std::string, Ref<ScriptClass>> clasesMap = ScriptEngine::GetEntityClasses();
				bool sricptClassExist = clasesMap.find(component.Name) != clasesMap.end();
#else
				bool sricptClassExist = ScriptingEngine::ClassExists(component.Name);
#endif
				static char buffer[64];
				strcpy(buffer, component.Name.c_str());

				if (!sricptClassExist)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));
					
				}


				ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
				if (ImGui::BeginCombo("Script: Class", buffer, ImGuiComboFlags_None))
				{
#if RY_SCRIPTING_HAZEL
					if (ImGui::MenuItem("None"))
					{
						component.Name = "None";
						ImGui::CloseCurrentPopup();
					}

					for (auto& [name, script] : clasesMap)
					{
						if (ImGui::MenuItem(name.c_str()))
						{

							component.Name = std::string(name);
							ImGui::CloseCurrentPopup();
						}
					}
					ImGui::EndCombo();
#else

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
#endif
				}
				ImGui::PopItemWidth();

				if (!sricptClassExist)
				{
					ImGui::PopStyleColor();
				}

			});

		// FrameBuffer
		Utils::DrawComponent<FrameBufferComponent>("FrameBuffer", entity, [](FrameBufferComponent& component)
		{
			auto& frameBuffer = component.FrameBuffer;
					
			
			Utils::DragDropButtenForAsset<Framebuffer>(&component.FrameBuffer, "FrameBuffer", AssetType::Framebuffer);
			

			if(frameBuffer)
			{
				component.FramebufferSpecification = component.FrameBuffer->GetFramebufferSpecification();
				
				auto& fbspec = component.FramebufferSpecification;
				auto& attachments = component.FramebufferSpecification.Attachments.Attachments;
				
				
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
					attachments.erase(attachments.begin()+delateNumber);
					component.FrameBuffer = Framebuffer::Create(component.FramebufferSpecification);
				}
			}
		});

		// Mesh
		Utils::DrawComponent<MeshComponent>("Mesh", entity, [&](MeshComponent& component)
		{
			
			auto& model = component.ModelR;
			ImGui::Button("Mesh", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetAssetTypeDragAndDropName(AssetType::Model).c_str()))
				{
					ImGui::OpenPopup("Loade-Mesh");
					model = AssetManager::GetAsset<Model>(*(AssetHandle*)payload->Data);
					ImGui::EndDragDropTarget();
				}
			}
			
			if (ImGui::BeginPopup("Loade-Mesh"))
			{
				
				if (ImGui::MenuItem("Static-Mesh"))
				{

					if (component.ModelR != nullptr)
					{
						UUID first = entity.GetComponent<RealtionShipComponent>().FirstID;
						if (first)
							Utils::DealteAllChilrenFromEntity(entity, m_Context, &m_SelectionContext);
						if (entity.HasComponent<StaticMeshComponent>())
						{
							auto& smC = entity.GetComponent<StaticMeshComponent>();
							smC.ModelR = nullptr;
						}
					}

					if (!entity.HasComponent<StaticMeshComponent>())
						entity.AddComponent<StaticMeshComponent>();
					StaticMeshComponent& staticMesh = entity.GetComponent<StaticMeshComponent>();
					staticMesh.ModelR = model;
					Matrix4x4Component& parentMat4C = entity.GetComponent<Matrix4x4Component>();

					auto& modelData = model->GetRootDatas();
					staticMesh.LocaleMeshMatrix.reserve(modelData.size());
					
					for (auto& meshData : modelData)
					{
						staticMesh.LocaleMeshMatrix.emplace_back(meshData.NodeMatrix);
						staticMesh.GlobleMeshMatrix.emplace_back(parentMat4C.GlobleMatrix4x4 * meshData.NodeMatrix);
					}
				}
				if (ImGui::MenuItem("Dynamic-Mesh"))
				{
					
					if (component.ModelR != nullptr)
					{
						UUID first = entity.GetComponent<RealtionShipComponent>().FirstID;
						if (first)
							Utils::DealteAllChilrenFromEntity(entity, m_Context, &m_SelectionContext);
						if (entity.HasComponent<StaticMeshComponent>())
							entity.RemoveComponent<StaticMeshComponent>();
					}

					if (!entity.HasComponent<Matrix4x4Component>())
						entity.AddComponent<Matrix4x4Component>();
					Matrix4x4Component& parentMat4C = entity.GetComponent<Matrix4x4Component>();

					if (!entity.HasComponent<RealtionShipComponent>())
						entity.AddComponent<RealtionShipComponent>();
					if (!entity.HasComponent<MaterialComponent>())
						entity.AddComponent<MaterialComponent>();
					MaterialComponent& materialCParent = entity.GetComponent<MaterialComponent>();

					std::vector<MeshRootData> rootDatas = model->GetRootDatas();
					std::vector<Ref<Mesh>> meshes = model->GetMeshes();
					uint32_t size = rootDatas.size() == meshes.size() ? meshes.size() : 0;
					for (uint32_t i = 0; i < size; i++)
					{
						MeshRootData& rootData = rootDatas[i];
						Ref<Mesh>& mesh = meshes[i];
						Entity childeEntity = Utils::AddChilrenToEntity(entity, m_Context, rootData.NodeName);

						childeEntity.AddComponent<DynamicMeshComponent>();
						childeEntity.AddComponent<MaterialComponent>();
						
						if (!childeEntity.HasComponent<Matrix4x4Component>())
							childeEntity.AddComponent<Matrix4x4Component>();
						Matrix4x4Component& childMat4C = childeEntity.GetComponent<Matrix4x4Component>();
						childMat4C.Matrix4x4 = rootData.NodeMatrix;
						childMat4C.GlobleMatrix4x4 = parentMat4C.Matrix4x4 * rootData.NodeMatrix;

						if (!childeEntity.HasComponent<TransformComponent>())
							childeEntity.AddComponent<TransformComponent>();
						TransformComponent& transformC = childeEntity.GetComponent<TransformComponent>();
						transformC.SetTransform(childMat4C.Matrix4x4);

						if (!childeEntity.HasComponent<DynamicMeshComponent>())
							childeEntity.AddComponent<DynamicMeshComponent>();
						DynamicMeshComponent& dynamicMeshC = childeEntity.GetComponent<DynamicMeshComponent>();
						dynamicMeshC.MeshR = mesh;

						if (!childeEntity.HasComponent<MaterialComponent>())
							childeEntity.AddComponent<MaterialComponent>();

						
					}
					Utils::UpdateChilrenMatrixeCompentens(entity, m_Context);
				}
				ImGui::EndPopup();
			}
			


			if (component.ModelR != nullptr)
			{
				ImGui::SameLine();
				if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
				{
					
					if (entity.HasComponent<RealtionShipComponent>())
					{
						UUID first = entity.GetComponent<RealtionShipComponent>().FirstID;
						if (first)
							Utils::DealteAllChilrenFromEntity(entity, m_Context, &m_SelectionContext);
					}
					if (entity.HasComponent<StaticMeshComponent>())
						entity.RemoveComponent<StaticMeshComponent>();
					component.ModelR = nullptr;
				}

			}
			
		});
#if 0
		Utils::DrawComponent<DynamicMeshComponent>("Dynamic-Mesh", entity, [](DynamicMeshComponent& component)
		{
			auto& model = component.MeshR;
			Utils::DragDropButtenForAsset<Mesh>(&component.MeshR, "Mesh", AssetType::Mesh);



		});
#endif
		// Ambient-Ligth
		Utils::DrawComponent<AmbientLigthComponent>("Ambient-Ligth", entity, [](AmbientLigthComponent& component)
			{
				ImGui::ColorEdit3("Color: ", glm::value_ptr(component.Color));
				ImGui::DragFloat("Intensitie: ", &component.Intensitie, 0.01f, -1.0f, 10.0f, "%.2f");
			});

		// Drirektionle-Ligth
		Utils::DrawComponent<DrirektionleLigthComponent>("Drirektionle-Ligth", entity, [](DrirektionleLigthComponent& component)
			{
				ImGui::ColorEdit3("Color: ", glm::value_ptr(component.Color));
				ImGui::DragFloat("Intensitie: ", &component.Intensitie, 0.01f, -1.0f, 10.0f, "%.2f");
			});

		// Point-Ligth
		Utils::DrawComponent<PointLigthComponent>("Point-Ligth", entity, [](PointLigthComponent& component)
			{
				ImGui::ColorEdit3("Color: ", glm::value_ptr(component.Color));
				ImGui::DragFloat("Intensitie: ", &component.Intensitie, 0.01f, -1.0f, 10.0f, "%.2f");
				ImGui::DragFloat("Distence: ", &component.Distence, 0.01f, -1.0f, 10.0f, "%.2f");
			});

		// Point-Ligth
		Utils::DrawComponent<SpotLigthComponent>("Spot-Ligth", entity, [](SpotLigthComponent& component)
			{
				ImGui::ColorEdit3("Color: ", glm::value_ptr(component.Color));
				ImGui::DragFloat("Intensitie: ", &component.Intensitie, 0.01f, -1.0f, 10.0f, "%.2f");
				ImGui::DragFloat("Distence: ", &component.Distence, 0.01f, -1.0f, 10.0f, "%.2f");
				ImGui::DragFloat("Inner: ", &component.Inner, 0.01f, -1.0f, 10.0f, "%.2f");
				ImGui::DragFloat("Outer: ", &component.Outer, 0.01f, -1.0f, 10.0f, "%.2f");
			});
	}

	void SceneHierachyPannel::OpenSceneHierachy()
	{
		m_WindowSceneHierachyOpen = true;
	}

	void SceneHierachyPannel::OpenProperties()
	{
		m_WindowPropertiesOpen = true;
	}

	void SceneHierachyPannel::OnImGuiRender()
	{
		if(m_WindowSceneHierachyOpen)
		{
			ImGui::Begin("Scene Hierachy", &m_WindowSceneHierachyOpen);

			// loop thorw all Entity
			m_Context->m_Registery.each([&](auto entityID)
				{
					Entity entity{ entityID, m_Context.get() };
					DrawEntityNode(entity);
				});
			EcexuterDeleteing();
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
		}

		if (m_WindowPropertiesOpen)
		{
			ImGui::Begin("Properties", &m_WindowPropertiesOpen);
			DrawProperties();
			ImGui::End();
		}
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

	void SceneHierachyPannel::DeleteEntity(Entity entitiy, bool childeren)
	{
		m_EntityDelete.emplace_back(entitiy, childeren);
	}

	void SceneHierachyPannel::EcexuterDeleteing()
	{
		for(auto&[entitiy, childeren] : m_EntityDelete)
		{
#if 0
			RealtionShipComponent& realtionship = enity.GetComponent<RealtionShipComponent>();

			Utils::DealteAllChilrenFromEntity(enity, m_Context, &m_SelectionContext);
			if (realtionship.ParentID)
			{
				UUID nextCilde = realtionship.NextID ? realtionship.NextID : 0ull;
				UUID previusCilde = realtionship.PreviusID  ? realtionship.PreviusID : 0ull;
				if (nextCilde )
				{
					Entity enitytNext = m_Context->GetEntitiyByUUID(realtionship.NextID);
					enitytNext.GetComponent<RealtionShipComponent>().PreviusID = previusCilde;
				}	
				if (previusCilde)
				{
					Entity enitytPrivus = m_Context->GetEntitiyByUUID(realtionship.PreviusID);
					enitytPrivus.GetComponent<RealtionShipComponent>().NextID = nextCilde;
				}
				else
				{
					Entity enitytPrente = m_Context->GetEntitiyByUUID(realtionship.ParentID);
					enitytPrente.GetComponent<RealtionShipComponent>().FirstID = nextCilde;
				}
				
			}
			
#endif
			if (childeren)
				entitiy.DestroyEntityChildrens();
			m_Context->DestroyEntity(entitiy);
		}

		m_EntityDelete.clear();
		if (!m_SelectionContext)
			m_SelectionContext = {};
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
#if RY_SCRIPTING_HAZEL
			std::unordered_map<std::string, Ref<ScriptClass>> clasesMap = ScriptEngine::GetEntityClasses();
			if(clasesMap.find(component.Name) == clasesMap.end())
				entity.SetState(Entity::State::Error);
			else
				entity.SetState(Entity::State::None);
#else
			if (!ScriptingEngine::ClassExists(component.Name))
				entity.SetState(Entity::State::Error);
			else
				entity.SetState(Entity::State::None);
#endif
			
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
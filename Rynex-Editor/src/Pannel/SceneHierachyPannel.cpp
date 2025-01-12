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
#include <Rynex/Renderer/Materials/BasicMaterial.h>
#include <Rynex/Utils/EnumString.h>

#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#define RY_ENABLE_UTILS_REALTION_SHIP_COMP 0
namespace Rynex {

	namespace Utils {
#if RY_ENABLE_UTILS_REALTION_SHIP_COMP
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
			ModelMatrixComponent& parentMat4C = parentEntity.GetComponent<ModelMatrixComponent>();
			TransformComponent& parentTransfromC = parentEntity.GetComponent<TransformComponent>();
			parentMat4C.Locale = parentTransfromC.GetTransform();
			if (parentRealtionshipC.ParentID)
			{
				parentMat4C.Globle = context->GetEntitiyByUUID(parentRealtionshipC.ParentID).GetComponent<ModelMatrixComponent>().Globle * parentMat4C.Locale;
			}
			else
			{
				parentMat4C.Globle = parentMat4C.Locale;
			}

			UUID parentIntEntiy = parentEntity.GetUUID();
			UUID nextCild = parentRealtionshipC.FirstID;
			while (nextCild)
			{
				Entity childEntity = context->GetEntitiyByUUID(nextCild);

				RealtionShipComponent& childeRealtionshipC = childEntity.GetComponent<RealtionShipComponent>();
				if (childeRealtionshipC.ParentID == parentIntEntiy)
				{
					ModelMatrixComponent& cildernMat4C = childEntity.GetComponent<ModelMatrixComponent>();
					TransformComponent& cildernTransfromC = childEntity.GetComponent<TransformComponent>();
					cildernMat4C.Locale = cildernTransfromC.GetTransform();
					cildernMat4C.Globle = parentMat4C.Globle * cildernMat4C.Locale;
					if (childeRealtionshipC.FirstID)
						UpdateChilrenMatrixeCompentens(childEntity, context);
					nextCild = childeRealtionshipC.NextID;
				}
			}
		}
#endif

		static bool DrawVec4Controler(const std::string& label, glm::vec4& values, float resetValue = 0.0f, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			ImFont* boldFont = io.Fonts->Fonts[0];

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

		static bool DrawVec4ControlerR(const std::string& label, glm::vec4& values, glm::vec4& resetValue, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			ImFont* boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());
			ImGui::Columns(2, label.c_str(), false);
			ImGui::SetColumnWidth(0, columeWith);
			ImGui::Text(label.c_str());
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(BufferAPI::GetStringFromShaderData(ShaderDataType::Float4).c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f , 0.0f });

			float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 butenSize = { lineHeigth + 3.0f, lineHeigth };



			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", butenSize))
			{
				values.x = resetValue.x;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1f", resetValue.x);
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
				values.y = resetValue.y;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1f", resetValue.y);
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
				values.z = resetValue.z;
				changes = true;
			}

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1f", resetValue.z);
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
			if (ImGui::Button("W", butenSize))
			{
				values.z = resetValue.w;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1ff", resetValue.w);
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

		static bool DrawVec3ControlerR(const std::string& label, glm::vec3& values, glm::vec3& resetValue, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			ImFont* boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());
			ImGui::Columns(2, label.c_str(), false);
			ImGui::SetColumnWidth(0, columeWith);
			ImGui::Text(label.c_str());
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(BufferAPI::GetStringFromShaderData(ShaderDataType::Float3).c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f , 0.0f });

			float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 butenSize = { lineHeigth - 2.0f, lineHeigth };



			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", butenSize))
			{
				values.x = resetValue.x;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value  %.1f", resetValue);
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
				values.y = resetValue.y;
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
				values.z = resetValue.z;
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

		static bool DrawVec3Controler(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			ImFont* boldFont = io.Fonts->Fonts[0];

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
			ImFont* boldFont = io.Fonts->Fonts[0];

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

		static bool DrawFloatControler(const std::string& label, float* values, float resetValue = 0.0f, float min = 0.0f, float max = 0.0f, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			ImFont* boldFont = io.Fonts->Fonts[0];

			float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 butenSize = { lineHeigth + 3.0f, lineHeigth };

			ImGui::PushID(label.c_str());
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columeWith);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("F", butenSize))
			{
				*values = resetValue;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1f", resetValue);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##F", values, 0.1f, min, max, "%.2f") || changes;
			ImGui::PopItemWidth();

			ImGui::Columns(1);
			ImGui::PopID();
			return changes;
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
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Asset::GetAssetTypeDragAndDropName(assetType).c_str() ))
				{
#if RY_EDITOR_ASSETMANGER_THREADE
					AssetManager::GetAssetAsync<T>(*(AssetHandle*)payload->Data, compontenItem);
#else
					*compontenItem = AssetManager::GetAsset<T>(*(AssetHandle*)payload->Data);
#endif
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

		template<typename T>
		static void DragDropButtenForAsset(Weak<T>* compontenItem, const char* buttenName, AssetType assetType)
		{
			ImGui::Button(buttenName, ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Asset::GetAssetTypeDragAndDropName(assetType).c_str()))
				{
#if RY_EDITOR_ASSETMANGER_THREADE
					AssetManager::GetAssetAsync<T>(*(AssetHandle*)payload->Data, compontenItem);
#else
					* compontenItem = AssetManager::GetAsset<T>(*(AssetHandle*)payload->Data);
#endif
				}
				ImGui::EndDragDropTarget();

			}

			
			if (compontenItem != nullptr)
			{
				Ref<T> item = compontenItem->lock();
				if(item != nullptr)
				{
					ImGui::SameLine();
					if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
					{
						item = nullptr;
						compontenItem->reset();
					}
				}
			}
			
		}

		static void DargDropTextureButten(Ref<Texture>* compontenItem)
		{
			DragDropButtenForAsset<Texture>(compontenItem, "Texture", AssetType::Texture2D);
		}

		static void DargDropTextureButten(Weak<Texture>* compontenItem)
		{
			DragDropButtenForAsset<Texture>(compontenItem, "Texture", AssetType::Texture2D);
		}

		static void DargDropShaderButten(Ref<Shader>* compontenItem)
		{
			DragDropButtenForAsset<Shader>(compontenItem, "Shader", AssetType::Shader);
		}


		static void DargDropFontButten(TextComponent& text)
		{
			bool pressed = ImGui::Button("Font", ImVec2(100.0f, 0.0f));
			

			if (ImGui::BeginDragDropTarget())
			{

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Asset::GetAssetTypeDragAndDropName(AssetType::TextFont).c_str()))
				{
					// TODO: make Import Class
					Ref<Project> project = Project::GetActive();
#if RY_EDITOR_ASSETMANGER_THREADE
					Ref<EditorAssetManegerThreade> editorAssetManager = project->GetEditorAssetManger();
					const AssetMetadata metadata = editorAssetManager->GetMetadata(*(AssetHandle*)payload->Data);
#else
					Ref<EditorAssetManager> editorAssetManager = project->GetEditorAssetManger();
					const AssetMetadata& metadata = editorAssetManager->GetMetadataConst(*(AssetHandle*)payload->Data);
#endif
					

					text.FontAsset = CreateRef<Font>(metadata.FilePath);
				
				}
				ImGui::EndDragDropTarget();

			}
			else if(pressed)
			{
				text.FontAsset = Font::GetDefault();
			}

			if (text.FontAsset != nullptr)
			{
				ImGui::SameLine();
				if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
				{
					text.FontAsset = nullptr;
				}

			}
		}

		template<typename T, typename UIFuction>
		static void DrawComponent(const std::string& name, Entity entity, UIFuction uiFunction)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap;
			if (entity.HasComponent<T>())
			{
				
				T& component = entity.GetComponent<T>();
				ImVec2 contenRegionAvablie = ImGui::GetContentRegionAvail();
				
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
				float linHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImGui::Separator();
				
				bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
				ImGui::PopStyleVar();

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
					uiFunction(component);
					ImGui::TreePop();
				}

				if (removeComponent)
					entity.RemoveComponent<T>();
			}
		}

		template<typename T>
		static void Materiel(Ref<T> materiel)
		{
			// static_assert(fasle);
			UniformDataBasic* uniforms = (UniformDataBasic*)materiel->GetUniformData();

			if (ImGui::ColorEdit3("Color", uniforms->Color, 0.1f))
				materiel->SetUniformValue({ ShaderDataType::Float3, "Color" }, uniforms->Color);
			if(DrawFloatControler("Alpha", &uniforms->Alpha, 1.0f, -0.1, 20.0f))
				materiel->SetUniformValue({ ShaderDataType::Float, "Alpha" }, &uniforms->Alpha);
			if(DrawFloatControler("Shinines", &uniforms->Shinines, 1.0f, -1.0, 20.0f))
				materiel->SetUniformValue({ ShaderDataType::Float, "Shinines" }, &uniforms->Shinines);
			if(DrawFloatControler("Specular", &uniforms->Specular, 1.0f, -1.0, 20.0f))
				materiel->SetUniformValue({ ShaderDataType::Float, "Specular" }, &uniforms->Specular);

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
		std::string& tag = entity.GetComponent<TagComponent>().Tag;
		UUID& id = entity.GetComponent<IDComponent>().ID;
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		RealtionShipComponent& realtionship = entity.GetComponent<RealtionShipComponent>();
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
				entiytChildrenDeleted = true;
			
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
#if RY_ENABLE_UTILS_REALTION_SHIP_COMP
			if(parent)
			{
				Entity enitytCilds = m_Context->GetEntitiyByUUID(realtionship.FirstID);
				DrawEntityNode(enitytCilds,false);
				UUID nextChild = enitytCilds.GetComponent<RealtionShipComponent>().NextID;
				UUID trueParentChild = entity.GetUUID();
				while (nextChild)
				{
					enitytCilds = m_Context->GetEntitiyByUUID(nextChild);
					DrawEntityNode(enitytCilds, false);
					nextChild = enitytCilds.GetComponent<RealtionShipComponent>().NextID;

				}

				ImGui::TreePop();

			}
#else
#endif
		}
		if(enitityCildeAdd)
		{
			
#if RY_ENABLE_UTILS_REALTION_SHIP_COMP
			Utils::AddChilrenToEntity(entity, m_Context, "Empty Childern Entiy");
			Utils::UpdateChilrenMatrixeCompentens(entity, m_Context);
#else
			entity.AddChildrenEntity("Empty Childern Entiy");
			entity.UpdateMatrix();
#endif
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
			std::string& tag = entity.GetComponent<TagComponent>().Tag;

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
			if(DisplayAddComponentEntry<CameraComponent>("Camera"))
				m_SelectionContext.AddComponent<ViewMatrixComponent>();
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
			{
				if(DisplayAddComponentEntry<DrirektionleLigthComponent>("Drirektionle-Ligth"))
					m_SelectionContext.AddComponent<ViewMatrixComponent>();
			}

			if (!m_SelectionContext.HasComponent<AmbientLigthComponent>() &&
				!m_SelectionContext.HasComponent<DrirektionleLigthComponent>() &&
				!m_SelectionContext.HasComponent<SpotLigthComponent>())
				DisplayAddComponentEntry<PointLigthComponent>("Point-Ligth");

			if (!m_SelectionContext.HasComponent<AmbientLigthComponent>() &&
				!m_SelectionContext.HasComponent<PointLigthComponent>() &&
				!m_SelectionContext.HasComponent<DrirektionleLigthComponent>())
				if (DisplayAddComponentEntry<SpotLigthComponent>("Spot-Ligth"))
					m_SelectionContext.AddComponent<ViewMatrixComponent>();
			DisplayAddComponentEntry<TextComponent>("Text");
			DisplayAddComponentEntry<ViewMatrixComponent>("View");
			ImGui::EndPopup();
		}
#if 0
		if (ImGui::BeginPopup("AddChildre-synComponent"))
		{
			DisplayAddComponentEntry<ScriptComponent>("Script");
			DisplayAddComponentEntry<CameraComponent>("Camera");
			if (!m_SelectionContext.HasComponent<MaterialComponent>() && !m_SelectionContext.HasComponent<GeomtryComponent>())
				DisplayAddComponentEntry<SpriteRendererComponent>("Sprite");

			if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
			{
				DisplayAddComponentEntry<MaterialComponent>("Material");

				if (!m_SelectionContext.HasComponent<MeshComponent>())
					DisplayAddComponentEntry<GeomtryComponent>("Geomtry");

				if (!m_SelectionContext.HasComponent<GeomtryComponent>())
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
			DisplayAddComponentEntry<TextComponent>("Text");
			ImGui::EndPopup();
		}
#endif // TODO: Children Add ore remove Componet Mode / target is a visuel option to add to alle childrens a componet and cahnge values from alle alle childrensat the same time.

		// Transform
		Utils::DrawComponent<TransformComponent>("Transform", entity, [&](TransformComponent& component)
		{
#if 0
			if (entity.HasComponent<DrirektionleLigthComponent>() && entity.HasComponent<ModelMatrixComponent>())
			{
				
				bool use = false;
				use = Utils::DrawVec3Controler("Center", component.Transaltion) || use;
				use = Utils::DrawVec3Controler("Eye", component.Rotation) || use;
				use = Utils::DrawVec3ControlerR("Up", component.Scale, glm::vec3(0.0f,1.0f,0.0f)) || use;

				if (use)
				{
					TransformComponent trasformC_copy = component;
					component.SetTransform(glm::lookAt(component.Rotation, component.Transaltion, component.Scale));
					entity.UpdateMatrix();
					component = trasformC_copy;
				}
			}
			else
			{
			}
#endif
			bool use = false;
			use = Utils::DrawVec3Controler("Translation", component.Transaltion) || use;
			glm::vec3 rotation = glm::degrees(component.Rotation);
			use = Utils::DrawVec3Controler("Rotation", rotation) || use;
			use = Utils::DrawVec3Controler("Scale", component.Scale, 1.0f) || use;

			if (use)
			{
				component.Rotation = glm::radians(rotation);
				entity.UpdateMatrix();
			}
			
		});

		// Camera
		Utils::DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
		{
			SceneCamera& camera = component.Camera;
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
			ImGui::Checkbox("ViewFustrum", &component.ViewFustrum);
		});

		// SpriteRenderer
		Utils::DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color), 0.1f);
			// Texture

			Utils::DargDropTextureButten(&component.Texture);

			ImGui::Checkbox("Render Single", &component.RenderSingle);
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
			Ref<Framebuffer>& frameBuffer = component.FrameBuffer;
					
			
			Utils::DragDropButtenForAsset<Framebuffer>(&component.FrameBuffer, "FrameBuffer", AssetType::Framebuffer);
			
			if(frameBuffer)
			{

				const FramebufferSpecification& fbspec = component.FrameBuffer->GetFramebufferSpecification();
				
				
				
				int delateNumber = -1;
				int colorAtachments = 0;
				size_t i = 0;
				for (const auto& attachment : fbspec.Attachments)
				{
					TextureFormat texFormat = attachment.TextureFormat;
					std::string_view format_view = EnumString::GetStringFromEnum<TextureFormat>(texFormat);
					
					ImGui::PushID(format_view.data() + ('0' + i));
					ImGui::Columns(2, "##FrameBuffer Textures", false);

					
					uint32_t textureID = 0;

					switch (texFormat)
					{
					case TextureFormat::Depth24Stencil8:
					{
						textureID = component.FrameBuffer->GetColorAttachmentRendererID(colorAtachments);
						break;
					}
					case TextureFormat::None: 
					{ 
						break;
					}
					default:
						textureID = component.FrameBuffer->GetDeathAttachmentRendererID();
						break;
					} 
				

					ImVec2 windowSize = ImGui::GetWindowSize();
					windowSize.x = windowSize.x * 0.92 - 110;
					ImGui::SetColumnWidth(0, windowSize.x+5);
					
					
					ImGui::SetColumnWidth(1,100);
					windowSize.y = windowSize.x / ((float)fbspec.Width / fbspec.Height);
					ImGui::Image((ImTextureID)textureID,  windowSize, ImVec2(0, 1), ImVec2(1, 0));
					
					ImGui::NextColumn();
					
					if (ImGui::Button(format_view.data(), { 100.0f , 0.0f}))
					{
					}

					if (ImGui::Button("Delete", ImVec2(100.0f, 0.0f)))
						delateNumber = i;						
					ImGui::NextColumn();
					ImGui::SameLine();
					ImGui::Columns(1);
					switch (texFormat)
					{
					case TextureFormat::Depth24Stencil8:
					case TextureFormat::Depth32FStencil8:
						ImGui::Text("On Death Attachment");
						break;
					case TextureFormat::None:
						break;
					default:
						ImGui::Text("On Color Attachment: %i", colorAtachments++);
					}
					ImGui::PopID();
					i++;
				}

				if (delateNumber != -1)
				{
					FramebufferSpecification fbspecNew = fbspec;
					fbspecNew.Attachments.Attachments.erase(fbspecNew.Attachments.begin()+delateNumber);
					component.FrameBuffer = Framebuffer::Create(fbspecNew);
				}
			}
		});

		// Mesh
		Utils::DrawComponent<MeshComponent>("Mesh", entity, [&](MeshComponent& component)
		{
			
			Ref<Model>& model = component.ModelR;
			ImGui::Button("Mesh", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Asset::GetAssetTypeDragAndDropName(AssetType::Model).c_str()))
				{
					ImGui::OpenPopup("Loade-Mesh");
					AssetHandle handle = *(AssetHandle*)payload->Data;
					component.ModelR = nullptr;
					AssetManager::GetAssetAsync<Model>(*(AssetHandle*)payload->Data, &component.ModelR);
					
				}
				ImGui::EndDragDropTarget();
			}
			
			if (component.ModelR &&ImGui::BeginPopup("Loade-Mesh"))
			{

				if (ImGui::MenuItem("Static-Mesh"))
				{

					
					UUID first = entity.GetComponent<RealtionShipComponent>().FirstID;
					if (first)
#if RY_ENABLE_UTILS_REALTION_SHIP_COMP
						Utils::DealteAllChilrenFromEntity(entity, m_Context, &m_SelectionContext);
#else
						entity.DestroyEntityChildrens();
#endif
					if (entity.HasComponent<StaticMeshComponent>())
					{
						StaticMeshComponent& smC = entity.GetComponent<StaticMeshComponent>();
						smC.ModelR = nullptr;
					}
					

					if (!entity.HasComponent<StaticMeshComponent>())
						entity.AddComponent<StaticMeshComponent>();
					StaticMeshComponent& staticMesh = entity.GetComponent<StaticMeshComponent>();
					staticMesh.ModelR = model;
					ModelMatrixComponent& parentMat4C = entity.GetComponent<ModelMatrixComponent>();

					const std::vector<MeshRootData>& modelData = model->GetRootDatas();
					staticMesh.LocaleMeshMatrix.reserve(modelData.size());
					
					for (const MeshRootData& meshData : modelData)
					{
						staticMesh.LocaleMeshMatrix.emplace_back(meshData.NodeMatrix);
						staticMesh.GlobleMeshMatrix.emplace_back(parentMat4C.Globle * meshData.NodeMatrix);
					}
				}

				if (ImGui::MenuItem("Dynamic-Mesh"))
				{
					
					
					UUID first = entity.GetComponent<RealtionShipComponent>().FirstID;
					if (first)
#if RY_ENABLE_UTILS_REALTION_SHIP_COMP
						Utils::DealteAllChilrenFromEntity(entity, m_Context, &m_SelectionContext);
#else
						entity.DestroyEntityChildrens();
#endif

					if (entity.HasComponent<StaticMeshComponent>())
						entity.RemoveComponent<StaticMeshComponent>();
					

					if (!entity.HasComponent<ModelMatrixComponent>())
						entity.AddComponent<ModelMatrixComponent>();
					ModelMatrixComponent& parentMat4C = entity.GetComponent<ModelMatrixComponent>();

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
#if RY_ENABLE_UTILS_REALTION_SHIP_COMP
						Entity childeEntity = Utils::AddChilrenToEntity(entity, m_Context, rootData.NodeName);
#else
						Entity childeEntity = entity.AddChildrenEntity(rootData.NodeName);
#endif

						childeEntity.AddComponent<DynamicMeshComponent>();
						childeEntity.AddComponent<MaterialComponent>();
						
						if (!childeEntity.HasComponent<ModelMatrixComponent>())
							childeEntity.AddComponent<ModelMatrixComponent>();
						ModelMatrixComponent& childMat4C = childeEntity.GetComponent<ModelMatrixComponent>();
						childMat4C.Locale = rootData.NodeMatrix;
						childMat4C.Globle = parentMat4C.Locale * rootData.NodeMatrix;

						if (!childeEntity.HasComponent<TransformComponent>())
							childeEntity.AddComponent<TransformComponent>();
						TransformComponent& transformC = childeEntity.GetComponent<TransformComponent>();
						transformC.SetTransform(childMat4C.Locale);

						if (!childeEntity.HasComponent<DynamicMeshComponent>())
							childeEntity.AddComponent<DynamicMeshComponent>();
						DynamicMeshComponent& dynamicMeshC = childeEntity.GetComponent<DynamicMeshComponent>();
						dynamicMeshC.MeshR = mesh;

						if (!childeEntity.HasComponent<MaterialComponent>())
							childeEntity.AddComponent<MaterialComponent>();

						
					}
					entity.UpdateMatrix();
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
#if RY_ENABLE_UTILS_REALTION_SHIP_COMP
							Utils::DealteAllChilrenFromEntity(entity, m_Context, &m_SelectionContext);
#else
							entity.DestroyEntityChildrens();
#endif
					}
					if (entity.HasComponent<StaticMeshComponent>())
						entity.RemoveComponent<StaticMeshComponent>();
					component.ModelR = nullptr;
				}
				else
				{
					if (entity.HasComponent<StaticMeshComponent>())
					{
						StaticMeshComponent& staticMeshCom = entity.GetComponent<StaticMeshComponent>();
						Ref<Material> materiel = staticMeshCom.ModelR->GetMesh(0)->GetMateriel();
						Utils::Materiel<Material>(materiel);
					}
				}
				

			}
			
		});

		// Ambient-Ligth
		Utils::DrawComponent<AmbientLigthComponent>("Ambient-Ligth", entity, [](AmbientLigthComponent& component)
			{
				ImGui::ColorEdit3("Color: ", glm::value_ptr(component.Color));
				ImGui::DragFloat("Intensitie: ", &component.Intensitie, 0.01f, -1.0f, 10.0f, "%.2f");
			});

		// Drirektionle-Ligth
		Utils::DrawComponent<DrirektionleLigthComponent>("Drirektionle-Ligth", entity, [&](DrirektionleLigthComponent& component)
			{
				ImGui::ColorEdit3("Color: ", glm::value_ptr(component.Color));
				ImGui::DragFloat("Intensitie: ", &component.Intensitie, 0.01f, -1.0f, 10.0f, "%.2f");
				
				
#if 0
				if(entity.HasComponent<ModelMatrixComponent>() && entity.HasComponent<TransformComponent>())
				{
					ModelMatrixComponent& mat4C = entity.GetComponent<ModelMatrixComponent>();

					float sizeOrth = component.CameraLigth.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &sizeOrth))
						component.CameraLigth.SetOrthograficSize(sizeOrth);

					float nearOrth = component.CameraLigth.GetOrthographicNearClipe();
					if (ImGui::DragFloat("Near", &nearOrth))
						component.CameraLigth.SetOrthograficNearClipe(nearOrth);

					float farOrth = component.CameraLigth.GetOrthographicFarClipe();
					if (ImGui::DragFloat("Far", &farOrth))
						component.CameraLigth.SetOrthograficFarClipe(farOrth);

					if (component.ShadowFrameBuffer)
					{
						 const glm::uvec2& sizeOrig = component.ShadowFrameBuffer->GetFrambufferSize();
						 glm::ivec2 size = (glm::ivec2)sizeOrig;
						 if(ImGui::DragInt2("size", glm::value_ptr(size), 1.0,0, 0, "%i"))
						 {
							 component.ShadowFrameBuffer->Resize(size.x, size.y);
							 component.CameraLigth.SetViewPortSize(size.x, size.y);
						 }
					}
				}
#endif
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

		// Text
		Utils::DrawComponent<TextComponent>("Text", entity, [](TextComponent& component)
		{
			static char bufferText[2048];
			strcpy(bufferText, component.TextString.c_str());
			if (ImGui::InputTextMultiline("Text String: ", bufferText, 2048))
				component.TextString = bufferText;
			ImGui::ColorEdit4("Color: ", glm::value_ptr(component.Color));
			
			ImGui::DragFloat("Kerning: ", &component.Kerning, 0.025f);
			ImGui::DragFloat("Line Spacing: ", &component.LineSpacing, 0.01f, -1.0f, 10.0f, "%.2f");

			Utils::DargDropFontButten(component);
		});

		// View
		Utils::DrawComponent<ViewMatrixComponent>("View Matrix", entity, [](ViewMatrixComponent& component) {});

		// Static
		Utils::DrawComponent<StaticMeshComponent>("Static-Mesh", entity, [](StaticMeshComponent& component) {});

		// Dynamic
		Utils::DrawComponent<DynamicMeshComponent>("Dynamic-Mesh", entity, [](DynamicMeshComponent& component) {});
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
				m_Context->CreateEntity(name.c_str() + ('0' + (char)entityNumber));
			else
				m_Context->CreateEntity("Empty Entiy");
		}
		else
		{
			CreateEntity(name.c_str() + ('0' + (char)entityNumber), entityNumber++);
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
#if RY_ENABLE_UTILS_REALTION_SHIP_COMP
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
			ScriptComponent component = entity.GetComponent<ScriptComponent>();
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
	bool SceneHierachyPannel::DisplayAddComponentEntry(const std::string& name)
	{
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(name.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
				return true;
			}
		}
		return false;
	}

}
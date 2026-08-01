#include "rypch.h"
#include "SceneHierachyPannel.h"

#include <Rynex/Scene/Components.h>
#if RY_SCRIPTING_HAZEL
#include <Rynex/Scripting/HazelScripting/ScriptEngine.h>
#else
#include <Rynex/Scripting/Mono/ScriptingEngine.h>
#endif
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Utils/EnumString.h>
#include <Rynex/Core/Input.h>


#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <magic_enum.hpp>
#include <Rynex/Renderer/RenderProxy/StaticeRenderProxys.h>

#define RY_ENABLE_UTILS_REALTION_SHIP_COMP 0
#define RY_SCENE_HIERACHY_PANNEL_DRAG_AND_DROP 0

namespace Rynex {

	namespace Utils {
	
		static void ConvertNummberToChar(uint32_t number, char* charBuffer, uint32_t bufferSize)
		{
			uint32_t index = 0;
			do {
				*--charBuffer = static_cast<char>('0' + number % 10);
				number /= 10;
				index++;
			} while (number != 0 && index < bufferSize);
		}
		

		static bool DrawIvec4ControlerR(const std::string& label, glm::ivec4& values, glm::ivec4&& resetValue, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			ImFont* boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());
			ImGui::Columns(2, label.c_str(), false);
			ImGui::SetColumnWidth(0, columeWith);
			ImGui::Text(label.c_str());
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(magic_enum::enum_name(ShaderDataType::Float4).data());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
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
				ImGui::SetTooltip("Reset Value %i", resetValue.x);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragInt("##X", &values.x, 1.0f, 0.0f, 0.0f, "%i") || changes;
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
				ImGui::SetTooltip("Reset Value %i", resetValue.y);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragInt("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%i") || changes;
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
				ImGui::SetTooltip("Reset Value %i", resetValue.z);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragInt("##Z", &values.z, 1.0f, 0.0f, 0.0f, "%i") || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.85f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.8f, 0.95f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.85f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("W", butenSize))
			{
				values.w = resetValue.w;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %i", resetValue.w);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragInt("##W", &values.w, 1.0f, 0.0f, 0.0f, "%i") || changes;
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PopStyleVar();

			ImGui::Columns(1);
			ImGui::PopID();
			return changes;
		}


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
				ImGui::SetTooltip(magic_enum::enum_name(ShaderDataType::Float4).data());
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

		static bool DrawVec4ControlerR(const std::string& label, glm::vec4& values, glm::vec4&& resetValue, float columeWith = 80.0f)
		{
			bool changes = false;
			ImGuiIO& io = ImGui::GetIO();
			ImFont* boldFont = io.Fonts->Fonts[0];

			ImGui::PushID(label.c_str());
			ImGui::Columns(2, label.c_str(), false);
			ImGui::SetColumnWidth(0, columeWith);
			ImGui::Text(label.c_str());
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(magic_enum::enum_name(ShaderDataType::Float4).data());
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
				values.w = resetValue.w;
				changes = true;
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Reset Value %.1ff", resetValue.w);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			changes = ImGui::DragFloat("##W", &values.w, 0.1f, 0.0f, 0.0f, "%.2f") || changes;
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
				ImGui::SetTooltip(magic_enum::enum_name(ShaderDataType::Float3).data());
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
				ImGui::SetTooltip(magic_enum::enum_name(ShaderDataType::Float4).data());
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
				ImGui::SetTooltip(magic_enum::enum_name(ShaderDataType::Float2).data());
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
#if RY_SCENE_HIERACHY_PANNEL_DRAG_AND_DROP
		template<typename T>
		static bool DragDropButtenForAsset(Ref<T> *compontenItem, const char* buttenName, AssetType assetType)
		{
			bool result = false;
			ImGui::Button(buttenName, ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Asset::GetAssetTypeDragAndDropName(assetType).c_str() ))
				{


#if RY_EDITOR_ASSETMANGER_THREADE
					AssetManager::GetAssetAsync<T>(*(AssetHandle*)payload->Data, compontenItem);
					result = true;
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
				{
					*compontenItem = nullptr;
					result = true;
				}
			}
			return result;
		}
#else

		template<typename Comp>
		static bool DragDropButtenForAssetMeshStatic(Entity entity, Ref<MeshStatic>& compontenItem, const char* buttenName, AssetType assetType)
		{
			bool result = false;
			ImGui::Button(buttenName, ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Asset::GetAssetTypeDragAndDropName(assetType).c_str()))
				{
					const AssetHandle* handlePtr = reinterpret_cast<const AssetHandle*>(payload->Data);
					AssetHandle handle = static_cast<AssetHandle>(*handlePtr);
#if 1
					const Scene* scenePtrConst = entity.GetScenePtr();
					Scene* scenePtr = const_cast<Scene*>(scenePtrConst);

					int entityID = entity.GetEntityHandle();
					Ref<Scene> sceneRef = Scene::GetRefInPlace(scenePtr);
					std::function<void(Ref<MeshStatic>, Ref<Scene>, int)> func = Entity::OnAssetLoded<Comp, MeshStatic>;

					Ref<LodePromisType<MeshStatic, Scene, int>> promis = CreateRef<LodePromisType<MeshStatic, Scene, int>>(sceneRef, func, entityID);
					AssetManager::GetAssetAsyncPromis<MeshStatic, Scene, int>(handle, promis);
#else
					compontenItem = AssetManager::GetAsset<MeshStatic>(handle);
#endif
				}
				ImGui::EndDragDropTarget();

			}
			if (compontenItem != nullptr)
			{
				ImGui::SameLine();
				if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
				{
					compontenItem = nullptr;
					result = true;
				}
			}
			return result;
		}

		template<typename T, typename N>
		static bool DragDropButtenForAsset(Entity entity, Weak<T>& compontenItem, const char* buttenName, AssetType assetType)
		{
			bool result = false;
			ImGui::Button(buttenName, ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Asset::GetAssetTypeDragAndDropName(assetType).c_str()))
				{
					const AssetHandle* handlePtr = reinterpret_cast<const AssetHandle*>(payload->Data);
					AssetHandle handle = static_cast<AssetHandle>(*handlePtr);

					{
						Ref<Project> project = Project::GetActive();
						Ref<EditorAssetManegerThreade> assetManger = project->GetEditorAssetManger();
						if (!assetManger->IsAssetHandleValid(handle))
						{
							uint64_t handleValue = handle;
							RY_CORE_ERROR("Handle {} is not in AssetManger", handleValue);
						}
					}
					const Scene* scenePtrConst = entity.GetScenePtr();
					Scene* scenePtr = (Scene*)scenePtrConst;

					int entityID = entity.GetEntityHandle();
					Ref<Scene> sceneRef = Scene::GetRefInPlace(scenePtr);
					std::function<void(Ref<T>, Ref<Scene>, int)> func = Entity::OnAssetLoded<N, T>;

					Ref<LodePromisType<T, Scene, int>> promis = CreateRef<LodePromisType<T, Scene, int>>(sceneRef, func, entityID);
					AssetManager::GetAssetAsyncPromis<T, Scene, int>(handle, promis);
				}
				ImGui::EndDragDropTarget();

			}
			if (Ref<T> refComponteItem = compontenItem.lock())
			{
				ImGui::SameLine();
				if (ImGui::Button("Dealte", ImVec2(100.0f, 0.0f)))
				{
					refComponteItem = nullptr;
					compontenItem.reset();
					result = true;
				}
			}
			return result;
		}

#endif
#if RY_SCENE_HIERACHY_PANNEL_DRAG_AND_DROP
		template<typename T>
		static bool DragDropButtenForAsset(Weak<T>* compontenItem, const char* buttenName, AssetType assetType)
		{
			bool result = false;
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
					result = true;
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
						result = true;
					}
				}
			}
			return result;
			
		}

		static bool DargDropTextureButten(Ref<Texture>* compontenItem)
		{
			DragDropButtenForAsset<Texture>(compontenItem, "Texture", AssetType::Texture2D);
		}


		static bool DargDropTextureButten(Weak<Texture>* compontenItem)
		{
			return DragDropButtenForAsset<Texture>(compontenItem, "Texture", AssetType::Texture2D);
		}

		static void DargDropShaderButten(Ref<Shader>* compontenItem)
		{
			DragDropButtenForAsset<Shader>(compontenItem, "Shader", AssetType::Shader);
		}


		static void DargDropShaderButten(Ref<MeshStatic>* compontenItem)
		{
			
			Ref<MeshSource>* meshSource = nullptr;
			DragDropButtenForAsset<MeshSource>(meshSource, "MeshStatic", AssetType::MeshSource);


			if (nullptr == compontenItem)
			{

			}
		}
#else
		template<typename Comp>
		static bool DargDropTextureButten(Entity e, Weak<Texture>& tex)
		{
			return DragDropButtenForAsset<Texture, Comp>(e, tex, "Texture", AssetType::Texture2D);
		}
#endif
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

		template<typename Comp>
		static bool DargDropMeshStaticButten(Entity e, Ref<MeshStatic>& meshStatic)
		{
			return DragDropButtenForAssetMeshStatic<Comp>(e, meshStatic, "Mesh-Static", AssetType::MeshStatic);
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
				const void* ptr = (void*)typeid(T).hash_code();
				bool open = ImGui::TreeNodeEx(ptr, treeNodeFlags, name.c_str());
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
					
					ImGui::PushItemWidth((contenRegionAvablie.x / 3.0f) * 2.0f);
					std::string_view typeNameStrView = typeid(T).name();
					std::string idName = "##" + std::string(typeNameStrView.data(), typeNameStrView.size());
					ImGui::PushID(idName.c_str());
					uiFunction(entity, component);
					ImGui::PopID();

					ImGui::PushItemWidth(-1);
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

	void SceneHierachyPannel::OnDetache()
	{
		m_Context.reset();
		m_Context = nullptr;
	}

	void SceneHierachyPannel::SetContext(const Ref<Scene>& context)
	{
		if(m_Context != nullptr)
		{
			TestSubmitStaticProxyLocal();

			m_Context->OnDisconectToRenderer();
			m_SceneList.clear();
		}
		
		RY_CORE_ASSERT(nullptr != context, "nullptr was set!");
		m_Context = context;
		m_Context->OnConectToRenderer();
		TestSubmitStaticProxyLocal();
		m_SelectionContext = {};
		Renderer3D::ResetMeshObject();
		m_Context->m_Registery.each([this](entt::entity e) {
			Entity entity = Entity(e, m_Context.get());
			m_SceneList.emplace_back(entity);
		});
	}

	uint32_t SceneHierachyPannel::DrawEntityNode(Entity entity, uint32_t hirachIndex, bool normale)
	{
		std::string& tag = entity.GetComponent<TagComponent>().Tag;
		UUID& id = entity.GetComponent<IDComponent>().ID;
		
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		// flags |= ImGuiTreeNodeFlags_Framed;
		RealtionShipUUIDComponent& realtionship = entity.GetComponent<RealtionShipUUIDComponent>();
		bool parent = realtionship.childrens.size() != 0ull,
			cildern = realtionship.parent != 0ull;
#if 0
		if (((cildern && !parent) || (cildern && parent)) && normale)
			return;
#else
		if (cildern && normale)
			return hirachIndex;
#if 1
		if (!entity.IsVisbble())
			return hirachIndex;
#endif
#endif
		hirachIndex++;
		if (!parent)
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		
		std::string idName = (tag + std::to_string(id));

		ImGui::PushID(idName.c_str());

		
		// ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.25, 0.25 });
		bool opende = ImGui::TreeNodeEx(&id, flags, tag.c_str());
		// ImGui::PopStyleVar();

		ImGuiDragDropFlags flagsDragDrop = ImGuiDragDropFlags_None;

		if (ImGui::BeginDragDropSource(flagsDragDrop))
		{
			uint32_t index = entity.GetEntityIndexHandle();
			ImGui::SetDragDropPayload(s_SceneHierachieDragAndDropRelationShipMoveChar, &index, sizeof(uint32_t));

			ImGui::Text("Drag/and drop Enity: %s", tag.c_str());


			ImGui::EndDragDropSource();
		}
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(s_SceneHierachieDragAndDropRelationShipMoveChar, flagsDragDrop))
			{
				RY_CORE_ASSERT(payload->DataSize == sizeof(uint32_t), "not Expexted Size!");
				const uint32_t* moveEntityIndexHandleToRealtionShipPtr = reinterpret_cast<const uint32_t*>(payload->Data);
				uint32_t moveEntityIndexHandleToRealtionShip = *moveEntityIndexHandleToRealtionShipPtr;
				entt::id_type entityType = static_cast<entt::id_type>(moveEntityIndexHandleToRealtionShip);
				entt::entity entityEntt = static_cast<entt::entity>(entityType);
				Entity moveEntityToRealtionShip = Entity(entityEntt, m_Context.get());
				if(moveEntityToRealtionShip.GetEntityIndexHandle() != entity.GetEntityIndexHandle())
				{
					std::function<void()> func = nullptr;
					if (entity.IsVaild())
					{
						func = [entity, moveEntityToRealtionShip]() {
							Entity toParent = entity;
							if (moveEntityToRealtionShip.IsNotVaild())
								return;
							if (toParent.IsNotVaild())
								return;
							Entity parentMoveEntity = moveEntityToRealtionShip.GetParentEntity();
							if (parentMoveEntity.IsVaild())
								parentMoveEntity.RemoveFromChildrens(moveEntityToRealtionShip);

							toParent.AddToChildrens(moveEntityToRealtionShip);
							};
					}
					else
					{
						func = [moveEntityToRealtionShip]() {

							if (moveEntityToRealtionShip.IsNotVaild())
								return;
							Entity parentMoveEntity = moveEntityToRealtionShip.GetParentEntity();
							if (parentMoveEntity.IsVaild())
								parentMoveEntity.RemoveFromChildrens(moveEntityToRealtionShip);
							};

					}
					m_ExecuteFunc.emplace_back(func);
				}

				


			}			
			ImGui::EndDragDropTarget();
		}
		

		if (ImGui::IsItemClicked() && Input::IsKeyPressed(Key::LeftShift) && m_SelectionContext != Entity())
		{
			m_SelectionContextEndeMarker = entity;
		}
		else if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
			m_SelectionContextEndeMarker = Entity();
		}
		

		if (ImGui::BeginPopupContextItem(idName.c_str()))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_ExecuteFunc.emplace_back([entity, this]()
					{
						CreateEntity("Empty Entity");
					});
			}
			
			if (ImGui::MenuItem("Add Childe Entity"))
			{
				m_ExecuteFunc.emplace_back([entity, this]()
					{
						AddChiledEntity(entity);
					});
			}

			if (ImGui::MenuItem("Create Copy"))
			{
				m_ExecuteFunc.emplace_back([entity, this]() 
					{
						CopyEntity(entity); 
					});
			}

			if (ImGui::MenuItem("Delete Entity"))
			{
				m_ExecuteFunc.emplace_back([entity, this]()
				{
					RemoveEntity(entity);
				});
			}

			if (ImGui::MenuItem("Delete Entity + Children"))
			{
				m_ExecuteFunc.emplace_back([entity, this]()
				{
					RemoveChiledEntity(entity);
				});
			}

			ImGui::EndPopup();
		}
		else if (m_SelectionContext == entity)
		{
			if (Input::IsKeyPressed(Key::LeftShift) && Input::IsKeyPressed(Key::Delete))
			{
				m_ExecuteFunc.emplace_back([this]()
					{
						RemoveChiledEntity(m_SelectionContext);
					});
			}
			else if (Input::IsKeyPressed(Key::Delete))
			{
				m_ExecuteFunc.emplace_back([this]()
					{
						RemoveEntity(m_SelectionContext);
					});
			}
		}

		if (opende)
		{

			if (parent)
			{ 
				std::vector<UUID>& childrens = entity.GetChildrens();
				uint32_t size = childrens.size();
				RY_CORE_ASSERT(size != 0);
				for (UUID childID : childrens)
				{
					RY_CORE_ASSERT(size == childrens.size());
					Entity enitytCilds = m_Context->GetEntitiyByUUID(childID);
					hirachIndex = DrawEntityNode(enitytCilds, hirachIndex, false);
				}
				ImGui::TreePop();
			}

		}

		static bool lastTimeHelped = false;
		if (ImGui::IsDragDropActive() && (!ImGui::IsDragDropPayloadBeingAccepted() || lastTimeHelped))
		{
			lastTimeHelped = true;
			constexpr float offsetValue = 8.0f;
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			float contentPosY = window->DC.CursorPos.y;
			float mousePosY = ImGui::GetMousePos().y;
			if(contentPosY - offsetValue < mousePosY && contentPosY + offsetValue > mousePosY)
			{
				ImVec2 region = ImGui::GetContentRegionAvail();
				region.y = 3.0f;
				ImGui::InvisibleButton("global_drop_zone", region);

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(s_SceneHierachieDragAndDropRelationShipMoveChar))
					{
						RY_CORE_ASSERT(payload->DataSize == sizeof(uint32_t), "not Expexted Size!");
						const uint32_t* moveEntityIndexHandleToRealtionShipPtr = reinterpret_cast<const uint32_t*>(payload->Data);
						uint32_t moveEntityIndexHandleToRealtionShip = *moveEntityIndexHandleToRealtionShipPtr;
						entt::id_type entityType = static_cast<entt::id_type>(moveEntityIndexHandleToRealtionShip);
						entt::entity entityEntt = static_cast<entt::entity>(entityType);
						Entity moveEntityToRealtionShip = Entity(entityEntt, m_Context.get());
						std::function<void()> func = [moveEntityToRealtionShip]() {
							if (moveEntityToRealtionShip.IsNotVaild())
								return;
							Entity parentMoveEntity = moveEntityToRealtionShip.GetParentEntity();
							if (parentMoveEntity.IsVaild())
								parentMoveEntity.RemoveFromChildrens(moveEntityToRealtionShip);
							};
						m_ExecuteFunc.emplace_back(func);

					}

					ImGui::EndDragDropTarget();
				}
			}
		}
		else
		{
			lastTimeHelped = false;
		}

		ImGui::PopID();


		return hirachIndex;
	}

	void SceneHierachyPannel::DrawComponents(Entity entity)
	{
		

		ImGui::Checkbox("Enable Materiel Mesh Config", &m_MeshConfig);
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
			DisplayAddComponentEntry<CameraComponent>("Camera");
				
			if(!m_SelectionContext.HasComponent<MaterialComponent>() && !m_SelectionContext.HasComponent<GeomtryComponent>())
				DisplayAddComponentEntry<SpriteRendererComponent>("Sprite");

			if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
			{
				DisplayAddComponentEntry<MaterialComponent>("Material");

			}
			
			DisplayAddComponentEntry<FrameBufferComponent>("FrameBuffer");

			

			if (!m_SelectionContext.HasComponent<PointLigthComponent>() &&
				!m_SelectionContext.HasComponent<SpotLigthComponent>())
			{
				DisplayAddComponentEntry<DrirectionleLigthComponent>("Drirektionle-Ligth");
			}

			if (!m_SelectionContext.HasComponent<DrirectionleLigthComponent>() &&
				!m_SelectionContext.HasComponent<SpotLigthComponent>())
				DisplayAddComponentEntry<PointLigthComponent>("Point-Ligth");

			if (!m_SelectionContext.HasComponent<PointLigthComponent>() &&
				!m_SelectionContext.HasComponent<DrirectionleLigthComponent>())
				DisplayAddComponentEntry<SpotLigthComponent>("Spot-Ligth");

			DisplayAddComponentEntry<TextComponent>("Text");
			DisplayAddComponentEntry<ViewMatrixComponent>("View");

			DisplayAddComponentEntry<ModelMangerComponent>("Static-Mesh");
			DisplayAddComponentEntry<StaticMeshComponent>("Static-Single-Mesh");
			DisplayAddComponentEntry<RenderTargetComponent>("RenderTarget");

			ImGui::EndPopup();
		}

		Utils::DrawComponent<TransformComponent>("Transform", entity, &SceneHierachyPannel::ComponentTransformGUI);
		Utils::DrawComponent<ModelMatrixComponent>("Matrix", entity, &SceneHierachyPannel::ComponentModelMatrixGUI);
		Utils::DrawComponent<CameraComponent>("Camera", entity, &SceneHierachyPannel::ComponentCameraGUI);
		Utils::DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, &SceneHierachyPannel::ComponentSpriteRendererGUI);
		Utils::DrawComponent<ScriptComponent>("Script", entity, &SceneHierachyPannel::ComponentScriptGUI);
		Utils::DrawComponent<FrameBufferComponent>("FrameBuffer", entity, &SceneHierachyPannel::ComponentFrameBufferGUI);
		Utils::DrawComponent<DrirectionleLigthComponent>("Drirektionle-Ligth", entity, &SceneHierachyPannel::ComponentDrirektionleLigthGUI);
		Utils::DrawComponent<PointLigthComponent>("Point-Ligth", entity, &SceneHierachyPannel::ComponentPointLigthGUI);
		Utils::DrawComponent<SpotLigthComponent>("Spot-Ligth", entity, &SceneHierachyPannel::ComponentSpotLigthGUI);
		Utils::DrawComponent<TextComponent>("Text", entity, &SceneHierachyPannel::ComponentTextGUI);
		Utils::DrawComponent<ViewMatrixComponent>("View Matrix", entity, &SceneHierachyPannel::ComponentViewMatrixGUI);
		Utils::DrawComponent<ModelMangerComponent>("Static-Mesh", entity, &SceneHierachyPannel::ComponentStaticMeshGUI);
		Utils::DrawComponent<StaticMeshComponent>("Static-Single-Mesh", entity, &SceneHierachyPannel::ComponentStaticSingleMeshGUI);
		Utils::DrawComponent<DynamicMeshComponent>("Dynamic-Mesh", entity, &SceneHierachyPannel::ComponentDynamicMeshGUI);
		Utils::DrawComponent<RenderTargetComponent>("RenderTarget", entity, &SceneHierachyPannel::ComponentRenderTargetGUI);

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
		if (m_SelectionContext.GetScenePtr() != m_Context.get())
			m_SelectionContext = {};


		if(m_WindowSceneHierachyOpen)
		{
			ImGui::Begin("Scene Hierachy", &m_WindowSceneHierachyOpen);

			// loop thorw all Entity
			uint32_t hirachyIndex = 0;
#if 1
			


			m_Context->m_Registery.each([&](entt::entity entityID)
			{
				Entity entity{ entityID, m_Context.get() };
				hirachyIndex = DrawEntityNode(entity, hirachyIndex);
			});
			
#else
			for (Entity entity : m_SceneList)
			{
				hirachyIndex = DrawEntityNode(entity, hirachyIndex);
			}
#endif
			
			EcexuterDeleteing();
			

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = Entity();

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
	
		m_Context->CreateEntity("Empty Entiy");
		
	}

	void SceneHierachyPannel::DeleteEntity(Entity entitiy, bool childeren)
	{
		m_EntityDelete.emplace_back(entitiy, childeren);
	}

	void SceneHierachyPannel::EcexuterDeleteing()
	{
		for (EntityFunc& func : m_ExecuteFunc)
		{
			func();
		}
		m_ExecuteFunc.clear();
	

		m_EntityDelete.clear();
		if (!m_SelectionContext)
			m_SelectionContext = {};
	}

	void SceneHierachyPannel::TestSubmitStaticProxyLocal()
	{
		return;

		EnttRender3DStaticModelView view3dStaticMesh = m_Context->m_Registery.view<ModelMatrixComponent, ModelMangerComponent>();
		StaticeRenderProxys staticRenderProxySystem;
		int countStoppAdd = 0;
		int* countStoppAddPtr = &countStoppAdd;
		view3dStaticMesh.each([&staticRenderProxySystem, countStoppAddPtr](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{
				
				if (nullptr == meshCompC.meshStatic || 2 < (*countStoppAddPtr) )
					return;

				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);
				const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshDataVec = meshCompC.meshStatic->GetSingleObjectMesDataVec();
				uint32_t subMesh = 0;
				for (const MeshStatic::SingleObjectMeshData& singleMeshData : singleObjectMeshDataVec)
				{
					glm::mat4 model = transformC.Globle * singleMeshData.LocaleCildrenMatrix;
					const Ref<MeshSingle>& meshSingle = singleMeshData._MeshSingle;
					const Ref<Material>& materiel = singleMeshData._Material;
					staticRenderProxySystem.Add(enitityID, subMesh, meshSingle, materiel, model);
					subMesh++;
					
				}
				(*countStoppAddPtr)++;
				
			});
		staticRenderProxySystem.RenderProxysMainGenarte();
		staticRenderProxySystem.RenderProxysMainSubmiteDrawList();
		countStoppAdd = 0;
		view3dStaticMesh.each([&staticRenderProxySystem, countStoppAddPtr](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{

				if (nullptr == meshCompC.meshStatic || 2 >= (*countStoppAddPtr))
				{
					(*countStoppAddPtr)++;
					return;
				}

				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);
				const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshDataVec = meshCompC.meshStatic->GetSingleObjectMesDataVec();
				uint32_t subMesh = 0;
				for (const MeshStatic::SingleObjectMeshData& singleMeshData : singleObjectMeshDataVec)
				{
					glm::mat4 model = transformC.Globle * singleMeshData.LocaleCildrenMatrix;
					const Ref<MeshSingle>& meshSingle = singleMeshData._MeshSingle;
					const Ref<Material>& materiel = singleMeshData._Material;
					staticRenderProxySystem.Add(enitityID, subMesh, meshSingle, materiel, model);
					subMesh++;

				}

			});
		staticRenderProxySystem.RenderProxysMainGenarte();
		staticRenderProxySystem.RenderProxysMainSubmiteDrawList();

		view3dStaticMesh.each([&staticRenderProxySystem](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{
				if (nullptr == meshCompC.meshStatic)
					return;

				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);
				const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshDataVec = meshCompC.meshStatic->GetSingleObjectMesDataVec();
				uint32_t subMesh = 0;
				for (const MeshStatic::SingleObjectMeshData& singleMeshData : singleObjectMeshDataVec)
				{
					glm::mat4 model = transformC.Globle * singleMeshData.LocaleCildrenMatrix;
					staticRenderProxySystem.UpdateTrasform(enitityID, subMesh, model);
					subMesh++;
				}
				
			});
		staticRenderProxySystem.RenderProxysMainGenarte();
		staticRenderProxySystem.RenderProxysMainSubmiteDrawList();

		view3dStaticMesh.each([&staticRenderProxySystem](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{
				if (nullptr == meshCompC.meshStatic)
					return;

				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);
				staticRenderProxySystem.Remove(enitityID);
			});

		view3dStaticMesh.each([&staticRenderProxySystem](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{
				if (nullptr == meshCompC.meshStatic)
					return;

				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);
				const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshDataVec = meshCompC.meshStatic->GetSingleObjectMesDataVec();
				uint32_t subMesh =0;
				for (const MeshStatic::SingleObjectMeshData& singleMeshData : singleObjectMeshDataVec)
				{
					glm::mat4 model = transformC.Globle * singleMeshData.LocaleCildrenMatrix;
					const Ref<MeshSingle>& meshSingle = singleMeshData._MeshSingle;
					const Ref<Material>& materiel = singleMeshData._Material;
					staticRenderProxySystem.Add(enitityID, subMesh, meshSingle, materiel, model);
					subMesh++;

				}

			});
		staticRenderProxySystem.RenderProxysMainGenarte();
		staticRenderProxySystem.RenderProxysMainSubmiteDrawList();
		view3dStaticMesh.each([&staticRenderProxySystem](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{
				

				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);
				
				staticRenderProxySystem.Remove(enitityID);

			}
		);
		staticRenderProxySystem.RenderProxysMainGenarte();
		staticRenderProxySystem.RenderProxysMainSubmiteDrawList();

		view3dStaticMesh.each([&staticRenderProxySystem](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{
				if (nullptr == meshCompC.meshStatic)
					return;

				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);
				const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshDataVec = meshCompC.meshStatic->GetSingleObjectMesDataVec();
				uint32_t subMesh = 0;
				for (const MeshStatic::SingleObjectMeshData& singleMeshData : singleObjectMeshDataVec)
				{
					glm::mat4 model = transformC.Globle * singleMeshData.LocaleCildrenMatrix;
					const Ref<MeshSingle>& meshSingle = singleMeshData._MeshSingle;
					const Ref<Material>& materiel = singleMeshData._Material;
					staticRenderProxySystem.Add(enitityID, subMesh, meshSingle, materiel, model);
					subMesh++;
				}

			});
		staticRenderProxySystem.Clear();

		view3dStaticMesh.each([&staticRenderProxySystem](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{
				if (nullptr == meshCompC.meshStatic)
					return;

				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);
				const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshDataVec = meshCompC.meshStatic->GetSingleObjectMesDataVec();
				uint32_t subMesh = 0;
				for (const MeshStatic::SingleObjectMeshData& singleMeshData : singleObjectMeshDataVec)
				{
					glm::mat4 model = transformC.Globle * singleMeshData.LocaleCildrenMatrix;
					uint32_t subMesh = singleMeshData.LocaleIndexMesh;
					const Ref<MeshSingle>& meshSingle = singleMeshData._MeshSingle;
					const Ref<Material>& materiel = singleMeshData._Material;
					staticRenderProxySystem.Add(enitityID, subMesh, meshSingle, materiel, model);
					subMesh++;
				}

			});

		
	}

	void SceneHierachyPannel::SwapEntitys(Entity a, Entity b)
	{
		bool aNotVaild = a.IsNotVaild();
		bool bNotVaild = b.IsNotVaild();
		bool aSceneNotEqual = a.GetScene() != m_Context;
		bool bSceneNotEqual = b.GetScene() != m_Context;

		if (aNotVaild || bNotVaild || aSceneNotEqual || bSceneNotEqual)
			return;

		uint32_t indexA = FindEnitityIndex(a);
		uint32_t indexB = FindEnitityIndex(b);

		if (m_SceneList.size() <= indexA || m_SceneList.size() <= indexB)
		{
			RY_CORE_ERROR("Swap is not Vaild");
			return;
		}

		m_SceneList.at(indexA) = b;
		m_SceneList.at(indexB) = a;
	}

	uint32_t SceneHierachyPannel::FindEnitityIndex(Entity e)
	{
		if (e.GetScene() != m_Context)
		{
			RY_CORE_FATAL("Enitity was never part of this Scene!");
			return MAXUINT32;
		}
		uint32_t index = e.GetEntityIndexHandle();
		if (index < m_SceneList.size() && e == m_SceneList.at(index))
			return index;

		index = 0u;
		for (Entity entity : m_SceneList)
		{
			if (entity == e)
				return index;
			index++;
		}
		RY_CORE_ERROR("Not Founded Eneitity In Lolcale Vec! search now Orignel Hirachie if we finde the the Entity the locale Vec will be overitten!");
		
		uint32_t foundIndex = MAXUINT32;
		index = 0u;
		m_Context->m_Registery.each([searchEnitity = e, this, &foundIndex, &index](entt::entity e) {
			if (foundIndex != MAXUINT32)
				return;

			Entity entity = Entity(e, m_Context.get());
			if (searchEnitity == entity)
				foundIndex = index;
			index++;
		});
		if (foundIndex == MAXUINT32)
		{
			RY_CORE_FATAL("Enitity Not Found Here!");
			return foundIndex;
		}
		else
		{
			RY_CORE_WARN("We found the enitity Clear the locale List Create the locale list new! (Every change, inorder will get lost)");
			m_SceneList.clear();
			m_Context->m_Registery.each([this](entt::entity e) {
				Entity entity = Entity(e, m_Context.get());
				m_SceneList.emplace_back(entity);
			});
			if (foundIndex < m_SceneList.size() && e == m_SceneList.at(foundIndex))
			{
				RY_CORE_INFO("We Found Entity Now and return Index {}", foundIndex);
				return foundIndex;
			}
			else
			{
				RY_CORE_WARN("We need to Search agian lolcale Vec!");
				return FindEnitityIndex(e);
			}
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
			ScriptComponent component = entity.GetComponent<ScriptComponent>();
#if RY_SCRIPTING_HAZEL
			std::unordered_map<std::string, Ref<ScriptClass>> clasesMap = ScriptEngine::GetEntityClasses();
			if(clasesMap.find(component.Name) == clasesMap.end())
				entity.SetState(Entity::State::Error);
			else
				entity.SetState(Entity::State::None);
#else
			if (ScriptingEngine::ClassExists(component.Name))
				entity.SetState(Entity::State::None);
			else
				entity.SetState(Entity::State::Error);
#endif
			
		}
	}


	void SceneHierachyPannel::AddChiledEntity(Entity e)
	{
		e.AddChildrenEntity("Empty Childern Entiy");
		e.UpdateMatrix();
	}

	void SceneHierachyPannel::RemoveEntity(Entity e)
	{
		m_Context->DestroyEntity(e);
	}

	void SceneHierachyPannel::RemoveChiledEntity(Entity e)
	{
		e.DestroyEntityChildrens();
		m_Context->DestroyEntity(e);
	}

	void SceneHierachyPannel::CopyEntity(Entity e)
	{
		Entity copy = e.CopyEntity();
		copy.CopyChildres(e);
	}


#pragma region CompontesGUI


	void SceneHierachyPannel::ComponentStaticSingleMeshGUI(Entity e, StaticMeshComponent& component)
	{
		if (nullptr != component.meshSingle)
		{
			uint32_t indexData = component.meshSingle->GetModelLocalMesheIndex();
			ImGui::Text("StaticSingleComponetsMeshComponent: %u", indexData);
		}

	}

	void SceneHierachyPannel::ComponentRenderTargetGUI(Entity e, RenderTargetComponent& component)
	{
		Ref<RenderTarget>& target = component.Target;
		static char bufferName[64];
		strcpy(bufferName, component.RenderPassName.c_str());
		bool selected = false;
		if(ImGui::InputText("RenderPassName", bufferName, 64))
		{
			component.RenderPassName = bufferName;
		}

		if (nullptr == target)
		{
			if(ImGui::Button("Create Target"))
			{
				target = CreateRef<RenderTarget>();
				e.UpdateComponent(component);
			}
			return;
		}
		else
		{
			if (ImGui::Button("Deltet Target"))
			{
				
				RY_DESTROY_REF(target);
				e.UpdateComponent(component);
				return;
			}
		}


		glm::ivec4 viewSsize = target->GetRenderViewSize();
		if (Utils::DrawIvec4ControlerR("RenderSize", viewSsize, glm::ivec4{ 1, 1, 0, 0 }, 80.0f))
		{
			target->ResizeView(viewSsize);
		}
		const Ref<Framebuffer>& fb = target->GetFramebuffer();
		
		constexpr const char* addAttachmentPopUpStrCreate = "POP_UP_Add_Color/Depth-Attachment_Create";
		
		if (nullptr == fb)
		{
			if (ImGui::Button("Create Frambuffer"))
			{
#if 0
				FramebufferSpecification spec = {
					viewSsize.x, viewSsize.y,
					FramebufferAttachmentSpecification{},
					1, false
				};
				Ref<Framebuffer> fbCreate = Framebuffer::Create(spec);
				target->SetFramebuffer(fbCreate);
#else
				ImGui::OpenPopup(addAttachmentPopUpStrCreate);
#endif
			}
		}
		else
		{
			if (ImGui::Button("Deltet Frambuffer"))
			{
				target->SetFramebuffer(nullptr);
				e.UpdateComponent(component);
				return;
			}
		}
		if (ImGui::BeginPopup(addAttachmentPopUpStrCreate))
		{
			constexpr size_t count = magic_enum::enum_count<TextureFormat>();
			constexpr std::array<std::string_view, count> fromatStrViewArray = magic_enum::enum_names<TextureFormat>();
			size_t textureFormatCount = count;
			for (size_t i = 2; i < textureFormatCount; i++)
			{
				TextureFormat textureFormatIndex = static_cast<TextureFormat>(i);
				
				std::string_view fromatStrView = fromatStrViewArray[i];
				const char* fileStr = fromatStrView.data();
				if (ImGui::MenuItem(fileStr))
				{
					uint32_t sampleCount = 1u;
					FramebufferSpecification fbspecNew = FramebufferSpecification();
					fbspecNew.Width = viewSsize.x;
					fbspecNew.Height = viewSsize.y;

					FramebufferTextureSpecification texSpec = FramebufferTextureSpecification(
						textureFormatIndex, sampleCount,
						{ TextureWrappingMode::ClampEdge, TextureWrappingMode::ClampEdge, TextureWrappingMode::ClampEdge },
						TextureFilteringMode::Linear, TextureCompareModes::None
					);
					
					fbspecNew.Attachments.Attachments.emplace_back(texSpec);
					Ref<Framebuffer> fbCreate = Framebuffer::Create(fbspecNew);
					target->SetFramebuffer(fbCreate);
					ImGui::CloseCurrentPopup();
					selected = true;
				}
			}
			ImGui::EndPopup();

			


		}

		if (nullptr == fb)
		{
			return;
		}
		
		const FramebufferSpecification& fbspec = fb->GetFramebufferSpecification();


		int delateNumber = -1;
		int colorAtachments = 0;
		size_t i = 0;
		

		for (const FramebufferTextureSpecification& attachment : fbspec.Attachments)
		{
			TextureFormat texFormat = attachment.TextureFormat;
			std::string_view format_view = magic_enum::enum_name<TextureFormat>(texFormat);
			

			ImGui::PushID(format_view.data() + ('0' + i));
			

#if 0
			uint32_t textureID = 0;

			switch (texFormat)
			{
			case TextureFormat::Depth24Stencil8:
			{
				textureID = fb->GetColorAttachmentRendererID(colorAtachments);
				break;
			}
			case TextureFormat::None:
			{
				break;
			}
			default:
				textureID = fb->GetDeathAttachmentRendererID();
				break;
			}


			ImVec2 windowSize = ImGui::GetWindowSize();
			windowSize.x = windowSize.x * 0.92 - 110;
			ImGui::SetColumnWidth(0, windowSize.x + 5);


			ImGui::SetColumnWidth(1, 100);
			windowSize.y = windowSize.x / ((float)fbspec.Width / fbspec.Height);
			ImGui::Image((ImTextureID)textureID, windowSize, ImVec2(0, 1), ImVec2(1, 0));

			ImGui::NextColumn();

			if (ImGui::Button(format_view.data(), { 100.0f , 0.0f }))
			{
			}
#endif

			switch (texFormat)
			{
			case TextureFormat::Depth24Stencil8:
			case TextureFormat::Depth32FStencil8:
				ImGui::Text("On Depth Attachment: %s",format_view.data());
				break;
			case TextureFormat::None:
				break;
			default:
				ImGui::Text("On Color Attachment(%zu): %s", i, format_view.data());
			}
			
			ImGui::SameLine();
			if (ImGui::Button("Delete", ImVec2(100.0f, 0.0f)))
			{
				delateNumber = i;
				selected = true;
			}

			
			
			
			
		
			ImGui::PopID();
			colorAtachments++;
			i++;
		}

		if (delateNumber != -1)
		{
			FramebufferSpecification fbspecNew = fbspec;
			fbspecNew.Attachments.Attachments.erase(fbspecNew.Attachments.begin() + delateNumber);
			if(fbspecNew.Attachments.Attachments.empty())
			{
				target->SetFramebuffer(nullptr);
			}
			else
			{
				Ref<Framebuffer> fbCreate = Framebuffer::Create(fbspecNew);
				target->SetFramebuffer(fbCreate);
				selected = true;

			}

		}
			
		constexpr const char* addAttachmentPopUpStr = "POP_UP_Add_Color/Depth-Attachment";
		
		if (ImGui::Button("+", ImVec2{ 25, 25 }))
		{
			ImGui::OpenPopup(addAttachmentPopUpStr);
		}

		if (ImGui::BeginPopup(addAttachmentPopUpStr))
		{
			Ref<Texture> depthTex = fb->GetDepthTexture();

			constexpr size_t count = magic_enum::enum_count<TextureFormat>();
			constexpr std::array<std::string_view, count> fromatStrViewArray = magic_enum::enum_names<TextureFormat>();
			constexpr size_t dethTextureCount = 7ull;
			size_t textureFormatCount = nullptr == depthTex ? count : count - dethTextureCount;

			for (size_t i = 2; i < textureFormatCount; i++)
			{
				TextureFormat textureFormatIndex = static_cast<TextureFormat>(i);

				std::string_view fromatStrView = fromatStrViewArray[i];
				const char* fileStr = fromatStrView.data();
				if (ImGui::MenuItem(fileStr))
				{
					
					FramebufferSpecification fbspecNew = fbspec;
					uint32_t sampleCount = fbspecNew.Samples;

					FramebufferTextureSpecification texSpec = FramebufferTextureSpecification(
						textureFormatIndex, sampleCount,
						{ TextureWrappingMode::ClampEdge, TextureWrappingMode::ClampEdge, TextureWrappingMode::ClampEdge },
						TextureFilteringMode::Linear, TextureCompareModes::None
					);

					fbspecNew.Attachments.Attachments.emplace_back(texSpec);
					Ref<Framebuffer> fbCreate = Framebuffer::Create(fbspecNew);
					target->SetFramebuffer(fbCreate);

					ImGui::CloseCurrentPopup();
					selected = true;
					break;
				}
			}
			ImGui::EndPopup();
			if(selected)
			{
				e.UpdateComponent(component);
			}

		}

		// ImGui::End();
	}

	void SceneHierachyPannel::ComponentTransformGUI(Entity e, TransformComponent& component)
	{
		bool use = false;
		use = Utils::DrawVec3Controler("Translation", component.Transaltion) || use;
		glm::vec3 rotation = glm::degrees(component.Rotation);
		use = Utils::DrawVec3Controler("Rotation", rotation) || use;
		use = Utils::DrawVec3Controler("Scale", component.Scale, 1.0f) || use;

		if (use)
		{
			component.Rotation = glm::radians(rotation);
			e.UpdateComponent(component);
			e.UpdateMatrix();
		}

	}

	void SceneHierachyPannel::ComponentModelMatrixGUI(Entity e, ModelMatrixComponent& component)
	{
		bool use = false;
		use = Utils::DrawVec4ControlerR("[0][...]", component.Locale[0], glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)) || use;
		use = Utils::DrawVec4ControlerR("[1][...]", component.Locale[1], glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)) || use;
		use = Utils::DrawVec4ControlerR("[2][...]", component.Locale[2], glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)) || use;
		use = Utils::DrawVec4ControlerR("[3][...]", component.Locale[3], glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) || use;

		if (use)
		{
			e.UpdateComponent(component);
			e.UpdateAutoMatrix();
		}
	}

	void SceneHierachyPannel::ComponentCameraGUI(Entity e, CameraComponent& component)
	{
		SceneCamera& camera = component.Camera;
		constexpr size_t count = magic_enum::enum_count<SceneCamera::ProjectionType>();
		constexpr std::array<std::string_view, count> fromatStrViewArray = magic_enum::enum_names<SceneCamera::ProjectionType>();
		SceneCamera::ProjectionType projtionType = camera.GetProjectionType();
		size_t projtionIndexType = static_cast<size_t>(projtionType);
		std::string_view currentProjtionTypeView = fromatStrViewArray[projtionIndexType];

		const char* currentProjectioneTypeStrings = currentProjtionTypeView.data();

		if (ImGui::BeginCombo("Projektion", currentProjectioneTypeStrings))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelelcted = currentProjtionTypeView == fromatStrViewArray[i];
				if (ImGui::Selectable(fromatStrViewArray[i].data(), isSelelcted))
				{
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
			if (ImGui::DragFloat("Near", &nearOrth, 1.0f))
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

		if (component.Primary)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}
		glm::ivec2 aspectViewSize = camera.GetViewAsspect();
		if (ImGui::InputInt2("Aspect Window Size", glm::value_ptr(aspectViewSize)))
		{
			
			camera.SetViewPortSize(
				0 < aspectViewSize.x ? aspectViewSize.x : 1u
				, 0 < aspectViewSize.y ? aspectViewSize.y : 1u
			);
		}
		float aspect = camera.GetAspectRotatio();
		if(ImGui::InputFloat("Aspect Rotatio", &aspect, 0.001, 0.0F, "%.4f"))
		{
			RY_REMBER_FUNC_CHANGE("Rember to set actuell aspect");
			RY_CORE_NOT_IMPL();
		}
		if (component.Primary)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}
	}

	void SceneHierachyPannel::ComponentSpriteRendererGUI(Entity e, SpriteRendererComponent& component)
	{
		ImGui::ColorEdit4("Color", glm::value_ptr(component.Color), 0.1f);
			// Texture
#if RY_SCENE_HIERACHY_PANNEL_DRAG_AND_DROP
			if (Utils::DargDropTextureButten(&component.Texture))
				component.Signel = RenderSginale::UpdateData;
#else
		Utils::DargDropTextureButten<SpriteRendererComponent>(e, component.Texture);
#endif
	}

	void SceneHierachyPannel::ComponentScriptGUI(Entity e, ScriptComponent& component)
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
	}

	void SceneHierachyPannel::ComponentFrameBufferGUI(Entity e, FrameBufferComponent& component)
	{
		Ref<Framebuffer>& frameBuffer = component.FrameBuffer;

#if RY_SCENE_HIERACHY_PANNEL_DRAG_AND_DROP
		Utils::DragDropButtenForAsset<Framebuffer>(&component.FrameBuffer, "FrameBuffer", AssetType::Framebuffer);
#else

#endif

		if (nullptr == frameBuffer)
			return;

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
			ImGui::SetColumnWidth(0, windowSize.x + 5);


			ImGui::SetColumnWidth(1, 100);
			windowSize.y = windowSize.x / ((float)fbspec.Width / fbspec.Height);
			ImGui::Image((ImTextureID)textureID, windowSize, ImVec2(0, 1), ImVec2(1, 0));

			ImGui::NextColumn();

			if (ImGui::Button(format_view.data(), { 100.0f , 0.0f }))
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
			
			fbspecNew.Attachments.Attachments.erase(fbspecNew.Attachments.begin() + delateNumber);
			component.FrameBuffer = Framebuffer::Create(fbspecNew);
		}
		
	}

	

	void SceneHierachyPannel::ComponentDrirektionleLigthGUI(Entity e, DrirectionleLigthComponent& component)
	{
		ImGui::ColorEdit3("Color: ", glm::value_ptr(component.color));
		ImGui::DragFloat("Intensitie: ", &component.intensitie, 0.01f, -1.0f, 10.0f, "%.2f");

	}

	void SceneHierachyPannel::ComponentPointLigthGUI(Entity e, PointLigthComponent& component)
	{
		ImGui::ColorEdit3("Color", glm::value_ptr(component.color));
		ImGui::DragFloat("Intensitie", &component.intensitie, 0.01f, -1.0f, 10.0f, "%.2f");
		ImGui::DragFloat("Distence", &component.distence, 0.5f, -1.0f, 3250.0f, "%.0f");

		ImGui::DragFloat("Constant", &component.constant, 0.001f, -2.0f, 2.0f, "%.4f");
		ImGui::DragFloat("Linear", &component.linear, 0.0001f, -0.001f, 1.0f, "%.5f");
		ImGui::DragFloat("Quadratic", &component.quadratic, 0.000001f, -0.00001f, 3.0f, "%.7f");
	}

	void SceneHierachyPannel::ComponentSpotLigthGUI(Entity e, SpotLigthComponent& component)
	{
		ImGui::ColorEdit3("Color", glm::value_ptr(component.color));
		ImGui::DragFloat("Intensitie", &component.intensitie, 0.01f, -1.0f, 10.0f, "%.2f");
		ImGui::DragFloat("Distence", &component.distence, 0.01f, -1.0f, 10.0f, "%.2f");
		ImGui::DragFloat("Inner", &component.inner, 0.01f, -1.0f, 10.0f, "%.2f");
		ImGui::DragFloat("Outer", &component.outer, 0.01f, -1.0f, 10.0f, "%.2f");
	}

	void SceneHierachyPannel::ComponentTextGUI(Entity e, TextComponent& component)
	{
		static char bufferText[2048];
		strcpy(bufferText, component.TextString.c_str());
		if (ImGui::InputTextMultiline("Text String", bufferText, 2048))
			component.TextString = bufferText;
		ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

		ImGui::DragFloat("Kerning", &component.Kerning, 0.025f);
		ImGui::DragFloat("Line Spacing", &component.LineSpacing, 0.01f, -1.0f, 10.0f, "%.2f");

		Utils::DargDropFontButten(component);
	}

	void SceneHierachyPannel::ComponentViewMatrixGUI(Entity e, ViewMatrixComponent& component)
	{
	}

	void SceneHierachyPannel::ComponentStaticMeshGUI(Entity e, ModelMangerComponent& component)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap;

		if (Utils::DargDropMeshStaticButten<ModelMangerComponent>(e, component.meshStatic))
		{
			e.UpdateComponent(component);

			component.rendereStoreIndexVec2.clear();
#if RY_DISABLE_FLAT_2D_VEC
			component.objectRendereIndexPiplineVec2.clear();
#else
			component.objectRendereIndexPiplineVec2.Clear();
#endif
			component.singleMeshes.clear();
		}

		if (nullptr == component.meshStatic)
			return;
		Ref<MeshStatic>& meshStatic = component.meshStatic;

		const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshDataVec = meshStatic->GetSingleObjectMesDataVec();

		uint32_t i = 0;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
		static UUID s_Selectedhandle = UUID(0ull);
		for(const MeshStatic::SingleObjectMeshData& object : singleObjectMeshDataVec)
		{
			const std::string& node = object.NodeName;
			const UUID& id = object.GetHandle();
			std::string idName = "Mesh-Static-List-Node-" + std::to_string(i);
	
			ImGui::PushID(idName.c_str());
			bool opende = ImGui::TreeNodeEx(&id, flags, node.c_str());
			if (opende)
			{
				
				
				
			}
			ImGui::PopID();
			i++;
		}


	}

	void SceneHierachyPannel::ComponentDynamicMeshGUI(Entity e, DynamicMeshComponent& component)
	{
	}

#pragma endregion


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
#include <rypch.h>
#include "ImGuiVector.h"
#include <Editor/ImGuiObj/ImGuiObjects.h>

#include <Rynex/Renderer/API/Buffer.h>

#include <imgui.h>
#include <imgui_internal.h>




namespace Rynex::UI {

    namespace Utils {
        constexpr const char* s_VECTOR_NAMES_ARRAY[4] = {
            "X",
            "Y",
            "Z",
            "W"
        };

        constexpr glm::vec3 s_VECTOR_COLOR_ARRAY[4] = {
            glm::vec3{ 0.8f, 0.1f, 0.15f },
            glm::vec3{ 0.2f, 0.7f, 0.2f },
            glm::vec3{ 0.1f, 0.25f, 0.8f },
            glm::vec3{ 0.8f, 0.85f, 0.8f }
        };

        template<typename T, typename DragFunc>
        static bool AttributeControllerResetType(const char* label, T& value, const glm::vec3 color, const ImVec2& buttenSize, T resetValue, float hoverColorDifference, float activeColorDifference, const char* resetTip, const char* charValue, DragFunc dragFunc)
        {
            constexpr glm::vec3 MIN_VALUE(0.0f);
            constexpr glm::vec3 MAX_VALUE(1.0f);
            ImGuiIO& io = ImGui::GetIO();
            ImFont* boldFont = io.Fonts->Fonts[0];

            bool changes = false;
            glm::vec3 colorDefault = glm::clamp(color, MIN_VALUE, MAX_VALUE);
            ImVec4 colorButten{ colorDefault.r, colorDefault.g, colorDefault.b, 1.0 };

            glm::vec3 colorHover = glm::clamp(color + activeColorDifference, MIN_VALUE, MAX_VALUE);
            ImVec4 colorButtenHover{ colorHover.r, colorHover.g, colorHover.b, 1.0 };

            glm::vec3 colorActive = glm::clamp(color + activeColorDifference, MIN_VALUE, MAX_VALUE);
            ImVec4 colorButtenActive{ colorActive.r, colorActive.g, colorActive.b, 1.0 };

            ImGui::PushStyleColor(ImGuiCol_Button, colorButten);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorButtenHover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorButtenActive);
            ImGui::PushFont(boldFont);
            if (ImGui::Button(label, buttenSize))
            {
                value = resetValue;
                changes = true;
            }

            // "Reset Value %i";
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(resetTip, resetValue);
            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            constexpr T SPEED_DEFAULT_DRAG = 1;
            constexpr T MAX_DEFAULT_DRAG = 0;
            constexpr T MIN_DEFAULT_DRAG = 0;
            constexpr int FLAG_DEFAULT_DRAG = 0;

            static std::string labelDarg("##");
            labelDarg = "##";
            labelDarg += label;

            changes = dragFunc(labelDarg.c_str(), &value, SPEED_DEFAULT_DRAG, MAX_DEFAULT_DRAG, MIN_DEFAULT_DRAG, charValue, FLAG_DEFAULT_DRAG);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            return changes;
        }

        template<typename T, typename DragFunc>
        static bool AttributeControllerType(const char* label, T& value, const glm::vec3 color, const ImVec2& buttenSize,  float hoverColorDifference, float activeColorDifference, const char* charValue, DragFunc dragFunc)
        {
            constexpr glm::vec3 MIN_VALUE(0.0f);
            constexpr glm::vec3 MAX_VALUE(1.0f);
            ImGuiIO& io = ImGui::GetIO();
            ImFont* boldFont = io.Fonts->Fonts[0];

            bool changes = false;
            glm::vec3 colorDefault = glm::clamp(color, MIN_VALUE, MAX_VALUE);
            ImVec4 colorButten{ colorDefault.r, colorDefault.g, colorDefault.b, 1.0 };

            glm::vec3 colorHover = glm::clamp(color + activeColorDifference, MIN_VALUE, MAX_VALUE);
            ImVec4 colorButtenHover{ colorHover.r, colorHover.g, colorHover.b, 1.0 };

            glm::vec3 colorActive = glm::clamp(color + activeColorDifference, MIN_VALUE, MAX_VALUE);
            ImVec4 colorButtenActive{ colorActive.r, colorActive.g, colorActive.b, 1.0 };

            ImGui::PushStyleColor(ImGuiCol_Button, colorButten);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorButtenHover);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorButtenActive);
            ImGui::PushFont(boldFont);
            ImGui::Button(label, buttenSize);



            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            constexpr T SPEED_DEFAULT_DRAG = 1;
            constexpr T MAX_DEFAULT_DRAG = 0;
            constexpr T MIN_DEFAULT_DRAG = 0;
            constexpr int FLAG_DEFAULT_DRAG = 0;
            static std::string labelDarg("##");
            labelDarg = "##";
            labelDarg += label;
            changes = dragFunc(labelDarg.c_str(), &value, SPEED_DEFAULT_DRAG, MAX_DEFAULT_DRAG, MIN_DEFAULT_DRAG, charValue, FLAG_DEFAULT_DRAG);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            return changes;
        }

        static bool AttributeController(const char* label, float& value, const glm::vec3 color, const ImVec2& buttenSize, const float hoverColorDifference = -0.075f, const float activeColorDifference = 0.1f)
        {
            return AttributeControllerType(label, value, color, buttenSize,  hoverColorDifference, activeColorDifference, "%.2f", &ImGui::DragFloat);
        }

        static bool AttributeController(const char* label, int& value, const glm::vec3 color, const ImVec2& buttenSize, const float hoverColorDifference = -0.075f, const float activeColorDifference = 0.1f)
        {
            return AttributeControllerType(label, value, color, buttenSize, hoverColorDifference, activeColorDifference,"%i", &ImGui::DragInt);
        }

        static ImVec2 ControllerStart(const std::string& label, const ShaderDataType shaderType, const uint32_t componentCount, const float collumWith)
        {

            ImGui::Columns(2, label.c_str(), false);
            ImGui::SetColumnWidth(0, collumWith);
            const char* labelPtr = label.c_str();
            ImGui::Text(labelPtr);



            if (ImGui::IsItemHovered())
            {
                const std::string_view typeName = magic_enum::enum_name(shaderType);
                const char* typeNamePtr = typeName.data();
                ImGui::SetTooltip(typeNamePtr);
            }
            ImGui::NextColumn();

            ImGui::PushMultiItemsWidths(componentCount, ImGui::CalcItemWidth());
            constexpr ImVec2 val{ 0.0f , 0.0f };
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, val);
            const ImGuiContext* context = ImGui::GetCurrentContext();
            const ImFont* font = context->Font;
            const ImGuiStyle& style = context->Style;
            const ImVec2 framePadding = style.FramePadding;
            const float fontSize = font->FontSize;
            const float framePaddingY = framePadding.y;


            const float lineHeight = fontSize + framePaddingY * 2.0f;
            const ImVec2 buttenSize{ lineHeight - 2.0f, lineHeight };
            return buttenSize;
        }

        static bool AttributeControllerReset(const char* label, float& value, const glm::vec3 color, const ImVec2& buttenSize, const float resetValue, const float hoverColorDifference = -0.075f, const float activeColorDifference = 0.1f)
        {
            return AttributeControllerResetType(label, value, color, buttenSize, resetValue, hoverColorDifference, activeColorDifference, "Reset Value %f", "%.2f", &ImGui::DragFloat);
        }

        static bool AttributeControllerReset(const char* label, int& value, const glm::vec3 color, const ImVec2& buttenSize, const int resetValue, const float hoverColorDifference = -0.075f, const float activeColorDifference = 0.1f)
        {
            return AttributeControllerResetType(label, value, color, buttenSize, resetValue, hoverColorDifference, activeColorDifference, "Reset Value %i", "%i", &ImGui::DragInt);
        }

        template<typename T, int componentCount>
        static bool DrawVectorControllerReset(const std::string& label, glm::vec<componentCount, T>& values, glm::vec<componentCount, T> resetValue, const float collumWith, const ShaderDataType shaderType)
        {
            bool changes = false;
            ImGuiScopeID scopeID(label);
            const ImVec2 buttenSize = ControllerStart(label, shaderType, componentCount, collumWith);

            for (int i = 0; i < componentCount; i++)
            {
                changes = Utils::AttributeControllerReset(s_VECTOR_NAMES_ARRAY[i], values[i], s_VECTOR_COLOR_ARRAY[i], buttenSize, resetValue[i]) || changes;
            }

            ImGui::PopStyleVar();
            ImGui::Columns(1);

            return changes;
        }

        template<typename T, int componentCount>
        static bool DrawVectorController(const std::string& label, glm::vec<componentCount, T>& values,const float collumWith, const ShaderDataType shaderType)
        {
            bool changes = false;
            ImGuiScopeID scopeID(label);
            const ImVec2 buttenSize = ControllerStart(label, shaderType, componentCount, collumWith);

            for (int i = 0; i < componentCount; i++)
            {
                changes = Utils::AttributeController(s_VECTOR_NAMES_ARRAY[i], values[i], s_VECTOR_COLOR_ARRAY[i], buttenSize) || changes;
            }

            ImGui::PopStyleVar();

            ImGui::Columns(1);
            scopeID.PopID();
            return changes;
        }

    }

    bool DrawIVec4ControllerR(const std::string& label, glm::ivec4& values, const glm::ivec4 resetValue, const float collumWith)
	{
        constexpr ShaderDataType shaderType = ShaderDataType::Int4;
		return Utils::DrawVectorControllerReset<int, 4>(label, values, resetValue, collumWith, shaderType);
	}

    bool __DrawIVec4ControllerR(const std::string& label, glm::ivec4& values, const glm::ivec4 resetValue, const float columWith = 80.0f)
		{
			bool changes = false;


			ImGui::PushID(label.c_str());
			ImGui::Columns(2, label.c_str(), false);
			ImGui::SetColumnWidth(0, columWith);
			ImGui::Text(label.c_str());
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(magic_enum::enum_name(ShaderDataType::Float4).data());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f , 0.0f });
		    ImGuiContext* context = ImGui::GetCurrentContext();
		    ImFont* font = context->Font;
		    ImGuiStyle& style = context->Style;
		    ImVec2 framePadding = style.FramePadding;
		    float fontSize = font->FontSize;
		    float framePaddingY = framePadding.y;


			float lineHeigth = fontSize + framePaddingY * 2.0f;
			ImVec2 butenSize{ lineHeigth - 2.0f, lineHeigth };



			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
			ImGui::PushFont(font);
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
			ImGui::PushFont(font);
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
			ImGui::PushFont(font);
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
			ImGui::PushFont(font);
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

	bool DrawVec4Controller(const std::string& label, glm::vec4& values, const float resetValue, const float collumWith)
	{
#if 0
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
#else
        constexpr ShaderDataType shaderType = ShaderDataType::Float4;
        const glm::vec4 resetValueVEc(resetValue);
        return Utils::DrawVectorControllerReset<float, 4>(label, values, resetValueVEc, collumWith, shaderType);
#endif

	}

	bool DrawVec4ControllerR(const std::string& label, glm::vec4& values, const glm::vec4 resetValue, const float columeWith)
	{
#if 0
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
#else
        constexpr ShaderDataType shaderType = ShaderDataType::Float4;
        return Utils::DrawVectorControllerReset<float, 4>(label, values, resetValue, columeWith, shaderType);
#endif

	}

	bool DrawVec3ControllerR(const std::string& label, glm::vec3& values, const glm::vec3 resetValue, const float collumWith)
	{
#if 0
		bool changes = false;
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());
		ImGui::Columns(2, label.c_str(), false);
		ImGui::SetColumnWidth(0, collumWith);
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
#else
        constexpr ShaderDataType shaderType = ShaderDataType::Float3;
        return Utils::DrawVectorControllerReset<float, 3>(label, values, resetValue, collumWith, shaderType);
#endif
	}

	bool DrawVec3Controller(const std::string& label, glm::vec3& values, float resetValue, const float collumWith)
	{
#if 0
		bool changes = false;
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());
		ImGui::Columns(2, label.c_str(), false);
		ImGui::SetColumnWidth(0, collumWith);
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
#else
        constexpr ShaderDataType shaderType = ShaderDataType::Float3;
        return Utils::DrawVectorController<float, 3>(label, values, collumWith, shaderType);
#endif

	}

	bool DrawVec2Controller(const std::string & label, glm::vec2& values, float resetValue, const float collumWith)
	{
#if 0
		bool changes = false;
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, collumWith);
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
#else
        constexpr ShaderDataType shaderType = ShaderDataType::Float3;
        return Utils::DrawVectorController<float, 2>(label, values, collumWith, shaderType);
#endif

	}

	bool DrawFloatController(const std::string& label, float* values, float resetValue, const float min, const float max, const float collumWith)
	{
#if 0
		bool changes = false;
		ImGuiIO& io = ImGui::GetIO();
		ImFont* boldFont = io.Fonts->Fonts[0];

		float lineHeigth = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 butenSize = { lineHeigth + 3.0f, lineHeigth };

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, collumWith);
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
#else
    ImGuiScopeID scopeID(label);
    constexpr ShaderDataType shaderType = ShaderDataType::Float3;
    constexpr int componentCount = 1;
    const ImVec2 buttenSize = Utils::ControllerStart(label, shaderType, componentCount, collumWith);
    float& v = *values;
    bool change =  Utils::AttributeControllerReset(Utils::s_VECTOR_NAMES_ARRAY[0], v, Utils::s_VECTOR_COLOR_ARRAY[0], buttenSize, resetValue);

    ImGui::PopStyleVar();
    ImGui::Columns(1);
    return change;
#endif

	}
}

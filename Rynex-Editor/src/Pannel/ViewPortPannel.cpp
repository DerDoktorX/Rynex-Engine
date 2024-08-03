#include <rypch.h>
#include "ViewPortPannel.h"

#include <Rynex/Core/Application.h>

#include <imgui/imgui.h>

namespace Rynex {

	ViewPortPannel::ViewPortPannel(Ref<Framebuffer>& frambuffer, const std::string& name, ViewPortFlages flags, uint32_t renderID)
		: m_Framebuffer(frambuffer)
		, m_Name(name)
		, m_AktivFlags(flags)
		, m_RenderID(renderID)
	{
		
	}

	void ViewPortPannel::OnUpdate(TimeStep ts)
	{

	}

	void ViewPortPannel::OnEvent(Event& e)
	{
	}

	void ViewPortPannel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0 ,0 });
		ImGui::Begin("Viewport");

		ImVec2 viewportOffset = ImGui::GetCursorPos();
		//if (BIT_AND(m_AktivFlags, ViewPortFlages::HoverFocuse))
		{
			m_ViewPortFocused = ImGui::IsWindowFocused();
			m_ViewPortHoverd = ImGui::IsWindowHovered();
		}
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused && !m_ViewPortHoverd);

		ImVec2 viewportPannelSize = ImGui::GetContentRegionAvail();

		//----------------------------------------
		//ImGuiViewPortResize(viewportPannelSize);
		//----------------------------------------

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0);
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ viewportPannelSize.x, viewportPannelSize.y }, ImVec2(0, 1), ImVec2(1, 0));

	}

	void ViewPortPannel::Resize()
	{
#if 0
		m_AktiveScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		if (FramebufferSpecification spec = m_Framebuffer->GetFramebufferSpecification(); m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && (spec.Width != m_ViewPortSize.x || spec.Height != m_ViewPortSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_CameraController->OnResize(m_ViewPortSize.x, m_ViewPortSize.y);
			m_EditorCamera->SetViewportSize(m_ViewPortSize.y, m_ViewPortSize.x);
		}
#endif
	}

	bool ViewPortPannel::GetMousePos(glm::vec<2, float>& tempMousePos)
	{
		return false;
	}
}
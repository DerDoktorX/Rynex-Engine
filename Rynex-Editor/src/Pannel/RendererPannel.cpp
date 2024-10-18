#include <rypch.h>
#include "RendererPannel.h"
#include <imgui/imgui.h>

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/Rendering/Renderer2D.h>
#include <Rynex/Renderer/Rendering/Renderer3D.h>

namespace Rynex {

	namespace Utils {

	}

	RendererPannel::RendererPannel(const std::string& name)
		: m_Name(name)
		, m_RendererMode(
			Renderer::CallFace_Back 
			| Renderer::Death_Buffer 
			| Renderer::A_Buffer
		)
	{
	}

	RendererPannel::~RendererPannel()
	{
		m_EditorLayer = nullptr;
	}

	void RendererPannel::OnAttache(EditorLayer* editorLayer)
	{
		m_EditorLayer = editorLayer;
		Renderer::SetMode(m_RendererMode);
	}

	void RendererPannel::OnDetache()
	{

	}

	void RendererPannel::OnEvent(Event& e)
	{

	}

	void RendererPannel::OpenWindow()
	{
		m_WindowOpen = true;
	}

	bool RendererPannel::OnKeyPressed(KeyPressedEvent& e)
	{
		return false;
	}

	bool RendererPannel::OnMousePressed(MouseButtenPressedEvent& e)
	{
		return false;
	}

	void RendererPannel::OnUpdate(TimeStep ts)
	{
		m_CurentTS = ts;
	}

	void RendererPannel::OnImGuiRender()
	{
		if(m_WindowOpen)
		{
			ImGui::Begin(m_Name.c_str(), &m_WindowOpen, ImGuiWindowFlags_None);
			ImGui::Text("Renderer:");
			ImGui::Text("FPS: %.1f", m_CurentTS.GetFPS());
			
			ImGui::Text("Update Time: %.2f milsec", m_CurentTS.GetMillsecounds());

			bool wireFrame = BIT_EQUAL(m_RendererMode, Renderer::WireFrame),
				a_Buffer = BIT_EQUAL(m_RendererMode, Renderer::A_Buffer),
				death_Buffer = BIT_EQUAL(m_RendererMode, Renderer::Death_Buffer),
				callFace_Nono = BIT_EQUAL(m_RendererMode,Renderer::CallFace_Nono),
				callFace_Front = BIT_EQUAL(m_RendererMode, Renderer::CallFace_Front),
				callFace_Back  = BIT_EQUAL(m_RendererMode, Renderer::CallFace_Back ),
				callFace_FrontBack = BIT_EQUAL(m_RendererMode,Renderer::CallFace_FrontBack);

			if (ImGui::Checkbox("Wirframe mode:", &wireFrame))
			{
				m_RendererMode = BIT_SET_ON(wireFrame, Renderer::WireFrame, m_RendererMode);
				
			}
			if (ImGui::Checkbox("A-Buffer mode:", &a_Buffer))
			{
				m_RendererMode = BIT_SET_ON(a_Buffer, Renderer::A_Buffer, m_RendererMode);
			}
			if (ImGui::Checkbox("Death-Buffer mode:", &death_Buffer))
			{
				m_RendererMode = BIT_SET_ON(death_Buffer, Renderer::Death_Buffer, m_RendererMode);
			}

			if (ImGui::RadioButton("None", callFace_Nono))
			{
				m_RendererMode = BIT_SET_ON(1, Renderer::CallFace_Nono	   , m_RendererMode);
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_Front	   , m_RendererMode);
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_Back	   , m_RendererMode);
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_FrontBack, m_RendererMode);
			}
			if (ImGui::RadioButton("Front", callFace_Front))
			{
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_Nono, m_RendererMode);
				m_RendererMode = BIT_SET_ON(1, Renderer::CallFace_Front, m_RendererMode);
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_Back, m_RendererMode);
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_FrontBack, m_RendererMode);
			}
			if (ImGui::RadioButton("Back", callFace_Back))
			{
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_Nono, m_RendererMode);
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_Front, m_RendererMode);
				m_RendererMode = BIT_SET_ON(1, Renderer::CallFace_Back, m_RendererMode);
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_FrontBack, m_RendererMode);
			}
			if (ImGui::RadioButton("Front Back", callFace_FrontBack))
			{
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_Nono, m_RendererMode);
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_Front, m_RendererMode);
				m_RendererMode = BIT_SET_ON(0, Renderer::CallFace_Back, m_RendererMode);
				m_RendererMode = BIT_SET_ON(1, Renderer::CallFace_FrontBack, m_RendererMode);
			}
			Renderer::SetMode(m_RendererMode);
			{
				ImGui::Text("Renderer 3D:");
				Renderer3D::Statistics renderer = Renderer3D::GetStats();
				ImGui::Text("DrawCalls: %i", renderer.DrawCalls);
				uint8_t index = 0;
				for(auto& drawPass : renderer.DrawTime)
				{
					ImGui::Text("DrawTime[%i]:  %.2lf mils", index, (double)drawPass);
					index++;
				}
				Renderer3D::ResetStats();
			}
			
			{
				ImGui::Text("Renderer 2D:");
				Renderer2D::Statistics stats = Renderer2D::GetStats();
				ImGui::Text("Draw Calls: %d", stats.DrawCalls);
				ImGui::Text("Quad   : %d", stats.QuadCount);
				ImGui::Text("Vertex : %d", stats.GetTotalVertexCount());
				ImGui::Text("Indexs : %d", stats.GetTotalIndexCount());
			}
			ImGui::End();
		}
		
	}

}
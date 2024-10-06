#include <rypch.h>
#include "RendererPannel.h"
#include <imgui/imgui.h>

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/Rendering/Renderer2D.h>
#include <Rynex/Renderer/Rendering/Renderer3D.h>
#include <bitset>

namespace Rynex {

	namespace Utils {

	}

	RendererPannel::RendererPannel(const std::string& name)
		: m_Name(name)
		, m_RendererMode(RenderMode::CallFace_Back
			| RenderMode::Death_Buffer)
	{
		RenderCommand::SetFace(CallFace::Back);
	}

	RendererPannel::~RendererPannel()
	{
		m_EditorLayer = nullptr;
	}

	void RendererPannel::OnAttache(EditorLayer* editorLayer)
	{
		m_EditorLayer = editorLayer;
	}


	void RendererPannel::OnDetache()
	{

	}

	void RendererPannel::OnEvent(Event& e)
	{

	}

	void RendererPannel::OpenWindow()
	{

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
			

			bool wireFrame = m_RendererMode & ( RenderMode::WireFrame) ,
#if 0
				a_Buffer = m_RendererMode & (RenderMode::A_Buffer),
#endif
				death_Buffer = m_RendererMode & (RenderMode::Death_Buffer),
				callFace_Nono = m_RendererMode & (RenderMode::CallFace_Nono),
				callFace_Front = m_RendererMode & (RenderMode::CallFace_Front),
				callFace_Back  = m_RendererMode & (RenderMode::CallFace_Back ),
				callFace_FrontBack = m_RendererMode & (RenderMode::CallFace_FrontBack);

			if (ImGui::Checkbox("Wirframe mode:", &wireFrame))
			{
				m_RendererMode = wireFrame ? m_RendererMode | RenderMode::WireFrame : m_RendererMode & ~RenderMode::WireFrame;
				RenderCommand::AktivePolyGunMode(wireFrame);
			}
#if 0
			if (ImGui::Checkbox("A-Buffer mode:", &a_Buffer))
			{
				m_RendererMode = a_Buffer ? m_RendererMode | RenderMode::A_Buffer : m_RendererMode & ~RenderMode::A_Buffer;
				
			}
#endif
			if (ImGui::Checkbox("Death-Buffer mode:", &death_Buffer))
			{
				m_RendererMode = death_Buffer ? m_RendererMode | RenderMode::Death_Buffer : m_RendererMode & ~RenderMode::Death_Buffer;
				RenderCommand::SetDethTest(death_Buffer);
			}

			if (ImGui::RadioButton("None", callFace_Nono))
			{
				m_RendererMode = 1 ? m_RendererMode | RenderMode::CallFace_Nono : m_RendererMode & ~RenderMode::CallFace_Nono;
				m_RendererMode = 0 ? m_RendererMode | RenderMode::CallFace_Front : m_RendererMode & ~RenderMode::CallFace_Front;
				m_RendererMode = 0 ? m_RendererMode | RenderMode::CallFace_Back : m_RendererMode & ~RenderMode::CallFace_Back;
				m_RendererMode = 0 ? m_RendererMode | RenderMode::CallFace_FrontBack : m_RendererMode & ~RenderMode::CallFace_FrontBack;
				RenderCommand::SetFace(CallFace::None);
			}
			if (ImGui::RadioButton("Front", callFace_Front))
			{
				m_RendererMode = 0  ? m_RendererMode | RenderMode::CallFace_Nono : m_RendererMode & ~RenderMode::CallFace_Nono;
				m_RendererMode = 1  ? m_RendererMode | RenderMode::CallFace_Front : m_RendererMode & ~RenderMode::CallFace_Front;
				m_RendererMode = 0  ? m_RendererMode | RenderMode::CallFace_Back : m_RendererMode & ~RenderMode::CallFace_Back;
				m_RendererMode = 0  ? m_RendererMode | RenderMode::CallFace_FrontBack : m_RendererMode & ~RenderMode::CallFace_FrontBack;
				RenderCommand::SetFace(CallFace::Front);
			}
			if (ImGui::RadioButton("Back", callFace_Back))
			{
				m_RendererMode = 0  ? m_RendererMode | RenderMode::CallFace_Nono : m_RendererMode & ~RenderMode::CallFace_Nono;
				m_RendererMode = 0  ? m_RendererMode | RenderMode::CallFace_Front : m_RendererMode & ~RenderMode::CallFace_Front;
				m_RendererMode = 1  ? m_RendererMode | RenderMode::CallFace_Back : m_RendererMode & ~RenderMode::CallFace_Back;
				m_RendererMode = 0  ? m_RendererMode | RenderMode::CallFace_FrontBack : m_RendererMode & ~RenderMode::CallFace_FrontBack;
				RenderCommand::SetFace(CallFace::Back);
			}
			if (ImGui::RadioButton("Front Back", callFace_FrontBack))
			{
				m_RendererMode =  0 ? m_RendererMode | RenderMode::CallFace_Nono : m_RendererMode & ~RenderMode::CallFace_Nono;
				m_RendererMode =  0 ? m_RendererMode | RenderMode::CallFace_Front : m_RendererMode & ~RenderMode::CallFace_Front;
				m_RendererMode =  0 ? m_RendererMode | RenderMode::CallFace_Back : m_RendererMode & ~RenderMode::CallFace_Back;
				m_RendererMode =  1 ? m_RendererMode | RenderMode::CallFace_FrontBack : m_RendererMode & ~RenderMode::CallFace_FrontBack;

				RenderCommand::SetFace(CallFace::FrontBacke);
			}
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
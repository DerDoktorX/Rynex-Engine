#include "Sandbox2D.h"

#include <Rynex.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

Sandbox2D::Sandbox2D()
	: Layer("Sanbox2D")
	, m_CameraController((1280.0f / 720.0f), true)
	, m_ChekebordRotaion(0.25f)
{
}

void Sandbox2D::OnAttach()
{
	m_ChekbordTex = Rynex::TextureImporter::LoadTexture2D("Assets/textures/Checkerboard.png");


    Rynex::FramebufferSpecification fbSpec;
    fbSpec.Attachments =
    {
         Rynex::FramebufferTextureFormat::RGBA8,
         Rynex::FramebufferTextureFormat::RGBA8,
         Rynex::FramebufferTextureFormat::RED_INTEGER,
         Rynex::FramebufferTextureFormat::Depth
    };
    fbSpec.Width = 1280;
    fbSpec.Height = 720;
    m_Framebuffer = Rynex::Framebuffer::Create(fbSpec);

    Rynex::Renderer::Init();


    m_Project = Rynex::Project::GetActive();
    m_AssetManger = m_Project->GetRuntimeAssetManger();
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Rynex::TimeStep ts)
{
	//RY_TRACE("Delta time: {0}s ({1})ms\t\t{2} FPS", ts.GetSecounds(), ts.GetMillsecounds(), 1 / ts);

	m_CameraController.OnUpdate(ts);

    //m_Framebuffer->Bind();
	Rynex::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Rynex::RenderCommand::Clear();
	
	////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////

	
	Rynex::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Rynex::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Rynex::Renderer2D::DrawRotatedQuad({ 0.5f, 0.5f }, { 0.5f, 0.75f }, m_ChekebordRotaion, { 0.2f, 0.3f, 0.8f, 1.0f });
	Rynex::Renderer2D::DrawQuad({ 0.0f, 0.0f,-0.1f}, { 10.0f, 10.0f }, m_ChekbordTex);
	Rynex::Renderer2D::EndScene();
	
    
	Rynex::Renderer2D::BeginScene(m_CameraController.GetCamera());
	for (float x = -4.5f; x < 5.0f; x+= 0.5f)
	{
		for (float y = -4.5f; y < 5.0f; y += 0.5f)
		{
			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f };
			Rynex::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
		}
	}
	Rynex::Renderer2D::EndScene();
    //m_Framebuffer->Unbind();

 
	//RY_INFO("m_ChekebordRotaion: {0}",glm::radians(m_ChekebordRotaion));
	//Rynex::Renderer::Submit(m_BlueShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
	//std::dynamic_pointer_cast<Rynex::OpenGLShader>(m_BlueShader)->Bind();
	//std::dynamic_pointer_cast<Rynex::OpenGLShader>(m_BlueShader)->UploadUnformFloat3("u_Color", m_SquareColor);
}

void Sandbox2D::OnImGuiRender()
{
    static bool dokingEnabled = false;
   
    if (dokingEnabled){
        
        static bool dokingSpaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        //else
        //{
        //    dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        //}

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dokingSpaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }


        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.


                if (ImGui::MenuItem("Exit", NULL, false)) Rynex::Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }



        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color: ", glm::value_ptr(m_SquareColor));
        ImGui::DragFloat("Checkeboard Rotation: ", &m_ChekebordRotaion, 1.f);
        //uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        //textureID = m_ChekbordTex->GetRenderID();
        //ImGui::Image((void*)textureID, ImVec2{ 320.f,180.f });

        ImGui::End();



        ImGui::End();
    }
    else
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color: ", glm::value_ptr(m_SquareColor));
        ImGui::DragFloat("Checkeboard Rotation: ", &m_ChekebordRotaion, 1.f);
        //uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        //uint32_t textureID = m_ChekbordTex->GetRenderID();
        //ImGui::Image((void*)textureID, ImVec2{ 320.f,180.f });
        
        ImGui::End();
    }


}

void Sandbox2D::OnEvent(Rynex::Event& e)
{
	m_CameraController.OnEnvent(e);
}

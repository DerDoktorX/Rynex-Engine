#include <rypch.h>
#include "Sandbox2D.h"

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
#if TEST_TESS_SHADER_OPENGL
	m_TestOpenGL = Rynex::CreateRef<Rynex::TestOpenGL>();
	m_TestOpenGL->OnAttache();
#else
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
    //m_Framebuffer = Rynex::Framebuffer::Create(fbSpec);

    


    m_Project = Rynex::Project::GetActive();
    m_AssetManger = m_Project->GetRuntimeAssetManger();
	Rynex::Renderer::Init();
#endif   
}

void Sandbox2D::OnDetach()
{
	//m_TestOpenGL->OnDetache();
	RY_WARN("Sandbox2D::OnDetach!");
}

void Sandbox2D::OnUpdate(Rynex::TimeStep ts)
{
#if TEST_TESS_SHADER_OPENGL
	m_TestOpenGL->OnUpdate();
#else
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
#endif
}

void Sandbox2D::OnImGuiRender()
{
#if TEST_TESS_SHADER_OPENGL
#else
    static bool dokingEnabled = false;
 
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color: ", glm::value_ptr(m_SquareColor));
        ImGui::DragFloat("Checkeboard Rotation: ", &m_ChekebordRotaion, 1.f);
        //uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
        //uint32_t textureID = m_ChekbordTex->GetRenderID();
        //ImGui::Image((void*)textureID, ImVec2{ 320.f,180.f });
        
        ImGui::End();
    }

#endif
}

void Sandbox2D::OnEvent(Rynex::Event& e)
{
	m_CameraController.OnEnvent(e);
}

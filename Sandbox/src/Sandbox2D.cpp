#include <rypch.h>
#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include <Rynex/Scripting/ScriptingEngine.h>
#include <Rynex/Scene/Components.h>


#define RY_TEST_ENITY 1

Sandbox2D::Sandbox2D()
	: Layer("Sanbox2D")
	, m_CameraController((1280.0f / 720.0f), true)
	, m_ChekebordRotaion(0.25f)
{
}

void Sandbox2D::OnAttach()
{

    Rynex::FramebufferSpecification fbSpec;
    fbSpec.Attachments =
    {
         Rynex::FramebufferTextureFormat::RGBA8,
         Rynex::FramebufferTextureFormat::RED_INTEGER,
         Rynex::FramebufferTextureFormat::Depth
    };
    fbSpec.Width = 1280;
    fbSpec.Height = 720;
    m_Framebuffer = Rynex::Framebuffer::Create(fbSpec);

    
	auto cLA = Rynex::Application::Get().GetSpecification().CommandLineArgs;
	if (cLA.Count > 1)
	{
		auto projFilePath = cLA[1];
		if (Rynex::Project::Load(projFilePath))
		{
			// Rynex::ScriptingEngine::Init();
			std::filesystem::path startScene = Rynex::Project::GetActive()->GetConfig().StartScene;
		}
	}
	else
	{
		RY_CORE_INFO("Open a Project!");
		if (Rynex::Project::Load("Sandbox.rproj"))
		{
			// Rynex::ScriptingEngine::Init();
			std::filesystem::path startScene = Rynex::Project::GetActive()->GetConfig().StartScene;
		}

	}
	m_AktiveScene = Rynex::CreateRef<Rynex::Scene>();
    m_Project = Rynex::Project::GetActive();
    m_AssetManger = m_Project->GetRuntimeAssetManger();
	Rynex::Renderer::Init();
	int rendererMode = Rynex::Renderer::CallFace_Back | Rynex::Renderer::Death_Buffer | Rynex::Renderer::A_Buffer;
	Rynex::Renderer::SetMode(rendererMode);

#if RY_TEST_ENITY

#pragma region CameraEnitity

	{
		Rynex::Entity cameraE = m_AktiveScene->CreateEntity("Camera");
		Rynex::CameraComponent cameraC = cameraE.AddComponent<Rynex::CameraComponent>();
		cameraC.Primary = true;
		cameraC.FixedAspectRotaion = false;
		cameraC.Camera.SetProjectionType(Rynex::SceneCamera::ProjectionType::Orthographic);
		if (!cameraE.HasComponent<Rynex::TransformComponent>())
			cameraE.AddComponent<Rynex::TransformComponent>();
		Rynex::TransformComponent& transformC = cameraE.GetComponent<Rynex::TransformComponent>();
		transformC.Transaltion = { -0.0f, 0.0f, 0.0f };
		transformC.Scale = { 1.0f, 1.0f, 1.0f };


		if (!cameraE.HasComponent<Rynex::TransformComponent>())
			cameraE.AddComponent<Rynex::TransformComponent>();
		Rynex::Matrix4x4Component& matrix4C = cameraE.GetComponent<Rynex::Matrix4x4Component>();
		matrix4C.Matrix4x4 = transformC.GetTransform();
		matrix4C.GlobleMatrix4x4 = transformC.GetTransform();
	}

#pragma endregion


#pragma region BackgroundEntity

	{
		Rynex::Entity entityE = m_AktiveScene->CreateEntity("Entity");
		Rynex::SpriteRendererComponent& spriteC = entityE.AddComponent<Rynex::SpriteRendererComponent>();
		spriteC.Color = { 0.8f, 0.2f, 0.3f, 1.0f };
		if (!entityE.HasComponent<Rynex::TransformComponent>())
			entityE.AddComponent<Rynex::TransformComponent>();
		Rynex::TransformComponent& transformC = entityE.GetComponent<Rynex::TransformComponent>();
		transformC.Transaltion = { -1.0f, 0.0f, 0.5f };
		transformC.Scale = { 0.8f, 0.8f, 1.0f };

		if (!entityE.HasComponent<Rynex::TransformComponent>())
			entityE.AddComponent<Rynex::TransformComponent>();
		Rynex::Matrix4x4Component& matrix4C = entityE.GetComponent<Rynex::Matrix4x4Component>();
		matrix4C.Matrix4x4 = transformC.GetTransform();
		matrix4C.GlobleMatrix4x4 = transformC.GetTransform();
	}

	{
		Rynex::Entity entityE = m_AktiveScene->CreateEntity("Entity");
		Rynex::SpriteRendererComponent& spriteC = entityE.AddComponent<Rynex::SpriteRendererComponent>();
		spriteC.Color = { 0.2f, 0.3f, 0.8f, 1.0f };
		if (!entityE.HasComponent<Rynex::TransformComponent>())
			entityE.AddComponent<Rynex::TransformComponent>();
		Rynex::TransformComponent& transformC = entityE.GetComponent<Rynex::TransformComponent>();
		transformC.Transaltion = { 0.5f, 0.5f, 0.5f };
		transformC.Scale = { 0.5f, 0.75f, 1.0f };
		
		if (!entityE.HasComponent<Rynex::TransformComponent>())
			entityE.AddComponent<Rynex::TransformComponent>();
		Rynex::Matrix4x4Component& matrix4C = entityE.GetComponent<Rynex::Matrix4x4Component>();
		matrix4C.Matrix4x4 = transformC.GetTransform();
		matrix4C.GlobleMatrix4x4 = transformC.GetTransform();
	}

	{
		Rynex::Entity entityE = m_AktiveScene->CreateEntity("Entity");
		Rynex::SpriteRendererComponent& spriteC = entityE.AddComponent<Rynex::SpriteRendererComponent>();
		spriteC.Texture = Rynex::TextureImporter::LoadTexture("Assets/textures/Checkerboard.png");
		spriteC.Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (!entityE.HasComponent<Rynex::TransformComponent>())
			entityE.AddComponent<Rynex::TransformComponent>();
		Rynex::TransformComponent& transformC = entityE.GetComponent<Rynex::TransformComponent>();
		transformC.Transaltion = { 0.0f, 0.0f,-0.1f };
		transformC.Scale = { 10.0f, 10.0f, 1.0f };

		if (!entityE.HasComponent<Rynex::TransformComponent>())
			entityE.AddComponent<Rynex::TransformComponent>();
		Rynex::Matrix4x4Component& matrix4C = entityE.GetComponent<Rynex::Matrix4x4Component>();
		matrix4C.Matrix4x4 = transformC.GetTransform();
		matrix4C.GlobleMatrix4x4 = transformC.GetTransform();
	}

	

	for (float x = -4.5f; x < 5.0f; x += 0.5f)
	{
		for (float y = -4.5f; y < 5.0f; y += 0.5f)
		{

			Rynex::Entity entityE = m_AktiveScene->CreateEntity("Entity");
			Rynex::SpriteRendererComponent& spriteC = entityE.AddComponent<Rynex::SpriteRendererComponent>();
			spriteC.Color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f };
			if (!entityE.HasComponent<Rynex::TransformComponent>())
				entityE.AddComponent<Rynex::TransformComponent>();
			Rynex::TransformComponent& transformC = entityE.GetComponent<Rynex::TransformComponent>();
			transformC.Transaltion = { x, y, 0.0f };
			transformC.Scale = { 0.45f, 0.45f, 1.0f };

			if (!entityE.HasComponent<Rynex::TransformComponent>())
				entityE.AddComponent<Rynex::TransformComponent>();
			Rynex::Matrix4x4Component& matrix4C = entityE.GetComponent<Rynex::Matrix4x4Component>();
			matrix4C.Matrix4x4 = transformC.GetTransform();
			matrix4C.GlobleMatrix4x4 = transformC.GetTransform();

		}
	}
#pragma endregion

#endif

}

void Sandbox2D::OnDetach()
{
	m_AssetManger.reset();
	m_Project.reset();
	m_AktiveScene.reset();
	m_Framebuffer.reset();
	m_ChekbordTex.reset();
	m_SquareVA.reset();
	m_BlueShader.reset();
	RY_WARN("Sandbox2D::OnDetach!");
}

void Sandbox2D::OnUpdate(Rynex::TimeStep ts)
{
	m_CameraController.OnUpdate(ts);
	m_AktiveScene->OnRenderRuntime(m_Framebuffer, 0);
}

void Sandbox2D::OnImGuiRender()
{
	static bool dokingEnabled = true;

	if (dokingEnabled)
	{
		static bool dokingSpaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;

		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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


		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dokingSpaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		ImGuiViewPortRender();
		ImGui::End();
	}
	else
	{
		ImGuiViewPortRender();
	}

}


void Sandbox2D::ImGuiViewPortResize(const glm::uvec2& vPSize)
{
	if (m_ViewPortSize != vPSize)
	{
		glm::uvec2 size = { vPSize.x , vPSize.y };
		m_Framebuffer->Resize(size.x, size.y);
		// m_FinaleImage->Resize(size.x, size.y);
		m_ViewPortSize = vPSize;
		m_AktiveScene->OnViewportResize(size.x, size.y);
	}
}

void Sandbox2D::ImGuiViewPortRender()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0 ,0 });
	ImGui::Begin("Viewport");
	ImVec2 viewportPannelSize = ImGui::GetContentRegionAvail();
	ImGuiViewPortResize({ viewportPannelSize.x,viewportPannelSize.y });
	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0);
	ImGui::Image(reinterpret_cast<void*>(textureID), viewportPannelSize, ImVec2(0, 1), ImVec2(1, 0));
	
	ImGui::End();
	ImGui::PopStyleVar();
}

void Sandbox2D::OnEvent(Rynex::Event& e)
{
	m_CameraController.OnEnvent(e);

}


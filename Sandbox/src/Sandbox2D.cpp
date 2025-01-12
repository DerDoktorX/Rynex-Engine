#include <rypch.h>
#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

#include <Rynex/Scripting/ScriptingEngine.h>
#include <Rynex/Scene/Components.h>
#include <Rynex/Serializers/SceneSerializer.h>


#define RY_TEST_ENITY 0
#define RY_ENABLE_GUI 1
Sandbox2D::Sandbox2D()
	: Layer("Sanbox2D")
	, m_CameraController((1280.0f / 720.0f), true)
{
}

void Sandbox2D::OnAttach()
{
	Rynex::Window* window = &Rynex::Application::Get().GetWindow();
	m_ViewPortSize = { window->GetWidth() ,window->GetHeight()};
	m_WindowPos = { window->GetPosX() ,window->GetPosY() };
	m_MousePos = { window->GetMousePosX() ,window->GetMousePosY() };
    m_AktiveScene = Rynex::CreateRef<Rynex::Scene>();

	auto cLA = Rynex::Application::Get().GetSpecification().CommandLineArgs;
	if (cLA.Count > 1)
	{
		auto projFilePath = cLA[1];
		if (Rynex::Project::Load(projFilePath))
		{
			Rynex::ScriptingEngine::Init();
			std::filesystem::path startScene = Rynex::Project::GetActive()->GetConfig().StartScene;
			m_Project = Rynex::Project::GetActive();
			m_AssetManger = m_Project->GetRuntimeAssetManger();
			if (startScene.string() != "")
			{
				Rynex::SceneSerializer serialzer(m_AktiveScene);
				serialzer.Deserialize(startScene.string(), false);
			}
		}
	}
	else
	{
		std::filesystem::path filepath = "";
		do
		{
			filepath = Rynex::FileDialoges::OpenFile("Rynex Project (*.rproj)\0*.rproj\0");
		} while (filepath == "");

		if (Rynex::Project::Load(filepath))
		{
			Rynex::ScriptingEngine::Init();
			std::filesystem::path startScene = Rynex::Project::GetActive()->GetConfig().StartScene;
			m_Project = Rynex::Project::GetActive();
			m_AssetManger = m_Project->GetRuntimeAssetManger();
			if (startScene.string() != "")
			{
				Rynex::SceneSerializer serialzer(m_AktiveScene);
				serialzer.Deserialize(startScene.string(), false);
			}
		}

	}
	m_AktiveScene->OnRuntimStart();

	Rynex::FramebufferSpecification fbSpec = {
		1280ul, 720ul,
		{
			{Rynex::TextureFormat::RGBA8},
			{Rynex::TextureFormat::RED_INTEGER},
			{Rynex::TextureFormat::Depth24Stencil8}
		}
	};
	std::vector<Rynex::FramebufferTextureSpecification> framTexSpec;


    m_Framebuffer = Rynex::Framebuffer::Create(fbSpec);


    
	Rynex::Renderer::Init();
	int rendererMode = Rynex::Renderer::CallFace_None | Rynex::Renderer::Death_Buffer | Rynex::Renderer::A_Buffer; // | Rynex::Renderer::WireFrame;
	Rynex::Renderer::SetMode(rendererMode);

	m_AktiveScene->OnViewportResize(m_ViewPortSize.x, m_ViewPortSize.y);
	Rynex::RenderCommand::SetViewPort(0, 0, m_ViewPortSize.x, m_ViewPortSize.y);
	m_Framebuffer->Resize(m_ViewPortSize.x, m_ViewPortSize.y);
#if RY_ENABLE_GUI-1
	

	// 0001 1011  0000 0000
	float vertecies[] = {
		0.75f,  0.75f,
		1.00f,  0.75f,
		0.75f,  1.00f,
		1.00f,  1.00f,
	};

	Rynex::Ref<Rynex::VertexBuffer> vertexBuffer = Rynex::VertexBuffer::Create(&vertecies, sizeof(float)*8, Rynex::BufferDataUsage::StaticDraw );
	vertexBuffer->SetLayout({
		{Rynex::ShaderDataType::Float2, "a_Vertex"},
	});
	
	uint32_t inidcies[] = {
		0u, 1u, 2u, 3u
	};
	Rynex::Ref<Rynex::IndexBuffer> indexBuffer = Rynex::IndexBuffer::Create(inidcies, 4, Rynex::BufferDataUsage::StaticDraw);
	m_FullScreenQuade = Rynex::VertexArray::Create();
	m_FullScreenQuade->AddVertexBuffer(vertexBuffer);
	m_FullScreenQuade->SetIndexBuffer(indexBuffer);
	m_FullScreenQuade->SetPrimitv(Rynex::VertexArray::Primitv::TraingleStrips);

	
	m_FullScreenShader = Rynex::ShaderImporter::LoadShader("../Rynex-Editor/Editor-Assets/shaders/FullScreeShader.glsl", "FullScreeShader", false);

	RY_ASSERT(m_FullScreenShader && indexBuffer && vertexBuffer && m_FullScreenQuade, "Screen Rendering ERROR");
#endif

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
		Rynex::
			& matrix4C = cameraE.GetComponent<Rynex::ModelMatrixComponent>();
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
		Rynex::ModelMatrixComponent& matrix4C = entityE.GetComponent<Rynex::ModelMatrixComponent>();
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
		Rynex::ModelMatrixComponent& matrix4C = entityE.GetComponent<Rynex::ModelMatrixComponent>();
		matrix4C.Matrix4x4 = transformC.GetTransform();
		matrix4C.GlobleMatrix4x4 = transformC.GetTransform();
	}

	{
		Rynex::Entity entityE = m_AktiveScene->CreateEntity("Entity");
		Rynex::SpriteRendererComponent& spriteC = entityE.AddComponent<Rynex::SpriteRendererComponent>();
		spriteC.Texture = Rynex::TextureImporter::LoadTexture("../Rynex-Editor/Editor-Assets/textures/Checkerboard.png");
		spriteC.Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (!entityE.HasComponent<Rynex::TransformComponent>())
			entityE.AddComponent<Rynex::TransformComponent>();
		Rynex::TransformComponent& transformC = entityE.GetComponent<Rynex::TransformComponent>();
		transformC.Transaltion = { 0.0f, 0.0f,-0.1f };
		transformC.Scale = { 10.0f, 10.0f, 1.0f };

		if (!entityE.HasComponent<Rynex::TransformComponent>())
			entityE.AddComponent<Rynex::TransformComponent>();
		Rynex::ModelMatrixComponent& matrix4C = entityE.GetComponent<Rynex::ModelMatrixComponent>();
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
			Rynex::ModelMatrixComponent& matrix4C = entityE.GetComponent<Rynex::ModelMatrixComponent>();
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
	// m_CameraController.OnUpdate(ts);
	if (m_FirstFrame)
	{
		ts = Rynex::TimeStep();
		m_FirstFrame = false;
	}
	m_AktiveScene->OnUpdateRuntime(ts);
	m_AktiveScene->OnRenderRuntime(m_Framebuffer, 0);
}

void Sandbox2D::OnImGuiRender()
{
#if RY_ENABLE_GUI
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
#else


	
#if 0

		glm::vec2 mausOffset = { m_MousePos.x - m_WindowPos.x, m_MousePos.y - m_WindowPos.y };
		glm::vec2 minBound = m_WindowPos;
		minBound.x += mausOffset.x;
		minBound.y += mausOffset.y;

		glm::vec2 maxBound = { minBound.x + m_ViewPortSize.x, minBound.y + m_ViewPortSize.y };
		m_WindowBounds[0] = { minBound.x,  minBound.y };
		m_WindowBounds[1] = { maxBound.x,  maxBound.y };

		m_MousePixelPos = m_MousePos;
		m_MousePixelPos -= m_WindowBounds[0];
		glm::vec2 viewPortSize = m_WindowBounds[1] - m_WindowBounds[0];
		m_MousePixelPos.y = viewPortSize.y - m_MousePixelPos.y;
		glm::ivec2 mause = m_MousePixelPos;
		if (mause.x >= 0 && mause.y >= 0 && mause.x < (int)viewPortSize.x && mause.y < (int)viewPortSize.y)
		{
			m_AktiveScene->SetMousPixelPos(mause);
			m_Hovered = true;
		}
		else
		{
			m_Hovered = false;
		}
		m_AktiveScene->SetHoverViewPort(m_Hovered);
#else
	
	

#endif
	
	Rynex::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	Rynex::RenderCommand::Clear();
	m_FullScreenShader->Bind();
	m_Framebuffer->BindColorAttachment(0, 0);
	Rynex::RenderCommand::DrawStripsMesh(m_FullScreenQuade, 4);
	m_FullScreenShader->UnBind();
	m_FullScreenQuade->UnBind();

	ImGui::Begin("test");
	ImGui::Text("Hi");
	ImGui::End();
#endif
}


void Sandbox2D::ImGuiViewPortResize(const glm::uvec2& vPSize)
{
	bool result = m_ViewPortSize != vPSize;
	m_AktiveScene->SetWindowResize(result);
	if (result)
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
	Rynex::EventDispatcher dispatcher(e);
	m_CameraController.OnEnvent(e);
	dispatcher.Dispatch<Rynex::WindowMovedEvent>(RY_BIND_EVENT_FN(Sandbox2D::OnWindowMove));
	dispatcher.Dispatch<Rynex::MouseMoveEvent>(RY_BIND_EVENT_FN(Sandbox2D::OnMouseMove));
	dispatcher.Dispatch<Rynex::WindowResizeEvent>(RY_BIND_EVENT_FN(Sandbox2D::OnResize));
	dispatcher.Dispatch<Rynex::WindowFocuseEvent>(RY_BIND_EVENT_FN(Sandbox2D::OnFocuse));
	dispatcher.Dispatch<Rynex::WindowLostFocuseEvent>(RY_BIND_EVENT_FN(Sandbox2D::OnFocuseLost));
	dispatcher.Dispatch<Rynex::WindowCurserEnterEvent>(RY_BIND_EVENT_FN(Sandbox2D::OnCurserEnter));
	dispatcher.Dispatch<Rynex::WindowCurserLeaveEvent>(RY_BIND_EVENT_FN(Sandbox2D::OnCurserLeave));
}

bool Sandbox2D::OnResize(Rynex::WindowResizeEvent& e)
{
	glm::uvec2 size = { e.GetWidth(), e.GetHeight() };
	
	if (m_ViewPortSize != size)
	{
		m_Resized = true;
		Rynex::RenderCommand::SetViewPort(0, 0, size.x, size.y);
		m_Framebuffer->Resize(size.x, size.y);
		m_ViewPortSize = size;
		m_AktiveScene->OnViewportResize(size.x, size.y);
		return true;
	}
	return false;
}

bool Sandbox2D::OnMouseMove(Rynex::MouseMoveEvent& e)
{
	m_MousePos = { e.GetX(), e.GetY() };
	return true;
}

bool Sandbox2D::OnWindowMove(Rynex::WindowMovedEvent& e)
{
	m_WindowPos = { e.GetPosX(), e.GetPosY() };
	
	return true;
}

bool Sandbox2D::OnFocuse(Rynex::WindowFocuseEvent& e)
{
	m_Focuse = true;
	return true;
}

bool Sandbox2D::OnFocuseLost(Rynex::WindowLostFocuseEvent& e)
{
	m_Focuse = false;
	return true;
}

bool Sandbox2D::OnCurserEnter(Rynex::WindowCurserEnterEvent& e)
{
	m_Hovered = true;
	m_AktiveScene->SetHoverViewPort(m_Hovered);
	return true;
}

bool Sandbox2D::OnCurserLeave(Rynex::WindowCurserLeaveEvent& e)
{
	m_Hovered = false;
	m_AktiveScene->SetHoverViewPort(m_Hovered);
	return true;
}


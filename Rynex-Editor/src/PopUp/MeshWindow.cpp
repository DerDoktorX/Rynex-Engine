#include "rypch.h"
#include "MeshWindow.h"

#include <Rynex/Asset/Import/ShaderImporter.h>
#include <Rynex/Renderer/Materials/ShaderMaterial.h>

#include <imgui/imgui.h>

namespace Rynex {

	MeshWindow::MeshWindow()
	{
	}

	MeshWindow::~MeshWindow()
	{
	}

	void MeshWindow::OnAtache()
	{
		m_WorkingScene = CreateRef<Scene>();
		m_WorkingScene->SetBackgroundColor({ 0.7f,1.0f, 0.7f, 1.0f });

		m_EditorCamera = CreateRef<EditorCamera>(30.0f, 1.778f, 0.0001, 500.0f);

		m_EntityWorking = m_WorkingScene->CreateEntity("Working-Mesh");
		// m_Grid = m_WorkingScene->CreateEntity("Grid");
		m_Ground = m_WorkingScene->CreateEntity("Ground");

		{
			if (!m_Ground.HasComponent<GeomtryComponent>())
				m_Ground.AddComponent<GeomtryComponent>();
			GeomtryComponent& geomtryC = m_Ground.GetComponent<GeomtryComponent>();
			Ref<VertexArray> va = VertexArray::Create();

			float vbVertexData[] = {
				1.0, 0.0,  1.0,		0.0, 0.0,		0.0, 1.0, 0.0,
				1.0, 0.0, -1.0,		0.0, 1.0,		0.0, 1.0, 0.0,
			   -1.0, 0.0,  1.0,		1.0, 0.0,		0.0, 1.0, 0.0,
			   -1.0, 0.0, -1.0,		1.0, 1.0,		0.0, 1.0, 0.0
			};

			Ref<VertexBuffer> vb = VertexBuffer::Create(vbVertexData, 32 * sizeof(float), BufferDataUsage::StaticDraw);
			vb->SetLayout({
				{ShaderDataType::Float3, "a_Postion"},
				{ShaderDataType::Float3, "a_UV"},
				{ShaderDataType::Float3, "a_Normals"},
			});
			uint32_t ibVertexData[] = {
				0,1,2,3
			};

			Ref<IndexBuffer> ib = IndexBuffer::Create(ibVertexData, 4 * sizeof(uint32_t), BufferDataUsage::StaticDraw);
			va->AddVertexBuffer(vb);
			va->SetIndexBuffer(ib);
			va->SetPrimitv(VertexArray::Primitv::TraingleStrips);
			if (!m_Ground.HasComponent<MaterialComponent>())
				m_Ground.AddComponent<MaterialComponent>();
			MaterialComponent& materialC = m_Ground.GetComponent<MaterialComponent>();

			ShaderMaterialDefaultNames names(
				{ ShaderDataType::Float3,	"u_Color" },
				{ ShaderDataType::Float,	"u_Alpha" },
				{ ShaderDataType::Float4x4, "u_ModelMatrix" }
			);
			materialC.Materiel = Material::CreateShader(names);
			materialC.Materiel->SetShader( ShaderImporter::LoadShader("../Rynex-Editor/Editor-Assets/shaders/3DLigthe.glsl", "3DLigthe.glsl", false));
			
			materialC.Materiel->SetFlage(Renderer::A_Buffer | Renderer::Death_Buffer | Renderer::CallFace_None);
			materialC.Materiel->SetColor({ 0.5,0.5,0.5 });
			
		}

		{
			if (!m_EntityWorking.HasComponent<GeomtryComponent>())
				m_EntityWorking.AddComponent<GeomtryComponent>();
			GeomtryComponent& geomtryC = m_EntityWorking.GetComponent<GeomtryComponent>();

			Ref<VertexArray> va = VertexArray::Create();

			float vbVertexData[] = {
				1.0, -1.0, 0.0,		0.0, 0.0,		0.0, 1.0, 0.0,
				0.0,  1.0, 0.0,		0.0, 1.0,		0.0, 1.0, 0.0,
			   -1.0, -1.0, 0.0,		1.0, 0.0,		0.0, 1.0, 0.0,
			};
			m_VertexData.resize( 32 * sizeof(float));
			unsigned char* vertexDataPtr = m_VertexData.data();
			*vertexDataPtr = *vbVertexData;

			Ref<VertexBuffer> vb = VertexBuffer::Create(vertexDataPtr, 24 * sizeof(float), BufferDataUsage::StaticDraw);
			vb->SetLayout({
				{ShaderDataType::Float3, "a_Postion"},
				{ShaderDataType::Float3, "a_UV"},
				{ShaderDataType::Float3, "a_Normals"},
				});
			std::vector<uint32_t> m_IndexData = {
				0,1,2
			};

			Ref<IndexBuffer> ib = IndexBuffer::Create(m_IndexData.data(), 3 * sizeof(uint32_t), BufferDataUsage::StaticDraw);
			va->AddVertexBuffer(vb);
			va->SetIndexBuffer(ib);
			va->SetPrimitv(VertexArray::Primitv::TraingleStrips);
			if (!m_EntityWorking.HasComponent<MaterialComponent>())
				m_EntityWorking.AddComponent<MaterialComponent>();
			MaterialComponent& materialC = m_EntityWorking.GetComponent<MaterialComponent>();
			materialC.Materiel->SetShader( ShaderImporter::LoadShader("../Rynex-Editor/Editor-Assets/shaders/3DLigthe.glsl", "3DLigthe.glsl", false));
			materialC.Materiel->SetFlage(Renderer::A_Buffer | Renderer::Death_Buffer | Renderer::CallFace_None);
			materialC.Materiel->SetColor({ 0.25,0.25, 0.25 });
		}

		{
			FramebufferSpecification fbSpec;
			fbSpec.Width = 1280;
			fbSpec.Height = 720;
			

			fbSpec.Attachments.Attachments.reserve(3);
			fbSpec.Attachments.Attachments.emplace_back(TextureFormat::RGBA8);
			fbSpec.Attachments.Attachments.emplace_back(TextureFormat::RED_INTEGER);
			fbSpec.Attachments.Attachments.emplace_back(TextureFormat::Depth24Stencil8);
				
			
		
			m_FrameBuffer = Framebuffer::Create(fbSpec);

		}
	}

	void MeshWindow::OnDettache()
	{
	}

	void MeshWindow::OnUpdate()
	{
		m_WorkingScene->OnRenderEditor(m_FrameBuffer, m_EditorCamera);
	}

	void MeshWindow::OnImGuiRender()
	{
		if (!m_Open)
			return;

		if (ImGui::BeginPopupModal("Settings-FrameBuffer", &m_Open, ImGuiWindowFlags_MenuBar))
		{
			ImVec2 windowSize = ImGui::GetWindowSize();
			
			if (m_ScreenSize != *((glm::uvec2*)&windowSize))
			{
				m_ScreenSize = { windowSize.x, m_ScreenSize.y };
				m_FrameBuffer->Resize(m_ScreenSize.x, m_ScreenSize.y);
				m_EditorCamera->SetViewportSize(m_ScreenSize.x, m_ScreenSize.y);

			}
			ImGui::Image(reinterpret_cast<void*>(m_FrameBuffer->GetColorAttachmentRendererID(0)), ImVec2{ (float)m_ScreenSize.x, (float)m_ScreenSize.y }, ImVec2(0, 1), ImVec2(1, 0));
		}

	}

	void MeshWindow::OnEvent(Event& e)
	{
		m_EditorCamera->OnEvent(e);
	}

	void MeshWindow::OnOpen(AssetHandle handle)
	{
		m_Open = true;
	}

	void MeshWindow::OnClose()
	{
		m_Open = false;
	}
}
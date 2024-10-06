#include "rypch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Rynex/Renderer/Rendering/Renderer2D.h"
#include "Rynex/Renderer/Rendering/Renderer3D.h"

namespace Rynex {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RY_CORE_MEMORY_ALICATION("m_SceneData", "Renderer::Init || namespace Rynex", Renderer::SceneData);
		RY_CORE_INFO("Renderer::Init Start!");
		RY_PROFILE_FUNCTION();
		RenderCommand::Init();
		BufferAPI::Init();
		Renderer2D::Init();
		Renderer3D::Init();
		RY_CORE_INFO("Renderer::Init Finished!");
	}

	void Renderer::Shutdown()
	{
		RY_PROFILE_FUNCTION();
		BufferAPI::Shutdown();
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
		RY_CORE_MEMORY_FREE("m_SceneData", "Renderer::Shutdown");
		delete m_SceneData;
	}

	void Renderer::OnWindowsResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewPort(0,0, width,height);
	}

	void Renderer::BeginScene(OrthograficCamera& camera)
	{
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Tranform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexedMesh(vertexArray);
	}

}
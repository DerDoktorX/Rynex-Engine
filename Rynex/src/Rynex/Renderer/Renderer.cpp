#include "rypch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Rynex/Renderer/Renderer2D.h"
namespace Rynex {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
		Renderer2D::Init();
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
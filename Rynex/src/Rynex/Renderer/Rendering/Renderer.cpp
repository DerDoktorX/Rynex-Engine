#include "rypch.h"
#include "Renderer.h"

#include "Rynex/Renderer/Rendering/Renderer2D.h"
#include "Rynex/Renderer/Rendering/Renderer3D.h"
#if RY_ANABLE_RENDERER_ICONE
#include "Rynex/Renderer/Rendering/RendererIcons.h"
#endif

namespace Rynex {

	void Renderer::Init()
	{
		RY_CORE_MEMORY_ALICATION("m_SceneData", "Renderer::Init || namespace Rynex", Renderer::SceneData);
		m_SceneData = CreateRef<SceneData>();

		RY_CORE_INFO("Renderer::Init Start!");
		RY_PROFILE_FUNCTION();

		RenderCommand::Init();
		BufferAPI::Init();
		Renderer2D::Init();
		Renderer3D::Init();

		

#if RY_ANABLE_RENDERER_ICONE
		RendererIcons::Init();
#endif

		RY_CORE_INFO("Renderer::Init Finished!");
	}

	void Renderer::InitEditor()
	{
		RY_CORE_MEMORY_ALICATION("m_SceneData", "Renderer::Init || namespace Rynex", Renderer::SceneData);
		RY_CORE_INFO("Renderer::Init Start!");
		RY_PROFILE_FUNCTION();
	
		Renderer2D::InitEditor();

#if RY_ANABLE_RENDERER_ICONE
		RendererIcons::Init();
#endif
		RY_CORE_INFO("Renderer::Init Finished!");
	}

	void Renderer::Shutdown()
	{
		RY_PROFILE_FUNCTION();
		RenderCommand::Shutdown();
		BufferAPI::Shutdown();
		Renderer2D::Shutdown();
		Renderer3D::Shutdown();
		Texture::Shutdown();
		Shader::Shutdown();
		m_SceneData.reset();
		RY_CORE_MEMORY_FREE("m_SceneData", "Renderer::Shutdown");
		
	}

	void Renderer::ShutdownEditor()
	{
#if RY_ANABLE_RENDERER_ICONE
		RendererIcons::Shutdown();
#endif
		Renderer2D::ShutdownEditor();
	}

	void Renderer::OnWindowsResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewPort(0,0, width,height);
	}
#if RY_OLD_RENDER_SYSTEM
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
		shader->SetMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_Tranform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexedMesh(vertexArray);
	}
#else

	void Renderer::SetFrambuffer(const Ref<Framebuffer>& frambuffer)
	{
		m_SceneData->RenderPass.SetFrambuffer(frambuffer);

	}

	void Renderer::ClearDrawCallsPass()
	{
		m_SceneData->RenderPass.ClearDrawCallList();
	}

	

	

	template<typename Func, typename ...Args>
	inline void Renderer::AddDrawCall(Func&& func, Args && ...args)
	{
		RY_CORE_ASSERT(indexPass < m_SceneData->RenderPass.size());
		m_SceneData->RenderPass.AddDrawCall3D(func, args);
	}



	template<typename Func, typename... Args>
	inline void Renderer::DrawObjects(Func&& func, Args&&... args)
	{
		m_SceneData->RenderPass.DrawObjects(func, args);
	}



	

#endif
	int Renderer::GetMode()
	{
		return RenderCommand::GetMode();
	}

	void Renderer::SetMode(int mode)
	{
		RenderCommand::SetMode(mode);
	}
	

	

	bool Renderer::IsInit()
	{
		return m_SceneData != nullptr;
	}

	

}
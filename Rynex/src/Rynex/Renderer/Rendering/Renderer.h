#pragma once

#include "Rynex/Renderer/RenderCommand.h"
#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Renderer/Camera/OrthograficCamera.h"

namespace Rynex {
	
	class Renderer 
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnWindowsResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthograficCamera& camera);
		static void EndScene();

	
		static void Submit(const Ref<Shader>& shader ,const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
		
}
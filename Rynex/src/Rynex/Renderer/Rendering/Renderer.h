#pragma once

#include "Rynex/Renderer/RenderCommand.h"
#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Renderer/Camera/OrthograficCamera.h"
#include "Rynex/Renderer/Rendering/RendererPass.h"

namespace Rynex {

	class RYNEX_API Renderer
	{
	public:
		enum Mode {
			None = 0,
			CallFace_None = BIT(0),
			CallFace_Front = BIT(1),
			CallFace_Back = BIT(2),
			CallFace_FrontBack = BIT(3),
			WireFrame = BIT(4),
			A_Buffer = BIT(5),
			Death_Buffer = BIT(6),
			Ligths = BIT(7),
			CastShadows = BIT(8),
			EmiteShadows = BIT(9),
		};

	public:

		static void Init();
		static void InitEditor();
		static void Shutdown();
		static void ShutdownEditor();
		
		static void OnWindowsResize(uint32_t width, uint32_t height);
		

		static void BeginScene(OrthograficCamera& camera);
		static void EndScene();

		

		static void Submit(const Ref<Shader>& shader ,const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		
		
		static void SetFrambuffer(const Ref<Framebuffer>& frambuffer);

		template<typename Func, typename... Args>
		inline static void AddDrawCall(Func&& func, Args&&... args);



		inline static void ClearDrawCallsPass();


		template<typename Func, typename ...Args>
		inline static void DrawObjects(Func&& func, Args && ...args);

		static int GetMode();
		static void SetMode(int mode);
		static bool IsInit();


		

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:

		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
			RendererPass RenderPass;
		};


		inline static Ref<SceneData> m_SceneData;
	};


}
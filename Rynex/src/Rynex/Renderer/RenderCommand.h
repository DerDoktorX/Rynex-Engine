#pragma once

#include "RendererAPI.h"

namespace Rynex {
	class RYNEX_API RenderCommand
	{
	public:
		inline static void Init()
		{
			// RY_CORE_MEMORY_ALICATION("s_RendererAPI", "RenderCommand::Init || namespace Rynex", OpenGLRendererAPI);
			s_RendererAPI->Init();
		}

		inline static void Shutdown()
		{
			// RY_CORE_MEMORY_FREE("s_RendererAPI", "RenderCommand::Shutdown");
			delete s_RendererAPI;
		}

		inline static void CreateComputePipline(glm::vec3& size)
		{
			s_RendererAPI->CreateComputePipline(size);
		}

		inline static void SetViewPort(uint32_t x, uint32_t y, uint32_t withe, uint32_t heigth)
		{
			s_RendererAPI->SetViewPort(x, y, withe, heigth);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void AktivePolyGunMode(bool active = true)
		{
			s_RendererAPI->AktivePolyGunMode(active);
		}

		inline static void SetDethTest(bool active = true)
		{
			s_RendererAPI->SetDethTest(active);
		}

		inline static void SetMode(int mode)
		{
			s_RendererAPI->SetMode(mode);
		}

		inline static int GetMode()
		{
			return s_RendererAPI->GetMode();
		}

		inline static bool IsModeEqual(int mode)
		{
			return s_RendererAPI->IsModeEqual(mode);
		}

		inline static void SetFace(CallFace callFace = CallFace::None)
		{
			s_RendererAPI->SetFace(callFace);
		}

		// This clears color and death
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		// This clears only death
		inline static void ClearDepth()
		{
			s_RendererAPI->ClearDepth();
		}

		inline static void DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0)
		{
			s_RendererAPI->DrawIndexedMesh(vertexArray, indexcount);
		};
		
		inline static void DrawIndexedMeshInstecing(uint32_t instecing, const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0)
		{
			s_RendererAPI->DrawIndexedMeshInstecing(instecing, vertexArray, indexcount);
		};

		inline static void DrawStripsMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawStripsMesh(vertexArray, indexCount);
		};

		inline static void DrawIndexedLine(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexedLine(vertexArray, indexCount);
		};


		inline static void DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexedLineLoop(vertexArray, indexCount);
		};

		inline static void DrawIndexedPoints(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexedPoints(vertexArray, indexCount);
		};

		inline static void DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawPatches(vertexArray, indexCount);
		};

		inline static void DispatcheCompute(const glm::vec<3, uint32_t>& groups)
		{
			s_RendererAPI->DispatcheCompute(groups);
		};

		inline static void DrawError()
		{
			s_RendererAPI->DrawError();
		};

	
	private:
		static RendererAPI* s_RendererAPI;
	};
}
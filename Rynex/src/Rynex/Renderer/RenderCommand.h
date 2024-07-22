#pragma once

#include "RendererAPI.h"

namespace Rynex {
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
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

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0)
		{
			s_RendererAPI->DrawIndexedMesh(vertexArray, indexcount);
		};

		inline static void DrawBatcheingIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0)
		{
			s_RendererAPI->DrawBatcheingIndexedMesh(vertexArray, indexcount);
		};

		inline static void DrawStripsMesh(const Ref<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawStripsMesh(vertexArray);
		};

		inline static void DrawIndexedLine(const Ref<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexedLine(vertexArray);
		};


		inline static void DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexedLineLoop(vertexArray, indexCount);
		};

		inline static void ComputePipline()
		{
			s_RendererAPI->ComputePipline();
		};

	private:
		static RendererAPI* s_RendererAPI;
	};
}
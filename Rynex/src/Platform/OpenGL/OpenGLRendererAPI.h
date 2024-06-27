#pragma once

#include "Rynex/Renderer/RendererAPI.h"

namespace Rynex {
	class OpenGLRendererAPI : public RendererAPI
	{
	public:

		virtual void Init() override;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t withe, uint32_t heigth) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawBatcheingIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

		virtual void DrawStripsMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)override;


		virtual void DrawIndexedLine(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)override;
		virtual void DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

		//virtual void DrawIndexedPoints(const Ref<VertexArray>& vertexArray) override;
	};
}


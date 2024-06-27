#pragma once


#include <glm/glm.hpp>
#include "VertexArray.h"

namespace Rynex {
	class RendererAPI 
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual void Init() = 0;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t withe, uint32_t heigth) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawBatcheingIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void DrawStripsMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void DrawIndexedLine(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		//virtual void DrawIndexedPoints(const Ref<VertexArray>& vertexArray) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}
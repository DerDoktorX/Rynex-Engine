#pragma once


#include <glm/glm.hpp>
#include "Rynex/Renderer/API/VertexArray.h"

namespace Rynex {

	enum class CallFace {
		None = 0,
		Front,
		Back,
		FrontBacke
	};

	class RendererAPI 
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};

	public:
		virtual void Init() = 0;
		virtual void AktivePolyGunMode(bool aktiv) = 0;
		virtual void SetDethTest(bool aktiv) = 0;
		virtual void SetFace(CallFace calolFace) = 0;
#if 0
		virtual void SetBlendMode(bool aktiv) = 0;
		virtual void SetFront(bool aktiv) = 0;
#endif
		

		virtual void CreateComputePipline(glm::vec3& size) = 0;

		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t withe, uint32_t heigth) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawBatcheingIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void DrawIndexedPoints(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void DrawStripsMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void DrawIndexedLine(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		

		virtual void DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void ComputePipline() = 0;

		virtual void DrawError() = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}
#pragma once
#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Renderer/Rendering/RenderTarget.h>



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
		virtual void ShutDown() = 0;
		virtual void AktivePolyGunMode(bool aktiv) = 0;
		virtual void SetDethTest(bool aktiv) = 0;
		virtual void SetFace(CallFace calolFace) = 0;

		virtual void SetMode(int mode) = 0;
		virtual void SetModeForce(int mode) = 0;
		virtual int GetMode() = 0;
		virtual bool IsModeEqual(int mode) = 0;
		virtual void SetBiasGPU(float factor, float units) = 0;
		virtual void DisableBiasGPU() = 0;
		

		virtual void CreateComputePipline(glm::vec3& size) = 0;

		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t withe, uint32_t heigth) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void ClearNoDepth() = 0;
		virtual void ClearDepth() = 0;

		virtual void DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawIndexedMeshInstecing(uint32_t instecing, const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void DrawIndexedPoints(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void DrawStripsMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void DrawIndexedLine(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, uint32_t drawCount, void* indrictDraw, uint32_t indirectStrideSize) = 0;
		virtual void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, uint32_t drawCount, uint32_t indirectStrideSize) = 0;
		virtual void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, const Ref<IndirectBuffer>& indriectBuffer) = 0;
		virtual void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, const Ref<IndirectBuffer>& indriectBuffer, uint32_t drawCount) = 0;
		virtual void DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawElement(const Ref<VertexArray>& vertexArray, const Mesh::PerDrawObject& drawObject) = 0;
		

		virtual void DispatcheCompute(const glm::vec<3, uint32_t>& groups) = 0;

		virtual void DrawError() = 0;
		virtual void RestPipline() = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};
}
#pragma once

#include "RendererAPI.h"

namespace Rynex {
#if RY_RENDERER_API_INDEIPENDENT
	struct DrawRendererSpec;
#endif

	class RYNEX_API RenderCommand
	{
	public:
		inline static void Init()
		{
			// RY_CORE_MEMORY_ALICATION("s_RendererAPI", "RenderCommand::Init || namespace Rynex", OpenGLRendererAPI);

			s_RendererAPI->Init();
		};

		inline static void Shutdown()
		{
			// RY_CORE_MEMORY_FREE("s_RendererAPI", "RenderCommand::Shutdown");
			s_RendererAPI->ShutDown();
			delete s_RendererAPI;
			s_RendererAPI = nullptr;
		};

		inline static void CreateComputePipline(glm::vec3& size)
		{
			s_RendererAPI->CreateComputePipline(size);
		};

		inline static void SetViewPort(uint32_t x, uint32_t y, uint32_t withe, uint32_t heigth)
		{
			s_RendererAPI->SetViewPort(x, y, withe, heigth);
		};

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		};

		inline static void AktivePolyGunMode(bool active = true)
		{
			s_RendererAPI->AktivePolyGunMode(active);
		};

		inline static void SetDethTest(bool active = true)
		{
			s_RendererAPI->SetDethTest(active);
		};

		inline static void SetBiasGPU(float factor, float units)
		{
			s_RendererAPI->SetBiasGPU(factor, units);
		};

		inline static void DisableBiasGPU()
		{
			s_RendererAPI->DisableBiasGPU();
		};

		inline static void SetMode(int mode)
		{
			s_RendererAPI->SetMode(mode);
		};

		inline static void SetModeForce(int mode)
		{
			s_RendererAPI->SetModeForce(mode);
		};

		inline static int GetMode()
		{
			return s_RendererAPI->GetMode();
		};

		inline static bool IsModeEqual(int mode)
		{
			return s_RendererAPI->IsModeEqual(mode);
		};

		inline static void SetFace(CallFace callFace = CallFace::None)
		{
			s_RendererAPI->SetFace(callFace);
		};

		// This clears color and death
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		};

		inline static void ClearNoDepth()
		{
			s_RendererAPI->ClearNoDepth();
		};

		// This clears only death
		inline static void ClearDepth()
		{
			s_RendererAPI->ClearDepth();
		};

		inline static void DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0)
		{
			s_RendererAPI->DrawIndexedMesh(vertexArray, indexcount);
		};
		
		inline static void DrawIndexedMeshInstecing(uint32_t instecing, const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0)
		{
			s_RendererAPI->DrawIndexedMeshInstecing(instecing, vertexArray, indexcount);
		};


		inline static void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, uint32_t drawCount, void* indirectData, uint32_t indirectStrideSize)
		{
			s_RendererAPI->DrawMultyMeshIndriect(vertexArray, drawCount, indirectData, indirectStrideSize);
		};

		template<typename T>
		inline static void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, uint32_t drawCount, T* indirectData)
		{
			s_RendererAPI->DrawMultyMeshIndriect(vertexArray, drawCount, indirectData, sizeof(T));
		}

		inline static void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, uint32_t drawCount, uint32_t indirectStrideSize)
		{
			s_RendererAPI->DrawMultyMeshIndriect(vertexArray, drawCount, indirectStrideSize);
		};

		inline static void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, const Ref<IndirectBuffer>& indriectBuffer, uint32_t drawCount)
		{
			s_RendererAPI->DrawMultyMeshIndriect(vertexArray, indriectBuffer, drawCount);
		};

		inline static void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, const Ref<IndirectBuffer>& indriectBuffer)
		{
			s_RendererAPI->DrawMultyMeshIndriect(vertexArray, indriectBuffer);
		};

		inline static void DrawElement(const Ref<VertexArray>& vertexArray, const Mesh::PerDrawObject& drawObject)
		{
			s_RendererAPI->DrawElement(vertexArray, drawObject);
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

		inline static void RestPipline()
		{
			s_RendererAPI->RestPipline();
		};

		
#if RY_RENDERER_API_INDEIPENDENT

		

		inline static void DrawIndexed(const DrawRendererSpec& drawSpec)
		{
			s_RendererAPI->DrawIndexed(drawSpec);
		};

#endif

	private:
		static RendererAPI* s_RendererAPI;
	};
}
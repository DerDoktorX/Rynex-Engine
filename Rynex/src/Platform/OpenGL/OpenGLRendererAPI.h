#pragma once

#include <Rynex/Renderer/RendererAPI.h>

namespace Rynex {
	class AktiveBindlesTexture;
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void ShutDown() override;

		virtual void CreateComputePipline(glm::vec3& size) override;

		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t withe, uint32_t heigth) override;
		virtual void SetClearColor(const glm::vec4& color) override;

		static void BeginRecordCommandBuffer();
		static bool IsRecordCommandBuffer();

		static void EndeRecordCommandBuffer();

		void BindFrambuffer(uint32_t renderID);
		void BindShader(uint32_t renderID);
		void BindVertexArray(uint32_t renderID);


		void BindBuffer(uint32_t target, uint32_t renderID);
		void BindBufferSlot(uint32_t target, uint32_t slot, uint32_t renderID);
		void BindIndexBuffer(uint32_t renderID);
		void BindIndrectBuffer(uint32_t renderID);
		void BindVertexBuffer(uint32_t renderID);
		void BindUniformBuffer(uint32_t renderID);
		void BindStorageBuffer(uint32_t renderID);
		void BindTexture(uint32_t renderID);

		virtual void BindTextureSlot(uint32_t slot, uint32_t renderID);
		virtual void BindSamplerSlot(uint32_t slot, uint32_t renderID);


		virtual void BindVertexBufferSlot(uint32_t slot, uint32_t renderID);
		virtual void BindUniformBufferSlot(uint32_t slot, uint32_t renderID);
		virtual void BindStorageBufferSlot(uint32_t slot, uint32_t renderID);

		virtual void SetFace(CallFace callface = CallFace::None) override;
		virtual void SetDethTest(bool aktiv = true) override;
		virtual void SetBiasGPU(float factor, float units) override;
		virtual void DisableBiasGPU() override;

		

		virtual void SetMode(int mode) override;
		virtual void SetModeForce(int mode) override;
		virtual int GetMode() override;
		virtual bool IsModeEqual(int mode) override;

		virtual void AktivePolyGunMode(bool active = true) override;

		virtual void Clear() override;
		virtual void ClearNoDepth() override;
		virtual void ClearDepth() override;

		virtual void DrawIndexedMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0u) override;
		virtual void DrawIndexedMeshInstecing(uint32_t instecing, const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0u) override;

		virtual void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, uint32_t drawCount, void* indrictDraw, uint32_t size) override;
		virtual void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, const Ref<IndirectBuffer>& indriectBuffer, uint32_t drawCount) override;
		virtual void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, const Ref<IndirectBuffer>& indriectBuffer) override;
		virtual void DrawMultyMeshIndriect(const Ref<VertexArray>& vertexArray, uint32_t size, uint32_t drawCount) override;

		virtual void DrawElement(const Ref<VertexArray>& vertexArray, const Mesh::PerDrawObject& drawObject) override;

		virtual void DrawStripsMesh(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0u)override;


		virtual void DrawIndexedLine(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0u)override;
		virtual void DrawIndexedLineLoop(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0u) override;

		virtual void DrawIndexedPoints(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0u) override;

		virtual void DrawPatches(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0u) override;
		virtual void RestPipline() override;
		virtual void PrintCurentStatePipline();


		virtual void DispatcheCompute(const glm::vec<3, uint32_t>& groups) override;

		virtual void DrawError() override;

		void ModeEnable(int bitCount);
		void ModeDisenable(int bitCount);
		uint32_t GetDefaultFrambufferRenderID() const;
	private:
		std::vector<uint32_t> m_BindSamplerStateVec;
		std::vector<uint32_t> m_BindTextureStateVec;
		std::vector<uint32_t> m_BindStorageStateVec;
		std::vector<uint32_t> m_BindUniformBufferStateVec;
		std::vector<uint32_t> m_BindVertexBufferStateVec;
		

		uint32_t m_BindFrameBufferState;
		uint32_t m_BindShaderState;
		uint32_t m_BindVertexArrayState;
		uint32_t m_BindVertexBufferState;
		uint32_t m_BindIndexBufferState;
		uint32_t m_BindIndirectState;
		uint32_t m_BindUnifomrBufferState;
		uint32_t m_BindStorageBufferState;

		uint32_t m_DefaultFrambufferRenderID;

		int m_CurentMode;


		int m_HigestBindSamplerStateSlot;
		int m_HigestBindStorageStateSlot;
		int m_HigestBindTextureStateSlot;
		int m_HigestBindUniformBufferStateSlot;
		int m_HigestBindVertexBufferStateSlot;

	};

}


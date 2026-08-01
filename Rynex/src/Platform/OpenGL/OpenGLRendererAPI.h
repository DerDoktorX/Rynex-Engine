#pragma once

#include <Rynex/Renderer/RendererAPI.h>
// #define RY_OPENGL_BINDLES_TEXTURE_API_STATE_RECORDE
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

#ifdef RY_OPENGL_UN_RESOURCESE
		void UnBindBuffer(uint32_t target);
		void UnBindBufferSlot(uint32_t target, uint32_t renderID);
		void UnBindIndexBuffer(uint32_t renderID);
		void UnBindIndrectBuffer(uint32_t renderID);
		void UnBindVertexBuffer(uint32_t renderID);
		void UnBindUniformBuffer(uint32_t renderID);
		void UnBindStorageBuffer(uint32_t renderID);
		void UnBindTexture(uint32_t renderID);

		virtual void UnBindTextureSlot(uint32_t slot, uint32_t renderID);
		virtual void UnBindSamplerSlot(uint32_t slot, uint32_t renderID);

		virtual void UnBindVertexBufferSlot(uint32_t slot, uint32_t renderID);
		virtual void UnBindUniformBufferSlot(uint32_t slot, uint32_t renderID);
		virtual void UnBindStorageBufferSlot(uint32_t slot, uint32_t renderID);
#endif

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
#ifdef RY_OPENGL_BINDLES_TEXTURE_IMAGE_API_STATE_RECORDE
		virtual void BindImageSlot(uint32_t slot, uint32_t renderID);
#endif

#ifdef RY_OPENGL_BINDLES_TEXTURE_API_STATE_RECORDE
		virtual void BindlesTextureAktivate(uint32_t renderID, uint64_t bindlesHadle);
		virtual void BindlesTextureDeaktivate(uint32_t renderID, uint64_t bindlesHadle);
#endif

		virtual void BindVertexBufferSlot(uint32_t slot, uint32_t renderID);
		virtual void BindUniformBufferSlot(uint32_t slot, uint32_t renderID);
		virtual void BindStorageBufferSlot(uint32_t slot, uint32_t renderID);
#ifdef RY_OPENGL_UNBIND_RENDER_ID_FROM_EVERY_SLOT
		virtual void UnBindTextureFromSlotsWitheRenderID(uint32_t renderID);
		virtual void UnBindSamplerFromSlotsWitheRenderID(uint32_t renderID);

		virtual void UnBindVertexBufferFromSlotsWitheRenderID(uint32_t renderID);
		virtual void UnBindUniformBufferFromSlotsWitheRenderID(uint32_t renderID);
		virtual void UnBindStorageBufferFromSlotsWitheRenderID(uint32_t renderID);
#endif

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
		// virtual const RendererAPI::API GetRendererAPI() const override { return RendererAPI::API::OpenGL; };


		virtual void DispatcheCompute(const glm::vec<3, uint32_t>& groups) override;

		virtual void DrawError() override;

#if RY_RENDERER_API_INDEIPENDENT

		virtual void DrawIndexed(const DrawRendererSpec& drawSpec) override;

#endif
		void ModeEnable(int bitCount);
		void ModeDisenable(int bitCount);
		uint32_t GetDefaultFrambufferRenderID() const;
	private:
		std::vector<uint32_t> m_BindSamplerStateVec;
#ifdef RY_OPENGL_BINDLES_TEXTURE_API_STATE_RECORDE
		robin_hood::unordered_flat_set<AktiveBindlesTexture> m_BindlesTextureAktiveStateVec;
#endif
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
#ifdef RY_OPENGL_BINDLES_TEXTURE_API_STATE_RECORDE
	class AktiveBindlesTexture
	{
	public:


		AktiveBindlesTexture(uint32_t renderID, uint64_t bindlessHandle)
			: m_RenderID(renderID), m_BindlessHandle(bindlessHandle)
		{
			RY_CORE_ASSERT(0 != renderID);
			RY_CORE_ASSERT(0 != bindlessHandle);
		}

		AktiveBindlesTexture(const AktiveBindlesTexture& copy)
			: m_RenderID(copy.m_RenderID), m_BindlessHandle(copy.m_BindlessHandle)
		{

		}

		operator size_t() const { return  GetHash(); }
		size_t GetHash() const
		{
			return robin_hood::hash<size_t>{}(m_BindlessHandle);
		}
	private:
		uint32_t m_RenderID;
		uint64_t m_BindlessHandle;
	private:
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(::Rynex::AktiveBindlesTexture, bool, == );
		RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(::Rynex::AktiveBindlesTexture, bool, != );
	};


	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::AktiveBindlesTexture, == , &&, m_RenderID, m_BindlessHandle);
	RY_NONE_MEBER_OPERATOR_BOOL(::Rynex::AktiveBindlesTexture, != , || , m_RenderID, m_BindlessHandle);
#endif

}

#ifdef RY_OPENGL_BINDLES_TEXTURE_API_STATE_RECORDE
namespace robin_hood {

	template<>
	struct hash<Rynex::AktiveBindlesTexture>
	{
		std::size_t operator()(const Rynex::AktiveBindlesTexture& bindelssTexture) const
		{
			std::size_t hashV = bindelssTexture.GetHash();
			return hashV;
		}




	};
}
#endif


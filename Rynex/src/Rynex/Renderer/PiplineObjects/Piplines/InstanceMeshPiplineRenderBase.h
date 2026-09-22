#pragma once
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>


#ifndef RY_RENDER_PIPLINES_CHACH_2
    #define RY_RENDER_PIPLINES_CHACH_1
#endif // RY_RENDER_PIPLINES_CHACH_2

#define RY_RENDER_PIPLINES_CHACH RY_RENDER_PIPLINES_CHACH_1 || RY_RENDER_PIPLINES_CHACH_2

#define RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE

// #define RY_INSTANCE_MESH_PIPLINE_RENDER_TEMPLATE
namespace Rynex {

    class InstanceMeshPiplineRenderBase : public PiplineRenderBase
	{
	public:
		enum
		{
			State_MaxEntityRender = 100000,
			State_MaxNotUpdateDraws = 100,


			UniformBinding_MainCamera = 1,
		};

	// public member methode --------------------------------------------------------------------------------------------------
		InstanceMeshPiplineRenderBase();
		InstanceMeshPiplineRenderBase(const InstanceMeshPiplineRenderBase&) = default;
		virtual ~InstanceMeshPiplineRenderBase();

		virtual void SubmitRenderTargetResource(ViewPassStorage & viewPass) = 0;
		virtual void SubmitRenderTargetResourceReadImg(const Ref<Texture>&texture) = 0;
		virtual void SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>&buffer) = 0;

		virtual BufferLayout GetExpectedOutput() const override;
		virtual void SetExpectedOutput(const BufferLayout& output) override;

		virtual void SetCameraUniformBuffer(Ref<UniformBuffer> cameraBuffer) override;
		virtual void SetDisplayUniformBuffer(Ref<UniformBuffer> displayBuffer) override;

		virtual void SubmitRenderObject(const glm::mat4 & model, uint32_t & storeIndex, int entityID) = 0;
		virtual PiplineResultState SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader
			, const glm::mat4& model, uint32_t& storeIndex, int entityID) = 0;



		virtual void SetDataMangingFlags(PiplineManagingState flags) override;
		virtual PiplineManagingState GetDataMangingFlags() const override { return m_ManagingMode; }

		virtual void SetRenderFlags(int flags) override { m_RenderMode = flags; }
		virtual int GetRenderFlags() const override { return m_RenderMode; };

		virtual void DrawNow() override;
		virtual void DrawNow(int flags) = 0;

		virtual uint32_t GetCurrentEntityRender() const override { return m_InstanceCount; }
		virtual uint32_t GetMaxEntityRender() const override { return State_MaxEntityRender; }

		virtual uint32_t GetFrameCountNotUpdate() const override { return m_DrawsAfterLastUpdate; }
		virtual uint32_t GetMaxFrameCountNotUpdate() const override { return State_MaxNotUpdateDraws; }

		virtual bool Empty() const = 0;
		virtual bool IsFull()const override = 0;

		virtual void Clear() override;
		virtual void ClearRenderObjects() override;

		virtual uint64_t GetShaderNumber() const override { return reinterpret_cast<uint64_t>(m_Shader.get()); }
		virtual uint64_t GetVertexBufferNumber() const = 0;
		virtual uint64_t GetIndexBufferNumber() const = 0;
		virtual uint64_t GetIndirectBufferNumber() const = 0;
		virtual uint64_t GetTextureNumber() const = 0;
		virtual uint64_t GetUniformBufferNumber() const = 0;

		virtual uint64_t GetStorageBufferNumber() const = 0;
		virtual Ref<PiplineRenderBase> Copy() const = 0;

	protected:
		template<typename T>
		static void CheckObject(Ref<T>& a, const Ref<T>& b, int& result, int notVaildState, int noSpaceLeft = 0)
		{
			if (a != b && nullptr != a)
				result = result | noSpaceLeft;

			if (nullptr == b)
				result = result | notVaildState;
		}

		virtual bool IsExpectedOutPut(const Ref<Shader>& shader) const;

		virtual void BeforeDrawCall() = 0;
		virtual void BindResources();
		virtual void UnbindResources();
	protected:
		BufferLayout		m_OutPut;
		SingleMeshObject	m_SingleMeshObject;
		uint32_t			m_InstanceCount;
		uint32_t			m_DrawsAfterLastUpdate;


		PiplineManagingState m_ManagingMode;

		Ref<Shader>			m_Shader;
		Ref<UniformBuffer>	m_CameraBuffer;

		Ref<VertexBuffer>	m_ModelBufferVAO;
		Ref<VertexArray>	m_VertexArray;
		int					m_RenderMode;
	};

} // Rynex



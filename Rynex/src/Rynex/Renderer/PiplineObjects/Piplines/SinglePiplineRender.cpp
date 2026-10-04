#include "rypch.h"
#include "SinglePiplineRender.h"

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/RenderCommand.h>


namespace Rynex {

#pragma region Shade

	SingleMeshPiplineRenderShade::SingleMeshPiplineRenderShade()
		: m_Shader(nullptr)
		, m_CameraBuffer(nullptr)
		, m_LightBuffer(nullptr)
		, m_ModelBuffer(nullptr)
		, m_MaterielBuffer(nullptr)
		, m_AlbedoTex(nullptr)
		, m_ShadowTex(nullptr)
		, m_RenderMode(0)
		, m_ManagingMode(PiplineManagingState::Managing_None)
		, m_DrawsAfterLastUpdate(0u)
		, m_InstanceCount(0u)
		, m_RenderObject()
	{
	}

	SingleMeshPiplineRenderShade::~SingleMeshPiplineRenderShade()
	{
		Clear();
		m_DrawsAfterLastUpdate = 0;
		RY_DESTROY_REF(m_ModelBuffer);
	}

	void SingleMeshPiplineRenderShade::SubmitRenderTargetResource(ViewPassStorage& viewPass)
	{
		if (nullptr == m_ShadowTex || nullptr == m_LightBuffer)
		{
			CamerRenderPackages& viewPassPackege = viewPass.m_CameraPackege;
			CamerRenderPackages::CamerPackage& camerPackage = viewPassPackege.GetCamerPackage();
			const Ref<UniformBuffer>& camerBuffer = camerPackage.GetBuffer();
			m_LightBuffer = camerBuffer;

			RenderTarget& target = viewPassPackege.GetRenderTarget();
			const Ref<Framebuffer>& fb = target.GetFramebuffer();
			const Ref<Texture>& shadow = fb->GetDepthTexture();
			m_ShadowTex = shadow;
		}
	}

	void SingleMeshPiplineRenderShade::SubmitRenderTargetResourceReadImg(const Ref<Texture>& texture)
	{
		m_ShadowTex = texture;
	}

	void SingleMeshPiplineRenderShade::SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>& buffer)
	{
		m_LightBuffer = buffer;
	}

	void SingleMeshPiplineRenderShade::SetCameraUniformBuffer(Ref<UniformBuffer> cameraBuffer)
	{
		m_CameraBuffer = cameraBuffer;
	}

	void SingleMeshPiplineRenderShade::SetDisplayUniformBuffer(Ref<UniformBuffer> displayBuffer)
	{
		
	}

	void SingleMeshPiplineRenderShade::SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		if (m_InstanceCount != storeIndex)
			storeIndex = m_InstanceCount;

		m_RenderObject.SetObject(model, entityID);
		m_InstanceCount++;
		m_DrawsAfterLastUpdate = 0u;
	}

	PiplineResultState SingleMeshPiplineRenderShade::SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		int result = CheckSubmitMeshObject(shader, singleMesh);
		if (State_MaxEntityRender <= m_InstanceCount)
		{
			result = result | Result_NoRenderObjectSpaceLeft;
			return static_cast<PiplineResultState>(result);
		}
		if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
		{
			SubmitResources(shader, singleMesh);
			if (storeIndex != m_InstanceCount)
				storeIndex = m_InstanceCount;

			m_RenderObject.SetObject(model,entityID);
			m_InstanceCount++;
			m_DrawsAfterLastUpdate = 0u;
			result = Result_Success;
		}


		return static_cast<PiplineResultState>(result);
	}

	int SingleMeshPiplineRenderShade::CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		int result = Result_None;

		CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
		CheckObject(m_SingleMeshObject.m_MeshSingle, singleMesh.m_MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft );
		const Ref<Material>& materiel = singleMesh.m_Material;
		CheckObject(m_SingleMeshObject.m_Material, materiel, result,  Result_NotAllowedShadeDefinition, Result_NoShadeDefinitionSpaceLeft);
		
		if(nullptr != materiel)
			CheckObject(m_AlbedoTex, materiel->GetAlbedoTextures(), result, Result_NotAllowedTexture, Result_NoTextureSpaceLeft);

		return result;
	}

	void SingleMeshPiplineRenderShade::SubmitResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		m_Shader = shader;
		m_SingleMeshObject = singleMesh;
		const Ref<Material>& materiel = m_SingleMeshObject.m_Material;

		m_AlbedoTex = materiel->GetAlbedoTextures();
		m_RenderMode = materiel->GetShadeRenderMode();
		m_MaterielBuffer = materiel->GetMaterielUniformBuffer();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		CheckVAOFromMeshSingleShade(m_VertexArray, meshSingle);

	}

	void SingleMeshPiplineRenderShade::BeforeDrawCall()
	{
		if (State_MaxNotUpdateDraws <= m_DrawsAfterLastUpdate)
		{
			RY_CORE_WARN("We have draw this object now {} times and never updated!", m_DrawsAfterLastUpdate);
		}
		m_DrawsAfterLastUpdate++;
		if(nullptr == m_ModelBuffer)
		{
			BufferLayout layout = BufferLayout({
				{ShaderDataType::Float4x4, "ModelMatrix"},
				{ShaderDataType::Float4x4, "NormaleMatrix"},
				{ShaderDataType::Int, "EntityID"},
				{ShaderDataType::Int3, "Empty"},
			});

			m_ModelBuffer = UniformBuffer::Create(&m_RenderObject.m_Object, sizeof(RenderObject), layout, UniformBinding_RenderObject);
		}
		else if (m_RenderObject.NeedUpdate())
		{
			m_ModelBuffer->SetData(&m_RenderObject.m_Object, sizeof(RenderObject));
			m_RenderObject.Updated();
		}
	}

	void SingleMeshPiplineRenderShade::BindResources()
	{
		m_Shader->Bind();

		m_CameraBuffer->Bind(UniformBinding_MainCamera);
		m_ModelBuffer->Bind(UniformBinding_RenderObject);
		m_MaterielBuffer->Bind(UniformBinding_Materiel);
		m_LightBuffer->Bind(UniformBinding_LightCamera);

		m_AlbedoTex->Bind(TextureBinding_Albedo);
		m_ShadowTex->Bind(TextureBinding_Shadow);
	}

	void SingleMeshPiplineRenderShade::UnbindResources()
	{
		m_Shader->UnBind();

		m_CameraBuffer->UnBind(UniformBinding_MainCamera);
		m_ModelBuffer->UnBind(UniformBinding_RenderObject);
		m_MaterielBuffer->UnBind(UniformBinding_Materiel);
		m_LightBuffer->UnBind(UniformBinding_LightCamera);

		m_AlbedoTex->UnBind(TextureBinding_Albedo);
		m_ShadowTex->UnBind(TextureBinding_Shadow);
	}

	void SingleMeshPiplineRenderShade::SetDataMangingFlags(PiplineManagingState flags)
	{
		RY_CORE_WARN("This PiplineManagingState is changing nothing on this is a one Render Object Call");
		m_ManagingMode = flags;
	}

	void SingleMeshPiplineRenderShade::SetRenderFlags(int flags)
	{
		m_RenderMode = flags;
	}

	int SingleMeshPiplineRenderShade::GetRenderFlags() const
	{
		return m_RenderMode;
	}

	PiplineManagingState SingleMeshPiplineRenderShade::GetDataMangingFlags() const
	{
		return m_ManagingMode;
	}

	void SingleMeshPiplineRenderShade::DrawNow()
	{
		DrawNow(m_RenderMode);
	}

	void SingleMeshPiplineRenderShade::DrawNow(int flags)
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;

		
		if (nullptr == m_Shader 
			|| nullptr == m_VertexArray 
			|| nullptr == m_MaterielBuffer 
			|| nullptr == m_CameraBuffer
			|| nullptr == m_LightBuffer
			|| nullptr == m_AlbedoTex
			|| nullptr == m_ShadowTex)
		{
			RY_CORE_ERROR("Draw call failed, because core resources are not set!");
			return;
		}
		BeforeDrawCall();

		Mesh::PerDrawObject drawElement = meshSingle->GetShadePerDrawObjectIndirect();
		RenderCommand::SetMode(flags);
		BindResources();
		RY_CORE_ASSERT(0 < m_InstanceCount);
		drawElement.m_InstancesCount = m_InstanceCount;
		
		RenderCommand::DrawElement(m_VertexArray, drawElement);
#if RY_UNBIND
		UnbindResources();
#endif
	}

	uint32_t SingleMeshPiplineRenderShade::GetCurrentEntityRender() const
	{
		return m_InstanceCount;
	}

	uint32_t SingleMeshPiplineRenderShade::GetMaxEntityRender() const
	{
		return State_MaxEntityRender;
	}

	uint32_t SingleMeshPiplineRenderShade::GetFrameCountNotUpdate() const
	{
		return m_DrawsAfterLastUpdate;
	}

	bool SingleMeshPiplineRenderShade::Empty() const
	{
		return m_InstanceCount == 0;
	}

	bool SingleMeshPiplineRenderShade::IsFull() const
	{
		if (State_MaxEntityRender < m_InstanceCount)
		{
			uint32_t toManyObjects = m_InstanceCount - State_MaxEntityRender;
			RY_CORE_WARN("This RenderPipline has {} more Stored then allowed", toManyObjects);
			return true;
		}
		return State_MaxEntityRender == m_InstanceCount;
		
	}

	void SingleMeshPiplineRenderShade::Clear()
	{
		m_InstanceCount = 0u;
		m_RenderMode = 0;
		RY_DESTROY_REF(m_Shader);
		RY_DESTROY_REF(m_AlbedoTex);
		RY_DESTROY_REF(m_CameraBuffer);
		RY_DESTROY_REF(m_LightBuffer);
		RY_DESTROY_REF(m_SingleMeshObject.m_Material);
		RY_DESTROY_REF(m_SingleMeshObject.m_MeshSingle);
	}

	void SingleMeshPiplineRenderShade::ClearRenderObjects()
	{
		m_InstanceCount = 0u;
	}

	

	uint64_t SingleMeshPiplineRenderShade::GetVertexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		uint64_t number = 0ull;
		uint32_t i = 0;
		const Ref<VertexBuffer>& buffer = meshSingle->GetVertexBuffer();
		{
			uint64_t numberVAB = reinterpret_cast<uint64_t>(buffer.get());
			number |= numberVAB << (i * Hash_BindingPointMultiplyNumberBitMove);
			i++;
		}
		
		return number;
	}

	uint64_t SingleMeshPiplineRenderShade::GetIndexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		const Ref<IndexBuffer>& iab = meshSingle->GetShadeIndexBuffer();
		uint64_t number = reinterpret_cast<uint64_t>(iab.get());
		return number;
	}

	uint64_t SingleMeshPiplineRenderShade::GetIndirectBufferNumber() const
	{
		return 0ull;
	}

	uint64_t SingleMeshPiplineRenderShade::GetTextureNumber() const
	{
		uint64_t number = 0;
		const uint64_t albedoNumber = reinterpret_cast<uint64_t>(m_AlbedoTex.get());
		const uint64_t shadowNumber = reinterpret_cast<uint64_t>(m_ShadowTex.get());
		number |= albedoNumber << (Hash_BindingPointMultiplyNumberBitMove * TextureBinding_Albedo);
		number |= shadowNumber << (Hash_BindingPointMultiplyNumberBitMove * TextureBinding_Shadow);
		return number;
	}

	uint64_t SingleMeshPiplineRenderShade::GetUniformBufferNumber() const
	{
		uint64_t number = 0;
		const uint64_t lightNumber = reinterpret_cast<uint64_t>(m_LightBuffer.get());
		const uint64_t cameraNumber = reinterpret_cast<uint64_t>(m_CameraBuffer.get());
		const uint64_t modelNumber = reinterpret_cast<uint64_t>(m_ModelBuffer.get());
		const uint64_t materialNumber = reinterpret_cast<uint64_t>(m_ModelBuffer.get());
		number |= lightNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_LightCamera);
		number |= cameraNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_MainCamera);
		number |= modelNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_RenderObject);
		number |= materialNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_Materiel);

		return number;
	}

	uint64_t SingleMeshPiplineRenderShade::GetStorageBufferNumber() const
	{
		return 0ull;
	}

	

#pragma endregion


#pragma region Depth

	SingleMeshPiplineRenderDepth::SingleMeshPiplineRenderDepth()
		: m_Shader(nullptr)
		, m_LightBuffer(nullptr)
		, m_ModelBuffer(nullptr)
		, m_RenderObject()
		, m_InstanceCount(0u)
		, m_DrawsAfterLastUpdate(0u)
		, m_ManagingMode(PiplineManagingState::Managing_None)
		, m_RenderMode(0)
	{
	}

	SingleMeshPiplineRenderDepth::~SingleMeshPiplineRenderDepth()
	{
		Clear();
		m_DrawsAfterLastUpdate = 0;
		RY_DESTROY_REF(m_ModelBuffer);
	}

	

	void SingleMeshPiplineRenderDepth::SetCameraUniformBuffer(Ref<UniformBuffer> ligthBuffer)
	{
		m_LightBuffer = ligthBuffer;
	}

	void SingleMeshPiplineRenderDepth::SetDisplayUniformBuffer(Ref<UniformBuffer> displayBuffer)
	{

	}
	
	void SingleMeshPiplineRenderDepth::SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		if (m_InstanceCount != storeIndex)
			storeIndex = m_InstanceCount;

		m_RenderObject.SetObject(model, entityID);
		m_InstanceCount++;
		m_DrawsAfterLastUpdate = 0u;
	}

	PiplineResultState SingleMeshPiplineRenderDepth::SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		int result = CheckSubmitMeshObject(shader, singleMesh);
		if (State_MaxEntityRender <= m_InstanceCount)
		{
			result = result | Result_NoRenderObjectSpaceLeft;
			return static_cast<PiplineResultState>(result);
		}
		if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
		{
			SubmitResources(shader, singleMesh);
			if (storeIndex != m_InstanceCount)
				storeIndex = m_InstanceCount;

			m_RenderObject.SetObject(model, entityID);
			m_InstanceCount++;
			m_DrawsAfterLastUpdate = 0u;
			result = Result_Success;
		}


		return static_cast<PiplineResultState>(result);
	}

	

	int SingleMeshPiplineRenderDepth::CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		int result = Result_None;

		CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
		CheckObject(m_SingleMeshObject.m_MeshSingle, singleMesh.m_MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);

		return result;
	}

	void SingleMeshPiplineRenderDepth::SubmitResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		m_Shader = shader;
		m_SingleMeshObject = singleMesh;
		const Ref<Material>& materiel = m_SingleMeshObject.m_Material;

		m_RenderMode = materiel->GetDepthRenderMode();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		CheckVAOFromMeshSingleDepth(m_VertexArray, meshSingle);

	}

	void SingleMeshPiplineRenderDepth::BeforeDrawCall()
	{
		if (State_MaxNotUpdateDraws <= m_DrawsAfterLastUpdate)
		{
			RY_CORE_WARN("We have draw this object now {} times and never updated!", m_DrawsAfterLastUpdate);
		}
		m_DrawsAfterLastUpdate++;
		if (nullptr == m_ModelBuffer)
		{
			BufferLayout layout = BufferLayout({
				{ShaderDataType::Float4x4, "ModelMatrix"},

			});
			m_ModelBuffer = UniformBuffer::Create(&m_RenderObject.Object, sizeof(RenderObject), layout, UniformBinding_RenderObject);
		}
		else if (m_RenderObject.NeedUpdate())
		{
			m_ModelBuffer->SetData(&m_RenderObject.Object, sizeof(RenderObject));
			m_RenderObject.Updated();
		}
	}



	void SingleMeshPiplineRenderDepth::BindResources()
	{
		m_Shader->Bind();

		m_ModelBuffer->Bind(UniformBinding_RenderObject);
		m_LightBuffer->Bind(UniformBinding_LightCamera);
	}

	void SingleMeshPiplineRenderDepth::UnbindResources()
	{
		m_Shader->UnBind();

		m_ModelBuffer->UnBind(UniformBinding_RenderObject);
		m_LightBuffer->UnBind(UniformBinding_LightCamera);

	}

	void SingleMeshPiplineRenderDepth::SetDataMangingFlags(PiplineManagingState flags)
	{
		RY_CORE_WARN("This PiplineManagingState is changing nothing on this is a one Render Object Call");
		m_ManagingMode = flags;
	}

	void SingleMeshPiplineRenderDepth::SetRenderFlags(const int flags)
	{
		m_RenderMode = flags;
	}

	int SingleMeshPiplineRenderDepth::GetRenderFlags() const
	{
		return m_RenderMode;
	}

	PiplineManagingState SingleMeshPiplineRenderDepth::GetDataMangingFlags() const
	{
		return m_ManagingMode;
	}

	void SingleMeshPiplineRenderDepth::DrawNow()
	{
		DrawNow(m_RenderMode);
	}

	void SingleMeshPiplineRenderDepth::DrawNow(const int flags)
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;

		if (nullptr == m_Shader
			|| nullptr == m_VertexArray
			|| nullptr == m_LightBuffer)
		{
			RY_CORE_ERROR("Draw call failed, because core resources are not set!");
			return;
		}
		BeforeDrawCall();

		const Mesh::PerDrawObject& drawElement = meshSingle->GetDepthPerDrawObjectIndirect();
		RenderCommand::SetMode(flags);
		BindResources();

		RenderCommand::DrawElement(m_VertexArray, drawElement);
	}

	uint32_t SingleMeshPiplineRenderDepth::GetCurrentEntityRender() const
	{
		return m_InstanceCount;
	}

	uint32_t SingleMeshPiplineRenderDepth::GetMaxEntityRender() const
	{
		return State_MaxEntityRender;
	}

	uint32_t SingleMeshPiplineRenderDepth::GetFrameCountNotUpdate() const
	{
		return m_DrawsAfterLastUpdate;
	}

	bool SingleMeshPiplineRenderDepth::Empty() const
	{
		return m_InstanceCount == 0;
	}

	bool SingleMeshPiplineRenderDepth::IsFull() const
	{
		if (State_MaxEntityRender < m_InstanceCount)
		{
			uint32_t toManyObjects = m_InstanceCount - State_MaxEntityRender;
			RY_CORE_WARN("This RenderPipline has {} more Stored then allowed", toManyObjects);
			return true;
		}
		return State_MaxEntityRender == m_InstanceCount;

	}

	void SingleMeshPiplineRenderDepth::Clear()
	{
		m_InstanceCount = 0u;
		m_RenderMode = 0;
		RY_DESTROY_REF(m_Shader);
		RY_DESTROY_REF(m_LightBuffer);
		RY_DESTROY_REF(m_SingleMeshObject.m_Material);
		RY_DESTROY_REF(m_SingleMeshObject.m_MeshSingle);
	}

	void SingleMeshPiplineRenderDepth::ClearRenderObjects()
	{
		m_InstanceCount = 0u;
	}

	

	uint64_t SingleMeshPiplineRenderDepth::GetVertexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
		uint64_t number = 0ull;
		uint64_t i = 0u;
		
		const uint64_t numberVAB = reinterpret_cast<uint64_t>(vab.get());
		number |= numberVAB << (i * Hash_BindingPointMultiplyNumberBitMove);
		i++;
		

		return number;
	}

	uint64_t SingleMeshPiplineRenderDepth::GetIndexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		const Ref<IndexBuffer>& iab = meshSingle->GetDepthIndexBuffer();
		const uint64_t number = reinterpret_cast<uint64_t>(iab.get());
		return number;
	}

	uint64_t SingleMeshPiplineRenderDepth::GetIndirectBufferNumber() const
	{
		return 0ull;
	}

	uint64_t SingleMeshPiplineRenderDepth::GetTextureNumber() const
	{
		return 0ull;
	}

	uint64_t SingleMeshPiplineRenderDepth::GetUniformBufferNumber() const
	{
		uint64_t number = 0;
		const uint64_t lightNumber = reinterpret_cast<uint64_t>(m_ModelBuffer.get());
		const uint64_t modelNumber = reinterpret_cast<uint64_t>(m_LightBuffer.get());
		number |= lightNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_LightCamera);
		number |= modelNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_RenderObject);
		return number;
	}

	uint64_t SingleMeshPiplineRenderDepth::GetStorageBufferNumber() const
	{
		return 0ull;
	}

#pragma endregion
}
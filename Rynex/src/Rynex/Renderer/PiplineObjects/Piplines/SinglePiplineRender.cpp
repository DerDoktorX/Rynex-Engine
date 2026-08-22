#include "rypch.h"
#include "SinglePiplineRender.h"

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/RenderCommand.h>


namespace Rynex {

#pragma region Shade

	SingleMeshPiplineRenderShade::SingleMeshPiplineRenderShade()
		: m_Shader(nullptr)
		, m_CameraBuffer(nullptr)
		, m_LigthBuffer(nullptr)
		, m_ModelBuffer(nullptr)
		, m_MaterielBuffer(nullptr)
		, m_AlbdeoTex(nullptr)
		, m_ShadowTex(nullptr)
		, m_RenderMode(0)
		, m_ManagingMode(PiplineManagingState::Managing_None)
		, m_DrawsAfterLastUpdate(0u)
		, m_InstencCount(0u)
		, m_RenderObject()
	{
	}

	SingleMeshPiplineRenderShade::~SingleMeshPiplineRenderShade()
	{
		Clear();
		m_DrawsAfterLastUpdate = 0;
		RY_DESTROY_REF(m_ModelBuffer);
	}

	void SingleMeshPiplineRenderShade::SubmitRenderTargetResurces(ViewPassStorage& viewPass)
	{
		if (nullptr == m_ShadowTex || nullptr == m_LigthBuffer)
		{
			CamerRenderPackages& viewPassPackege = viewPass.CameraPackege;
			CamerRenderPackages::CamerPackage& camerPackage = viewPassPackege.GetCamerPackage();
			const Ref<UniformBuffer>& camerBuffer = camerPackage.GetBuffer();
			m_LigthBuffer = camerBuffer;

			RenderTarget& target = viewPassPackege.GetRenderTarget();
			const Ref<Framebuffer>& fb = target.GetFramebuffer();
			const Ref<Texture>& shadow = fb->GetDepthTexture();
			m_ShadowTex = shadow;
		}
	}

	void SingleMeshPiplineRenderShade::SubmitRenderTargetResurcesReadImg(const Ref<Texture>& texture)
	{
		m_ShadowTex = texture;
	}

	void SingleMeshPiplineRenderShade::SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>& buffer)
	{
		m_LigthBuffer = buffer;
	}

	void SingleMeshPiplineRenderShade::SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer)
	{
		m_CameraBuffer = camerbuffer;
	}

	void SingleMeshPiplineRenderShade::SetDisplayUniformBuffer(Ref<UniformBuffer> dispalaybuffer)
	{
		
	}

	void SingleMeshPiplineRenderShade::SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		if (m_InstencCount != storeIndex)
			storeIndex = m_InstencCount;

		m_RenderObject.SetObject(model, entityID);
		m_InstencCount++;
		m_DrawsAfterLastUpdate = 0u;
	}

	PiplineResultState SingleMeshPiplineRenderShade::SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		int result = CheckSubmiteMeshObject(shader, singleMesh);
		if (State_MaxEntityRender <= m_InstencCount)
		{
			result = result | Result_NoRenderObjectSpaceLeft;
			return static_cast<PiplineResultState>(result);
		}
		if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
		{
			SubmiteResources(shader, singleMesh);
			if (storeIndex != m_InstencCount)
				storeIndex = m_InstencCount;

			m_RenderObject.SetObject(model,entityID);
			m_InstencCount++;
			m_DrawsAfterLastUpdate = 0u;
			result = Result_Success;
		}


		return static_cast<PiplineResultState>(result);
	}

	int SingleMeshPiplineRenderShade::CheckSubmiteMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		int result = Result_None;

		CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
		CheckObject(m_SingleMeshObject._MeshSingle, singleMesh._MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft );
		const Ref<Material>& materiel = singleMesh._Material;
		CheckObject(m_SingleMeshObject._Material, materiel, result,  Result_NotAllowedShadeDefinition, Result_NoShadeDefinitionSpaceLeft);
		
		if(nullptr != materiel)
			CheckObject(m_AlbdeoTex, materiel->GetAlbedoTextures(), result, Result_NotAllowedTexture, Result_NoTextexurSpaceLeft);

		return result;
	}

	void SingleMeshPiplineRenderShade::SubmiteResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		m_Shader = shader;
		m_SingleMeshObject = singleMesh;
		const Ref<Material>& materiel = m_SingleMeshObject._Material;

		m_AlbdeoTex = materiel->GetAlbedoTextures();
		m_RenderMode = materiel->GetShadeRenderMode();
		m_MaterielBuffer = materiel->GetMaterielUniformBuffer();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
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

			m_ModelBuffer = UniformBuffer::Create(&m_RenderObject.Object, sizeof(RenderObject), layout, UniformBinding_RenderObject);
		}
		else if (m_RenderObject.NeedUpdate())
		{
			m_ModelBuffer->SetData(&m_RenderObject.Object, sizeof(RenderObject));
			m_RenderObject.Updated();
		}
	}

	void SingleMeshPiplineRenderShade::BindResources()
	{
		m_Shader->Bind();

		m_CameraBuffer->Bind(UniformBinding_MainCamer);
		m_ModelBuffer->Bind(UniformBinding_RenderObject);
		m_MaterielBuffer->Bind(UniformBinding_Materiel);
		m_LigthBuffer->Bind(UniformBinding_LigthCamera);

		m_AlbdeoTex->Bind(TextureBinding_Abldoe);
		m_ShadowTex->Bind(TextureBinding_Shadow);
	}

	void SingleMeshPiplineRenderShade::UnbindResources()
	{
		m_Shader->UnBind();

		m_CameraBuffer->UnBind(UniformBinding_MainCamer);
		m_ModelBuffer->UnBind(UniformBinding_RenderObject);
		m_MaterielBuffer->UnBind(UniformBinding_Materiel);
		m_LigthBuffer->UnBind(UniformBinding_LigthCamera);

		m_AlbdeoTex->UnBind(TextureBinding_Abldoe);
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
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;

		
		if (nullptr == m_Shader 
			|| nullptr == m_VertexArray 
			|| nullptr == m_MaterielBuffer 
			|| nullptr == m_CameraBuffer
			|| nullptr == m_LigthBuffer
			|| nullptr == m_AlbdeoTex
			|| nullptr == m_ShadowTex)
		{
			RY_CORE_ERROR("Draw call faild, becouse core resurces are not set!");
			return;
		}
		BeforeDrawCall();

		Mesh::PerDrawObject drawElement = meshSingle->GetShadePerDrawObjectIndrect();
		RenderCommand::SetMode(flags);
		BindResources();
		RY_CORE_ASSERT(0 < m_InstencCount);
		drawElement.InstancesCount = m_InstencCount;
		
		RenderCommand::DrawElement(m_VertexArray, drawElement);
#if RY_UNBIND
		UnbindResources();
#endif
	}

	uint32_t SingleMeshPiplineRenderShade::GetCurentEntityRender() const
	{
		return m_InstencCount;
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
		return m_InstencCount == 0;
	}

	bool SingleMeshPiplineRenderShade::IsFull() const
	{
		if (State_MaxEntityRender < m_InstencCount)
		{
			uint32_t toManyObjects = m_InstencCount - State_MaxEntityRender;
			RY_CORE_WARN("This RenderPiline has {} more Stored then allowd", toManyObjects);
			return true;
		}
		return State_MaxEntityRender == m_InstencCount;
		
	}

	void SingleMeshPiplineRenderShade::Clear()
	{
		m_InstencCount = 0u;
		m_RenderMode = 0;
		RY_DESTROY_REF(m_Shader);
		RY_DESTROY_REF(m_AlbdeoTex);
		RY_DESTROY_REF(m_CameraBuffer);
		RY_DESTROY_REF(m_LigthBuffer);
		RY_DESTROY_REF(m_SingleMeshObject._Material);
		RY_DESTROY_REF(m_SingleMeshObject._MeshSingle);
	}

	void SingleMeshPiplineRenderShade::ClearRenderObjects()
	{
		m_InstencCount = 0u;
	}

	

	uint64_t SingleMeshPiplineRenderShade::GetVertexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		uint64_t number = 0ull;
		uint32_t i = 0;
		const Ref<VertexBuffer>& buffer = meshSingle->GetVertexBuffer();
		{
			uint64_t numberVAB = reinterpret_cast<uint64_t>(buffer.get());
			number |= numberVAB << (i * Hash_BindingPointMultyplyNumberBitMove);
			i++;
		}
		
		return number;
	}

	uint64_t SingleMeshPiplineRenderShade::GetIndexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
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
		uint64_t albedoNumber = reinterpret_cast<uint64_t>(m_AlbdeoTex.get());
		uint64_t shadowNumber = reinterpret_cast<uint64_t>(m_ShadowTex.get());
		number |= albedoNumber << (Hash_BindingPointMultyplyNumberBitMove * TextureBinding_Abldoe);
		number |= shadowNumber << (Hash_BindingPointMultyplyNumberBitMove * TextureBinding_Shadow);
		return number;
	}

	uint64_t SingleMeshPiplineRenderShade::GetUniformBufferNumber() const
	{
		uint64_t number = 0;
		uint64_t ligtheNumber = reinterpret_cast<uint64_t>(m_LigthBuffer.get());
		uint64_t cameraNumber = reinterpret_cast<uint64_t>(m_CameraBuffer.get());
		uint64_t modelNumber = reinterpret_cast<uint64_t>(m_ModelBuffer.get());
		uint64_t materilNumber = reinterpret_cast<uint64_t>(m_ModelBuffer.get());
		number |= ligtheNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_LigthCamera);
		number |= cameraNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_MainCamer);
		number |= modelNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_RenderObject);
		number |= materilNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_Materiel);

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
		, m_LigthBuffer(nullptr)
		, m_ModelBuffer(nullptr)
		, m_RenderMode(0)
		, m_ManagingMode(PiplineManagingState::Managing_None)
		, m_DrawsAfterLastUpdate(0u)
		, m_InstencCount(0u)
		, m_RenderObject()
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
		m_LigthBuffer = ligthBuffer;
	}

	void SingleMeshPiplineRenderDepth::SetDisplayUniformBuffer(Ref<UniformBuffer> dispalaybuffer)
	{

	}
	
	void SingleMeshPiplineRenderDepth::SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		if (m_InstencCount != storeIndex)
			storeIndex = m_InstencCount;

		m_RenderObject.SetObject(model, entityID);
		m_InstencCount++;
		m_DrawsAfterLastUpdate = 0u;
	}

	PiplineResultState SingleMeshPiplineRenderDepth::SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		int result = CheckSubmiteMeshObject(shader, singleMesh);
		if (State_MaxEntityRender <= m_InstencCount)
		{
			result = result | Result_NoRenderObjectSpaceLeft;
			return static_cast<PiplineResultState>(result);
		}
		if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
		{
			SubmiteResources(shader, singleMesh);
			if (storeIndex != m_InstencCount)
				storeIndex = m_InstencCount;

			m_RenderObject.SetObject(model, entityID);
			m_InstencCount++;
			m_DrawsAfterLastUpdate = 0u;
			result = Result_Success;
		}


		return static_cast<PiplineResultState>(result);
	}

	

	int SingleMeshPiplineRenderDepth::CheckSubmiteMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		int result = Result_None;

		CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
		CheckObject(m_SingleMeshObject._MeshSingle, singleMesh._MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);

		return result;
	}

	void SingleMeshPiplineRenderDepth::SubmiteResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		m_Shader = shader;
		m_SingleMeshObject = singleMesh;
		const Ref<Material>& materiel = m_SingleMeshObject._Material;

		m_RenderMode = materiel->GetDepthRenderMode();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
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
		m_LigthBuffer->Bind(UniformBinding_LigthCamera);
	}

	void SingleMeshPiplineRenderDepth::UnbindResources()
	{
		m_Shader->UnBind();

		m_ModelBuffer->UnBind(UniformBinding_RenderObject);
		m_LigthBuffer->UnBind(UniformBinding_LigthCamera);

	}

	void SingleMeshPiplineRenderDepth::SetDataMangingFlags(PiplineManagingState flags)
	{
		RY_CORE_WARN("This PiplineManagingState is changing nothing on this is a one Render Object Call");
		m_ManagingMode = flags;
	}

	void SingleMeshPiplineRenderDepth::SetRenderFlags(int flags)
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

	void SingleMeshPiplineRenderDepth::DrawNow(int flags)
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;

		if (nullptr == m_Shader
			|| nullptr == m_VertexArray
			|| nullptr == m_LigthBuffer)
		{
			RY_CORE_ERROR("Draw call faild, becouse core resurces are not set!");
			return;
		}
		BeforeDrawCall();

		const Mesh::PerDrawObject& drawElement = meshSingle->GetDepthPerDrawObjectIndrect();
		RenderCommand::SetMode(flags);
		BindResources();

		RenderCommand::DrawElement(m_VertexArray, drawElement);
	}

	uint32_t SingleMeshPiplineRenderDepth::GetCurentEntityRender() const
	{
		return m_InstencCount;
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
		return m_InstencCount == 0;
	}

	bool SingleMeshPiplineRenderDepth::IsFull() const
	{
		if (State_MaxEntityRender < m_InstencCount)
		{
			uint32_t toManyObjects = m_InstencCount - State_MaxEntityRender;
			RY_CORE_WARN("This RenderPiline has {} more Stored then allowd", toManyObjects);
			return true;
		}
		return State_MaxEntityRender == m_InstencCount;

	}

	void SingleMeshPiplineRenderDepth::Clear()
	{
		m_InstencCount = 0u;
		m_RenderMode = 0;
		RY_DESTROY_REF(m_Shader);
		RY_DESTROY_REF(m_LigthBuffer);
		RY_DESTROY_REF(m_SingleMeshObject._Material);
		RY_DESTROY_REF(m_SingleMeshObject._MeshSingle);
	}

	void SingleMeshPiplineRenderDepth::ClearRenderObjects()
	{
		m_InstencCount = 0u;
	}

	

	uint64_t SingleMeshPiplineRenderDepth::GetVertexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
		uint64_t number = 0ull;
		uint64_t i = 0u;
		
		uint64_t numberVAB = reinterpret_cast<uint64_t>(vab.get());
		number |= numberVAB << (i * Hash_BindingPointMultyplyNumberBitMove);
		i++;
		

		return number;
	}

	uint64_t SingleMeshPiplineRenderDepth::GetIndexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		const Ref<IndexBuffer>& iab = meshSingle->GetDepthIndexBuffer();
		uint64_t number = reinterpret_cast<uint64_t>(iab.get());
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
		uint64_t ligtheNumber = reinterpret_cast<uint64_t>(m_ModelBuffer.get());
		uint64_t modelNumber = reinterpret_cast<uint64_t>(m_LigthBuffer.get());
		number |= ligtheNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_LigthCamera);
		number |= modelNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_RenderObject);
		return number;
	}

	uint64_t SingleMeshPiplineRenderDepth::GetStorageBufferNumber() const
	{
		return 0ull;
	}

#pragma endregion
}
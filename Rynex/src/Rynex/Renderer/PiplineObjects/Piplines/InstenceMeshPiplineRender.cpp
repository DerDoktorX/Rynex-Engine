#include "rypch.h"
#include "InstenceMeshPiplineRender.h"

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/RenderCommand.h>

namespace Rynex {


#pragma region Base


	static glm::mat4 computePSMMatrix(const glm::mat4& camView, const glm::mat4& camProj, const glm::vec3& lightDirWorld, float nearPlane, float farPlane)
	{
		// 1. Matrix für Welt -> post-perspektivischen Raum
		glm::mat4 cameraMatrix = camProj * camView;

		// 2. Lichtrichtung im post-perspektivischen Raum
		//    Direktionales Licht: Wähle einen sehr weit entfernten Punkt entgegen der Lichtrichtung
		glm::vec3 lightPosWorld = glm::vec3(0.0f) - lightDirWorld * 1000.0f; // oder camPos - lightDir * large
		glm::vec4 lightPosPost = cameraMatrix * glm::vec4(lightPosWorld, 1.0f);
		//    Falls w < 0 (hinter der Kamera), Vorzeichen speziell behandeln (hier vereinfacht)
		if (lightPosPost.w <= 0.0f) lightPosPost.w = 1e-6f;
		lightPosPost /= lightPosPost.w; // perspektivische Division -> NDC-Raum

		// 3. Blickrichtung: Zentrum des Einheitswürfels
		glm::vec3 centerPost = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 upPost = glm::vec3(0.0f, 1.0f, 0.0f);

		glm::mat4 lightViewPost = glm::lookAt(glm::vec3(lightPosPost), centerPost, upPost);

		// 4. Alle 8 Ecken des Kamera-NDC-Würfels im post-persp. Lichtraum
		std::vector<glm::vec4> corners = {
			{-1,-1,-1,1}, { 1,-1,-1,1}, {-1, 1,-1,1}, { 1, 1,-1,1},
			{-1,-1, 1,1}, { 1,-1, 1,1}, {-1, 1, 1,1}, { 1, 1, 1,1}
		};
		glm::vec3 minCorner(FLT_MAX), maxCorner(-FLT_MAX);
		for (auto& c : corners) {
			glm::vec4 p = lightViewPost * c;
			p /= p.w;
			minCorner = glm::min(minCorner, glm::vec3(p));
			maxCorner = glm::max(maxCorner, glm::vec3(p));
		}

		// 5. Perspektivische Lichtprojektion, die den Bereich [minCorner, maxCorner] abdeckt
		float nearL = std::max(0.001f, -maxCorner.z);
		float farL = -minCorner.z;
		float left = minCorner.x;
		float right = maxCorner.x;
		float bottom = minCorner.y;
		float top = maxCorner.y;

		glm::mat4 lightProjPost = glm::frustum(left, right, bottom, top, nearL, farL);

		// 6. Gesamte PSM-Matrix
		return lightProjPost * lightViewPost * cameraMatrix;
	}


	InstenceMeshPiplineRenderBase::InstenceMeshPiplineRenderBase()
		: m_Shader(nullptr)
		, m_CameraBuffer(nullptr)
		, m_RenderMode(0)
		, m_ManagingMode(PiplineManagingState::Managing_None)
		, m_DrawsAfterLastUpdate(0u)
		, m_InstencCount(0u)
		, m_SingleMeshObject()
	{
	}

	InstenceMeshPiplineRenderBase::~InstenceMeshPiplineRenderBase()
	{
		RY_DESTROY_REF(m_ModelBufferVAO);
		RY_DESTROY_REF(m_VertexArray);
	}

	const BufferLayout& InstenceMeshPiplineRenderBase::GetExpetedOutput() const
	{
		return m_OutPut;
	}

	void InstenceMeshPiplineRenderBase::SetExpetedOutput(const BufferLayout& output)
	{
		m_OutPut = output;
	}

	void InstenceMeshPiplineRenderBase::SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer)
	{
		m_CameraBuffer = camerbuffer;

	}

	void InstenceMeshPiplineRenderBase::SetDisplayUniformBuffer(Ref<UniformBuffer> dispalaybuffer)
	{
	}

	

	void InstenceMeshPiplineRenderBase::SetDataMangingFlags(PiplineManagingState flags)
	{
		RY_CORE_WARN("This PiplineManagingState is changing nothing on this is a one Render Object Call");
		m_ManagingMode = flags;
	}

	void InstenceMeshPiplineRenderBase::DrawNow()
	{
		DrawNow(m_RenderMode);
	}

	bool InstenceMeshPiplineRenderBase::IsExpextedOutPut(const Ref<Shader>& shader) const
	{
		RY_REMBER_FUNC_CHANGE("Implemnt function check if the out put layout matches the out put layout from shader!");
		const BufferLayout& layout = shader->GetOutPut();
		return layout == m_OutPut;
	}

	void InstenceMeshPiplineRenderBase::BindResources()
	{
		m_Shader->Bind();
		m_CameraBuffer->Bind(UniformBinding_MainCamer);
	}

	void InstenceMeshPiplineRenderBase::UnbindResources()
	{
		m_Shader->UnBind();
		m_CameraBuffer->UnBind(UniformBinding_MainCamer);
	}

	void InstenceMeshPiplineRenderBase::Clear()
	{
		m_InstencCount = 0u;
		m_RenderMode = 0;
		RY_DESTROY_REF(m_Shader);
		RY_DESTROY_REF(m_CameraBuffer);
	}

	void InstenceMeshPiplineRenderBase::ClearRenderObjects()
	{
		m_InstencCount = 0u;
	}


#pragma endregion


#pragma region Shade

    InstenceMeshPiplineRenderShade::InstenceMeshPiplineRenderShade()
		: InstenceMeshPiplineRenderBase()
		, m_LigthBuffer(nullptr)	
		, m_RenderObject()
		, m_MaterielBuffer(nullptr)
		, m_AlbdeoTex(nullptr)
		, m_ShadowTex(nullptr)
    {
    }

	InstenceMeshPiplineRenderShade::~InstenceMeshPiplineRenderShade()
    {
		Clear();

		
    }

	void InstenceMeshPiplineRenderShade::SubmitRenderTargetResurces(ViewPassStorage& viewPass)
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

	void InstenceMeshPiplineRenderShade::SubmitRenderTargetResurcesReadImg(const Ref<Texture>& texture)
	{
		m_ShadowTex = texture;
	}

	void InstenceMeshPiplineRenderShade::SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>& buffer)
	{
		m_LigthBuffer = buffer;
	}

	

	void InstenceMeshPiplineRenderShade::BindResources()
	{
		InstenceMeshPiplineRenderBase::BindResources();

		m_MaterielBuffer->Bind(UniformBinding_Materiel);
		m_LigthBuffer->Bind(UniformBinding_LigthCamera);

		m_AlbdeoTex->Bind(TextureBinding_Abldoe);
		m_ShadowTex->Bind(TextureBinding_Shadow);
	}

	void InstenceMeshPiplineRenderShade::UnbindResources()
	{
		InstenceMeshPiplineRenderBase::UnbindResources();
		m_LigthBuffer->UnBind(UniformBinding_LigthCamera);
		m_MaterielBuffer->UnBind(UniformBinding_Materiel);



		m_AlbdeoTex->UnBind(TextureBinding_Abldoe);
		m_ShadowTex->UnBind(TextureBinding_Shadow);
	}


	bool InstenceMeshPiplineRenderShade::Empty() const
	{
		return 0 == m_InstencCount;

	}

	bool InstenceMeshPiplineRenderShade::IsFull() const
	{
		if (State_MaxEntityRender < m_InstencCount)
		{
			uint32_t toManyObjects = m_InstencCount - State_MaxEntityRender;
			RY_CORE_WARN("This RenderPiline has {} more Stored then allowd", toManyObjects);
			return true;
		}
		return State_MaxEntityRender == m_InstencCount;
	}

	void InstenceMeshPiplineRenderShade::Clear()
	{
		InstenceMeshPiplineRenderBase::Clear();
		RY_DESTROY_REF(m_AlbdeoTex);
		RY_DESTROY_REF(m_ShadowTex);
		RY_DESTROY_REF(m_LigthBuffer);
		RY_DESTROY_REF(m_SingleMeshObject._Material);
		RY_DESTROY_REF(m_SingleMeshObject._MeshSingle);

	}



	uint64_t InstenceMeshPiplineRenderShade::GetVertexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
		uint64_t number = 0ull;
		uint32_t i = 0;
		
		
		const Ref<VertexBuffer>& buffer = vab;
		uint64_t numberVAB = reinterpret_cast<uint64_t>(buffer.get());
		number |= numberVAB << (i * Hash_BindingPointMultyplyNumberBitMove);
		i++;
		
		return number;
	}

	uint64_t InstenceMeshPiplineRenderShade::GetIndexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		const Ref<IndexBuffer>& iba = meshSingle->GetShadeIndexBuffer();
		uint64_t number = reinterpret_cast<uint64_t>(iba.get());
		return number;
	}

	uint64_t InstenceMeshPiplineRenderShade::GetIndirectBufferNumber() const
	{
		return 0ull;
	}

	uint64_t InstenceMeshPiplineRenderShade::GetTextureNumber() const
	{
		uint64_t number = 0;
		uint64_t albedoNumber = reinterpret_cast<uint64_t>(m_AlbdeoTex.get());
		uint64_t shadowNumber = reinterpret_cast<uint64_t>(m_ShadowTex.get());
		number |= albedoNumber << (Hash_BindingPointMultyplyNumberBitMove * TextureBinding_Abldoe);
		number |= shadowNumber << (Hash_BindingPointMultyplyNumberBitMove * TextureBinding_Shadow);
		return number;
	}

	uint64_t InstenceMeshPiplineRenderShade::GetUniformBufferNumber() const
	{
		uint64_t number = 0ull;
		uint64_t ligtheNumber = reinterpret_cast<uint64_t>(m_LigthBuffer.get());
		uint64_t cameraNumber = reinterpret_cast<uint64_t>(m_CameraBuffer.get());
		uint64_t materilNumber = reinterpret_cast<uint64_t>(m_MaterielBuffer.get());

		number |= ligtheNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_LigthCamera);
		number |= cameraNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_MainCamer);
		number |= materilNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_Materiel);

		return number;
	}

	uint64_t InstenceMeshPiplineRenderShade::GetStorageBufferNumber() const
	{		
		uint64_t number = 0ull;
		return number;
	}

	void InstenceMeshPiplineRenderShade::SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		if (m_InstencCount != storeIndex)
			storeIndex = m_InstencCount;

		m_RenderObject.SetObject(storeIndex, model, entityID);
		m_InstencCount++;
		m_DrawsAfterLastUpdate = 0u;
	}

	PiplineResultState InstenceMeshPiplineRenderShade::SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		int result = CheckSubmiteMeshObject(shader, singleMesh);
		if (State_MaxEntityRender <= m_InstencCount)
		{
			result = result | Result_NoRenderObjectSpaceLeft;
			return static_cast<PiplineResultState>(result);
		}
		if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
		{
			
			if(m_InstencCount != storeIndex)
				storeIndex = m_InstencCount;
			
			if (SubmiteResources(shader, singleMesh))
				m_RenderObject.SetObjectForce(storeIndex, model, entityID);
			else
				m_RenderObject.SetObject(storeIndex, model, entityID);
			m_InstencCount++;
			
			m_DrawsAfterLastUpdate = 0u;
			result = Result_Success;
		}


		return static_cast<PiplineResultState>(result);
	}

	void InstenceMeshPiplineRenderShade::BeforeDrawCall()
	{
		if (State_MaxNotUpdateDraws <= m_DrawsAfterLastUpdate)
		{
			RY_CORE_WARN("We have draw this object now {} times and never updated!", m_DrawsAfterLastUpdate);
		}
		m_DrawsAfterLastUpdate++;


		if (nullptr == m_ModelBufferVAO)
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = count * sizeof(RenderObject);
			constexpr uint32_t instanceIndex = 1u;
			constexpr bool aktive = true;
			BufferLayout layout = BufferLayout({
				{ SDT::Float4x4, "a_ModelMarix" },
				{ SDT::Float4x4, "a_NormleMatrix" },
				{ SDT::Int, "a_EntityID" },
				{ SDT::Int3, "a_Empty" }
			}, instanceIndex);
			layout.SetAutoCompress(true);

			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferFlag::None, layout);
			RY_CORE_ASSERT(m_VertexArray->GetVertexBuffersCount() != 2)
			m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
		}
		else if (m_RenderObject.NeedUpdate())
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
			uint32_t bufferBytesSize = m_ModelBufferVAO->GetByteSize();
			uint32_t halfByteSize = bufferBytesSize / 2u;

			if (bytesSize <= bufferBytesSize && halfByteSize < bytesSize)
			{

				m_ModelBufferVAO->SetData(dataPtr, bytesSize);
				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
			}
			else
			{

				m_ModelBufferVAO->ResizeBuffer(dataPtr, bytesSize);
			
				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);


			}				

			m_RenderObject.Updated();

		}
	}


	void InstenceMeshPiplineRenderShade::DrawNow(int flags)
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
#if 0
		Mesh::PerDrawObject drawElement = meshSingle->GetShadePerDrawObjectIndrect();
		RenderCommand::SetMode(flags);
		BindResources();
		RY_CORE_ASSERT(0 < m_InstencCount);
		drawElement.InstancesCount = m_InstencCount;
		RenderCommand::DrawElement(m_VertexArray, drawElement);
#else
		ShaderDrawResource drawList = CreateShaderDrawResource();
		
		drawList.renderMode = m_SingleMeshObject._Material->GetShadeRenderMode();
		drawList.shaderProgramm = m_Shader;
		drawList.vao = m_VertexArray;
		drawList.drawElement = meshSingle->GetShadePerDrawObjectIndrect();
		drawList.drawElement.InstancesCount = m_InstencCount;
		drawList.GetBindeUniform().at(UniformBinding_MainCamer) = m_CameraBuffer;
		drawList.GetBindeUniform().at(UniformBinding_Materiel) = m_MaterielBuffer;
		drawList.GetBindeUniform().at(UniformBinding_LigthCamera) = m_LigthBuffer;

		drawList.GetBindeTextures().at(TextureBinding_Abldoe) = m_AlbdeoTex;
		drawList.GetBindeTextures().at(TextureBinding_Shadow) = m_ShadowTex;
#if 0
		Ref<RenderTarget>& target = Renderer::GetRenderTargetMain();
		target->AddDrawPass(drawList);
#else
		std::vector<ShaderDrawResource>& shaderDrawResourceVec = Renderer::GetShaderDrawResourceMain();
		shaderDrawResourceVec.emplace_back(drawList);
#endif

#endif
#if RY_UNBIND
		UnbindResources();
#endif
	}

	bool InstenceMeshPiplineRenderShade::SubmiteResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		m_Shader = shader;
		m_SingleMeshObject = singleMesh;

		const Ref<Material>& materiel = m_SingleMeshObject._Material;

		m_AlbdeoTex = materiel->GetAlbedoTextures();
		m_RenderMode = materiel->GetShadeRenderMode();
		
		m_MaterielBuffer = materiel->GetMaterielUniformBuffer();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		return CheckVAOFromMeshSingleShade(m_VertexArray, meshSingle);
	}

	int InstenceMeshPiplineRenderShade::CheckSubmiteMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		int result = Result_None;

		CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
		CheckObject(m_SingleMeshObject._MeshSingle, singleMesh._MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);
		const Ref<Material>& materiel = singleMesh._Material;
		CheckObject(m_SingleMeshObject._Material, materiel, result, Result_NotAllowedShadeDefinition, Result_NoShadeDefinitionSpaceLeft);

		if (nullptr != materiel)
			CheckObject(m_AlbdeoTex, materiel->GetAlbedoTextures(), result, Result_NotAllowedTexture, Result_NoTextexurSpaceLeft);

		return result;
	}

	

#pragma endregion

#pragma region Depth

	InstenceMeshPiplineRenderDepth::InstenceMeshPiplineRenderDepth()
		: InstenceMeshPiplineRenderBase()
		, m_RenderObject()
	{
	}

	InstenceMeshPiplineRenderDepth::~InstenceMeshPiplineRenderDepth()
	{
		Clear();
	}

	void InstenceMeshPiplineRenderDepth::SubmitRenderTargetResurces(ViewPassStorage& viewPass)
	{
	}

	void InstenceMeshPiplineRenderDepth::SubmitRenderTargetResurcesReadImg(const Ref<Texture>& texture)
	{
	}

	void InstenceMeshPiplineRenderDepth::SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>& buffer)
	{
	}

	void InstenceMeshPiplineRenderDepth::SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer)
	{

		const CameraPackege& packegeMain = Renderer::GetCameraPackegeMain();
		const CameraPackege& packegeCurent = Renderer::GetCameraPackegeCurent();
		glm::mat4 psm = computePSMMatrix(packegeMain.ViewMatrix, packegeMain.ProjetionMatrix, packegeCurent.Postion, 0.1, 50);
		m_CameraBuffer = UniformBuffer::Create(
			&psm, sizeof(glm::mat4)
		);
	}

	void InstenceMeshPiplineRenderDepth::BindResources()
	{

		
		InstenceMeshPiplineRenderBase::BindResources();
		// Renderer::GetPackegeCamerUniformMain()->Bind(0);

	}



	void InstenceMeshPiplineRenderDepth::UnbindResources()
	{
		InstenceMeshPiplineRenderBase::UnbindResources();
	}


	bool InstenceMeshPiplineRenderDepth::Empty() const
	{
		return 0 == m_InstencCount;
	}

	bool InstenceMeshPiplineRenderDepth::IsFull() const
	{
		if (State_MaxEntityRender < m_InstencCount)
		{
			uint32_t toManyObjects = m_InstencCount - State_MaxEntityRender;
			RY_CORE_WARN("This RenderPiline has {} more Stored then allowd", toManyObjects);
			return true;
		}
		return State_MaxEntityRender == m_InstencCount;
	}

	void InstenceMeshPiplineRenderDepth::Clear()
	{
		InstenceMeshPiplineRenderBase::Clear();
		RY_DESTROY_REF(m_SingleMeshObject._Material);
		RY_DESTROY_REF(m_SingleMeshObject._MeshSingle);
	}



	uint64_t InstenceMeshPiplineRenderDepth::GetVertexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
		uint64_t number = 0ull;
		uint32_t i = 0;
		
			
		uint64_t numberVAB = reinterpret_cast<uint64_t>(vab.get());
		number |= numberVAB << (i * Hash_BindingPointMultyplyNumberBitMove);
		i++;
		
		return number;
	}

	uint64_t InstenceMeshPiplineRenderDepth::GetIndexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		const Ref<IndexBuffer>& ib = meshSingle->GetDepthIndexBuffer();
		uint64_t number = reinterpret_cast<uint64_t>(ib.get());
		return number;
	}

	uint64_t InstenceMeshPiplineRenderDepth::GetIndirectBufferNumber() const
	{
		return 0ull;
	}

	uint64_t InstenceMeshPiplineRenderDepth::GetTextureNumber() const
	{
		uint64_t number = 0ull;
		return number;
	}

	uint64_t InstenceMeshPiplineRenderDepth::GetUniformBufferNumber() const
	{
		uint64_t number = 0ull;
		uint64_t cameraNumber = reinterpret_cast<uint64_t>(m_CameraBuffer.get());
		number |= cameraNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_MainCamer);
		return number;
	}

	uint64_t InstenceMeshPiplineRenderDepth::GetStorageBufferNumber() const
	{
		uint64_t number = 0;
		return number;
	}

	void InstenceMeshPiplineRenderDepth::SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		if (m_InstencCount != storeIndex)
			storeIndex = m_InstencCount;

		m_RenderObject.SetObject(storeIndex, model, entityID);
		m_InstencCount++;
		m_DrawsAfterLastUpdate = 0u;
	}

	PiplineResultState InstenceMeshPiplineRenderDepth::SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		int result = CheckSubmiteMeshObject(shader, singleMesh);
		if (State_MaxEntityRender <= m_InstencCount)
		{
			result = result | Result_NoRenderObjectSpaceLeft;
			return static_cast<PiplineResultState>(result);
		}
		if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
		{

			if (m_InstencCount != storeIndex)
				storeIndex = m_InstencCount;

			if (SubmiteResources(shader, singleMesh))
				m_RenderObject.SetObjectForce(storeIndex, model, entityID);
			else
				m_RenderObject.SetObject(storeIndex, model, entityID);

			m_InstencCount++;

			m_DrawsAfterLastUpdate = 0u;
			result = Result_Success;
		}


		return static_cast<PiplineResultState>(result);
	}

	void InstenceMeshPiplineRenderDepth::BeforeDrawCall()
	{

		if (State_MaxNotUpdateDraws <= m_DrawsAfterLastUpdate)
		{
			RY_CORE_WARN("We have draw this object now {} times and never updated!", m_DrawsAfterLastUpdate);
		}
		m_DrawsAfterLastUpdate++;

		if (nullptr == m_ModelBufferVAO)
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);

			constexpr uint32_t instanceAddIndex = 1u;
			constexpr bool aktive = true;
			constexpr bool normilze = false;
			constexpr uint32_t countElements = 0u;
			BufferLayout layout = BufferLayout({
				{ SDT::Float4, "a_ModelMarix[0]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[1]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[2]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[3]", aktive, countElements, normilze },
			}, instanceAddIndex);

			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferFlag::None, layout);
			RY_CORE_ASSERT(m_VertexArray->GetVertexBuffersCount() != 2);
			m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
		}
		else if (m_RenderObject.NeedUpdate())
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
			uint32_t bufferBytesSize = m_ModelBufferVAO->GetByteSize();
			uint32_t halfByteSize = bufferBytesSize / 2u;

			if (bytesSize <= bufferBytesSize && halfByteSize < bytesSize)
			{

				m_ModelBufferVAO->SetData(dataPtr, bytesSize);
		
				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);

			}
			else
			{

				m_ModelBufferVAO->ResizeBuffer(dataPtr, bytesSize);
		
				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
				else
					m_VertexArray->SetVertexBufferNew(m_ModelBufferVAO);
			}
			m_RenderObject.Updated();

		}
	}


	void InstenceMeshPiplineRenderDepth::DrawNow(int flags)
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;

		if (nullptr == m_Shader
			|| nullptr == m_VertexArray
			|| nullptr == m_CameraBuffer)
		{
			RY_CORE_ERROR("Draw call faild, becouse core resurces are not set!");
			return;
		}
		BeforeDrawCall();

		Mesh::PerDrawObject drawElement = meshSingle->GetDepthPerDrawObjectIndrect();
		RenderCommand::SetMode(flags);
		BindResources();
		RY_CORE_ASSERT(0 < m_InstencCount);
		drawElement.InstancesCount = m_InstencCount;
		RenderCommand::DrawElement(m_VertexArray, drawElement);
	}

	bool InstenceMeshPiplineRenderDepth::SubmiteResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		m_Shader = shader;
		m_SingleMeshObject = singleMesh;
		const Ref<Material>& materiel = m_SingleMeshObject._Material;

		m_RenderMode = materiel->GetShadeRenderMode();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		return CheckVAOFromMeshSingleDepth(m_VertexArray, meshSingle);
	}


	int InstenceMeshPiplineRenderDepth::CheckSubmiteMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		int result = Result_None;

		CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
		CheckObject(m_SingleMeshObject._MeshSingle, singleMesh._MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);
		const Ref<Material>& materielTest = singleMesh._Material;
		const Ref<Material>& materielThis = m_SingleMeshObject._Material;
		if (nullptr == materielTest)
			return (result | Result_NotAllowedShadeDefinition);
		if (nullptr == materielThis)
			return result;

		if(materielThis->GetDepthRenderMode() != materielTest->GetDepthRenderMode())
			result |= Result_NotAllowedShadeDefinition | Result_NoShadeDefinitionSpaceLeft;


		return result;
	}


#pragma endregion

#pragma region Shape

	InstenceMeshPiplineRenderShape::InstenceMeshPiplineRenderShape()
		: InstenceMeshPiplineRenderBase()
		, m_RenderObject()
	{
	}

	InstenceMeshPiplineRenderShape::~InstenceMeshPiplineRenderShape()
	{
		Clear();
	}

	void InstenceMeshPiplineRenderShape::SubmitRenderTargetResurces(ViewPassStorage& viewPass)
	{
	}

	void InstenceMeshPiplineRenderShape::SubmitRenderTargetResurcesReadImg(const Ref<Texture>& texture)
	{
	}

	void InstenceMeshPiplineRenderShape::SubmitRenderTargetResurcesReadUB(const Ref<UniformBuffer>& buffer)
	{
	}


	void InstenceMeshPiplineRenderShape::BindResources()
	{

		InstenceMeshPiplineRenderBase::BindResources();
		m_AlbdeoTex->Bind(TextureBinding_Abldoe);
	}

	void InstenceMeshPiplineRenderShape::UnbindResources()
	{
		InstenceMeshPiplineRenderBase::UnbindResources();
		m_AlbdeoTex->UnBind(TextureBinding_Abldoe);
	}


	bool InstenceMeshPiplineRenderShape::Empty() const
	{
		return 0 == m_InstencCount;

	}

	bool InstenceMeshPiplineRenderShape::IsFull() const
	{
		if (State_MaxEntityRender < m_InstencCount)
		{
			uint32_t toManyObjects = m_InstencCount - State_MaxEntityRender;
			RY_CORE_WARN("This RenderPiline has {} more Stored then allowd", toManyObjects);
			return true;
		}
		return State_MaxEntityRender == m_InstencCount;
	}

	void InstenceMeshPiplineRenderShape::Clear()
	{

		InstenceMeshPiplineRenderBase::Clear();
		RY_DESTROY_REF(m_AlbdeoTex);
	}

	
	

	uint64_t InstenceMeshPiplineRenderShape::GetVertexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
		uint64_t number = 0ull;
		uint32_t i = 0;

		uint64_t numberVAB = reinterpret_cast<uint64_t>(vab.get());
		number |= numberVAB << (i * Hash_BindingPointMultyplyNumberBitMove);
		i++;

		return number;
	}

	uint64_t InstenceMeshPiplineRenderShape::GetIndexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		const Ref<IndexBuffer>& iab = meshSingle->GetDepthIndexBuffer();
		uint64_t number = reinterpret_cast<uint64_t>(iab.get());
		return number;
	}

	uint64_t InstenceMeshPiplineRenderShape::GetIndirectBufferNumber() const
	{
		uint64_t number = 0;
		return number;
	}

	uint64_t InstenceMeshPiplineRenderShape::GetTextureNumber() const
	{
		uint64_t number = 0;
		uint64_t albedoNumber = reinterpret_cast<uint64_t>(m_AlbdeoTex.get());
		number |= albedoNumber << (Hash_BindingPointMultyplyNumberBitMove * TextureBinding_Abldoe);
		return number;
	}

	uint64_t InstenceMeshPiplineRenderShape::GetUniformBufferNumber() const
	{
		uint64_t number = 0;
		uint64_t cameraNumber = reinterpret_cast<uint64_t>(m_CameraBuffer.get());
		number |= cameraNumber << (Hash_BindingPointMultyplyNumberBitMove * UniformBinding_MainCamer);

		return number;
	}

	uint64_t InstenceMeshPiplineRenderShape::GetStorageBufferNumber() const
	{
		uint64_t number = 0;
		return number;
	}

	void InstenceMeshPiplineRenderShape::SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		if (m_InstencCount != storeIndex)
			storeIndex = m_InstencCount;

		m_RenderObject.SetObject(storeIndex, model, entityID);
		m_InstencCount++;
		m_DrawsAfterLastUpdate = 0u;
	}

	PiplineResultState InstenceMeshPiplineRenderShape::SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		int result = CheckSubmiteMeshObject(shader, singleMesh);
		if (State_MaxEntityRender <= m_InstencCount)
		{
			result = result | Result_NoRenderObjectSpaceLeft;
			return static_cast<PiplineResultState>(result);
		}
		if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
		{
			

			if(m_InstencCount != storeIndex)
				storeIndex = m_InstencCount;

			if (SubmiteResources(shader, singleMesh))
				m_RenderObject.SetObjectForce(storeIndex, model, entityID);
			else
				m_RenderObject.SetObject(storeIndex, model, entityID);

			m_InstencCount++;
			m_DrawsAfterLastUpdate = 0u;
			result = Result_Success;
		}


		return static_cast<PiplineResultState>(result);
	}

	void InstenceMeshPiplineRenderShape::BeforeDrawCall()
	{
		if (State_MaxNotUpdateDraws <= m_DrawsAfterLastUpdate)
		{
			RY_CORE_WARN("We have draw this object now {} times and never updated!", m_DrawsAfterLastUpdate);
		}
		m_DrawsAfterLastUpdate++;

		if (nullptr == m_ModelBufferVAO)
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = count * sizeof(RenderObject);
			constexpr uint32_t instanceAddIndex = 1u;
			constexpr bool aktive = true;
			constexpr uint32_t countElements = 0u;
			constexpr bool normilze = false;

			BufferLayout layout = BufferLayout({
				{ SDT::Float4, "a_ModelMarix[0]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[1]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[2]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[3]", aktive, countElements, normilze },
				}, instanceAddIndex);
			

			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferFlag::None, layout);
			RY_CORE_ASSERT(2 != m_VertexArray->GetVertexBuffersCount());
			m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
		}
		else if (m_RenderObject.NeedUpdate())
		{

			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
			uint32_t bufferBytesSize = m_ModelBufferVAO->GetByteSize();
			uint32_t halfByteSize = bufferBytesSize / 2u;

			if (bytesSize <= bufferBytesSize && halfByteSize < bytesSize)
			{

				m_ModelBufferVAO->SetData(dataPtr, bytesSize);

				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
			}
			else
			{
				m_ModelBufferVAO->ResizeBuffer(dataPtr, bytesSize);

				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
				else
					m_VertexArray->SetVertexBufferNew(m_ModelBufferVAO);
			}

			
			m_RenderObject.Updated();
		}
	}


	void InstenceMeshPiplineRenderShape::DrawNow(int flags)
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;

		if (nullptr == m_Shader
			|| nullptr == m_VertexArray
			|| nullptr == m_CameraBuffer
			|| nullptr == m_AlbdeoTex)
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

	}

	bool InstenceMeshPiplineRenderShape::SubmiteResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		m_Shader = shader;
		m_SingleMeshObject = singleMesh;

		const Ref<Material>& materiel = m_SingleMeshObject._Material;


		m_AlbdeoTex = materiel->GetAlbedoTextures();
		m_RenderMode = materiel->GetShadeRenderMode();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		return CheckVAOFromMeshSingleShape(m_VertexArray, meshSingle);
	}



	int InstenceMeshPiplineRenderShape::CheckSubmiteMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		int result = Result_None;

		CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
		CheckObject(m_SingleMeshObject._MeshSingle, singleMesh._MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);
		const Ref<Material>& materiel = singleMesh._Material;
		CheckObject(m_SingleMeshObject._Material, materiel, result, Result_NotAllowedShadeDefinition, Result_NoShadeDefinitionSpaceLeft);

		if (nullptr != materiel)
			CheckObject(m_AlbdeoTex, materiel->GetAlbedoTextures(), result, Result_NotAllowedTexture, Result_NoTextexurSpaceLeft);

		return result;
	}

#pragma endregion
	
	

}

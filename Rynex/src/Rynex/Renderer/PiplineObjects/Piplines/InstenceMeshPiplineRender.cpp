#include "rypch.h"
#include "InstenceMeshPiplineRender.h"

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/RenderCommand.h>

namespace Rynex {

#define RY_UNBIND 0

#pragma region Base

#ifdef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE

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
#endif

#pragma endregion


#pragma region Shade

    InstenceMeshPiplineRenderShade::InstenceMeshPiplineRenderShade()
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		: m_Shader(nullptr)
		, m_CameraBuffer(nullptr)
		, m_RenderMode(0)
		, m_ManagingMode(PiplineManagingState::Managing_None)
		, m_DrawsAfterLastUpdate(0u)
		, m_InstencCount(0u)
		
		, m_ModelBufferSSOB(nullptr)
#else
		: InstenceMeshPiplineRenderBase()
#endif 
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
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		RY_DESTROY_REF(m_ModelBufferSSOB);
		m_DrawsAfterLastUpdate = 0;
		RY_DESTROY_REF(m_ModelBufferVAO);
#endif
		
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
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
	void InstenceMeshPiplineRenderShade::SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer)
	{
		m_CameraBuffer = camerbuffer;
	}

	void InstenceMeshPiplineRenderShade::SetDisplayUniformBuffer(Ref<UniformBuffer> dispalaybuffer)
	{
	}
	
#endif
	

	void InstenceMeshPiplineRenderShade::BindResources()
	{
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_Shader->Bind();

		m_CameraBuffer->Bind(UniformBinding_MainCamer);
#else
		InstenceMeshPiplineRenderBase::BindResources();
#endif
		m_MaterielBuffer->Bind(UniformBinding_Materiel);
		m_LigthBuffer->Bind(UniformBinding_LigthCamera);

#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		m_ModelBufferSSOB->Bind(StorageBinding_RenderObject);
#endif
		m_AlbdeoTex->Bind(TextureBinding_Abldoe);
		m_ShadowTex->Bind(TextureBinding_Shadow);
	}

	void InstenceMeshPiplineRenderShade::UnbindResources()
	{
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_Shader->UnBind();

		m_CameraBuffer->UnBind(UniformBinding_MainCamer);
#else
		InstenceMeshPiplineRenderBase::UnbindResources();
#endif
		m_LigthBuffer->UnBind(UniformBinding_LigthCamera);
		m_MaterielBuffer->UnBind(UniformBinding_Materiel);


#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		m_ModelBufferSSOB->UnBind(StorageBinding_RenderObject);
#endif
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
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_InstencCount = 0u;
		m_RenderMode = 0;
		RY_DESTROY_REF(m_Shader);
		RY_DESTROY_REF(m_CameraBuffer);
		
#else
		InstenceMeshPiplineRenderBase::Clear();
#endif
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
#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		uint64_t modelNumber = reinterpret_cast<uint64_t>(m_ModelBufferSSOB.get());
		number |= modelNumber << (Hash_BindingPointMultyplyNumberBitMove * StorageBinding_RenderObject);
#endif
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

#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		if (nullptr == m_ModelBufferSSOB)
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = count * sizeof(RenderObject);

			m_ModelBufferSSOB = StorageBuffer::Create(dataPtr, bytesSize, BufferType::ShaderStorage, StorageBuffer::Type::None);
		}
		else if (m_RenderObject.NeedUpdate())
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
			uint32_t bufferBytesSize = m_ModelBufferSSOB->GetByteSize();
			if(bytesSize <= bufferBytesSize )
			{
				m_ModelBufferSSOB->SetData(dataPtr, bytesSize);
			}
			else
			{
				m_ModelBufferSSOB->Resize2D(dataPtr, bytesSize);
			}
			m_RenderObject.Updated();
		}
#else
		if (nullptr == m_ModelBufferVAO)
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = count * sizeof(RenderObject);
			constexpr uint32_t instanceIndex = 1u;
			constexpr bool aktive = true;
			BufferLayout layout = BufferLayout({
#if 0
				{ SDT::Float4, "a_ModelMarix[0]" },
				{ SDT::Float4, "a_ModelMarix[1]" },
				{ SDT::Float4, "a_ModelMarix[2]" },
				{ SDT::Float4, "a_ModelMarix[3]" },

				{ SDT::Float4, "a_NormleMatrix[0]" },
				{ SDT::Float4, "a_NormleMatrix[1]" },
				{ SDT::Float4, "a_NormleMatrix[2]" },
				{ SDT::Float4, "a_NormleMatrix[3]" },
#else
				{ SDT::Float4x4, "a_ModelMarix" },
				{ SDT::Float4x4, "a_NormleMatrix" },

#endif
				{ SDT::Int, "a_EntityID" },
				{ SDT::Int3, "a_Empty" }

			}, instanceIndex);
			layout.SetAutoCompress(true);
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferDataUsage::DynamicDraw, layout);
#else
			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferFlag::None, layout);
#endif
			RY_CORE_ASSERT(m_VertexArray->GetVertexBuffersCount() != 2)
			m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
		}
		else if (m_RenderObject.NeedUpdate())
		{
#ifndef RY_ONCHNAGE_CREATE_INSTANCE_VAB_COMPLET_NEW
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
			uint32_t bufferBytesSize = m_ModelBufferVAO->GetByteSize();
			uint32_t halfByteSize = bufferBytesSize / 2u;

			if (bytesSize <= bufferBytesSize && halfByteSize < bytesSize)
			{
#ifdef RY_INSTANCE_ONCHANGE_PRINT
				RY_CORE_TRACE("InstenceMeshPiplineRenderShade: Update Buffer!");
#endif

				m_ModelBufferVAO->SetData(dataPtr, bytesSize);
				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
#if 0
				else
					m_VertexArray->SetVertexBufferNew(m_ModelBufferVAO);
#endif

			}
			else
			{
#ifdef RY_INSTANCE_ONCHANGE_PRINT
				RY_CORE_TRACE("InstenceMeshPiplineRenderShade: Resize Buffer!");
#endif
				m_ModelBufferVAO->ResizeBuffer(dataPtr, bytesSize);
			
				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
#if 0
				else
					m_VertexArray->SetVertexBufferNew(m_ModelBufferVAO);
#endif

			}				

			m_RenderObject.Updated();
#else
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
#if 0
			uint32_t bytesSize = count * sizeof(RenderObject);
#else
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
#endif

			constexpr uint32_t instanceAddIndex = 1u;
			constexpr bool aktive = true;
			constexpr bool normilze = false;
			constexpr uint32_t countElements = 0u;
			BufferLayout layout = {
#if 1
				{ SDT::Float4, "a_ModelMarix[0]", aktive, instanceAddIndex },
				{ SDT::Float4, "a_ModelMarix[1]", aktive, instanceAddIndex },
				{ SDT::Float4, "a_ModelMarix[2]", aktive, instanceAddIndex },
				{ SDT::Float4, "a_ModelMarix[3]", aktive, instanceAddIndex },

				{ SDT::Float4, "a_NormleMatrix[0]", aktive, instanceAddIndex },
				{ SDT::Float4, "a_NormleMatrix[1]", aktive, instanceAddIndex },
				{ SDT::Float4, "a_NormleMatrix[2]", aktive, instanceAddIndex },
				{ SDT::Float4, "a_NormleMatrix[3]", aktive, instanceAddIndex },
#else
				{ SDT::Float4x4, "a_ModelMarix", aktive, instanceAddIndex },
				{ SDT::Float4x4, "a_NormleMatrix", aktive, instanceAddIndex },

#endif
			};
			if (m_VertexArray->GetVertexBuffersCount() == 2u)
			{
				m_VertexArray->ClearVertexBuffers();
				CheckVAOFromMeshSingleShade(m_VertexArray, m_SingleMeshObject._MeshSingle);
			}
			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferDataUsage::None, layout);

			RY_CORE_ASSERT(m_VertexArray->GetVertexBuffersCount() != 2);
			m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
#endif
		}
#endif
	}

#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
	void InstenceMeshPiplineRenderShade::SetDataMangingFlags(PiplineManagingState flags)
	{
		RY_CORE_WARN("This PiplineManagingState is changing nothing on this is a one Render Object Call");
		m_ManagingMode = flags;
	}
	void InstenceMeshPiplineRenderShade::DrawNow()
	{
		DrawNow(m_RenderMode);
	}
#endif

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
		// RY_CORE_ASSERT(nullptr == m_Shader || m_Shader == shader);
		m_Shader = shader;
		// RY_CORE_ASSERT(nullptr == m_SingleMeshObject._MeshSingle || m_SingleMeshObject._MeshSingle == singleMesh._MeshSingle);
		// RY_CORE_ASSERT(nullptr == m_SingleMeshObject._Material || m_SingleMeshObject._Material == singleMesh._Material);
		m_SingleMeshObject = singleMesh;

		const Ref<Material>& materiel = m_SingleMeshObject._Material;

		// RY_CORE_ASSERT(nullptr == m_AlbdeoTex || m_AlbdeoTex == materiel->GetAlbedoTextures());
		// RY_CORE_ASSERT(0 == m_RenderMode || m_RenderMode == materiel->GetShadeRenderMode());

		m_AlbdeoTex = materiel->GetAlbedoTextures();
		m_RenderMode = materiel->GetShadeRenderMode();
		
		m_MaterielBuffer = materiel->GetMaterielUniformBuffer();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		return CheckVAOFromMeshSingleShade(m_VertexArray, meshSingle);
	}

#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
	uint32_t InstenceMeshPiplineRenderShade::GetCurentEntityRender() const
	{
		return m_InstencCount;
	}

	uint32_t InstenceMeshPiplineRenderShade::GetMaxEntityRender() const
	{
		return State_MaxEntityRender;
	}

	uint32_t InstenceMeshPiplineRenderShade::GetFrameCountNotUpdate() const
	{
		return m_DrawsAfterLastUpdate;
	}
#endif


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
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		: m_Shader(nullptr)
		, m_CameraBuffer(nullptr)
		, m_ModelBufferSSOB(nullptr)
		, m_RenderMode(0)
		, m_ManagingMode(PiplineManagingState::Managing_None)
		, m_DrawsAfterLastUpdate(0u)
		, m_InstencCount(0u)
#else
		: InstenceMeshPiplineRenderBase()
#endif
		, m_RenderObject()
	{
	}

	InstenceMeshPiplineRenderDepth::~InstenceMeshPiplineRenderDepth()
	{
		Clear();
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_DrawsAfterLastUpdate = 0;

		RY_DESTROY_REF(m_ModelBufferSSOB);
		RY_DESTROY_REF(m_ModelBufferVAO);
#endif
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

#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
	void InstenceMeshPiplineRenderDepth::SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer)
	{
		m_CameraBuffer = camerbuffer;
	}

	void InstenceMeshPiplineRenderDepth::SetDisplayUniformBuffer(Ref<UniformBuffer> dispalaybuffer)
	{
	}
#endif
	void InstenceMeshPiplineRenderDepth::BindResources()
	{
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_Shader->Bind();
		m_CameraBuffer->Bind(UniformBinding_MainCamer);

#else
		
		InstenceMeshPiplineRenderBase::BindResources();
		// Renderer::GetPackegeCamerUniformMain()->Bind(0);
#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		m_ModelBufferSSOB->Bind(StorageBinding_RenderObject);
#endif
#endif
	}



	void InstenceMeshPiplineRenderDepth::UnbindResources()
	{
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_Shader->UnBind();

		m_CameraBuffer->UnBind(UniformBinding_MainCamer);
#else
		InstenceMeshPiplineRenderBase::UnbindResources();
#endif

#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		m_ModelBufferSSOB->UnBind(StorageBinding_RenderObject);
#endif
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
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_InstencCount = 0u;
		m_RenderMode = 0;
		RY_DESTROY_REF(m_Shader);
		RY_DESTROY_REF(m_CameraBuffer);
#else
		InstenceMeshPiplineRenderBase::Clear();
#endif
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
#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		uint64_t modelNumber = reinterpret_cast<uint64_t>(m_ModelBufferSSOB.get());
		number |= modelNumber << (Hash_BindingPointMultyplyNumberBitMove * StorageBinding_RenderObject);
#endif
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

#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		if (nullptr == m_ModelBufferSSOB)
		{

			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = count * sizeof(RenderObject);

			m_ModelBufferSSOB = StorageBuffer::Create(dataPtr, bytesSize, BufferType::ShaderStorage, StorageBuffer::Type::None);
			m_RenderObject.Updated();
		}
		else if (m_RenderObject.NeedUpdate())
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
			uint32_t bufferBytesSize = m_ModelBufferSSOB->GetByteSize();
			uint32_t halfByteSize = bufferBytesSize / 2u;
			if (bytesSize <= bufferBytesSize && halfByteSize < bytesSize)
			{
				m_ModelBufferSSOB->SetData(dataPtr, bytesSize);
			}
			else
			{
				m_ModelBufferSSOB->Resize2D(dataPtr, bytesSize);
			}
			m_RenderObject.Updated();
		}
#else
		if (nullptr == m_ModelBufferVAO)
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
#if 0
			uint32_t bytesSize = count * sizeof(RenderObject);
#else
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
#endif

			constexpr uint32_t instanceAddIndex = 1u;
			constexpr bool aktive = true;
			constexpr bool normilze = false;
			constexpr uint32_t countElements = 0u;
			BufferLayout layout = BufferLayout({
#if 1 || !defined(RY_CHNAGE_OF_ORGNISE_LAYOUTE_INSTANC_INCREASE) || !defined(RY_HOLD_LAYOUT_ELEMENT_INCREAS_INSTANCE)
				{ SDT::Float4, "a_ModelMarix[0]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[1]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[2]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[3]", aktive, countElements, normilze },
			}, instanceAddIndex);
#elif !0 || !defined(RY_CHNAGE_OF_ORGNISE_LAYOUTE_INSTANC_INCREASE) || !defined(RY_HOLD_LAYOUT_ELEMENT_INCREAS_INSTANCE)
				{ SDT::Float4x4, "a_ModelMarix" },
			}, instanceAddIndex);

#elif 1 || defined(RY_CHNAGE_OF_ORGNISE_LAYOUTE_INSTANC_INCREASE) || defined(RY_HOLD_LAYOUT_ELEMENT_INCREAS_INSTANCE)
				{ SDT::Float4, "a_ModelMarix[0]", aktive, instanceAddIndex, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[1]", aktive, instanceAddIndex, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[2]", aktive, instanceAddIndex, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[3]", aktive, instanceAddIndex, countElements, normilze },
			});
#else
				{ SDT::Float4x4, "a_ModelMarix", aktive, instanceAddIndex },
			});
#endif
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferDataUsage::DynamicDraw, layout);
#else
			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferFlag::None, layout);
#endif
			RY_CORE_ASSERT(m_VertexArray->GetVertexBuffersCount() != 2);
			m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
		}
		else if (m_RenderObject.NeedUpdate())
		{
#ifndef RY_ONCHNAGE_CREATE_INSTANCE_VAB_COMPLET_NEW
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
			uint32_t bufferBytesSize = m_ModelBufferVAO->GetByteSize();
			uint32_t halfByteSize = bufferBytesSize / 2u;

			if (bytesSize <= bufferBytesSize && halfByteSize < bytesSize)
			{
#ifdef RY_INSTANCE_ONCHANGE_PRINT
				RY_CORE_TRACE("InstenceMeshPiplineRenderDepth: Update Buffer!");
#endif
				m_ModelBufferVAO->SetData(dataPtr, bytesSize);
		
				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);

			}
			else
			{
#ifdef RY_INSTANCE_ONCHANGE_PRINT
				RY_CORE_TRACE("InstenceMeshPiplineRenderDepth: Resize Buffer!");
#endif
				m_ModelBufferVAO->ResizeBuffer(dataPtr, bytesSize);
		
				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
				else
					m_VertexArray->SetVertexBufferNew(m_ModelBufferVAO);
			}
			m_RenderObject.Updated();
#else
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
#if 0
			uint32_t bytesSize = count * sizeof(RenderObject);
#else
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
#endif
			uint32_t halfByteSize = bufferBytesSize / 2u;

			constexpr uint32_t instanceAddIndex = 1u;
			constexpr bool aktive = true;
			constexpr bool normilze = false;
			constexpr uint32_t countElements = 0u;
			BufferLayout layout = {
#if 1

				{ SDT::Float4, "a_ModelMarix[0]", aktive, instanceAddIndex, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[1]", aktive, instanceAddIndex, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[2]", aktive, instanceAddIndex, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[3]", aktive, instanceAddIndex, countElements, normilze },
#else
				{ SDT::Float4x4, "a_ModelMarix", aktive, instanceAddIndex },
#endif
			};
			if (m_VertexArray->GetVertexBuffersCount() == 2u)
			{
				m_VertexArray->ClearVertexBuffers();
				CheckVAOFromMeshSingleDepth(m_VertexArray, m_SingleMeshObject._MeshSingle);
			}
			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferDataUsage::None, layout);

			RY_CORE_ASSERT(m_VertexArray->GetVertexBuffersCount() != 2);
			m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
#endif
		}
#endif
	}

#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
	void InstenceMeshPiplineRenderDepth::SetDataMangingFlags(PiplineManagingState flags)
	{
		RY_CORE_WARN("This PiplineManagingState is changing nothing on this is a one Render Object Call");
		m_ManagingMode = flags;
	}

	void InstenceMeshPiplineRenderDepth::DrawNow()
	{
		DrawNow(m_RenderMode);
	}
#endif

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
#if RY_UNBIND
		UnbindResources();
#endif
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

#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
	uint32_t InstenceMeshPiplineRenderDepth::GetCurentEntityRender()const
	{
		return m_InstencCount;
	}

	uint32_t InstenceMeshPiplineRenderDepth::GetMaxEntityRender()const
	{
		return State_MaxEntityRender;
	}

	uint32_t InstenceMeshPiplineRenderDepth::GetFrameCountNotUpdate()const
	{
		return m_DrawsAfterLastUpdate;
	}
#endif


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
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		: m_Shader(nullptr)
		, m_CameraBuffer(nullptr)
		, m_ModelBufferSSOB(nullptr)
		, m_AlbdeoTex(nullptr)
		, m_RenderMode(0)
		, m_ManagingMode(PiplineManagingState::Managing_None)
		, m_DrawsAfterLastUpdate(0u)
		, m_InstencCount(0u)
#else
		: InstenceMeshPiplineRenderBase()
#endif
		, m_RenderObject()
	{
	}

	InstenceMeshPiplineRenderShape::~InstenceMeshPiplineRenderShape()
	{
		Clear();
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_DrawsAfterLastUpdate = 0;
		RY_DESTROY_REF(m_ModelBufferSSOB);
		RY_DESTROY_REF(m_ModelBufferVAO);
#endif
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

#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
	void InstenceMeshPiplineRenderShape::SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer)
	{
		m_CameraBuffer = camerbuffer;
	}

	void InstenceMeshPiplineRenderShape::SetDisplayUniformBuffer(Ref<UniformBuffer> dispalaybuffer)
	{
	}
#endif

	void InstenceMeshPiplineRenderShape::BindResources()
	{
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_Shader->Bind();

		m_CameraBuffer->Bind(UniformBinding_MainCamer);
#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		m_ModelBufferSSOB->Bind(StorageBinding_RenderObject);
#endif
#else
		InstenceMeshPiplineRenderBase::BindResources();
#endif
		m_AlbdeoTex->Bind(TextureBinding_Abldoe);
	}

	void InstenceMeshPiplineRenderShape::UnbindResources()
	{
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_Shader->UnBind();

		m_CameraBuffer->UnBind(UniformBinding_MainCamer);
#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		m_ModelBufferSSOB->UnBind(StorageBinding_RenderObject);
#endif
#else
		InstenceMeshPiplineRenderBase::UnbindResources();
#endif

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
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
		m_InstencCount = 0u;
		m_RenderMode = 0;
		RY_DESTROY_REF(m_Shader);
		
		RY_DESTROY_REF(m_CameraBuffer);
		RY_DESTROY_REF(m_SingleMeshObject._Material);
		RY_DESTROY_REF(m_SingleMeshObject._MeshSingle);
#else
		InstenceMeshPiplineRenderBase::Clear();
#endif
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
#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		uint64_t modelNumber = reinterpret_cast<uint64_t>(m_ModelBufferSSOB.get());
		number |= modelNumber << (Hash_BindingPointMultyplyNumberBitMove * StorageBinding_RenderObject);
#endif
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

#ifdef RY_ENABLE_SSOB_MODEL_BUFFER
		if (nullptr == m_ModelBufferSSOB)
		{

			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			RY_CORE_ASSERT(m_InstencCount <= count, "To many instec too renderen!");

			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);

			m_ModelBufferSSOB = StorageBuffer::Create(dataPtr, bytesSize, BufferType::ShaderStorage, StorageBuffer::Type::None);
		}
		else if (m_RenderObject.NeedUpdate())
		{
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			RY_CORE_ASSERT(m_InstencCount <= count, "To many instec too renderen!");
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
			uint32_t bufferBytesSize = m_ModelBufferSSOB->GetByteSize();
			if (bytesSize <= bufferBytesSize && halfByteSize < bytesSize)
			{
				m_ModelBufferSSOB->SetData(dataPtr, bytesSize);
			}
			else
			{
				m_ModelBufferSSOB->Resize2D(dataPtr, bytesSize);
			}
			m_RenderObject.Updated();
		}
#else
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
#if 1 || !defined(RY_CHNAGE_OF_ORGNISE_LAYOUTE_INSTANC_INCREASE) || !defined(RY_HOLD_LAYOUT_ELEMENT_INCREAS_INSTANCE)
				{ SDT::Float4, "a_ModelMarix[0]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[1]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[2]", aktive, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[3]", aktive, countElements, normilze },
				}, instanceAddIndex);
#elif !0 || !defined(RY_CHNAGE_OF_ORGNISE_LAYOUTE_INSTANC_INCREASE) || !defined(RY_HOLD_LAYOUT_ELEMENT_INCREAS_INSTANCE)
				{ SDT::Float4x4, "a_ModelMarix" },
			}, instanceAddIndex);

#elif 1 || defined(RY_CHNAGE_OF_ORGNISE_LAYOUTE_INSTANC_INCREASE) || defined(RY_HOLD_LAYOUT_ELEMENT_INCREAS_INSTANCE)
				{ SDT::Float4, "a_ModelMarix[0]", aktive, instanceAddIndex, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[1]", aktive, instanceAddIndex, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[2]", aktive, instanceAddIndex, countElements, normilze },
				{ SDT::Float4, "a_ModelMarix[3]", aktive, instanceAddIndex, countElements, normilze },
			});
#else
				{ SDT::Float4x4, "a_ModelMarix", aktive, instanceAddIndex },
			});
#endif			

#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferDataUsage::DynamicDraw, layout);
#else
			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferFlag::None, layout);
#endif
			RY_CORE_ASSERT(m_VertexArray->GetVertexBuffersCount() != 2)
			m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
		}
		else if (m_RenderObject.NeedUpdate())
		{

#ifndef RY_ONCHNAGE_CREATE_INSTANCE_VAB_COMPLET_NEW
			RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
			uint32_t count = m_RenderObject.ObjectVec.size();
			uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
			uint32_t bufferBytesSize = m_ModelBufferVAO->GetByteSize();
			uint32_t halfByteSize = bufferBytesSize / 2u;

			if (bytesSize <= bufferBytesSize && halfByteSize < bytesSize)
			{
#ifdef RY_INSTANCE_ONCHANGE_PRINT
				RY_CORE_TRACE("InstenceMeshPiplineRenderShape: Update Buffer!");
#endif
				m_ModelBufferVAO->SetData(dataPtr, bytesSize);

				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
			}
			else
			{
#ifdef RY_INSTANCE_ONCHANGE_PRINT
				RY_CORE_TRACE("InstenceMeshPiplineRenderShape: Resize Buffer!");
#endif
				m_ModelBufferVAO->ResizeBuffer(dataPtr, bytesSize);

				if (m_VertexArray->GetVertexBuffersCount() != 2)
					m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
				else
					m_VertexArray->SetVertexBufferNew(m_ModelBufferVAO);
			}
#else
				RenderObject* dataPtr = m_RenderObject.ObjectVec.data();
				uint32_t count = m_RenderObject.ObjectVec.size();
#if 0
				uint32_t bytesSize = count * sizeof(RenderObject);
#else
				uint32_t bytesSize = m_InstencCount * sizeof(RenderObject);
#endif

				constexpr uint32_t instanceAddIndex = 1u;
				constexpr bool aktive = true;
				constexpr bool normilze = false;
				constexpr uint32_t countElements = 0u;
				BufferLayout layout = {
	#if 1

					{ SDT::Float4, "a_ModelMarix[0]", aktive, instanceAddIndex, countElements, normilze },
					{ SDT::Float4, "a_ModelMarix[1]", aktive, instanceAddIndex, countElements, normilze },
					{ SDT::Float4, "a_ModelMarix[2]", aktive, instanceAddIndex, countElements, normilze },
					{ SDT::Float4, "a_ModelMarix[3]", aktive, instanceAddIndex, countElements, normilze }
	#else
					{ SDT::Float4x4, "a_ModelMarix", aktive, instanceAddIndex },
	#endif
				};

				if (m_VertexArray->GetVertexBuffersCount() == 2u)
				{
					m_VertexArray->ClearVertexBuffers();
					CheckVAOFromMeshSingleDepth(m_VertexArray, m_SingleMeshObject._MeshSingle);
				}
				m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferDataUsage::None, layout);

				RY_CORE_ASSERT(m_VertexArray->GetVertexBuffersCount() != 2);
				m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
#endif
			
			m_RenderObject.Updated();
		}
#endif
	}

#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
	void InstenceMeshPiplineRenderShape::SetDataMangingFlags(PiplineManagingState flags)
	{
		RY_CORE_WARN("This PiplineManagingState is changing nothing on this is a one Render Object Call");
		m_ManagingMode = flags;
	}

	void InstenceMeshPiplineRenderShape::DrawNow()
	{
		DrawNow(m_RenderMode);
	}
#endif

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
#if RY_UNBIND
		UnbindResources();
#endif
	}

	bool InstenceMeshPiplineRenderShape::SubmiteResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		// RY_CORE_ASSERT(nullptr == m_Shader || m_Shader == shader);
		m_Shader = shader;
		// RY_CORE_ASSERT(nullptr == m_SingleMeshObject._MeshSingle || m_SingleMeshObject._MeshSingle == singleMesh._MeshSingle);
		// RY_CORE_ASSERT(nullptr == m_SingleMeshObject._Material || m_SingleMeshObject._Material == singleMesh._Material);
		m_SingleMeshObject = singleMesh;

		const Ref<Material>& materiel = m_SingleMeshObject._Material;

		// RY_CORE_ASSERT(nullptr == m_AlbdeoTex || m_AlbdeoTex == materiel->GetAlbedoTextures());
		// RY_CORE_ASSERT(0 == m_RenderMode || m_RenderMode == materiel->GetShadeRenderMode());

		m_AlbdeoTex = materiel->GetAlbedoTextures();
		m_RenderMode = materiel->GetShadeRenderMode();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject._MeshSingle;
		return CheckVAOFromMeshSingleShape(m_VertexArray, meshSingle);
	}

#ifndef RY_INSTANCE_MESH_PIPLINE_RENDERER_BASE
	uint32_t InstenceMeshPiplineRenderShape::GetCurentEntityRender() const
	{
		return m_InstencCount;
	}

	uint32_t InstenceMeshPiplineRenderShape::GetMaxEntityRender() const
	{
		return State_MaxEntityRender;
	}

	uint32_t InstenceMeshPiplineRenderShape::GetFrameCountNotUpdate() const
	{
		return m_DrawsAfterLastUpdate;
	}
#endif

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

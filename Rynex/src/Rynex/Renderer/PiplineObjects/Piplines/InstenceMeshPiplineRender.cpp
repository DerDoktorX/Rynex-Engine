#include "rypch.h"
#include "InstenceMeshPiplineRender.h"

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/RenderCommand.h>

namespace Rynex {



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
			glm::vec4{-1,-1,-1,1},
            glm::vec4{ 1,-1,-1,1},
            glm::vec4{-1, 1,-1,1},
            glm::vec4{ 1, 1,-1,1},
			glm::vec4{-1,-1, 1,1},
            glm::vec4{ 1,-1, 1,1},
            glm::vec4{-1, 1, 1,1},
            glm::vec4{ 1, 1, 1,1}
		};
		glm::vec3 minCorner(std::numeric_limits<float>::max());
		glm::vec3 maxCorner(-std::numeric_limits<float>::max());
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



#ifndef RY_INSTANCE_MESH_PIPLINE_RENDER_SHADE_TEMPLATE
#pragma region Shade

    InstanceMeshPiplineRenderShade::InstanceMeshPiplineRenderShade()
		: InstanceMeshPiplineRenderBase()
		, m_LightBuffer(nullptr)
		, m_MaterielBuffer(nullptr)
		, m_AlbedoTex(nullptr)
		, m_ShadowTex(nullptr)
		, m_RenderObject()
    {
    }

	InstanceMeshPiplineRenderShade::~InstanceMeshPiplineRenderShade()
    {
		Clear();
    }

	void InstanceMeshPiplineRenderShade::SubmitRenderTargetResource(ViewPassStorage& viewPass)
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

	void InstanceMeshPiplineRenderShade::SubmitRenderTargetResourceReadImg(const Ref<Texture>& texture)
	{
		m_ShadowTex = texture;
	}

	void InstanceMeshPiplineRenderShade::SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>& buffer)
	{
		m_LightBuffer = buffer;
	}



	void InstanceMeshPiplineRenderShade::BindResources()
	{
		InstanceMeshPiplineRenderBase::BindResources();

		m_MaterielBuffer->Bind(UniformBinding_Materiel);
		m_LightBuffer->Bind(UniformBinding_LightCamera);

		m_AlbedoTex->Bind(TextureBinding_Albedo);
		m_ShadowTex->Bind(TextureBinding_Shadow);
	}

	void InstanceMeshPiplineRenderShade::UnbindResources()
	{
		InstanceMeshPiplineRenderBase::UnbindResources();
		m_LightBuffer->UnBind(UniformBinding_LightCamera);
		m_MaterielBuffer->UnBind(UniformBinding_Materiel);


		m_AlbedoTex->UnBind(TextureBinding_Albedo);
		m_ShadowTex->UnBind(TextureBinding_Shadow);
	}


	bool InstanceMeshPiplineRenderShade::Empty() const
	{
		return 0 == m_InstanceCount;
	}

	bool InstanceMeshPiplineRenderShade::IsFull() const
	{
		if (State_MaxEntityRender < m_InstanceCount)
		{
			uint32_t toManyObjects = m_InstanceCount - State_MaxEntityRender;
			RY_CORE_WARN("This RenderPipline has {} more Stored then allowed", toManyObjects);
			return true;
		}
		return State_MaxEntityRender == m_InstanceCount;
	}

	void InstanceMeshPiplineRenderShade::Clear()
	{
		InstanceMeshPiplineRenderBase::Clear();
		RY_DESTROY_REF(m_AlbedoTex);
		RY_DESTROY_REF(m_ShadowTex);
		RY_DESTROY_REF(m_LightBuffer);
		RY_DESTROY_REF(m_SingleMeshObject.m_Material);
		RY_DESTROY_REF(m_SingleMeshObject.m_MeshSingle);

	}


	uint64_t InstanceMeshPiplineRenderShade::GetVertexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
		uint64_t number = 0ull;
		uint32_t i = 0;

		const Ref<VertexBuffer>& buffer = vab;
		uint64_t numberVAB = reinterpret_cast<uint64_t>(buffer.get());
		number |= numberVAB << (i * Hash_BindingPointMultiplyNumberBitMove);
		i++;

		return number;
	}

	uint64_t InstanceMeshPiplineRenderShade::GetIndexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		const Ref<IndexBuffer>& iba = meshSingle->GetShadeIndexBuffer();
		uint64_t number = reinterpret_cast<uint64_t>(iba.get());
		return number;
	}

	uint64_t InstanceMeshPiplineRenderShade::GetIndirectBufferNumber() const
	{
		return 0ull;
	}

	uint64_t InstanceMeshPiplineRenderShade::GetTextureNumber() const
	{
		uint64_t number = 0;
		uint64_t albedoNumber = reinterpret_cast<uint64_t>(m_AlbedoTex.get());
		uint64_t shadowNumber = reinterpret_cast<uint64_t>(m_ShadowTex.get());
		number |= albedoNumber << (Hash_BindingPointMultiplyNumberBitMove * TextureBinding_Albedo);
		number |= shadowNumber << (Hash_BindingPointMultiplyNumberBitMove * TextureBinding_Shadow);
		return number;
	}

	uint64_t InstanceMeshPiplineRenderShade::GetUniformBufferNumber() const
	{
		uint64_t number = 0ull;
		uint64_t ligtheNumber = reinterpret_cast<uint64_t>(m_LightBuffer.get());
		uint64_t cameraNumber = reinterpret_cast<uint64_t>(m_CameraBuffer.get());
		uint64_t materilNumber = reinterpret_cast<uint64_t>(m_MaterielBuffer.get());

		number |= ligtheNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_LightCamera);
		number |= cameraNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_MainCamera);
		number |= materilNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_Materiel);

		return number;
	}

	uint64_t InstanceMeshPiplineRenderShade::GetStorageBufferNumber() const
	{
		uint64_t number = 0ull;
		return number;
	}

	void InstanceMeshPiplineRenderShade::SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		if (m_InstanceCount != storeIndex)
			storeIndex = m_InstanceCount;

		m_RenderObject.SetObject(storeIndex, model, entityID);
		m_InstanceCount++;
		m_DrawsAfterLastUpdate = 0u;
	}

	PiplineResultState InstanceMeshPiplineRenderShade::SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		int result = CheckSubmitMeshObject(shader, singleMesh);
		if (State_MaxEntityRender <= m_InstanceCount)
		{
			result = result | Result_NoRenderObjectSpaceLeft;
			return static_cast<PiplineResultState>(result);
		}
		if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
		{

			if(m_InstanceCount != storeIndex)
				storeIndex = m_InstanceCount;

			if (SubmitResources(shader, singleMesh))
				m_RenderObject.SetObjectForce(storeIndex, model, entityID);
			else
				m_RenderObject.SetObject(storeIndex, model, entityID);
			m_InstanceCount++;

			m_DrawsAfterLastUpdate = 0u;
			result = Result_Success;
		}


		return static_cast<PiplineResultState>(result);
	}

	void InstanceMeshPiplineRenderShade::BeforeDrawCall()
	{
		if (State_MaxNotUpdateDraws <= m_DrawsAfterLastUpdate)
		{
			RY_CORE_WARN("We have draw this object now {} times and never updated!", m_DrawsAfterLastUpdate);
		}
		m_DrawsAfterLastUpdate++;


		if (nullptr == m_ModelBufferVAO)
		{
			RenderObject* dataPtr = m_RenderObject.m_ObjectVec.data();
			uint32_t count = m_RenderObject.m_ObjectVec.size();
			uint32_t bytesSize = count * sizeof(RenderObject);
			constexpr uint32_t instanceIndex = 1u;
			constexpr bool active = true;
			BufferLayout layout({
				{ SDT::Float4x4, "a_ModelMarix" },
				{ SDT::Float4x4, "a_NormleMatrix" },
				{ SDT::Int, "a_EntityID" },
				{ SDT::Int3, "a_Empty" }
			}, instanceIndex);
			layout.SetAutoCompress(true);

			m_ModelBufferVAO = VertexBuffer::Create(dataPtr, bytesSize, BufferFlag::None, layout);
			RY_CORE_ASSERT(2 != m_VertexArray->GetVertexBuffersCount())
			m_VertexArray->AddVertexBuffer(m_ModelBufferVAO);
		}
		else if (m_RenderObject.NeedUpdate())
		{
			RenderObject* dataPtr = m_RenderObject.m_ObjectVec.data();
			uint32_t count = m_RenderObject.m_ObjectVec.size();
			uint32_t bytesSize = m_InstanceCount * sizeof(RenderObject);
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


	void InstanceMeshPiplineRenderShade::DrawNow(int flags)
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
#if 0
		Mesh::PerDrawObject drawElement = meshSingle->GetShadePerDrawObjectIndirect();
		RenderCommand::SetMode(flags);
		BindResources();
		RY_CORE_ASSERT(0 < m_InstanceCount);
		drawElement.m_InstancesCount = m_InstanceCount;
		RenderCommand::DrawElement(m_VertexArray, drawElement);
#else
		ShaderDrawResource drawList = CreateShaderDrawResource();

		drawList.m_RenderMode = m_SingleMeshObject.m_Material->GetShadeRenderMode();
		drawList.m_ShaderProgram = m_Shader;
		drawList.m_VAO = m_VertexArray;
		drawList.m_DrawElement = meshSingle->GetShadePerDrawObjectIndirect();
		drawList.m_DrawElement.m_InstanceCount = m_InstanceCount;
		drawList.GetBindUniform().at(UniformBinding_MainCamera) = m_CameraBuffer;
		drawList.GetBindUniform().at(UniformBinding_Materiel) = m_MaterielBuffer;
		drawList.GetBindUniform().at(UniformBinding_LightCamera) = m_LightBuffer;

		drawList.GetBindTextures().at(TextureBinding_Albedo) = m_AlbedoTex;
		drawList.GetBindTextures().at(TextureBinding_Shadow) = m_ShadowTex;
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

	bool InstanceMeshPiplineRenderShade::SubmitResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		m_Shader = shader;
		m_SingleMeshObject = singleMesh;

		const Ref<Material>& materiel = m_SingleMeshObject.m_Material;

		m_AlbedoTex = materiel->GetAlbedoTextures();
		m_RenderMode = materiel->GetShadeRenderMode();

		m_MaterielBuffer = materiel->GetMaterielUniformBuffer();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		return CheckVAOFromMeshSingleShade(m_VertexArray, meshSingle);
	}

	int InstanceMeshPiplineRenderShade::CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		int result = Result_None;

		CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
		CheckObject(m_SingleMeshObject.m_MeshSingle, singleMesh.m_MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);
	    const Ref<Material>& materiel = singleMesh.m_Material;
	    Ref<Material>& materielThis = m_SingleMeshObject.m_Material;
		CheckObject(materielThis, materiel, result, Result_NotAllowedShadeDefinition, Result_NoShadeDefinitionSpaceLeft);

		if (nullptr != materiel)
			CheckObject(m_AlbedoTex, materiel->GetAlbedoTextures(), result, Result_NotAllowedTexture, Result_NoTextureSpaceLeft);

		return result;
	}



#pragma endregion
#endif
#ifndef RY_INSTANCE_MESH_PIPLINE_RENDER_DEPTH_TEMPLATE
#pragma region Depth

	InstanceMeshPiplineRenderDepth::InstanceMeshPiplineRenderDepth()
		: InstanceMeshPiplineRenderBase()
		, m_RenderObject()
	{
	}

	InstanceMeshPiplineRenderDepth::~InstanceMeshPiplineRenderDepth()
	{
		Clear();
	}

	void InstanceMeshPiplineRenderDepth::SubmitRenderTargetResource(ViewPassStorage& viewPass)
	{
	}

	void InstanceMeshPiplineRenderDepth::SubmitRenderTargetResourceReadImg(const Ref<Texture>& texture)
	{
	}

	void InstanceMeshPiplineRenderDepth::SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>& buffer)
	{
	}

	void InstanceMeshPiplineRenderDepth::SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer)
	{

		const CameraPackege& packegeMain = Renderer::GetCameraPackegeMain();
		const CameraPackege& packegeCurent = Renderer::GetCameraPackegeCurrent();
		glm::mat4 psm = computePSMMatrix(packegeMain.m_ViewMatrix, packegeMain.m_ProjectionMatrix, packegeCurent.m_Position, 0.1, 50);
		m_CameraBuffer = UniformBuffer::Create(
			&psm, sizeof(glm::mat4)
		);
	}

	void InstanceMeshPiplineRenderDepth::BindResources()
	{


		InstanceMeshPiplineRenderBase::BindResources();
		// Renderer::GetPackegeCamerUniformMain()->Bind(0);

	}



	void InstanceMeshPiplineRenderDepth::UnbindResources()
	{
		InstanceMeshPiplineRenderBase::UnbindResources();
	}


	bool InstanceMeshPiplineRenderDepth::Empty() const
	{
		return 0 == m_InstanceCount;
	}

	bool InstanceMeshPiplineRenderDepth::IsFull() const
	{
		if (State_MaxEntityRender < m_InstanceCount)
		{
			uint32_t toManyObjects = m_InstanceCount - State_MaxEntityRender;
			RY_CORE_WARN("This RenderPiline has {} more Stored then allowd", toManyObjects);
			return true;
		}
		return State_MaxEntityRender == m_InstanceCount;
	}

	void InstanceMeshPiplineRenderDepth::Clear()
	{
		InstanceMeshPiplineRenderBase::Clear();
		RY_DESTROY_REF(m_SingleMeshObject.m_Material);
		RY_DESTROY_REF(m_SingleMeshObject.m_MeshSingle);
	}



	uint64_t InstanceMeshPiplineRenderDepth::GetVertexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
		uint64_t number = 0ull;
		uint32_t i = 0;


		uint64_t numberVAB = reinterpret_cast<uint64_t>(vab.get());
		number |= numberVAB << (i * Hash_BindingPointMultiplyNumberBitMove);
		i++;

		return number;
	}

	uint64_t InstanceMeshPiplineRenderDepth::GetIndexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		const Ref<IndexBuffer>& ib = meshSingle->GetDepthIndexBuffer();
		uint64_t number = reinterpret_cast<uint64_t>(ib.get());
		return number;
	}

	uint64_t InstanceMeshPiplineRenderDepth::GetIndirectBufferNumber() const
	{
		return 0ull;
	}

	uint64_t InstanceMeshPiplineRenderDepth::GetTextureNumber() const
	{
		uint64_t number = 0ull;
		return number;
	}

	uint64_t InstanceMeshPiplineRenderDepth::GetUniformBufferNumber() const
	{
		uint64_t number = 0ull;
		uint64_t cameraNumber = reinterpret_cast<uint64_t>(m_CameraBuffer.get());
		number |= cameraNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_MainCamera);
		return number;
	}

	uint64_t InstanceMeshPiplineRenderDepth::GetStorageBufferNumber() const
	{
		uint64_t number = 0;
		return number;
	}

	void InstanceMeshPiplineRenderDepth::SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		if (m_InstanceCount != storeIndex)
			storeIndex = m_InstanceCount;

		m_RenderObject.SetObject(storeIndex, model, entityID);
		m_InstanceCount++;
		m_DrawsAfterLastUpdate = 0u;
	}

	PiplineResultState InstanceMeshPiplineRenderDepth::SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		int result = CheckSubmitMeshObject(shader, singleMesh);
		if (State_MaxEntityRender <= m_InstanceCount)
		{
			result = result | Result_NoRenderObjectSpaceLeft;
			return static_cast<PiplineResultState>(result);
		}
		if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
		{

			if (m_InstanceCount != storeIndex)
				storeIndex = m_InstanceCount;

			if (SubmitResources(shader, singleMesh))
				m_RenderObject.SetObjectForce(storeIndex, model, entityID);
			else
				m_RenderObject.SetObject(storeIndex, model, entityID);

			m_InstanceCount++;

			m_DrawsAfterLastUpdate = 0u;
			result = Result_Success;
		}


		return static_cast<PiplineResultState>(result);
	}

	void InstanceMeshPiplineRenderDepth::BeforeDrawCall()
	{

		if (State_MaxNotUpdateDraws <= m_DrawsAfterLastUpdate)
		{
			RY_CORE_WARN("We have draw this object now {} times and never updated!", m_DrawsAfterLastUpdate);
		}
		m_DrawsAfterLastUpdate++;

		if (nullptr == m_ModelBufferVAO)
		{
			RenderObject* dataPtr = m_RenderObject.m_ObjectVec.data();
			uint32_t count = m_RenderObject.m_ObjectVec.size();
			uint32_t bytesSize = m_InstanceCount * sizeof(RenderObject);

			constexpr uint32_t instanceAddIndex = 1u;
			constexpr bool aktive = true;
			constexpr bool normilze = false;
			constexpr uint32_t countElements = 0u;
			BufferLayout layout({
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
			RenderObject* dataPtr = m_RenderObject.m_ObjectVec.data();
			uint32_t count = m_RenderObject.m_ObjectVec.size();
			uint32_t bytesSize = m_InstanceCount * sizeof(RenderObject);
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


	void InstanceMeshPiplineRenderDepth::DrawNow(int flags)
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;

		if (nullptr == m_Shader
			|| nullptr == m_VertexArray
			|| nullptr == m_CameraBuffer)
		{
			RY_CORE_ERROR("Draw call failed, because core resources are not set!");
			return;
		}
		BeforeDrawCall();

		Mesh::PerDrawObject drawElement = meshSingle->GetDepthPerDrawObjectIndirect();
		RenderCommand::SetMode(flags);
		BindResources();
		RY_CORE_ASSERT(0 < m_InstanceCount);
		drawElement.m_InstanceCount = m_InstanceCount;
		RenderCommand::DrawElement(m_VertexArray, drawElement);
	}

	bool InstanceMeshPiplineRenderDepth::SubmitResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		m_Shader = shader;
		m_SingleMeshObject = singleMesh;
		const Ref<Material>& materiel = m_SingleMeshObject.m_Material;

		m_RenderMode = materiel->GetShadeRenderMode();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		return CheckVAOFromMeshSingleDepth(m_VertexArray, meshSingle);
	}


	int InstanceMeshPiplineRenderDepth::CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		int result = Result_None;

		CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
		CheckObject(m_SingleMeshObject.m_MeshSingle, singleMesh.m_MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);
	    const Ref<Material>& materiel = singleMesh.m_Material;
	    Ref<Material>& materielThis = m_SingleMeshObject.m_Material;
		if (nullptr == materiel)
			return (result | Result_NotAllowedShadeDefinition);
		if (nullptr == materielThis)
			return result;

		if(materielThis->GetDepthRenderMode() != materiel->GetDepthRenderMode())
			result |= Result_NotAllowedShadeDefinition | Result_NoShadeDefinitionSpaceLeft;


		return result;
	}



#pragma endregion
#endif

#ifndef RY_INSTANCE_MESH_PIPLINE_RENDER_SHAPE_TEMPLATE
#pragma region Shape

	InstanceMeshPiplineRenderShape::InstanceMeshPiplineRenderShape()
		: InstanceMeshPiplineRenderBase()
		, m_RenderObject()
	{
	}

	InstanceMeshPiplineRenderShape::~InstanceMeshPiplineRenderShape()
	{
		Clear();
	}

	void InstanceMeshPiplineRenderShape::SubmitRenderTargetResource(ViewPassStorage& viewPass)
	{
	}

	void InstanceMeshPiplineRenderShape::SubmitRenderTargetResourceReadImg(const Ref<Texture>& texture)
	{
	}

	void InstanceMeshPiplineRenderShape::SubmitRenderTargetResourceReadUB(const Ref<UniformBuffer>& buffer)
	{
	}


	void InstanceMeshPiplineRenderShape::BindResources()
	{

		InstanceMeshPiplineRenderBase::BindResources();
		m_AlbedoTex->Bind(TextureBinding_Albedo);
	}

	void InstanceMeshPiplineRenderShape::UnbindResources()
	{
		InstanceMeshPiplineRenderBase::UnbindResources();
		m_AlbedoTex->UnBind(TextureBinding_Albedo);
	}


	bool InstanceMeshPiplineRenderShape::Empty() const
	{
		return 0 == m_InstanceCount;

	}

	bool InstanceMeshPiplineRenderShape::IsFull() const
	{
		if (State_MaxEntityRender < m_InstanceCount)
		{
			uint32_t toManyObjects = m_InstanceCount - State_MaxEntityRender;
			RY_CORE_WARN("This RenderPipline has {} more Stored then allowed", toManyObjects);
			return true;
		}
		return State_MaxEntityRender == m_InstanceCount;
	}

	void InstanceMeshPiplineRenderShape::Clear()
	{

		InstanceMeshPiplineRenderBase::Clear();
		RY_DESTROY_REF(m_AlbedoTex);
	}




	uint64_t InstanceMeshPiplineRenderShape::GetVertexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		const Ref<VertexBuffer>& vab = meshSingle->GetVertexBuffer();
		uint64_t number = 0ull;
		uint32_t i = 0;

		const uint64_t numberVAB = reinterpret_cast<uint64_t>(vab.get());
		number |= numberVAB << (i * Hash_BindingPointMultiplyNumberBitMove);
		i++;

		return number;
	}

	uint64_t InstanceMeshPiplineRenderShape::GetIndexBufferNumber() const
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		const Ref<IndexBuffer>& iab = meshSingle->GetDepthIndexBuffer();
		const uint64_t number = reinterpret_cast<uint64_t>(iab.get());
		return number;
	}

	uint64_t InstanceMeshPiplineRenderShape::GetIndirectBufferNumber() const
	{
		uint64_t number = 0;
		return number;
	}

	uint64_t InstanceMeshPiplineRenderShape::GetTextureNumber() const
	{
		uint64_t number = 0;
		uint64_t albedoNumber = reinterpret_cast<uint64_t>(m_AlbedoTex.get());
		number |= albedoNumber << (Hash_BindingPointMultiplyNumberBitMove * TextureBinding_Albedo);
		return number;
	}

	uint64_t InstanceMeshPiplineRenderShape::GetUniformBufferNumber() const
	{
		uint64_t number = 0;
		uint64_t cameraNumber = reinterpret_cast<uint64_t>(m_CameraBuffer.get());
		number |= cameraNumber << (Hash_BindingPointMultiplyNumberBitMove * UniformBinding_MainCamera);

		return number;
	}

	uint64_t InstanceMeshPiplineRenderShape::GetStorageBufferNumber() const
	{
		uint64_t number = 0;
		return number;
	}

	void InstanceMeshPiplineRenderShape::SubmitRenderObject(const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		if (m_InstanceCount != storeIndex)
			storeIndex = m_InstanceCount;

		m_RenderObject.SetObject(storeIndex, model, entityID);
		m_InstanceCount++;
		m_DrawsAfterLastUpdate = 0u;
	}

	PiplineResultState InstanceMeshPiplineRenderShape::SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID)
	{
		int result = CheckSubmitMeshObject(shader, singleMesh);
		if (State_MaxEntityRender <= m_InstanceCount)
		{
			result = result | Result_NoRenderObjectSpaceLeft;
			return static_cast<PiplineResultState>(result);
		}
		if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
		{


			if(m_InstanceCount != storeIndex)
				storeIndex = m_InstanceCount;

			if (SubmitResources(shader, singleMesh))
				m_RenderObject.SetObjectForce(storeIndex, model, entityID);
			else
				m_RenderObject.SetObject(storeIndex, model, entityID);

			m_InstanceCount++;
			m_DrawsAfterLastUpdate = 0u;
			result = Result_Success;
		}


		return static_cast<PiplineResultState>(result);
	}

	void InstanceMeshPiplineRenderShape::BeforeDrawCall()
	{
		if (State_MaxNotUpdateDraws <= m_DrawsAfterLastUpdate)
		{
			RY_CORE_WARN("We have draw this object now {} times and never updated!", m_DrawsAfterLastUpdate);
		}
		m_DrawsAfterLastUpdate++;

		if (nullptr == m_ModelBufferVAO)
		{
			RenderObject* dataPtr = m_RenderObject.m_ObjectVec.data();
			uint32_t count = m_RenderObject.m_ObjectVec.size();
			uint32_t bytesSize = count * sizeof(RenderObject);
			constexpr uint32_t instanceAddIndex = 1u;
			constexpr bool aktive = true;
			constexpr uint32_t countElements = 0u;
			constexpr bool normilze = false;

			BufferLayout layout({
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

			RenderObject* dataPtr = m_RenderObject.m_ObjectVec.data();
			uint32_t count = m_RenderObject.m_ObjectVec.size();
			uint32_t bytesSize = m_InstanceCount * sizeof(RenderObject);
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


	void InstanceMeshPiplineRenderShape::DrawNow(int flags)
	{
		const Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;

		if (nullptr == m_Shader
			|| nullptr == m_VertexArray
			|| nullptr == m_CameraBuffer
			|| nullptr == m_AlbedoTex)
		{
			RY_CORE_ERROR("Draw call faild, becouse core resurces are not set!");
			return;
		}
		BeforeDrawCall();

		Mesh::PerDrawObject drawElement = meshSingle->GetShadePerDrawObjectIndirect();
		RenderCommand::SetMode(flags);
		BindResources();
		RY_CORE_ASSERT(0 < m_InstanceCount);
		drawElement.m_InstanceCount = m_InstanceCount;
		RenderCommand::DrawElement(m_VertexArray, drawElement);

	}

	bool InstanceMeshPiplineRenderShape::SubmitResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		m_Shader = shader;
		m_SingleMeshObject = singleMesh;

		const Ref<Material>& materiel = m_SingleMeshObject.m_Material;


		m_AlbedoTex = materiel->GetAlbedoTextures();
		m_RenderMode = materiel->GetShadeRenderMode();

		Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
		return CheckVAOFromMeshSingleShape(m_VertexArray, meshSingle);
	}



	int InstanceMeshPiplineRenderShape::CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
	{
		int result = Result_None;

		CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
		CheckObject(m_SingleMeshObject.m_MeshSingle, singleMesh.m_MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);
		const Ref<Material>& materiel = singleMesh.m_Material;
	    Ref<Material>& materielThis = m_SingleMeshObject.m_Material;
		CheckObject(materielThis, materiel, result, Result_NotAllowedShadeDefinition, Result_NoShadeDefinitionSpaceLeft);

		if (nullptr != materiel)
			CheckObject(m_AlbedoTex, materiel->GetAlbedoTextures(), result, Result_NotAllowedTexture, Result_NoTextureSpaceLeft);

		return result;
	}

#pragma endregion
#endif

}

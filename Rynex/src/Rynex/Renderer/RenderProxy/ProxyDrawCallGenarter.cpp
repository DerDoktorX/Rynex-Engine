#include "rypch.h"
#include "ProxyDrawCallGenarter.h"
#include <magic_enum/magic_enum.hpp>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/RenderProxy/StaticeRenderProxys.h>


#define RY_DYNAMIC_DATA_STRUCT
namespace Rynex {

	ProxyDrawCallGenarter::ProxyDrawCallGenarter()
		: m_ProxyPtr(nullptr)
		, m_RenderMeshBatchPtr(nullptr)
		, m_ModelMatrixPtr(nullptr)
		, m_NormaleMatrixPtr(nullptr)
		, m_Batch3DKey()
		, m_DrawSpecifcation(0)
		, m_SubmitCount(0)
	{
	}

	ProxyDrawCallGenarter::~ProxyDrawCallGenarter()
	{
		
	}

	void ProxyDrawCallGenarter::SetRenderTargetLayoute(const BufferLayout& layoute)
	{
		m_RenderTragetLayoute = layoute;
	}

	bool ProxyDrawCallGenarter::SetProxyData(RenderProxy& proxy)
	{		

		proxy.Check();
		if (!CheckMaterielConfigs(proxy))
			return false;
		m_ProxyPtr = &proxy;
		return true;
	}

	bool ProxyDrawCallGenarter::SetRenderMeshBatch(const RenderProxy& proxy)
	{
		return SetRenderMeshBatch(proxy.meshBatch);
	}

	bool ProxyDrawCallGenarter::SetRenderMeshBatch(const RenderMeshBatch& renderMeshBatch)
	{
		renderMeshBatch.Check();
		bool result = CheckMaterielConfigs(renderMeshBatch);
		RY_CORE_ASSERT(result, "expext to get only new data!");
		return result;
	}

	void ProxyDrawCallGenarter::SetubShaderDrawList()
	{
		FinishLastDrawCall();
	}

	void ProxyDrawCallGenarter::SubmiteShaderDrawListMain()
	{
		std::vector<ShaderDrawResource>& shaderDarwResourceVec = Renderer::GetShaderDrawResourceMain();
		std::vector<ShaderDrawResource>::iterator itEnd = shaderDarwResourceVec.end();
		shaderDarwResourceVec.insert(itEnd, m_DrawCallsVec.begin(), m_DrawCallsVec.end());
	}

	void ProxyDrawCallGenarter::SubmiteShaderDrawListCurent()
	{
		std::vector<ShaderDrawResource>& shaderDarwResourceVec = Renderer::GetShaderDrawResourceCurent();
		std::vector<ShaderDrawResource>::iterator itEnd = shaderDarwResourceVec.end();
		shaderDarwResourceVec.insert(itEnd, m_DrawCallsVec.begin(), m_DrawCallsVec.end());
	}

	void ProxyDrawCallGenarter::Clear()
	{
		m_CurentDrawCall.Clear();
		m_ProxyPtr = nullptr;
		m_Batch3DKey = Batch3DKey();
		ClearBatchManger();
		ClearDrawList();
	}

	void ProxyDrawCallGenarter::ClearBatchManger()
	{		
		m_BatchManager.Clear();
	}

	void ProxyDrawCallGenarter::ClearDrawList()
	{
		m_DrawCallsVec.clear();
	}


	void ProxyDrawCallGenarter::SubmiteDraw(ProxyGroupView& groupView)
	{
#if 0
		RenderProxy& proxy = groupView.First();

		SetRenderMeshBatch(groupView.renderMeshBatch);
		NextDrawCall(groupView.renderMeshBatch);


		for (RenderProxy& renderProxy : groupView)
		{
			SetProxyData(proxy);
			AddProxyData();
		}
		SetubShaderDrawList();
#else
		BeforFirstProxy(groupView.renderMeshBatch);
		for (RenderProxy& renderProxy : groupView)
		{
			AddProxyData(renderProxy);
		}
		AfterLastProxy();
#endif
	}


	void ProxyDrawCallGenarter::AddProxyData()
	{
		if (s_UseDyamincDatatStruct)
		{
			AddProxyDataDynamic();
		}
		else
		{
			AddProxyDataStatic();
		}


		m_CurentDrawCall.drawElement.InstancesCount++;
	}

	void ProxyDrawCallGenarter::AddTranfromModelData(const glm::mat4& modelMatrix)
	{
		m_ModelMatrixPtr = &modelMatrix;
	}

	void ProxyDrawCallGenarter::AddTranfromNormaleData(const glm::mat4& normaleMatrix)
	{
		m_NormaleMatrixPtr = &normaleMatrix;
	}

	void ProxyDrawCallGenarter::NextDrawCall(RenderProxy& proxy)
	{
		if(CheckMaterielConfigs(proxy))
		{
			proxy.Check();
			m_ProxyPtr = &proxy;
			NextDrawCall(proxy.meshBatch);
		}
	}

	void ProxyDrawCallGenarter::NextDrawCall(RenderMeshBatch& meshBatch)
	{
		meshBatch.Check();
		m_RenderMeshBatchPtr = &meshBatch;
		ResetCurentDrawCall();

		BatchVAO& storeSubmiteVAO = m_BatchManager.GetBatchVAO(m_Batch3DKey);
		storeSubmiteVAO.Check();
		storeSubmiteVAO.Reset();
		m_BatchRenderObjectVAOPtr = storeSubmiteVAO.GetDataEndPtr();
		PertepairNewDrawCall();
	}

	void ProxyDrawCallGenarter::FinishLastDrawCall()
	{
		if (nullptr == m_CurentDrawCall.shaderProgramm || (nullptr == m_CurentDrawCall.vao && 0u == m_CurentDrawCall.indicesCount && nullptr == m_CurentDrawCall.drawBuffer &&
				(0u == m_CurentDrawCall.drawElement.InstancesCount || 0u == m_CurentDrawCall.drawElement.Count || -1 == m_CurentDrawCall.drawElement.BaseVertex)))
		{
			RY_CORE_ERROR("m_CurentDrawCall has some critel reource not set!");
			return;
		}
		UplodeArrayDataBufferGPU();
		AddDrawListGlobleResources();

		m_DrawCallsVec.emplace_back(m_CurentDrawCall);

		constexpr int levelLOD = 0;
		const Ref<Material>& materiel = m_ProxyPtr->GetMaterial();
		if (materiel->HasSpecForDraw(m_RenderTragetLayoute, levelLOD))
		{
			// Renderer::GetDrawContext().PopScope();
		}
		// Renderer::GetDrawContext().PopScope();
	}

	void ProxyDrawCallGenarter::ResetCurentDrawCall()
	{		
		m_CurentDrawCall = ShaderDrawResource();
	}

	void ProxyDrawCallGenarter::SetArrayDataBufferGPUContainerVertexTransformtion()
	{
		RY_CORE_ASSERT(nullptr != m_BatchRenderObjectVAOPtr);

		if (!s_UseDyamincDatatStruct)
			return;

		std::vector<RenderProxyDynamicEllmenenttData>& renderProxyDynamicDataTypeVec = m_BatchRenderObjectVAOPtr->renderProxyDynamicDataTypeVec;
		Memory::DynamicDataStruct& dynamicDataStruct = m_BatchRenderObjectVAOPtr->rendeObjectDynamicElementStruct;

	

		if(m_RenderTragetLayoute.Empty())
		{
			BufferLayout layout = BufferLayout({
				{ SDT::Float4x4 ,"TransformMatrix" },
			}, 1u);

			dynamicDataStruct = Memory::DynamicDataStruct(layout);
			if (renderProxyDynamicDataTypeVec.size() <= 1)
			{
				renderProxyDynamicDataTypeVec.resize(1);
				renderProxyDynamicDataTypeVec.at(0) = RenderProxyDynamicEllmenenttData::TransformMatrix;
			}
		}
		else
		{
			
			BufferLayout layout = BufferLayout({
				{ SDT::Float4x4 ,"TransformMatrix" },
				{ SDT::Float4x4 ,"NormaleMatrix" },
				{ SDT::Int ,"Entity" },
				}, 1u);
			layout.SetAutoCompress(true);
			dynamicDataStruct = Memory::DynamicDataStruct(layout);
			constexpr uint64_t maxCount = 3ull;
			if (renderProxyDynamicDataTypeVec.size() <= maxCount)
			{
				renderProxyDynamicDataTypeVec.resize(maxCount);

				renderProxyDynamicDataTypeVec.at(0) = RenderProxyDynamicEllmenenttData::TransformMatrix;
				renderProxyDynamicDataTypeVec.at(1) = RenderProxyDynamicEllmenenttData::NormaleMatrix;
				renderProxyDynamicDataTypeVec.at(2) = RenderProxyDynamicEllmenenttData::EntityID;
			}
		}
	}

	void ProxyDrawCallGenarter::UplodeArrayDataBufferGPU()
	{
		if (!m_BatchRenderObjectVAOPtr->needUpdate)
		{
			return;
		}

		if (s_UseDyamincDatatStruct)
			UplodeArrayDataBufferDynamicGPU();
		else
			UplodeArrayDataBufferStaticGPU();

		m_BatchRenderObjectVAOPtr->curentIndex = 0;
		m_BatchRenderObjectVAOPtr->needUpdate = false;
	}

	void ProxyDrawCallGenarter::UplodeArrayDataBufferDynamicGPU()
	{
		using DynamicStructPtr = Memory::DynamicDataStruct::ContainerValueTypePtr;


		
		Memory::DynamicDataStruct& dynamicDataStruct = m_BatchRenderObjectVAOPtr->rendeObjectDynamicElementStruct;
		BufferLayout layout = dynamicDataStruct.GetLayout();
		DynamicStructPtr dataPtr = dynamicDataStruct.Data();
		const uint64_t layouteBytesSize = dynamicDataStruct.LayouteBytesSize();
		const uint64_t& instanceCount = m_CurentDrawCall.drawElement.InstancesCount;

		const uint64_t count = dynamicDataStruct.Size();
		const uint64_t dynamicDataStructByteSize = dynamicDataStruct.BufferByteSize();
		const uint64_t bytesSize = layouteBytesSize * instanceCount;
		RY_CORE_ASSERT(instanceCount <= count, "Buffer overflow by element count!");
		RY_CORE_ASSERT(bytesSize <= dynamicDataStructByteSize, "Buffer overflow by byte Size!");

		
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(dataPtr);
		UplodedeVertexBufferFromContainer(m_BatchRenderObjectVAOPtr->instanceObjectBuffer, layout, BufferFlag::None, bytePtr, bytesSize);

	}

	void ProxyDrawCallGenarter::UplodeArrayDataBufferStaticGPU()
	{

		const BufferFlagGPU flag = BufferFlag::Dynamic;

		if (m_RenderTragetLayoute.Empty())
		{
			static const BufferLayout layoutDepth = BufferLayout({
			   { SDT::Float4x4 ,"TransformMatrix" },
			}, 1u);
			Batch::Render3DMeshObjectTrasform* dataBufferPtr = m_BatchRenderObjectVAOPtr->rendeObjectTransformVec.data();
			uint8_t* dataPtr = reinterpret_cast<uint8_t*>(dataBufferPtr);
			uint64_t layouteBytesSize = sizeof(Batch::Render3DMeshObject);
			const uint32_t& instaceCount = m_CurentDrawCall.drawElement.InstancesCount;

			const uint64_t bytesSize = layouteBytesSize * instaceCount;
			UplodedeVertexBufferFromContainer(m_BatchRenderObjectVAOPtr->instanceObjectBuffer, layoutDepth, flag, dataPtr, bytesSize);

			return;
		}
		static const BufferLayout layoutShade = BufferLayout({
			   { SDT::Float4x4 ,"TransformMatrix" },
			   { SDT::Float4x4 ,"NormaleMatrix" },
			   { SDT::Int ,"Entity" },
			   { SDT::Int3 ,"Empty" }
		}, 1u);
		Batch::Render3DMeshObject* dataBufferPtr = m_BatchRenderObjectVAOPtr->rendeObjectVec.data();
		uint8_t* dataPtr = reinterpret_cast<uint8_t*>(dataBufferPtr);
		uint64_t layouteBytesSize = sizeof(Batch::Render3DMeshObject);
		const uint32_t& instaceCount = m_CurentDrawCall.drawElement.InstancesCount;
		const uint64_t bytesSize = layouteBytesSize * instaceCount;
		UplodedeVertexBufferFromContainer(m_BatchRenderObjectVAOPtr->instanceObjectBuffer, layoutShade, flag, dataPtr, bytesSize);
	}

	void ProxyDrawCallGenarter::AddProxyDataDynamic()
	{
		AddDataArray(0, m_BatchRenderObjectVAOPtr->rendeObjectDynamicElementStruct, m_BatchRenderObjectVAOPtr->curentIndex);
		m_BatchRenderObjectVAOPtr->needUpdate = true;
		m_BatchRenderObjectVAOPtr->curentIndex++;
	}

	void ProxyDrawCallGenarter::AddProxyDataStatic()
	{
		const int& entityID = m_ProxyPtr->entity;
#ifdef RY_USE_SINGLE_DATA_STRUCTS_FOR_DATA
		const glm::mat4& transfromMatrix = *m_ModelMatrixPtr;
		const glm::mat4& normaleMatrix = *m_NormaleMatrixPtr;
		if (m_RenderTragetLayoute.Empty())
		{
			Batch::Render3DMeshObject batchedRenderObjectVAO = Batch::Render3DMeshObject{
				transfromMatrix
			};
			m_BatchRenderObjectVAOPtr->AddRenderObject(batchedRenderObjectVAO);
			return;
		}
#else
		const glm::mat4& transfromMatrix = m_ProxyPtr->model;
		if (m_RenderTragetLayoute.Empty())
		{
			Batch::Render3DMeshObject batchedRenderObjectVAO = Batch::Render3DMeshObject{
				transfromMatrix 
			};
			m_BatchRenderObjectVAOPtr->AddRenderObject(batchedRenderObjectVAO);
			return;
		}
		glm::mat4 normaleMatrix = glm::inverse(glm::transpose(transfromMatrix));
#endif
		Batch::Render3DMeshObject batchedRenderObjectVAO = Batch::Render3DMeshObject{
			transfromMatrix, normaleMatrix,
			glm::vec4{ entityID, -10, -11, -12 }
		};
		m_BatchRenderObjectVAOPtr->AddRenderObject(batchedRenderObjectVAO);
	}

#ifdef RY_OPENGL_USE_ARRAY_BUFFER
	void ProxyDrawCallGenarter::UplodedeVertexBufferFromContainer(Ref<VertexBuffer>& vertexBuffer, const BufferLayout& layoute, BufferDataUsage usage, const uint8_t* dataPtr, uint64_t bytesSize)
#else
	void ProxyDrawCallGenarter::UplodedeVertexBufferFromContainer(Ref<VertexBuffer>& vertexBuffer, const BufferLayout& layoute, BufferFlagGPU flag, const uint8_t* dataPtr, uint64_t bytesSize)

#endif
	{
		RY_CORE_ASSERT(nullptr != m_CurentDrawCall.vao);

		if (nullptr == vertexBuffer)
		{
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
			vertexBuffer = VertexBuffer::Create(dataPtr, bytesSize, usage, layoute);
#else
			vertexBuffer = VertexBuffer::Create(dataPtr, bytesSize, flag, layoute);
#endif
			m_CurentDrawCall.vao->AddVertexBuffer(vertexBuffer, layoute);
		}
		else
		{
			uint32_t bufferByteSize = vertexBuffer->GetByteSize();
			if (bufferByteSize < bytesSize)
			{
				vertexBuffer->ResizeBuffer(dataPtr, bytesSize);
			}
			else
			{
				vertexBuffer->SetData(dataPtr, bytesSize);
			}
		}
	}


	void ProxyDrawCallGenarter::UplodedeStorageBufferFromContainer(Ref<StorageBuffer>& storageBuffer, BufferFlagGPU flag, const uint8_t* dataPtr, uint64_t bytesSize)
	{
		if (nullptr == storageBuffer)
		{
			storageBuffer = StorageBuffer::Create(dataPtr, bytesSize, flag);
		}
		else
		{
			uint32_t bufferByteSize = storageBuffer->GetByteSize();
			if (bufferByteSize < bytesSize)
			{
				storageBuffer->ResizeBuffer(dataPtr, bytesSize);
			}
			else
			{
				storageBuffer->SetData(dataPtr, bytesSize);
			}
		}
	}

#ifdef RY_OPENGL_OLD_UNIFORM
	void ProxyDrawCallGenarter::UplodedeUniformBufferFromContainer(Ref<UniformBuffer>& uniformBuffer, const BufferLayout& layoute, BufferDataUsage usage, const uint8_t* dataPtr, uint64_t bytesSize)
#else
	void ProxyDrawCallGenarter::UplodedeUniformBufferFromContainer(Ref<UniformBuffer>& uniformBuffer, const BufferLayout& layoute, BufferFlagGPU flag, const uint8_t* dataPtr, uint64_t bytesSize)
#endif
	{
		if (nullptr == uniformBuffer)
		{
#ifdef RY_OPENGL_OLD_UNIFORM
			uniformBuffer = UniformBuffer::Create(dataPtr, bytesSize, layoute, usage);
#else
			uniformBuffer = UniformBuffer::Create(dataPtr, bytesSize, layoute, flag);
#endif


		}
		else
		{
			uint32_t bufferByteSize = uniformBuffer->GetByteSize();
			if (bufferByteSize < bytesSize)
			{
#ifdef RY_OPENGL_OLD_UNIFORM
				uniformBuffer = UniformBuffer::Create(dataPtr, bytesSize, layoute, usage);
#else
				uniformBuffer = UniformBuffer::Create(dataPtr, bytesSize, layoute, flag);
#endif

			}
			else
			{
				uniformBuffer->SetData(dataPtr, bytesSize);
			}
		}
	}

	void ProxyDrawCallGenarter::AddDrawListGlobleResources()
	{
		AddDrawListGlobleResourcesCameraData();
		AddDrawListGlobleResourcesShadow();
	}

	void ProxyDrawCallGenarter::AddDrawListGlobleResourcesCameraData()
	{
		constexpr uint32_t bindSlotCameraUB = 1;
		Ref<UniformBuffer> buffer;
		if (m_RenderTragetLayoute.Empty())
		{
			buffer = Renderer::GetPackegeCamerUniformCurent();
		}
		else
		{
			buffer = Renderer::GetPackegeCamerUniformMain();
		}
		m_CurentDrawCall.GetBindeUniform().at(bindSlotCameraUB) = buffer;
	}

	void ProxyDrawCallGenarter::AddDrawListGlobleResourcesShadow()
	{
		if (m_RenderTragetLayoute.Empty())
			return;
		uint32_t foundCount = 0;
		
		constexpr uint32_t bindSlotShadowUB = 2;
		constexpr uint32_t bindSlotDepthShadowTex = 1;
		constexpr uint32_t maxCount = 1;

		Renderer::ForEchStoredPassedRenderPass(
			[&](const RenderPass& renderPass)
			{
				if(maxCount <= foundCount)
					return;
				if ("Shadow" != renderPass.Name)
					return;

				const Ref<RenderTarget>& target = renderPass.Target;
				if (nullptr == target)
					return;

				const Ref<Framebuffer>& fb = target->GetFramebuffer();
				if (nullptr == fb)
					return;

				Ref<Texture> tex = fb->GetDepthTexture();
				if (nullptr == tex)
					return;

				const Ref<UniformBuffer>& ub = renderPass.CameraDataPackBufferUB;
				if (nullptr == ub)
					return;

				m_CurentDrawCall.GetBindeUniform().at(bindSlotShadowUB) = ub;
				m_CurentDrawCall.GetBindeTextures().at(bindSlotShadowUB) = tex;
				foundCount++;
				
			}
		);

		if (maxCount <= foundCount)
			return;

		m_CurentDrawCall.GetBindeUniform().at(bindSlotShadowUB) = Renderer::GetPackegeCamerUniformMain();
		m_CurentDrawCall.GetBindeTextures().at(bindSlotDepthShadowTex) = Texture::White();

	}

	

	void ProxyDrawCallGenarter::PertepairNewDrawCall()
	{
		constexpr uint32_t bindSlotMaterielUB = 3;
		constexpr uint32_t bindTexAlbedo = 0;

		constexpr int levelLOD = 0;
		const Ref<Material>& materiel = m_ProxyPtr->GetMaterial();
		const Ref<MeshSingle>& mesh = m_ProxyPtr->GetMesh();

		Ref<VertexArray>& vertexArrayObject = m_BatchRenderObjectVAOPtr->vertexArray;
		const Ref<IndexBuffer>& iab = m_RenderTragetLayoute.Empty() ? mesh->GetDepthIndexBuffer() : mesh->GetShadeIndexBuffer();
		
		// Renderer::GetDrawContext().PushScope("Mesh", mesh->Handle);
		if (nullptr == vertexArrayObject)
		{
			vertexArrayObject = VertexArray::Create();
			vertexArrayObject->AddVertexBuffer(mesh->GetVertexBuffer());
			vertexArrayObject->SetIndexBuffer(iab);
			SetArrayDataBufferGPUContainerVertexTransformtion();
			m_BatchRenderObjectVAOPtr->needUpdate = true;
		}
		

		if (nullptr == m_BatchRenderObjectVAOPtr->materielUB)
		{
			m_BatchRenderObjectVAOPtr->materielUB = materiel->GetMaterielUniformBuffer();
		}
		m_CurentDrawCall.drawElement.BaseInstance = 0;
		m_CurentDrawCall.drawElement.BaseVertex = 0;
		m_CurentDrawCall.vao = vertexArrayObject;
		m_CurentDrawCall.drawElement.Count = iab->GetCount();

		if(materiel->HasSpecForDraw(m_RenderTragetLayoute, levelLOD))
		{
			// Renderer::GetDrawContext().PushScope("Materiel", materiel->Handle);
			m_DrawSpecifcation = materiel->GetDrawSpecification(m_RenderTragetLayoute, levelLOD);
			m_CurentDrawCall.shaderProgramm = materiel->GetShaderForDraw(m_RenderTragetLayoute, levelLOD);

			if (!m_RenderTragetLayoute.Empty())
			{
				m_CurentDrawCall.GetBindeTextures().at(bindTexAlbedo) = materiel->GetTextureForDraw(m_RenderTragetLayoute, levelLOD).front();
				m_CurentDrawCall.GetBindeUniform().at(bindSlotMaterielUB) = m_BatchRenderObjectVAOPtr->materielUB;
			}
			m_CurentDrawCall.renderMode = materiel->GetRenderMode(m_RenderTragetLayoute, levelLOD);

		}
		
		m_CurentDrawCall.drawElement.InstancesCount = 0;

	}

	void ProxyDrawCallGenarter::SetProxyBufferData()
	{
		uint32_t index = 0;
		
	}

	void ProxyDrawCallGenarter::SetArrayDataFromProxy(const RenderProxyDynamicEllmenenttData& dynamicElementData, Memory::DynamicDataStruct& dynamicData, uint64_t elementIndex, uint32_t arrayIndex)
	{
		switch (dynamicElementData)
		{
		case RenderProxyDynamicEllmenenttData::None:
			break;

		case RenderProxyDynamicEllmenenttData::EntityID:
		{
			RY_CORE_ASSERT(nullptr != m_ProxyPtr, "not set ptr");
			const int& entityID = m_ProxyPtr->entity;
			dynamicData.Set<int>(elementIndex, arrayIndex, entityID);
			break;
		}
		case RenderProxyDynamicEllmenenttData::TransformMatrix:
		{
#ifdef RY_USE_SINGLE_DATA_STRUCTS_FOR_DATA
			RY_CORE_ASSERT(nullptr != m_ModelMatrixPtr, "not set ptr");
			const glm::mat4& transfromMatrix = *m_ModelMatrixPtr;
#else
			const glm::mat4& transfromMatrix = m_ProxyPtr->model;
#endif
			dynamicData.Set<glm::mat4>(elementIndex, arrayIndex, transfromMatrix);
			break;
		}
		case RenderProxyDynamicEllmenenttData::NormaleMatrix:
		{
#ifdef RY_USE_SINGLE_DATA_STRUCTS_FOR_DATA
			RY_CORE_ASSERT(nullptr != m_NormaleMatrixPtr, "not set ptr");
			const glm::mat4& normaleMatrix = *m_NormaleMatrixPtr;
#else
			const glm::mat4& transfromMatrix = m_ProxyPtr->model;
			glm::mat4 normaleMatrix = glm::inverse(glm::transpose(transfromMatrix));
#endif
			dynamicData.Set<glm::mat4>(elementIndex, arrayIndex, normaleMatrix);
			break;
		}
		case RenderProxyDynamicEllmenenttData::Empty_Int_1:
		case RenderProxyDynamicEllmenenttData::Empty_Uint_1:
		{
			int empty0 = -10;
			dynamicData.Set<int>(elementIndex, arrayIndex, empty0);
			break;
		}
		case RenderProxyDynamicEllmenenttData::Empty_Int_2:
		case RenderProxyDynamicEllmenenttData::Empty_Uint_2:
		{
			glm::ivec2 empty1 = { -10, -11 };
			dynamicData.Set<glm::ivec2>(elementIndex, arrayIndex, empty1);
			break;
		}
		case RenderProxyDynamicEllmenenttData::Empty_Int_3:
		case RenderProxyDynamicEllmenenttData::Empty_Uint_3:
		{
			glm::ivec3 empty2 = { -10, -11, -12 };
			dynamicData.Set<glm::ivec3>(elementIndex, arrayIndex, empty2);
			break;
		}

		case RenderProxyDynamicEllmenenttData::Empty_Int_4:
		case RenderProxyDynamicEllmenenttData::Empty_Uint_4:
		{
			glm::ivec4 empty3 = { -10, -11, -12, -13 };
			dynamicData.Set<glm::ivec4>(elementIndex, arrayIndex, empty3);
			break;
		}

		case RenderProxyDynamicEllmenenttData::Empty_Float_1:
		{
			float empty0 = -10.0f;
			dynamicData.Set<float>(elementIndex, arrayIndex, empty0);
			break;
		}
		case RenderProxyDynamicEllmenenttData::Empty_Float_2:
		{
			glm::vec2 empty1 = { -10.0f, -11.0f };
			dynamicData.Set<glm::vec2>(elementIndex, arrayIndex, empty1);
			break;
		}
		case RenderProxyDynamicEllmenenttData::Empty_Float_3:
		{
			glm::vec3 empty2 = { -10.0f, -11.0f, -12.0f };
			dynamicData.Set<glm::vec3>(elementIndex, arrayIndex, empty2);
			break;
		}
		case RenderProxyDynamicEllmenenttData::Empty_Float_4:
		{
			glm::vec4 empty3 = { -10.0f, -11.0f, -12.0f,- 13.0f };
			dynamicData.Set<glm::vec4>(elementIndex, arrayIndex, empty3);
			break;
		}

		case RenderProxyDynamicEllmenenttData::Texture_0_Index:
		case RenderProxyDynamicEllmenenttData::Texture_1_Index:
		case RenderProxyDynamicEllmenenttData::Texture_2_Index:
		case RenderProxyDynamicEllmenenttData::Texture_3_Index:
		case RenderProxyDynamicEllmenenttData::Texture_4_Index:
		case RenderProxyDynamicEllmenenttData::Texture_5_Index:
		case RenderProxyDynamicEllmenenttData::Texture_6_Index:
		case RenderProxyDynamicEllmenenttData::Texture_7_Index:
		case RenderProxyDynamicEllmenenttData::MaterilIndex:
		{
			RY_CORE_FATAL("not defined behavior for enum: {}.", magic_enum::enum_name(dynamicElementData));
		}
		default:
		{
			RY_CORE_ASSERT(false);
			break;
		}
			
		}
	}

	void ProxyDrawCallGenarter::AddDataArray(uint32_t indexVec, Memory::DynamicDataStruct& dynamicData, uint32_t curentIndex)
	{
		RY_CORE_ASSERT(nullptr != m_ProxyPtr);
		uint64_t elementCount = dynamicData.LayoutElementCount();

		if (dynamicData.Size() <= curentIndex)
		{
			uint64_t resizeSize = curentIndex + 1ull;
			dynamicData.Resize(resizeSize);
		}

		RY_CORE_ASSERT(curentIndex < dynamicData.Size());

		for(uint64_t i = 0ull; i < elementCount; i++)
		{
			const RenderProxyDynamicEllmenenttData& renderProxyDynamicTypeData = m_BatchRenderObjectVAOPtr->renderProxyDynamicDataTypeVec.at(i);
			SetArrayDataFromProxy(renderProxyDynamicTypeData, dynamicData, i, curentIndex);
		}
		
	}

	bool ProxyDrawCallGenarter::CheckMaterielConfigs(const RenderProxy& proxy)
	{
		return CheckMaterielConfigs(proxy.meshBatch);
	}

	void ProxyDrawCallGenarter::BeforFirstProxy(RenderMeshBatch& meshBatch)
	{
	}

	void ProxyDrawCallGenarter::AddProxyData(RenderProxy& proxy)
	{
		SetProxyData(proxy);
		AddProxyData();
	}

	void ProxyDrawCallGenarter::AfterLastProxy()
	{
		SetubShaderDrawList();
	}

	bool ProxyDrawCallGenarter::CheckMaterielConfigs(const RenderMeshBatch& meshBatch)
	{
		constexpr int levelLOD = 0;
		const Ref<Material>& materiel = meshBatch.materiel;
		const Ref<MeshSingle>& mesh = meshBatch.mesh;
		DrawSpecification::BatchConfig darwSpec = materiel->GetDrawSpecification(m_RenderTragetLayoute, levelLOD);
		Batch3DKey batch3DKey = Batch3DKey(materiel, mesh, m_RenderTragetLayoute, levelLOD);
		bool result = batch3DKey == m_Batch3DKey;
		if (!result)
		{
			m_Batch3DKey = batch3DKey;
		}

		return result;
	}
}
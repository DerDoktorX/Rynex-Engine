#pragma once
#include <Rynex/Memory/DynamicDataStruct.h>
#include <Rynex/Renderer/Rendering/Batch3DKey.h>
#include <Rynex/Renderer/Rendering/RenderTarget.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchManager.h>


namespace Rynex {

	struct RenderMeshBatch;
	struct RenderProxy;
	struct ProxyGroupView;
	struct BatchedRenderObjectVAO;
	class Batch3DKey;
	enum class RenderProxyDynamicElenenttData;


	class ProxyDrawCallGenarter
	{
	public:
		using BufferGPU = std::variant<Ref<Texture>, Ref<LinkedTextureArray>, Ref<BindlesTextureArray>>;
		using ElementBufferGPU = std::variant<Ref<VertexBuffer>, Ref<StorageBuffer>, Ref<UniformBuffer>>;
		using ContainerBufferGPU = std::vector<BufferGPU>;
	// public member variable -------------------------------------------------------------------------------------------------
		ProxyDrawCallGenarter();
		~ProxyDrawCallGenarter();
		void SetRenderTargetLayout(const BufferLayout& layout);
		bool SetProxyData(RenderProxy& proxy);
		bool SetRenderMeshBatch(const RenderProxy& proxy);
		bool SetRenderMeshBatch(const RenderMeshBatch& renderMeshBatch);

		void AddProxyData();
		void AddTranfromModelData(const glm::mat4& modelMatrix);
		void AddTranfromNormaleData(const glm::mat4& normaleMatrix);


		void SetubShaderDrawList();

		void NextDrawCall(RenderProxy& proxy);
		void NextDrawCall(RenderMeshBatch& meshBatch);
		void SubmiteShaderDrawListMain();
		void SubmiteShaderDrawListCurent();
		void Clear();
		void ClearBatchManger();
		void ClearDrawList();
		static bool& GetUseDyamincDatatStruct() { return s_UseDyamincDatatStruct; }
#if 1
		virtual void SubmiteDraw(ProxyGroupView& groupView);
#endif
	    ShaderDrawResource BuildDrawTemplate(const RenderMeshBatch& meshBatch);
	    void SubmiteShaderDrawList(std::vector<ShaderDrawResource>& out);
	private:
		void FinishLastDrawCall();
		void ResetCurentDrawCall();
		void SetArrayDataBufferGPUContainerVertexTransformtion();

		void UplodeArrayDataBufferGPU();
		void UplodeArrayDataBufferDynamicGPU();
		void UplodeArrayDataBufferStaticGPU();

		void AddProxyDataDynamic();
		void AddProxyDataStatic();


		void UplodedeVertexBufferFromContainer(Ref<VertexBuffer>& vertexBuffer, const BufferLayout& layout, BufferFlagGPU flag, const uint8_t* dataPtr, uint64_t bytesSize);
		void UplodedeUniformBufferFromContainer(Ref<UniformBuffer>& uniformBuffer, const BufferLayout& layout, BufferFlagGPU flag, const uint8_t* dataPtr, uint64_t bytesSize);


		void UplodedeStorageBufferFromContainer(Ref<StorageBuffer>& storageBuffer, BufferFlagGPU flag, const uint8_t* dataPtr, uint64_t bytesSize);

		void AddDrawListGlobleResources();
		void AddDrawListGlobleResourcesCameraData();
		void AddDrawListGlobleResourcesShadow();

		void PertepairNewDrawCall();
		void SetProxyBufferData();
		void SetArrayDataFromProxy(const RenderProxyDynamicElenenttData& dynamicElementData, Memory::DynamicDataStruct& dynamicData, uint64_t elementIndex, uint32_t arrayIndex);
		void AddDataArray(uint32_t indexVec, Memory::DynamicDataStruct& dynamicDataStruct, uint32_t curentIndex);
		bool CheckMaterielConfigs(const RenderMeshBatch& meshBatch);
		bool CheckMaterielConfigs(const RenderProxy& proxy);

		void BeforFirstProxy(RenderMeshBatch& meshBatch);
		void AddProxyData(RenderProxy& proxy);
		void AfterLastProxy();
	private:
		RenderProxy* m_ProxyPtr;
		RenderMeshBatch* m_RenderMeshBatchPtr;
		const glm::mat4* m_ModelMatrixPtr;
		const glm::mat4* m_NormaleMatrixPtr;

		BufferLayout m_RenderTragetLayoute;
		ShaderDrawResource m_CurentDrawCall;

		Batch3DKey m_Batch3DKey;
		
		std::vector<ShaderDrawResource> m_DrawCallsVec;
		BatchedRenderObjectVAO* m_BatchRenderObjectVAOPtr;
		BatchManager m_BatchManager;
		DrawSpecification::BatchConfig m_DrawSpecifcation;
		uint32_t m_SubmitCount;
		inline static bool s_UseDyamincDatatStruct = true;
	};
}

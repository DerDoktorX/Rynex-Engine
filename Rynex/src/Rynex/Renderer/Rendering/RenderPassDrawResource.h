#pragma once
#include "Batch3DKey.h"
#include <Rynex/Renderer/RenderProxy/RenderWorld.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchedTextureBind.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchedSimplePiplineRender.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchDataStructs.h>




namespace Rynex {

		using Shape = std::variant<Ref<VertexArray>, std::pair<Ref<VertexBuffer>, Ref<IndexBuffer>>>;
		using DrawSpec = std::variant<Mesh::PerDrawObject, Ref<IndirectBuffer>>;

		template<typename ...Args>
		using BindRes = std::pair<uint32_t, std::variant<Args...>>;
#if RY_BIND_RESOURCES_TYPE_VEC
		typedef BindRes<Ref<Texture>, Ref<BindlesTextureArray>> BindTexture;
		typedef BindRes<Ref<UniformBuffer>> BindUniform;
		typedef BindRes<Ref<StorageBuffer>> BindSSBO;
#else
		using BindAll = BindRes<Ref<Texture>, Ref<BindlesTextureArray>, Ref<UniformBuffer>, Ref<StorageBuffer>>;
#endif
		template<typename T>
		struct PerBatch 
		{
			uint32_t m_MaxElements;
			uint32_t m_CurrentIndex;
			std::vector<T> m_Batch;
		};

		struct DrawCall
		{
			Ref<Shader> m_Shader;
			Shape m_Shape;
			DrawSpec m_DrawSpec;
#if RY_BIND_RESOURCES_TYPE_VEC
			std::vector<BindTexture> texturesVec;
			std::vector<BindSSBO> ssboVec;
			std::vector<BindUniform> uniformVec;
#else
#if 1
			std::vector<BindAll> m_ResourceVec;
#else
			std::vector<std::pair<uint32_t, std::variant<typename Ref<typename Texture>, typename Ref<typename BindlesTextureArray>, typename Ref<typename UniformBuffer>, typename Ref<typename StorageBuffer>>>> resourceVec;
#endif
#endif
		};

		template<typename T, typename Buffer>
		struct InstanceBatch
		{
			Ref<MeshSingle> m_Mesh;
			Ref<Material> m_Materiel;
			uint32_t m_InstanceIndex;
			std::vector<T> m_BatchData;
			Ref<Buffer> m_Buffer;
		};

		class RenderPassDrawResource
		{
		public:
			using RenderProxyStore = RenderProxy;
			using RenderProxyPtr = RenderProxy*;
#if 0
			struct Batch3DData
			{
				int lodLevel;
				std::vector<RenderProxyStore> renderProxis;
#if 0
				PerBatch<Ref<BindlesTextureArray>> bindlesTextureArrayPerBatch;
				PerBatch<TextureResourceBindles> textureResourceBindlesPerBatch;
				PerBatch<Batch::Render3DMeshObject> render3DMeshBindlesPerBatch;
				PerBatch<Batch::Render3DMeshObjectTransform> render3DMeshBindlesTrasformPerBatch;
				PerBatch<Batch::Render3DMeshObjectTransform> render3DMeshBindlesTrasformPerBatch;
#endif
			};
#else
			using Batch3DData = std::vector<std::pair<int, RenderProxyStore>>;
#endif
		private:
			using ItMapObject = std::unordered_map<Batch3DKey, Batch3DData>::iterator;
			using ConstItMapObject = std::unordered_map<Batch3DKey, Batch3DData>::const_iterator;

		public:
			RenderPassDrawResource();
			~RenderPassDrawResource();

			void SetOutPutLayout(const BufferLayout& outPut);
			void SubmitVisibleObjects(RenderProxyPtr renderProxy, int lodTier);
			void ClearFrame();
			void GenerateDrawCalls(std::vector<DrawCall>& drawCallList);

		private:			
			static void SubmitToList(RenderProxy* proxy, int lodTier, std::vector<std::pair<int, RenderProxy>>& list);
			static void SubmitToList(RenderProxy* proxy, int lodTier, std::vector<std::pair<int, RenderProxy*>>& list);

			void DrawCallsFromList(std::vector<DrawCall>& drawCallList, const Batch3DKey& key, std::vector<std::pair<int, RenderProxy>>& list);
			void DrawCallsFromList(std::vector<DrawCall>& drawCallList, const Batch3DKey& key, std::vector<std::pair<int, RenderProxy*>>& list);

		private:
			std::unordered_map<Batch3DKey, Batch3DData> m_BatchedRenderObjectsMap;
			std::unordered_map<Batch3DKey, Batch3DData> m_LatFrameBatchedRenderObjectsMap;


			BufferLayout m_OutPutLayout;
		};


}

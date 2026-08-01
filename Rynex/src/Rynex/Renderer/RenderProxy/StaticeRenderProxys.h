#pragma once
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Renderer/Mesh/MeshSingle.h>
#include <Rynex/Renderer/Objects/BoundingVolume.h>
#include <Rynex/Renderer/RenderProxy/RenderProxysListData.h>

#include <Rynex/Renderer/PiplineObjects/Batching/BatchManager.h>
#include <Rynex/Renderer/RenderProxy/ProxyDrawCallGenarter.h>

#include <Rynex/Renderer/Rendering/RenderTarget.h>
#include <Rynex/Memory/ThreadQueue.h>

#define RY_PROCESS_ON_THREAD_MAIN
#define RY_RENDER_PROXY_VEC_CLASS
// #define RY_BATCHING_UPDATE_BUFFER_RANGE_CHANGE_BASED
// #define RY_TEST_DRAW_CALL
#define RY_THREAD_QUEUE
#define RY_NO_EXAPTION_HANDLING
#define RY_RENDER_PRROXY_INSTAECING
// #define RY_SORTED_PROXY_LIST
#if 0 && defined(RY_TEXTURE_VARIENTS)
	#define RY_ALBEDO_TEXTURE_ARRAY
#endif
#define RY_DRAW_GENRATER
// #define RY_RENDER_PROXY_LIST_DATA_INLINE
// #define RY_USE_SORTED_PROXY_LIST

namespace Rynex {
	struct ShaderDrawList;
	
	class StaticeRenderProxys
	{
	private:
		struct AddFuncArgs
		{
			AddFuncArgs() = delete;
			AddFuncArgs(AddFuncArgs&&) = delete;
			AddFuncArgs(const AddFuncArgs&) = default;

			RenderProxy proxy;
			glm::mat4 model;
		};

		struct UpdateFuncArgs
		{
			int entity;
			uint32_t subMesh;
			glm::mat4 model;

			UpdateFuncArgs() = delete;
			UpdateFuncArgs(UpdateFuncArgs&&) = delete;
			UpdateFuncArgs(const UpdateFuncArgs&) = default;

			UpdateFuncArgs(int entity, uint32_t subMesh, const glm::mat4& model)
				: entity(entity)
				, subMesh(subMesh)
				, model(model)
			{
			}
		};

		struct RemoveFuncArgs
		{
			int entity;
			uint32_t subMesh;
			RemoveFuncArgs() = delete;
			RemoveFuncArgs(RemoveFuncArgs&&) = delete;
			RemoveFuncArgs(const RemoveFuncArgs&) = default;

			RemoveFuncArgs(int entity, uint32_t subMesh)
				: entity(entity)
				, subMesh(subMesh)
			{
			}
		};

		struct RemoveFuncArgs2
		{
			int entity;
			RemoveFuncArgs2() = delete;
			RemoveFuncArgs2(RemoveFuncArgs2&&) = delete;
			RemoveFuncArgs2(const RemoveFuncArgs2&) = default;

			RemoveFuncArgs2(int entity)
				: entity(entity)
			{
			}
		};

		struct DrawRenderResource
		{
			Ref<Texture> textureShadow = nullptr;
			Ref<UniformBuffer> shadowCameraBuffer = nullptr;
			Ref<UniformBuffer> cameraBuffer = nullptr;
			BufferLayout layoutOutPut;

		};

		struct DrawCallResource
		{
			const Ref<Texture>& texture;
			const Ref<Shader>& shader;
			const BatchedRenderObjectVAO& batchVAO;
			const Mesh::PerDrawObject& perDrawObject;
			int renderMode;

			DrawCallResource() = delete;
			DrawCallResource(const DrawCallResource&) = delete;
			DrawCallResource(
				const Ref<Texture>& texture,  
				const Ref<Shader>& shader, const BatchedRenderObjectVAO& batchVAO
				, const Mesh::PerDrawObject& perDrawObject, int renderMode)
				: texture(texture)
				, shader(shader)
				, batchVAO(batchVAO)
				, perDrawObject(perDrawObject)
				, renderMode(renderMode)
			{
			}
		};

		
	public:
		using IndicesPair = std::pair<uint32_t, uint32_t>;
	public:
		StaticeRenderProxys();
		~StaticeRenderProxys();


		void Clear();
		void BatchesClear();

		void Add(int entity, uint32_t subMesh, const Ref<MeshSingle>& mesh, const Ref<Material>& materiel, const glm::mat4& model);
		void UpdateTrasform(int entity, const glm::mat4& model);
		void UpdateTrasform(int entity, uint32_t subMesh, const glm::mat4& model);

		void Remove(int entity);
		bool HasEntity(int entity);

		void EventCallback();
		void RenderProxysMainGenarte();
		void RenderProxysMainSubmiteDrawList();

		void RenderProxysCurentGenarte();
		void RenderProxysCurentSubmiteDrawList();


		static bool& GetTestDrawCall() { return s_TestDrawCall; }
		static bool& GetRenderProxyInstaecing() { return s_RenderProxyInstaecing; }
		static bool& GetUseDyamincDatatStruct() { return ProxyDrawCallGenarter::GetUseDyamincDatatStruct(); }
	private:
		bool CreateDrawListMain();
		bool CreateDrawListCurent();

		uint32_t CreateToDrawList(const DrawRenderResource& drawResources, uint32_t proxyIndex);

		void GenarteDrawCall(const DrawRenderResource& drawResources, const DrawCallResource& drawCallResource);
		void SubmitDrawList();
		uint32_t RenderProxysMainBatch(uint32_t proxyIndex, const RenderProxy& first, uint32_t proxyCount);

		static void SetupVertexBufferRenderObject(BatchedRenderObjectVAO& batchedRenderObjectVAO, const Ref<MeshSingle>& mesh, int lodLevel);
		static void CreateVertexBufferRenderObject(BatchedRenderObjectVAO& batchedRenderObjectVAO, const Ref<MeshSingle>& mesh, int lodLevel);
		static void ResizeVertexBufferRenderObject(BatchedRenderObjectVAO& batchedRenderObjectVAO);
		static void UploadeVertexBufferRenderObject(BatchedRenderObjectVAO& batchedRenderObjectVAO);


		bool CheckProxyFromIndex(uint32_t proxyIndex, uint32_t proxyCount, ProxyDrawCallGenarter& drawCallGenrater);


	private:

		ProxyDrawCallGenarter m_ProxyDrawCallGenarterMain;
		ProxyDrawCallGenarter m_ProxyDrawCallGenarterCurent;

		RenderProxysListData m_RenderProxysListData;
		BatchManager m_BatchManager;
		std::vector<ShaderDrawResource> m_DrawCallsVec;
	private:
		inline static bool s_TestDrawCall = true;
		inline static bool s_RenderProxyInstaecing = true;


	};


}

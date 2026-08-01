#pragma once
#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Scene/Components.h>
// #define RY_RENDER_PROXY_IN_MAIN_RENDER_CALL

namespace Rynex {
	struct ShaderDrawEntityList;
	class PiplineRefBaseVec;

	typedef enum LigthSourceEnum {

		LigthSource_None = 0,
		LigthSource_Dirctionel = 1,

	}LigthSourceEnum;

	struct LigthSource 
	{
		glm::vec3 postion;
		float intensty;
		glm::vec3 color; 
		int type;
		glm::vec3 direction;

		LigthSource()
			: postion(0.0f, 0.0f, 0.0f)
			, color(0.0f, 0.0f, 0.0f)
			, intensty(0.0f)
			, direction(0.0f)
			, type(LigthSourceEnum::LigthSource_None)
		{

		}

		LigthSource(const LigthSource&) = default;
	};
	
	class Renderer3D
	{
	public:
		static void Init();
		static void InitEditor();

		static void Shutdown();
		static void ShutdownEditor();

		static void ClearRenderProxy();
		static void ClearBatchesFromRenderProxy();

		static void AddMeshComponentRenderProxy(int entityID, const ModelMangerComponent& comp, const glm::mat4& model);
		static void UpdateTransformMeshComponentRenderProxy(int entityID, const ModelMangerComponent& comp, const glm::mat4& model);
		static void RemoveMeshComponentRenderProxy(int entityID);
		static void UpdateEventProxys();
		static void RenderProxysMain();
		static void RenderProxysCurent();

		static void MeshCompont(const glm::mat4& model, ModelMangerComponent& comp, int entityID);
		static void MeshCompontMain(const glm::mat4& model, ModelMangerComponent& comp, int entityID);
		static void MeshCompontCurent(const glm::mat4& model, ModelMangerComponent& comp, int entityID);

		static void MeshCompontDirekt(const glm::mat4& model, ModelMangerComponent& comp, int entityID);
		static void MeshCompontSetData(const glm::mat4& model, ModelMangerComponent& comp, int entityID);


		static void MeshCompont(const glm::mat4& model, StaticMeshComponent& comp, int entityID);
		static void MeshCompontMain(const glm::mat4& model, StaticMeshComponent& comp, int entityID);
		static void MeshCompontCurent(const glm::mat4& model, StaticMeshComponent& comp, int entityID);

		static void MeshCompontDirekt(const glm::mat4& model, StaticMeshComponent& comp, int entityID);
		static void MeshCompontSetData(const glm::mat4& model, StaticMeshComponent& comp, int entityID);

		
		static void SubmitMeshStaticObjectSetData(const Ref<MeshStatic>& mesh, const glm::mat4& model, int entityID, std::vector<std::vector<uint32_t>>& objectRendereVec2);
		static void SubmitMeshObjectSetData(const SingleMeshObject& singleMesh, const glm::mat4& model, int entityID, std::vector<uint32_t>& indexVec);
		static void SubmitMeshObjectSetDataShade(const SingleMeshObject& singleMesh, const glm::mat4& model, int entityID, uint32_t& storeIndex);
		static void SubmitMeshObjectSetDataShape(const SingleMeshObject& singleMesh, const glm::mat4& model, int entityID, uint32_t& storeIndex);
		static void SubmitMeshObjectSetDataDepth(const SingleMeshObject& singleMesh, const glm::mat4& model, int entityID, uint32_t& storeIndex);

		static void SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRenderePiplineVec);
		static void SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRendereIndex>& objectRenderePiplineVec);
		static void SubmitShadeMeshObjectMain(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRenderePipline);
		static void SubmitShadeMeshObjectCurent(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRenderePipline);

		static void SubmitShapeMeshObjectDirekt(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID);

		static void SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<std::vector<ObjectRendereIndex>>& objectRendereVec);
		static void SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData2D<ObjectRendereIndex>& objectRendereVec);
		static void SubmitShadeMeshStaticObjectMain(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRendereVec);
		static void SubmitShadeMeshStaticObjectMain(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRendereIndex>& objectRendereVec);

		static void SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<SingleMeshRender>& singleMeshRendereVec);
		static void SubmitShadeMeshStaticObjectCurent(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRendereVec);
		static void SubmitShadeMeshStaticObjectCurent(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRendereIndex>& objectRendereVec);

		static void SubmitShadeDataMeshObjectToPipline();
		static void SubmitDepthDataMeshObjectToPipline();


		static void SubmitShapeMeshStaticObjectDirekt(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID);


		static void SubmitMeshObjectToHash(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRendere);

		static void SubmitHashMeshesToPipline();
		static void SubmitMeshObjectToPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRendere);
#ifdef RY_RENERER_DESIGN_CURENT_MAIN
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetMain(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere);
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetCurent(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere);
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetCurentDirekt(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID);

#else
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetShade(CamerRenderPackages& cameraPackege,  const SingleMeshObject& meshObject, const Ref<Shader>& shader,const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere);
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetDepth(CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere);
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetShape(CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID);

		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTarget(PiplineRefBaseVec* piplineBaseVecPtr, CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere);
#endif
		static void PrepairMainScene();



		static void ClearMeshObjects();
		
		// x/r/[0]: BaseVertex,  y/g/[1]: FirstIndex
		static glm::uvec2 GetIndriectOffsetsFromMeshArray(const Ref<MeshStatic>& meshStatic);
		static const Ref<VertexArray>& GetMeshArrayVAO();
		static void SubmitRenderIndrectDrawList();
		static void SubmitRenderSingleIndrectDrawList();
		static void SubmitRenderMeshDrawList();
		static Ref<Texture> GetDefoultChekebordTex();
		static Ref<Texture> GetErrorTex();

		static void ResetTargetRenderPtr();
		static void ResetMeshObject();
		static void FrameFinshed();
	private:
		static Ref<Material> GetMaterilNotInFustrem();
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
		static void ForEchViewPass(const std::function<void(const std::string name, ViewPassStorage& viewPass)>& func);
		static void ForEchViewPassType(const std::string& name, const std::function<void(ViewPassStorage& viewPass)>& func);
#endif // !RY_RENERER_DESIGN_CURENT_MAIN

		static void InitBatchedMeshArray(const std::vector<uint8_t>& vertices, const std::vector<uint32_t>& indices);
		static void InitBatchedMeshArrayVAO();
		static void InitBatchedMeshArrayVB(const std::vector<uint8_t>& vertices);
		static void InitBatchedMeshArrayIB(const std::vector<uint32_t>& indices);
		static void CheckMesh(const Ref<MeshStatic>& meshStatic);
		static void CheckMesh(const Ref<MeshDynamic>& meshDynamic);

		static void AddMeshData(const Ref<MeshStatic>& meshStatic);
		static void AddMeshData(const Ref<MeshDynamic>& meshDynamic);
		static void SetOffsetBatchedMesh();
		static void BatcheMeshData(const Ref<VertexBuffer>& vb, const Ref<IndexBuffer>& ib);
		
		static UUID& GetEntityShaderDrawList(int entity);
#if 0
		static ShaderDrawEntityList& GetEntitiyShaderDrawList(const UUID& handle, const int& entityID);
		static DrawList& FindShaderDrawList(const Ref<Shader>& shader, const int& entityID);
		static DrawList& FindShaderDrawList(const Ref<Shader>& shader, const int& entityID, uint32_t index);
#endif
		static Ref<PiplineRenderBase> GetPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel);
	
		
	};
}

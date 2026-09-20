#pragma once
#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Scene/Components.h>
// #define RY_RENDER_PROXY_IN_MAIN_RENDER_CALL

namespace Rynex {
	struct ShaderDrawEntityList;
	class PiplineRefBaseVec;

    enum LightSourceEnum {
		LightSource_None = 0,
		LightSource_Directional = 1,
	};

	struct LightSource
	{
		glm::vec3 m_Position;
		float m_Intensity;
		glm::vec3 m_Color;
		int m_Type;
		glm::vec3 m_Direction;

		LightSource()
			: m_Position(0.0f, 0.0f, 0.0f)
			, m_Intensity(0.0f)
			, m_Color(0.0f, 0.0f, 0.0f)
			, m_Type(LightSourceEnum::LightSource_None)
			, m_Direction(0.0f)
		{
		}
		LightSource(const LightSource&) = default;

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
		static void RenderProxysCurrent();

		static void MeshComponent(const glm::mat4& model, ModelMangerComponent& comp, int entityID);
		static void MeshComponentMain(const glm::mat4& model, ModelMangerComponent& comp, int entityID);
		static void MeshComponentCurrent(const glm::mat4& model, ModelMangerComponent& comp, int entityID);

		static void MeshComponentDirect(const glm::mat4& model, ModelMangerComponent& comp, int entityID);
		static void MeshComponentSetData(const glm::mat4& model, ModelMangerComponent& comp, int entityID);


		static void MeshComponent(const glm::mat4& model, StaticMeshComponent& comp, int entityID);
		static void MeshComponentMain(const glm::mat4& model, StaticMeshComponent& comp, int entityID);
		static void MeshComponentCurrent(const glm::mat4& model, StaticMeshComponent& comp, int entityID);

		static void MeshComponentDirect(const glm::mat4& model, StaticMeshComponent& comp, int entityID);
		static void MeshComponentSetData(const glm::mat4& model, StaticMeshComponent& comp, int entityID);

		
		static void SubmitMeshStaticObjectSetData(const Ref<MeshStatic>& mesh, const glm::mat4& model, int entityID, std::vector<std::vector<uint32_t>>& objectRendereVec2);
		static void SubmitMeshObjectSetData(const SingleMeshObject& singleMesh, const glm::mat4& model, int entityID, std::vector<uint32_t>& indexVec);
		static void SubmitMeshObjectSetDataShade(const SingleMeshObject& singleMesh, const glm::mat4& model, int entityID, uint32_t& storeIndex);
		static void SubmitMeshObjectSetDataShape(const SingleMeshObject& singleMesh, const glm::mat4& model, int entityID, uint32_t& storeIndex);
		static void SubmitMeshObjectSetDataDepth(const SingleMeshObject& singleMesh, const glm::mat4& model, int entityID, uint32_t& storeIndex);

		static void SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRenderePiplineVec);
		static void SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRenderIndex>& objectRenderePiplineVec);
		static void SubmitShadeMeshObjectMain(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRenderePipline);
		static void SubmitShadeMeshObjectCurrent(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRenderePipline);

		static void SubmitShapeMeshObjectDirect(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID);

		static void SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<std::vector<ObjectRenderIndex>>& objectRendereVec);
		static void SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData2D<ObjectRenderIndex>& objectRendereVec);
		static void SubmitShadeMeshStaticObjectMain(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRendereVec);
		static void SubmitShadeMeshStaticObjectMain(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRenderIndex>& objectRendereVec);

		static void SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<SingleMeshRender>& singleMeshRendereVec);
		static void SubmitShadeMeshStaticObjectCurrent(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRendereVec);
		static void SubmitShadeMeshStaticObjectCurrent(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRenderIndex>& objectRendereVec);

		static void SubmitShadeDataMeshObjectToPipline();
		static void SubmitDepthDataMeshObjectToPipline();


		static void SubmitShapeMeshStaticObjectDirect(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID);


		static void SubmitMeshObjectToHash(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRendere);

		static void SubmitHashMeshesToPipline();
		static void SubmitMeshObjectToPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRendere);
#ifdef RY_RENERER_DESIGN_CURENT_MAIN
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetMain(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRendere);
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetCurrent(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRendere);
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetCurrentDirect(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID);

#else
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetShade(CamerRenderPackages& cameraPackege,  const SingleMeshObject& meshObject, const Ref<Shader>& shader,const glm::mat4& model, int entityID, ObjectRenderIndex& objectRendere);
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetDepth(CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRendere);
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetShape(CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID);

		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTarget(PiplineRefBaseVec* piplineBaseVecPtr, CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRendere);
#endif

		static void PrepareMainScene();



		static void ClearMeshObjects();
		
		// x/r/[0]: BaseVertex,  y/g/[1]: FirstIndex
		static glm::uvec2 GetIndictOffsetsFromMeshArray(const Ref<MeshStatic>& meshStatic);
		static const Ref<VertexArray>& GetMeshArrayVAO();
		static void SubmitRenderIndictDrawList();
		static void SubmitRenderSingleIndictDrawList();
		static void SubmitRenderMeshDrawList();

		static Ref<Texture> GetDefaultCheckerboardTexture();
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

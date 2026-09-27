#pragma once
#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Scene/Components.h>


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


		static void SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRendererPiplineVec);
		static void SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRenderIndex>& objectRendererPiplineVec);
		static void SubmitShadeMeshObjectMain(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRendererPipline);
		static void SubmitShadeMeshObjectCurrent(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRendererPipline);

		static void SubmitShapeMeshObjectDirect(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID);

		static void SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<std::vector<ObjectRenderIndex>>& objectRendererVec);
		static void SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData2D<ObjectRenderIndex>& objectRendererVec);
		static void SubmitShadeMeshStaticObjectMain(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRendererVec);
		static void SubmitShadeMeshStaticObjectMain(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRenderIndex>& objectRendererVec);

		static void SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<SingleMeshRender>& objectRendererVec);
		static void SubmitShadeMeshStaticObjectCurrent(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRendererVec);
		static void SubmitShadeMeshStaticObjectCurrent(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRenderIndex>& objectRendererVec);

		static void SubmitShadeDataMeshObjectToPipline();
		static void SubmitDepthDataMeshObjectToPipline();


		static void SubmitShapeMeshStaticObjectDirect(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID);


		static void SubmitMeshObjectToHash(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRenderer);

		static void SubmitHashMeshesToPipline();
		static void SubmitMeshObjectToPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRenderer);


		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetMain(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRenderer);
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetCurrent(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRenderer);
		static Ref<PiplineRenderBase> SubmitMeshObjectToRenderTargetCurrentDirect(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID);


		static Ref<Texture> GetDefaultCheckerboardTexture();
		static Ref<Texture> GetErrorTex();


		static void FrameFinshed();
	private:
		static Ref<Material> GetMaterilNotInFustrem();
		static Ref<PiplineRenderBase> GetPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel);
	};
}

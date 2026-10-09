#include <rypch.h>
#include "Renderer3D.h"

#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Asset/Import/TextureImporter.h>


#include <Rynex/Renderer/Rendering/Render3D/IndirectDrawMap.h>


#include <Rynex/Renderer/PiplineObjects/Piplines/SinglePiplineRender.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/InstenceMeshPiplineRender.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplinePolicies.h>

#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>
#include <Rynex/Renderer/RenderProxy/StaticeRenderProxys.h>



#define RY_ENABELE_LIST_STYSTEM 1
#define RY_ENTITY_MESH_LIST 0
#define RY_ENABELE_LIST_STYSTEM_OPTIMIZE 1

#define RY_RENDERER_3D_HARDCODED_PIPLINES 1
#define RY_RENDERER_3D_PROTOYPE_PIPLINES 0

#define RY_RENDER_PIPLINE_INSTANCE 1
#define RY_RENDER_PIPLINE_INSTANCE_SHADOW 1

#define RY_STATIC_OPTIMZE 0
#define RY_ENABLE_CHECK_FUSTREM 0
#define RY_CHECK_FUSTREM_SHADE_ONLY 0
#define RY_CHECK_FUSTREM_SHADOW_ONLY 0
#define RY_CHECK_FUSTREM !(RY_CHECK_FUSTREM_SHADE_ONLY || RY_CHECK_FUSTREM_SHADOW_ONLY) && RY_ENABLE_CHECK_FUSTREM 



#define RY_HASH_GROUPING_OPTIMZE 0
#define RY_HASH_MEMORY_CLEAR 0
#define RY_HASH_VEC_MEMORY_CLEAR 0


#define RY_RENDER_3D_CACHING

namespace Rynex {















	struct RenderEnitityObject
	{
		glm::mat4 m_Matrix;
		int m_EntityID;
		Ref<Material> m_MaterielRef;
		std::vector<ObjectRenderIndex>* m_ObjectRendereIndexVec;
	};


	struct Renderer3DStorage
	{



		PiplineRefBaseVec m_SinglePiplineBaseVec;
		PiplineRefBaseVec m_InstencPiplineBaseVec;

		Ref<Shader> m_SingleShaderShade;
		Ref<Shader> m_SingleShaderDepth;

		Ref<Shader> m_InstenceShaderShade;
		Ref<Shader> m_InstencShaderDepth;
		Ref<Shader> m_InstencShaderShape;

		std::unordered_map<Ref<MeshSingle>, std::vector<RenderEnitityObject>> m_RenderEntityFrame;
		std::vector<std::unordered_map<uint64_t, Weak<PiplineRenderBase>>> m_RenderPiplinesHashMap;

		

		Ref<RenderTarget> m_MainTarget;
		Ref<RenderTarget> m_ViewPortTarget;
		Ref<Shader> m_MeshDefaultShader;
		Ref<Texture> m_CheckebordTex;
		Ref<Texture> m_ErrorTex;
		Ref<DefaultMaterial> m_MaterilNotInFrustem;
		Ref<Shader> m_IndrectMultyShadowShader;


		std::array<StaticeRenderProxys, 1> m_RenderProxysArray;
	};


	

	static Scope<Renderer3DStorage> s_Storage3D;



#define R3 s_Storage3D

	void Renderer3D::Init()
	{
		s_Storage3D = CreateScope<Renderer3DStorage>();
		s_Storage3D->m_MeshDefaultShader = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShader.glsl"));
		s_Storage3D->m_IndrectMultyShadowShader = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShadowShader.glsl"));



		Ref<Shader> shaderDepth = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/SingelMeshShadow.glsl"));
		Ref<Shader> shaderShade = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/SingelMesh.glsl"));
		Ref<Shader> instenceShaderShade = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMesh.glsl"));
		Ref<Shader> instenceShaderShape = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShape.glsl"));
		Ref<Shader> instenceShaderDepth = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShadow.glsl"));

		s_Storage3D->m_SingleShaderDepth = shaderDepth;
		s_Storage3D->m_SingleShaderShade = shaderShade;
		s_Storage3D->m_InstenceShaderShade = instenceShaderShade;
		s_Storage3D->m_InstencShaderShape = instenceShaderShape;
		s_Storage3D->m_InstencShaderDepth = instenceShaderDepth;


	}

	void Renderer3D::InitEditor()
	{
	}

	void Renderer3D::Shutdown()
	{
		s_Storage3D->m_SinglePiplineBaseVec.Destroy();
		s_Storage3D->m_InstencPiplineBaseVec.Destroy();

		for(auto& renderProxy : s_Storage3D->m_RenderProxysArray)
			renderProxy.Clear();

		ClearRenderProxy();
		ClearBatchesFromRenderProxy();

		RY_DESTROY_SCOPE(s_Storage3D);
	}

	void Renderer3D::ShutdownEditor()
	{
	}

	void Renderer3D::ClearRenderProxy()
	{
		s_Storage3D->m_RenderProxysArray[0].Clear();
	}
	void Renderer3D::ClearBatchesFromRenderProxy()
	{
		s_Storage3D->m_RenderProxysArray[0].BatchesClear();
	}

	void Renderer3D::AddMeshComponentRenderProxy(int entityID, const ModelMangerComponent& comp, const glm::mat4& model)
	{
		const Ref<MeshStatic>& meshStatic = comp.m_MeshStatic;
		if (nullptr == meshStatic)
			return;

		const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshData = meshStatic->GetSingleObjectMesDataVec();
		uint32_t index = 0u;
		for (const MeshStatic::SingleObjectMeshData& singleObject : singleObjectMeshData)
		{
			glm::mat4 globleMatrix = model * singleObject.m_LocaleCildrenMatrix;
			const Ref<MeshSingle>& mesh = singleObject.m_MeshSingle;
			const Ref<Material>& material = singleObject.m_Material;

			s_Storage3D->m_RenderProxysArray[0].Add(entityID, index, mesh, material, globleMatrix);
			index++;
		}
	}

	void Renderer3D::UpdateTransformMeshComponentRenderProxy(int entityID, const ModelMangerComponent& comp, const glm::mat4& model)
	{
		const Ref<MeshStatic>& meshStatic = comp.m_MeshStatic;
		if (nullptr == meshStatic)
		{
			s_Storage3D->m_RenderProxysArray[0].Remove(entityID);
			return;
		}

		if (!s_Storage3D->m_RenderProxysArray[0].HasEntity(entityID))
		{
			AddMeshComponentRenderProxy(entityID, comp, model);
			return;
		}

		const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshData = meshStatic->GetSingleObjectMesDataVec();
		uint32_t index = 0u;
		for (const MeshStatic::SingleObjectMeshData& singleObject : singleObjectMeshData)
		{
			glm::mat4 globalMatrix = model * singleObject.m_LocaleCildrenMatrix;
			s_Storage3D->m_RenderProxysArray[0].UpdateTrasform(entityID, index, globalMatrix);
			index++;
		}

	}

	void Renderer3D::RemoveMeshComponentRenderProxy(int entityID)
	{
		s_Storage3D->m_RenderProxysArray[0].Remove(entityID);
	}

	void Renderer3D::UpdateEventProxys()
	{
		s_Storage3D->m_RenderProxysArray[0].EventCallback();
	}

	void Renderer3D::RenderProxysMain()
	{
		s_Storage3D->m_RenderProxysArray[0].RenderProxysMainGenarte();
		s_Storage3D->m_RenderProxysArray[0].RenderProxysMainSubmiteDrawList();

	}

	void Renderer3D::RenderProxysCurrent()
	{
		s_Storage3D->m_RenderProxysArray[0].RenderProxysCurentGenarte();
		s_Storage3D->m_RenderProxysArray[0].RenderProxysCurentSubmiteDrawList();
	}



	void Renderer3D::MeshComponent(const glm::mat4& model, ModelMangerComponent& comp, int entityID)
	{
		const Ref<MeshStatic>& mesh = comp.m_MeshStatic;

		if (nullptr == mesh)
			return;

		SubmitShadeMeshStaticObject(mesh, s_Storage3D->m_InstenceShaderShade, model, entityID, comp.m_ObjectRenderIndexPiplineVec2);
	}

	void Renderer3D::MeshComponentMain(const glm::mat4& model, ModelMangerComponent& comp, int entityID)
	{
		const Ref<MeshStatic>& mesh = comp.m_MeshStatic;
		if (nullptr == mesh)
			return;
		constexpr uint32_t piplineIndex = 0u;

		Memory::VectorData2D<ObjectRenderIndex>& objectRendereIndexPiplineVec2 = comp.m_ObjectRenderIndexPiplineVec2;
		if (objectRendereIndexPiplineVec2.SizeDX() <= piplineIndex)
		{

			const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
			uint32_t count = meshSingleVec.size();
			uint32_t nextIndex = objectRendereIndexPiplineVec2.SizeDX() + 1;
			objectRendereIndexPiplineVec2.Resize2D(nextIndex, count);
		}

		Memory::VectorData<ObjectRenderIndex> objectRendererIndexPiplineVec = objectRendereIndexPiplineVec2.At(piplineIndex);

		SubmitShadeMeshStaticObjectMain(mesh, s_Storage3D->m_InstenceShaderShade, model, entityID, objectRendererIndexPiplineVec);

	}

	void Renderer3D::MeshComponentCurrent(const glm::mat4& model, ModelMangerComponent& comp, int entityID)
	{
		const Ref<MeshStatic>& mesh = comp.m_MeshStatic;
		if (nullptr == mesh)
			return;
		uint32_t piplineIndex = Renderer::GetCurrentIndex();

		Memory::VectorData2D<ObjectRenderIndex>& objectRendererIndexPiplineVec2 = comp.m_ObjectRenderIndexPiplineVec2;
		if (objectRendererIndexPiplineVec2.SizeDX() <= piplineIndex)
		{
			piplineIndex = objectRendererIndexPiplineVec2.SizeDX();
			const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
			uint32_t count = meshSingleVec.size();
			if (objectRendererIndexPiplineVec2.SizeDY()==0)
				objectRendererIndexPiplineVec2.Resize2D(1 + piplineIndex, 1);
			else
				objectRendererIndexPiplineVec2.ResizeX(1+piplineIndex);
		}
		Memory::VectorData<ObjectRenderIndex> objectRendereIndexPiplineVec = objectRendererIndexPiplineVec2.At(piplineIndex);

		SubmitShadeMeshStaticObjectCurrent(mesh, s_Storage3D->m_InstencShaderDepth, model, entityID, objectRendereIndexPiplineVec);
	}

	void Renderer3D::MeshComponentDirect(const glm::mat4& model, ModelMangerComponent& comp, int entityID)
	{
		const Ref<MeshStatic>& mesh = comp.m_MeshStatic;
		if (nullptr == mesh)
			return;

		SubmitShapeMeshStaticObjectDirect(mesh, s_Storage3D->m_InstencShaderShape, model, entityID);

	}

	void Renderer3D::MeshComponentSetData(const glm::mat4& model, ModelMangerComponent& comp, int entityID)
	{
		RY_CORE_NOT_IMPL();
	}

	void Renderer3D::MeshComponent(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		SingleMeshObject singleMeshObject;
		singleMeshObject.m_Material = comp.m_Material;
		singleMeshObject.m_MeshSingle = comp.m_MeshSingle;

		SubmitShadeMeshObject(singleMeshObject, s_Storage3D->m_InstenceShaderShade, model , entityID, comp.m_ObjectRenderIndexPiplineVec);
	}

	void Renderer3D::MeshComponentMain(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		const Ref<MeshSingle>& mesh = comp.m_MeshSingle;
		if (nullptr == mesh)
			return;
		constexpr uint32_t piplineIndex = 0u;
		std::vector<ObjectRenderIndex>& objectRendereIndexPiplineVec = comp.m_ObjectRenderIndexPiplineVec;
		if (objectRendereIndexPiplineVec.size() <= piplineIndex)
			objectRendereIndexPiplineVec.emplace_back();

		SingleMeshObject singleMeshObject;
		singleMeshObject.m_Material = comp.m_Material;
		singleMeshObject.m_MeshSingle = comp.m_MeshSingle;

		ObjectRenderIndex& objectRendererIndexPipline = objectRendereIndexPiplineVec.at(piplineIndex);
		SubmitShadeMeshObjectMain(singleMeshObject, s_Storage3D->m_InstencShaderDepth, model, entityID, objectRendererIndexPipline);
	}

	void Renderer3D::MeshComponentCurrent(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		const Ref<MeshSingle>& mesh = comp.m_MeshSingle;
		if (nullptr == mesh)
			return;
		uint32_t piplineIndex = Renderer::GetCurrentIndex();
		std::vector<ObjectRenderIndex>& objectRendereIndexPiplineVec = comp.m_ObjectRenderIndexPiplineVec;
		if (objectRendereIndexPiplineVec.size() <= piplineIndex)
			objectRendereIndexPiplineVec.emplace_back();

		SingleMeshObject singleMeshObject;
		singleMeshObject.m_Material = comp.m_Material;
		singleMeshObject.m_MeshSingle = comp.m_MeshSingle;
		ObjectRenderIndex& objectRendererIndexPipline = objectRendereIndexPiplineVec.at(piplineIndex);
		SubmitShadeMeshObjectCurrent(singleMeshObject, s_Storage3D->m_InstencShaderDepth, model, entityID, objectRendererIndexPipline);
	}

	void Renderer3D::MeshComponentDirect(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		SubmitShapeMeshObjectDirect(SingleMeshObject{ comp.m_Material, comp.m_MeshSingle }, s_Storage3D->m_InstencShaderShape, model, entityID);
	}

	void Renderer3D::MeshComponentSetData(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		RY_CORE_NOT_IMPL();
	}








	void Renderer3D::SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<std::vector<ObjectRenderIndex>>& objectRendererVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		uint32_t count = meshSingleVec.size();
		if (objectRendererVec.size() < count)

		{
			objectRendererVec.resize(count);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {} Reset Caching", entityID);
#endif

		}




		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			std::vector<ObjectRenderIndex>& objectRendere = objectRendererVec.at(i);
			glm::mat4 modelMatrix = model * meshSingle.m_LocaleCildrenMatrix;

			SubmitShadeMeshObject(meshSingle, shader, modelMatrix, entityID, objectRendere);
#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif
			i++;
		}
	}

	void Renderer3D::SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData2D<ObjectRenderIndex>& objectRendererVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		uint32_t count = meshSingleVec.size();
		if (objectRendererVec.SizeDY() < count)
		{
			objectRendererVec.ResizeY(count );

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {} Reset Caching", entityID);
#endif

		}




		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			Memory::VectorData<ObjectRenderIndex> objectRendere = objectRendererVec.At(i);
			glm::mat4 modelMatrix = model * meshSingle.m_LocaleCildrenMatrix;
			SubmitShadeMeshObject(meshSingle, shader, modelMatrix, entityID, objectRendere);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif
			i++;
		}
	}

	void Renderer3D::SubmitShadeMeshStaticObjectMain(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRendererVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		const uint32_t count = meshSingleVec.size();
		if (objectRendererVec.size() < count)
		{
			objectRendererVec.resize(count);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {} Reset Caching", entityID);
#endif
		}

		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			ObjectRenderIndex& objectRendere = objectRendererVec.at(i);
			glm::mat4 modelMatrix = model * meshSingle.m_LocaleCildrenMatrix;
			SubmitShadeMeshObjectMain(meshSingle, shader, modelMatrix, entityID, objectRendere);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif
			i++;
		}
	}

	void Renderer3D::SubmitShadeMeshStaticObjectMain(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRenderIndex>& objectRendererVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		const uint32_t count = meshSingleVec.size();

		if (objectRendererVec.Size() < count)
		{
			objectRendererVec.Resize2D(count);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {} Reset Caching", entityID);
#endif
		}




		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			ObjectRenderIndex& objectRenderer = objectRendererVec.At(i);
			glm::mat4 modelMatrix = model * meshSingle.m_LocaleCildrenMatrix;
			SubmitShadeMeshObjectMain(meshSingle, shader, modelMatrix, entityID, objectRenderer);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif

			i++;
		}

	}

	void Renderer3D::SubmitShadeMeshStaticObjectCurrent(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID,std::vector<ObjectRenderIndex>& objectRendereVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		const uint32_t count = meshSingleVec.size();

		if (objectRendereVec.size() < count)
		{
			objectRendereVec.resize(count);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {} Reset Caching", entityID);
#endif

		}




		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			ObjectRenderIndex& objectRenderer = objectRendereVec.at(i);
			glm::mat4 modelMatrix = model * meshSingle.m_LocaleCildrenMatrix;
			SubmitShadeMeshObjectCurrent(meshSingle, shader, modelMatrix, entityID, objectRenderer);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif
			i++;
		}
	}

	void Renderer3D::SubmitShadeMeshStaticObjectCurrent(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRenderIndex>& objectRendereVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		const uint32_t count = meshSingleVec.size();

		if (objectRendereVec.Size() < count)
		{
			objectRendereVec.Resize2D(count);
		}

		objectRendereVec.ValueBegin();
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			ObjectRenderIndex& objectRendere = objectRendereVec.Get();
			glm::mat4 modelMatrix = model * meshSingle.m_LocaleCildrenMatrix;

			SubmitShadeMeshObjectCurrent(meshSingle, shader, modelMatrix, entityID, objectRendere);
			objectRendereVec.Increase();
		}

	}

	void Renderer3D::SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<SingleMeshRender>& objectRendererVec)
	{

		if (objectRendererVec.empty())
		{
			const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
			uint32_t count = meshSingleVec.size();

			objectRendererVec.reserve(count);
			uint32_t i = 0u;
			for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
			{
				const glm::mat4& localeMatrix = meshSingle.m_LocaleCildrenMatrix;
				glm::mat4 modelMatrix = model * localeMatrix;
				const Ref<MeshSingle>& singleMesh = meshSingle.m_MeshSingle;
				const Ref<Material>& material = meshSingle.m_Material;
				SingleMeshRender& single = objectRendererVec.emplace_back<SingleMeshRender>(
					SingleMeshRender{ singleMesh, material, localeMatrix }
				);

				std::vector<ObjectRenderIndex>& renderIndex = single.m_IndexVec;
				if (material == nullptr)
				{
					RY_CORE_WARN("No Materiel is Defnied, we set a nullptr!");
				}
				if (singleMesh == nullptr)
				{
					RY_CORE_WARN("No SingleMesh is Defnied, we set a nullptr!");
				}
#ifdef RY_RENDER_3D_PRINT
				RY_CORE_TRACE("Render3D: Genarate Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif

				SubmitShadeMeshObject(single, shader, modelMatrix, entityID, renderIndex);
				i++;
			}
		}
		else
		{
			uint32_t i = 0u;
			for (SingleMeshRender& single : objectRendererVec)
			{
				glm::mat4 modelMatrix = model * single.m_LocaleCildrenMatrix;
				std::vector<ObjectRenderIndex>& renderIndex = single.m_IndexVec;

				SubmitShadeMeshObject(single, shader, modelMatrix, entityID, renderIndex);
#ifdef RY_RENDER_3D_PRINT
				std::string indexVecString = "";
				for (const ObjectRenderIndex& index : renderIndex)
				{
					std::string batchIndexString =
						"Batching: " + std::to_string(index.m_BatchIndex)
						+ "Pipline: " + std::to_string(index.m_PiplineIndex);
					indexVecString += batchIndexString;
				}
				RY_CORE_TRACE("Render3D: Enity Submite Mesh {}, used in {}", entityID, i, indexVecString);

#endif  // RY_RENDER_3D_PRINT
				i++;

			}
		}
	}

	void Renderer3D::SubmitShadeDataMeshObjectToPipline()
	{
		RY_CORE_NOT_IMPL();
	}

	void Renderer3D::SubmitDepthDataMeshObjectToPipline()
	{
		RY_CORE_NOT_IMPL();
	}




	void Renderer3D::SubmitShapeMeshStaticObjectDirect(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			glm::mat4 modelMatrix = model * meshSingle.m_LocaleCildrenMatrix;

			SubmitShapeMeshObjectDirect(meshSingle, shader, modelMatrix, entityID);

		}
	}

	void Renderer3D::SubmitMeshObjectToHash(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRendereVec)
	{
#if !RY_STATIC_OPTIMZE
		std::vector<RenderEnitityObject>& vec = s_Storage3D->m_RenderEntityFrame[meshSingle];
		vec.emplace_back(RenderEnitityObject{ model, entityID, material, &objectRendereVec });
#endif
	}



	void Renderer3D::SubmitHashMeshesToPipline()
	{
		for (auto& [meshSingle, vec] : s_Storage3D->m_RenderEntityFrame)
		{
			for (RenderEnitityObject& e : vec)
			{
				std::vector<ObjectRenderIndex>& objectRenderer = *e.m_ObjectRendereIndexVec;
				SubmitMeshObjectToPipline(meshSingle, e.m_MaterielRef, e.m_Matrix, e.m_EntityID, objectRenderer);
			}
		}
	}

	void Renderer3D::SubmitMeshObjectToPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRenderer)
	{
		SingleMeshObject singleMesh{ material, meshSingle };
		SubmitShadeMeshObject(singleMesh, s_Storage3D->m_InstenceShaderShade, model, entityID, objectRenderer);
	}




	void Renderer3D::SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRenderIndex>& objectRendererPiplineVec)
	{
		uint32_t piplineIndex = 0;
		if (objectRendererPiplineVec.Size() <= piplineIndex)
			objectRendererPiplineVec.Push();

		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetMain(singleMesh, shader, model, entityID, objectRendererPiplineVec.At(0));

		Ref<Texture> texture = Texture::White();
		Ref<UniformBuffer> ub = Renderer::GetPackegeCameraUniformMain();
		pipline->SubmitRenderTargetResourceReadImg(texture);
		pipline->SubmitRenderTargetResourceReadUB(ub);

		RY_REMBER_FUNC_CHANGE("");
	}

	void Renderer3D::SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRenderIndex>& objectRendererPiplineVec)
	{
		uint32_t piplineIndex = 0;
		if (objectRendererPiplineVec.size() <= piplineIndex)
			objectRendererPiplineVec.emplace_back<ObjectRenderIndex>(ObjectRenderIndex{});

		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetMain(singleMesh, shader, model, entityID, objectRendererPiplineVec.at(0));

		Ref<Texture> texture = Texture::White();
		Ref<UniformBuffer> ub = Renderer::GetPackegeCameraUniformMain();
		pipline->SubmitRenderTargetResourceReadImg(texture);
		pipline->SubmitRenderTargetResourceReadUB(ub);

		RY_REMBER_FUNC_CHANGE("");

	}


	void Renderer3D::SubmitShadeMeshObjectMain(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRendererPipline)
	{
		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetMain(singleMesh, shader, model, entityID, objectRendererPipline);

		uint32_t count = 0;
		RY_REMBER_FUNC_CHANGE("Move the folwing logic somewhere, to don't call per entity as per Pipline!");
		Renderer::ForEchStoredPassedRenderPass(
			[pipline, &countRef = count](const RenderPass& renderPass)->void
			{
				if ("Shadow" != renderPass.m_Name)
					return;

				const Ref<RenderTarget>& renderTarget = renderPass.m_Target;
				if (nullptr == renderTarget)
					return;

				const Ref<Framebuffer>& framebuffer = renderTarget->GetFramebuffer();
				if (nullptr == framebuffer)
					return;

				Ref<Texture> texture = framebuffer->GetDepthTexture();
				if (nullptr == texture)
					return;

				const Ref<UniformBuffer>& uniformBuffer = renderPass.m_CameraDataPackBufferUB;
				if (nullptr == uniformBuffer)
					return;

				pipline->SubmitRenderTargetResourceReadImg(texture);
				pipline->SubmitRenderTargetResourceReadUB(uniformBuffer);
				countRef++;
			}
		);
		if (0u == count)
		{
			Ref<Texture> texture = Texture::White();
			Ref<UniformBuffer>& uniformBuffer = Renderer::GetPackegeCameraUniformMain();

			pipline->SubmitRenderTargetResourceReadImg(texture);
			pipline->SubmitRenderTargetResourceReadUB(uniformBuffer);

		}



		RY_REMBER_FUNC_CHANGE();

	}

	void Renderer3D::SubmitShadeMeshObjectCurrent(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRendererPipline)
	{
		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetCurrent(singleMesh, shader, model, entityID, objectRendererPipline);
		RY_REMBER_FUNC_CHANGE();
	}





	void Renderer3D::SubmitShapeMeshObjectDirect(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID)
	{
		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetCurrentDirect(singleMesh, shader, model, entityID);
	}


	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTargetMain(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRendere)
	{
		Ref<PiplineRenderBase> pipline = nullptr;




		PiplineRefBaseVec& piplineBaseVec = Renderer::GetRenderPiplineMain();


		uint32_t& piplineIndex = objectRendere.m_PiplineIndex;
		uint32_t& instanceIndex = objectRendere.m_BatchIndex;

		PiplineResultState result = PiplineResultState::Result_None;
		int checkResult = (result & PiplineResultState::Result_Success);
		const uint32_t countPiplines = piplineBaseVec.GetIndexSize();


		if (piplineIndex <= countPiplines)
		{
			pipline = piplineBaseVec.GetPiplineType<InstanceMeshPiplineRenderShade>(piplineIndex);
			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instanceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);

		}

		uint32_t i = 0;
		while (checkResult == 0 && i <= countPiplines)
		{
			pipline = piplineBaseVec.GetPiplineType<InstanceMeshPiplineRenderShade>(i);
			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instanceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);

			i++;
		}
		if (i > 0)
			piplineIndex = i - 1;
		RY_CORE_ASSERT(checkResult != 0, "we didnt find a Pipline or/and even a new did not work!");


		Ref<UniformBuffer>& camerPackedUB = Renderer::GetPackegeCameraUniformMain();
		pipline->SetCameraUniformBuffer(camerPackedUB);


		return pipline;
	}

	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTargetCurrent(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRenderIndex& objectRenderer)
	{
		Ref<PiplineRenderBase> pipline = nullptr;
		PiplineRefBaseVec& piplineBaseVec = Renderer::GetRenderPiplineCurrent();


		PiplineResultState result = PiplineResultState::Result_None;
		int checkResult = (result & PiplineResultState::Result_Success);
		const uint32_t countPiplines = piplineBaseVec.GetIndexSize();

		uint32_t& pilineIndex = objectRenderer.m_PiplineIndex;
		uint32_t& instenceIndex = objectRenderer.m_BatchIndex;

		if (pilineIndex <= countPiplines)
		{

			pipline = piplineBaseVec.GetPiplineType<InstanceMeshPiplineRenderDepth>(pilineIndex);

			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			pipline->GetFrameCountNotUpdate();
			checkResult = (result & PiplineResultState::Result_Success);

		}

		uint32_t i = 0;
		while (checkResult == 0 && i <= countPiplines)
		{
			Ref<PiplineRenderBase> piplineRef = piplineBaseVec.GetPiplineType<InstanceMeshPiplineRenderDepth>(i);
			if (nullptr == piplineRef)
				piplineRef = CreateRef<InstanceMeshPiplineRenderDepth>();
			result = piplineRef->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);
			pipline = piplineRef;

			i++;
		}
		if (i > 0)
			pilineIndex = i - 1;
		RY_CORE_ASSERT(checkResult != 0, "we dident find a Pipline or/and even a new did not work!");


		Ref<UniformBuffer>& cameraPackedUB = Renderer::GetPackegeCameraUniformCurrent();
		pipline->SetCameraUniformBuffer(cameraPackedUB);


		return pipline;
	}

	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTargetCurrentDirect(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID)
	{
		Ref<PiplineRenderBase> pipline = nullptr;
		Ref<RenderTarget>& target = Renderer::GetRenderTargetCurrent();
		PiplineRefBaseVec& piplineBaseVec = s_Storage3D->m_InstencPiplineBaseVec;

		PiplineResultState result = PiplineResultState::Result_None;
		int checkResult = (result & PiplineResultState::Result_Success);
		const uint32_t countPipline = piplineBaseVec.GetIndexSize();


		uint32_t instanceIndex = 0xFFFFFFFFu;
		uint32_t i = 0;
		while (checkResult == 0 && i <= countPipline)
		{
			pipline = piplineBaseVec.GetPiplineType<InstanceMeshPiplineRenderShape>(i, target);
			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instanceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);
			i++;
		}

		RY_CORE_ASSERT(checkResult != 0, "we didn't find a Pipline or/and even a new did not work!");
		Ref<UniformBuffer>& cameraPackedUB = Renderer::GetPackegeCameraUniformCurrent();
		pipline->SetCameraUniformBuffer(cameraPackedUB);


		return pipline;
	}









	Ref<Material> Renderer3D::GetMaterilNotInFustrem()
	{
		if (nullptr == s_Storage3D->m_MaterilNotInFrustem)
		{
			MaterielShaderData materielData = MaterielShaderData();
			materielData.Color = glm::vec3(1.0f, 0.0f, 0.0f);
			materielData.Alpha = 0.15f;
			materielData.AmbientLigthe = 1.0f;
			Ref<Texture> texture = Texture::White();
			Ref<Shader> instanceShaderShade = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMesh.glsl"));
			Ref<Shader> instanceShaderShape = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShape.glsl"));
			Ref<Shader> instanceShaderDepth = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShadow.glsl"));

			s_Storage3D->m_MaterilNotInFrustem = CreateRef<DefaultMaterial>(materielData, texture, instanceShaderShade, instanceShaderDepth);
		}
		return s_Storage3D->m_MaterilNotInFrustem;
	}

#pragma region MeshArrayVertexArray






	Ref<PiplineRenderBase> Renderer3D::GetPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel)
	{
		RY_REMBER_FUNC_CHANGE("Add at befor using somthing to selkect the rigth renderPipline class!");
		uint64_t hashNumber = 0ull;
		uint64_t meshNumber = reinterpret_cast<uint64_t>(meshSingle.get());
		uint64_t materielNumber = reinterpret_cast<uint64_t>(meshSingle.get());
		constexpr uint64_t hashBitsOffset = 16ull;
		constexpr uint64_t meshHashBitsOffset = 1 * hashBitsOffset;
		constexpr uint64_t materielHashBitsOffset = 2 * hashBitsOffset;
		hashNumber |= meshNumber << meshHashBitsOffset;
		hashNumber |= materielNumber << materielHashBitsOffset;

		const uint32_t index = Renderer::GetCurrentIndex();
		std::unordered_map<uint64_t, Weak<PiplineRenderBase>>& hashMapPipline = s_Storage3D->m_RenderPiplinesHashMap.at(index);

		Weak<PiplineRenderBase>& renderPiplineWeak = hashMapPipline[hashNumber];
		Ref<PiplineRenderBase> renderPiplineRef = renderPiplineWeak.lock();
		if (nullptr == renderPiplineRef)
		{
			renderPiplineRef = CreateRef<SingleMeshPiplineRenderShade>();
			renderPiplineWeak = renderPiplineRef;
		}
		return renderPiplineRef;
	}


	Ref<Texture> Renderer3D::GetDefaultCheckerboardTexture()
	{

		if (nullptr == s_Storage3D->m_CheckebordTex)
		{
			s_Storage3D->m_CheckebordTex = Texture::Create({ 2, 2, 1, TexTar::Texture2D, TexFrom::S_RGBA8, 1, TexFilter::Nearest });
			constexpr uint32_t a = 0xFFCCCCCCu;
			constexpr uint32_t b = 0xFF555555u;
			constexpr uint32_t c = 0xFF000000u;

			uint32_t data[2][2];
			for (uint32_t x = 0; x < 2; x++)
			{
				for (uint32_t y = 0; y < 2; y++)
				{
					if (x % 2 == 0 && y % 2 == 0)
						data[x][y] = a;
					else if (x % 2 == 1 && y % 2 == 0)
						data[x][y] = b;
					else if (x % 2 == 0 && y % 2 == 1)
						data[x][y] = b;
					else if (x % 2 == 1 && y % 2 == 1)
						data[x][y] = a;
					else
						data[x][y] = b;

				}
			}
			s_Storage3D->m_CheckebordTex->SetData(data, sizeof(data));
		}
		return s_Storage3D->m_CheckebordTex;
	}

	Ref<Texture> Renderer3D::GetErrorTex()
	{
		if (nullptr == s_Storage3D->m_ErrorTex)
		{
		    const FileSystem::Path path("Engine-Resources/Resources/Icons/ErrorTex.png");
			s_Storage3D->m_ErrorTex = TextureImporter::LoadTexture(path);
		}
		return s_Storage3D->m_ErrorTex;
	}
#pragma endregion

	void Renderer3D::FrameFinshed()
	{
		RY_REMBER_FUNC_CHANGE("Change Function! ");
		s_Storage3D->m_InstencPiplineBaseVec.ResetFramePipline();
	}



}
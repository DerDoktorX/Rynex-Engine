#include "rypch.h"
#include "Renderer3D.h"

#include <Rynex/Core/MapVector.h>
#include <Rynex/Core/UnorderDoubleMap.h>

#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Asset/Import/TextureImporter.h>

#include <Rynex/Renderer/Rendering/Render3D/RenderResourceList.h>
#include <Rynex/Renderer/Rendering/Render3D/IndirectDrawMap.h>


#include <Rynex/Renderer/PiplineObjects/Piplines/SinglePiplineRender.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/InstenceMeshPiplineRender.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>
#include <Rynex/Renderer/RenderProxy/StaticeRenderProxys.h>


#define RY_OLD_SINGLE_MAP 0
#define RY_OLD_DRAW_LIST 0
#define RY_STATIC_SCREEN_DRAW 0
#define RY_ENABELE_LIST_STYSTEM 1
#define RY_ENTITY_MESH_LIST 0
#define RY_ENABELE_LIST_STYSTEM_OPTIMIZE 1
#define RY_PIPLINE_VEC_CLASS_DISABLE 0

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
// #define RY_RENDER_3D_PRINT
// #define RY_RENDER_3D_SEARCH_FOR_CHANGE

namespace Rynex {


	struct ShaderDrawEntityList2
	{
		Ref<Shader>			Shader;
		Ref<StorageBuffer>	OffsetBuffer;
		Ref<StorageBuffer>	ObjectBuffer;
		Ref<IndirectBuffer>	drawBuffer;

		IndirectDrawMap<MeshRenderObjectTransform> DrawMap;
	};

	struct ShaderDrawEntityListShadow
	{
		Ref<Shader>			Shader;
		Ref<StorageBuffer>	OffsetBuffer;
		Ref<StorageBuffer>	ObjectBuffer;
		Ref<IndirectBuffer>	drawBuffer;

		IndirectDrawMap<glm::mat4> DrawMap;
	};



	namespace Utils {

		template<typename T>
		static void SubmitMeshObject(MapVector<UUID, T>& mapVec, const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const std::vector<uint32_t>& indicesMeshes, const glm::mat4& model, int entityID)
		{

			const UUID& id = shader->Handle;
			if (!mapVec.HasKey(id))
			{
				T& shaderDrawList = mapVec.AddData(id, T{ shader });
				shaderDrawList.DrawMap.SubmitMeshes(mesh, indicesMeshes, entityID, model);
			}
			else
			{
				// ShaderDrawEntityList2& list2 = FindShaderDrawList2(shader, entityID);
				T& shaderDrawList = mapVec.GetDataFromKey(id);
				shaderDrawList.DrawMap.SubmitMeshes(mesh, indicesMeshes, entityID, model);
			}
		}

	}

	struct DrawContent
	{
		Ref<VertexBuffer> VertexB;
		Ref<IndexBuffer> IndexB;
		uint32_t BaseVertex;
		uint32_t FirstIndex;
		uint32_t LodLevel = 0;

	};

	using BatchingMeshArray = RenderResourceList<MeshStatic, DrawContent>;

	struct BatchingMeshArrayStorage
	{
		Ref<BatchingMeshArray> MeshArray;
		Ref<VertexArray> vao;
		Ref<VertexBuffer> VB;
		Ref<IndexBuffer> IB;

		glm::uvec2 OffsetCount = { 0u,0u };

		void Clear()
		{
			RY_DESTROY_REF(MeshArray);
			if (vao)
				vao->ClearVertexBuffers();
			RY_DESTROY_REF(vao);
			RY_DESTROY_REF(VB);
			RY_DESTROY_REF(IB);
			OffsetCount = { 0u,0u };
		}
	};



	struct RenderEnitityObject
	{
		glm::mat4 Matrix;
		int EntityID;
		Ref<Material> MaterielRef;
#if RY_STATIC_OPTIMZE
		ObjectRendereIndexStaticArray* ObjectRendereIndexArray;
#else
		std::vector<ObjectRendereIndex>* ObjectRendereIndexVec;
#endif
	};


	struct RenderPiplineObject
	{
		uint32_t batchIndex = 0xFFFFFFFF;
		uint32_t passIndex = 0xFFFFFFFF;

		Ref<PiplineRenderBase> renderPiplineBase = nullptr;
	};

	struct RenderObject
	{
		glm::mat4 matrix;
		int entityID;
		SingleMeshObject singleMeshObject;
		BoundingVolume volume;

		std::vector<RenderPiplineObject> piplineVec;
		 
		RenderObject(const glm::mat4& model, int entity, const SingleMeshObject& singleMesh)
			: matrix(model)
			, entityID(entity)
			, singleMeshObject(singleMesh)
		{
		}

		RenderObject(const glm::mat4& model, int entity, const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel)
			: matrix(model)
			, entityID(entity)
			, singleMeshObject(SingleMeshObject{materiel, meshSingle })
		{
		}

		void AddPipline(uint32_t passIndex, const Ref<PiplineRenderBase>& pipline, const Ref<Shader>& shader)
		{
			if (passIndex <= piplineVec.size())
				piplineVec.resize(passIndex);

			RenderPiplineObject& piplineObject = piplineVec.at(passIndex);
			piplineObject.passIndex = passIndex;
			pipline->SubmitEntityMeshObject(singleMeshObject, shader, matrix, piplineObject.batchIndex, entityID);
		}
	};

	struct Renderer3DStorage
	{

		BatchingMeshArrayStorage BatchedMesh;
#if RY_OLD_SINGLE_MAP
		std::unordered_map<UUID, ShaderDrawEntityList> ShaderDrawMap;
		std::unordered_map<int, UUID> EnitityKeyShaderDrawMap;
#endif
#if RY_OLD_DRAW_LIST
		UnorderDoubleMap<UUID, int, ShaderDrawEntityList> EnitityShaderDrawMap;

		MapVector<UUID, ShaderDrawEntityList2> ShaderDrawVectorMain;
		MapVector<UUID, ShaderDrawEntityListShadow> ShaderDrawVectorShadow;
#endif
#if RY_RENDERER_3D_HARDCODED_PIPLINES


#elif RY_RENDERER_3D_PROTOYPE_PIPLINES
		PiplinesMap RendererPiplinesMap;
#else

#endif
#if !RY_STATIC_SCREEN_DRAW
#if RY_PIPLINE_VEC_CLASS_DISABLE
		std::vector<Ref<SingleMeshPiplineRenderShade>>		PiplineRenderBaseVec;
		uint32_t											PiplineIndex;
#else

#ifndef RY_RENERER_DESIGN_CURENT_MAIN

		PiplineRefVec<SingleMeshPiplineRenderShade> PiplineShadeVec;
		PiplineRefVec<SingleMeshPiplineRenderShade> PiplineNotInFustremVec;
		PiplineRefVec<SingleMeshPiplineRenderDepth> PiplineDepthVec;

		PiplineRefVec<InstenceMeshPiplineRenderShade> InstencPiplineShadeVec;
		PiplineRefVec<InstenceMeshPiplineRenderShape> InstencPiplineShapeVec;
		PiplineRefVec<InstenceMeshPiplineRenderDepth> InstencPiplineDepthVec;
#else
		PiplineRefBaseVec SinglePiplineBaseVec;
		PiplineRefBaseVec InstencPiplineBaseVec;
#endif		
		Ref<Shader> SingleShaderShade;
		Ref<Shader> SingleShaderDepth;

		Ref<Shader> InstencShaderShade;
		Ref<Shader> InstencShaderDepth;
		Ref<Shader> InstencShaderShape;

		std::unordered_map<Ref<MeshSingle>, std::vector<RenderEnitityObject>> RenderEntityFrame;
		std::vector<std::unordered_map<uint64_t, Weak<PiplineRenderBase>>> RenderPiplinesHashMap;
#endif
		
#endif
		Ref<RenderTarget> MainTarget;
		Ref<RenderTarget> ViewPortTarget;
		Ref<Shader> MeshDefaultShader;
		Ref<Texture> CheckebordTex;
		Ref<Texture> ErrorTex;
		Ref<DefaultMaterial> MaterilNotInFustrem;
		Ref<Shader> IndrectMultyShadowShader;

		// Memory::StoreSubmite<> DiretionlLigthStore;

		std::array<StaticeRenderProxys, 1> renderProxysArray;
	};


	

	static Renderer3DStorage s_Storarage3D;



#define R3 s_Storarage3D

	void Renderer3D::Init()
	{
		s_Storarage3D.BatchedMesh.MeshArray = CreateRef<BatchingMeshArray>();
		s_Storarage3D.MeshDefaultShader = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShader.glsl"));
		s_Storarage3D.IndrectMultyShadowShader = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShadowShader.glsl"));

#if RY_RENDERER_3D_HARDCODED_PIPLINES
#elif RY_RENDERER_3D_PROTOYPE_PIPLINES
		s_Storarage3D.RendererPiplinesMap.SetIndrectMultyShadowShader(s_Storarage3D.IndrectMultyShadowShader);
#else
#endif
#if !RY_STATIC_SCREEN_DRAW
#endif

		Ref<Shader> shaderDepth = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/SingelMeshShadow.glsl"));
		Ref<Shader> shaderShade = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/SingelMesh.glsl"));
		Ref<Shader> instenceShaderShade = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMesh.glsl"));
		Ref<Shader> instenceShaderShape = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShape.glsl"));
		Ref<Shader> instenceShaderDepth = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShadow.glsl"));

#if !RY_STATIC_SCREEN_DRAW

#ifndef RY_RENERER_DESIGN_CURENT_MAIN
		shaderDepth->SetDefine("RY_OLD_CAMER_PACKEGE");
		shaderShade->SetDefine("RY_OLD_CAMER_PACKEGE");

		instenceShaderShade->SetDefine("RY_OLD_CAMER_PACKEGE");
		instenceShaderShape->SetDefine("RY_OLD_CAMER_PACKEGE");
		instenceShaderDepth->SetDefine("RY_OLD_CAMER_PACKEGE");
#endif
		s_Storarage3D.SingleShaderDepth = shaderDepth;
		s_Storarage3D.SingleShaderShade = shaderShade;
		s_Storarage3D.InstencShaderShade = instenceShaderShade;
		s_Storarage3D.InstencShaderShape = instenceShaderShape;
		s_Storarage3D.InstencShaderDepth = instenceShaderDepth;
#endif

	}

	void Renderer3D::InitEditor()
	{
	}

	void Renderer3D::Shutdown()
	{
#if RY_OLD_SINGLE_MAP
		s_Storarage3D.ShaderDrawMap.clear();
		s_Storarage3D.EnitityKeyShaderDrawMap.clear();
#else
#if RY_OLD_DRAW_LIST
		s_Storarage3D.EnitityShaderDrawMap.Clear();

		s_Storarage3D.ShaderDrawVectorMain.Clear();
		s_Storarage3D.ShaderDrawVectorShadow.Clear();
#else

#if RY_RENDERER_3D_HARDCODED_PIPLINES


#elif RY_RENDERER_3D_PROTOYPE_PIPLINES
		s_Storarage3D.RendererPiplinesMap.Clear();
#else
#endif

#if !RY_STATIC_SCREEN_DRAW
#if RY_PIPLINE_VEC_CLASS_DISABLE
		s_Storarage3D.PiplineRenderBaseVec.clear();
#else
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
		s_Storarage3D.PiplineShadeVec.Destroy();
		s_Storarage3D.PiplineDepthVec.Destroy();
		s_Storarage3D.InstencPiplineShadeVec.Destroy();
		s_Storarage3D.InstencPiplineDepthVec.Destroy();
		s_Storarage3D.InstencPiplineShapeVec.Destroy();
		s_Storarage3D.PiplineNotInFustremVec.Destroy();
#else
		s_Storarage3D.SinglePiplineBaseVec.Destroy();
		s_Storarage3D.InstencPiplineBaseVec.Destroy();
#endif // RY_RENERER_DESIGN_CURENT_MAIN

#endif

#endif

#endif
#endif
		for(auto& renderProxy : s_Storarage3D.renderProxysArray)
			renderProxy.Clear();
		ClearRenderProxy();
		ClearBatchesFromRenderProxy();

		ClearMeshObjects();
#if !RY_STATIC_SCREEN_DRAW
		RY_DESTROY_REF(s_Storarage3D.SingleShaderDepth);
		RY_DESTROY_REF(s_Storarage3D.SingleShaderShade);

		RY_DESTROY_REF(s_Storarage3D.InstencShaderShade);
		RY_DESTROY_REF(s_Storarage3D.InstencShaderDepth);
		RY_DESTROY_REF(s_Storarage3D.InstencShaderShape);
#endif
		RY_DESTROY_REF(s_Storarage3D.MainTarget);
		RY_DESTROY_REF(s_Storarage3D.MeshDefaultShader);
		RY_DESTROY_REF(s_Storarage3D.CheckebordTex);
		RY_DESTROY_REF(s_Storarage3D.IndrectMultyShadowShader);
		RY_DESTROY_REF(s_Storarage3D.ErrorTex);
		RY_DESTROY_REF(s_Storarage3D.MaterilNotInFustrem);
	}

	void Renderer3D::ShutdownEditor()
	{
	}

	void Renderer3D::ClearRenderProxy()
	{
		s_Storarage3D.renderProxysArray[0].Clear();
	}
	void Renderer3D::ClearBatchesFromRenderProxy()
	{
		s_Storarage3D.renderProxysArray[0].BatchesClear();
	}

	void Renderer3D::AddMeshComponentRenderProxy(int entityID, const ModelMangerComponent& comp, const glm::mat4& model)
	{
		const Ref<MeshStatic>& meshStatic = comp.meshStatic;
		if (nullptr == meshStatic)
			return;

		const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshData = meshStatic->GetSingleObjectMesDataVec();
		uint32_t index = 0u;
		for (const MeshStatic::SingleObjectMeshData& singleObject : singleObjectMeshData)
		{
			glm::mat4 globleMatrix = model * singleObject.LocaleCildrenMatrix;
			const Ref<MeshSingle>& mesh = singleObject._MeshSingle;
			const Ref<Material>& material = singleObject._Material;

			s_Storarage3D.renderProxysArray[0].Add(entityID, index, mesh, material, globleMatrix);
			index++;
		}
	}

	void Renderer3D::UpdateTransformMeshComponentRenderProxy(int entityID, const ModelMangerComponent& comp, const glm::mat4& model)
	{
		const Ref<MeshStatic>& meshStatic = comp.meshStatic;
#if 1
		if (nullptr == meshStatic)
		{
			s_Storarage3D.renderProxysArray[0].Remove(entityID);
			return;
		}

		if (!s_Storarage3D.renderProxysArray[0].HasEntity(entityID))
		{
			AddMeshComponentRenderProxy(entityID, comp, model);
			return;
		}
#endif
		const std::vector<MeshStatic::SingleObjectMeshData>& singleObjectMeshData = meshStatic->GetSingleObjectMesDataVec();
		uint32_t index = 0u;
		for (const MeshStatic::SingleObjectMeshData& singleObject : singleObjectMeshData)
		{
			glm::mat4 globleMatrix = model * singleObject.LocaleCildrenMatrix;
			s_Storarage3D.renderProxysArray[0].UpdateTrasform(entityID, index, globleMatrix);
			index++;
		}
		
	}

	void Renderer3D::RemoveMeshComponentRenderProxy(int entityID)
	{
		s_Storarage3D.renderProxysArray[0].Remove(entityID);
	}

	void Renderer3D::UpdateEventProxys()
	{
		s_Storarage3D.renderProxysArray[0].EventCallback();
	}

	void Renderer3D::RenderProxysMain()
	{
		s_Storarage3D.renderProxysArray[0].RenderProxysMainGenarte();
		s_Storarage3D.renderProxysArray[0].RenderProxysMainSubmiteDrawList();

	}

	void Renderer3D::RenderProxysCurent()
	{
		s_Storarage3D.renderProxysArray[0].RenderProxysCurentGenarte();
		s_Storarage3D.renderProxysArray[0].RenderProxysCurentSubmiteDrawList();
	}



	void Renderer3D::MeshCompont(const glm::mat4& model, ModelMangerComponent& comp, int entityID)
	{
		const Ref<MeshStatic>& mesh = comp.meshStatic;
#if RY_STATIC_SCREEN_DRAW
		if (nullptr == mesh && !BIT_EQUAL(comp.Stage, ModelMangerComponent::Removed))
			return;

		if (!BIT_EQUAL(comp.Stage, ModelMangerComponent::Submited))
			SubmitMeshCompont(model, comp, entityID);
		else if (BIT_EQUAL(comp.Stage, ModelMangerComponent::UpdateMesh) || BIT_EQUAL(comp.Stage, ModelMangerComponent::UpdateTransform))
			UpdateMeshCompont(model, comp, entityID);
		else if (BIT_EQUAL(comp.Stage, ModelMangerComponent::Removed))
			DeleateMeshCompont(entityID);
#else
		if (nullptr == mesh)
			return;
#if RY_RENDER_PIPLINE_INSTANCE
#if RY_STATIC_OPTIMZE
		SubmitShadeMeshStaticObject(mesh, s_Storarage3D.InstencShaderShade, model, entityID, comp.ObjectRendereIndexPiplineArrayVec);
#elif RY_ENTITY_MESH_LIST
		SubmitShadeMeshStaticObject(mesh, s_Storarage3D.InstencShaderShade, model, entityID, comp.NodeMeshVec);
#else
		SubmitShadeMeshStaticObject(mesh, s_Storarage3D.InstencShaderShade, model, entityID, comp.objectRendereIndexPiplineVec2);
#endif
#else
		SubmitShadeMeshStaticObject(mesh, s_Storarage3D.SingleShaderShade, model, entityID, comp.objectRendereIndexPiplineVec2);
#endif
#endif
	}

	void Renderer3D::MeshCompontMain(const glm::mat4& model, ModelMangerComponent& comp, int entityID)
	{
#ifdef RY_RENERER_DESIGN_CURENT_MAIN
		const Ref<MeshStatic>& mesh = comp.meshStatic;
		if (nullptr == mesh)
			return;
		constexpr uint32_t piplineIndex = 0u;
#if RY_DISABLE_FLAT_2D_VEC
		std::vector<std::vector<ObjectRendereIndex>>& objectRendereIndexPiplineVec2 = comp.objectRendereIndexPiplineVec2;
		if (objectRendereIndexPiplineVec2.size() <= piplineIndex)
			objectRendereIndexPiplineVec2.emplace_back();

		std::vector<ObjectRendereIndex>& objectRendereIndexPiplineVec = objectRendereIndexPiplineVec2.at(piplineIndex);
#else
		Memory::VectorData2D<ObjectRendereIndex>& objectRendereIndexPiplineVec2 = comp.objectRendereIndexPiplineVec2;
		if (objectRendereIndexPiplineVec2.SizeDX() <= piplineIndex)
		{
			
			const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
			uint32_t count = meshSingleVec.size();
			uint32_t nextIndex = objectRendereIndexPiplineVec2.SizeDX() + 1;
			objectRendereIndexPiplineVec2.Resize2D(nextIndex, count);
		}

		Memory::VectorData<ObjectRendereIndex> objectRendereIndexPiplineVec = objectRendereIndexPiplineVec2.At(piplineIndex);
#endif
		SubmitShadeMeshStaticObjectMain(mesh, s_Storarage3D.InstencShaderShade, model, entityID, objectRendereIndexPiplineVec);
#else
		RY_CORE_NOT_IMPL();
#endif

	}

	void Renderer3D::MeshCompontCurent(const glm::mat4& model, ModelMangerComponent& comp, int entityID)
	{
#ifdef RY_RENERER_DESIGN_CURENT_MAIN
		const Ref<MeshStatic>& mesh = comp.meshStatic;
		if (nullptr == mesh)
			return;
		uint32_t piplineIndex = Renderer::GetCurentIndex();
#if RY_DISABLE_FLAT_2D_VEC
		std::vector<std::vector<ObjectRendereIndex>>& objectRendereIndexPiplineVec2 = comp.objectRendereIndexPiplineVec2;
		if (objectRendereIndexPiplineVec2.size() <= piplineIndex)
		{
			piplineIndex = objectRendereIndexPiplineVec2.size();
			objectRendereIndexPiplineVec2.emplace_back();
		}
		std::vector<ObjectRendereIndex>& objectRendereIndexPiplineVec = objectRendereIndexPiplineVec2.at(piplineIndex);
#else
		Memory::VectorData2D<ObjectRendereIndex>& objectRendereIndexPiplineVec2 = comp.objectRendereIndexPiplineVec2;
		if (objectRendereIndexPiplineVec2.SizeDX() <= piplineIndex)
		{
			piplineIndex = objectRendereIndexPiplineVec2.SizeDX();
			const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
			uint32_t count = meshSingleVec.size();
			if (objectRendereIndexPiplineVec2.SizeDY()==0)
				objectRendereIndexPiplineVec2.Resize2D(1 + piplineIndex, 1);
			else
				objectRendereIndexPiplineVec2.ResizeX(1+piplineIndex);
		}
		Memory::VectorData<ObjectRendereIndex> objectRendereIndexPiplineVec = objectRendereIndexPiplineVec2.At(piplineIndex);

#endif
		SubmitShadeMeshStaticObjectCurent(mesh, s_Storarage3D.InstencShaderDepth, model, entityID, objectRendereIndexPiplineVec);
#else
		RY_CORE_NOT_IMPL();
#endif
	}

	void Renderer3D::MeshCompontDirekt(const glm::mat4& model, ModelMangerComponent& comp, int entityID)
	{
		const Ref<MeshStatic>& mesh = comp.meshStatic;
		if (nullptr == mesh)
			return;

		SubmitShapeMeshStaticObjectDirekt(mesh, s_Storarage3D.InstencShaderShape, model, entityID);

	}

	void Renderer3D::MeshCompontSetData(const glm::mat4& model, ModelMangerComponent& comp, int entityID)
	{
		RY_CORE_NOT_IMPL();
	}

	void Renderer3D::MeshCompont(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		SingleMeshObject singleMeshObject;
		singleMeshObject._Material = comp.material;
		singleMeshObject._MeshSingle = comp.meshSingle;

		SubmitShadeMeshObject(singleMeshObject, s_Storarage3D.InstencShaderShade, model
			, entityID, comp.objectRendereIndexPiplineVec);
	}

	void Renderer3D::MeshCompontMain(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
#ifdef RY_RENERER_DESIGN_CURENT_MAIN
		const Ref<MeshSingle>& mesh = comp.meshSingle;
		if (nullptr == mesh)
			return;
		constexpr uint32_t piplineIndex = 0u;
		std::vector<ObjectRendereIndex>& objectRendereIndexPiplineVec = comp.objectRendereIndexPiplineVec;
		if (objectRendereIndexPiplineVec.size() <= piplineIndex)
			objectRendereIndexPiplineVec.emplace_back();

		SingleMeshObject singleMeshObject;
		singleMeshObject._Material = comp.material;
		singleMeshObject._MeshSingle = comp.meshSingle;

		ObjectRendereIndex& objectRendereIndexPipline = objectRendereIndexPiplineVec.at(piplineIndex);
		SubmitShadeMeshObjectMain(singleMeshObject, s_Storarage3D.InstencShaderDepth, model, entityID, objectRendereIndexPipline);
#else
		RY_CORE_NOT_IMPL();
#endif
	}

	void Renderer3D::MeshCompontCurent(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
#ifdef RY_RENERER_DESIGN_CURENT_MAIN
		const Ref<MeshSingle>& mesh = comp.meshSingle;
		if (nullptr == mesh)
			return;
		uint32_t piplineIndex = Renderer::GetCurentIndex();
		std::vector<ObjectRendereIndex>& objectRendereIndexPiplineVec = comp.objectRendereIndexPiplineVec;
		if (objectRendereIndexPiplineVec.size() <= piplineIndex)
			objectRendereIndexPiplineVec.emplace_back();

		SingleMeshObject singleMeshObject;
		singleMeshObject._Material = comp.material;
		singleMeshObject._MeshSingle = comp.meshSingle;
		ObjectRendereIndex& objectRendereIndexPipline = objectRendereIndexPiplineVec.at(piplineIndex);
		SubmitShadeMeshObjectCurent(singleMeshObject, s_Storarage3D.InstencShaderDepth, model, entityID, objectRendereIndexPipline);
#else
		RY_CORE_NOT_IMPL();
#endif

	}

	void Renderer3D::MeshCompontDirekt(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		SubmitShapeMeshObjectDirekt(SingleMeshObject{ comp.material, comp.meshSingle }, s_Storarage3D.InstencShaderShape, model, entityID);
	}

	void Renderer3D::MeshCompontSetData(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		RY_CORE_NOT_IMPL();
	}








	void Renderer3D::SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<std::vector<ObjectRendereIndex>>& objectRendereVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		uint32_t count = meshSingleVec.size();
#ifdef RY_RENDER_3D_CACHING
		if (objectRendereVec.size() < count)
#endif
		{
			objectRendereVec.resize(count);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {} Reset Caching", entityID);
#endif

		}
		



		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			std::vector<ObjectRendereIndex>& objectRendere = objectRendereVec.at(i);
			glm::mat4 modelMatrix = model * meshSingle.LocaleCildrenMatrix;

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
			std::vector<ObjectRendereIndex> copyObjectRendereIndexVec = objectRendere;
#endif


#if RY_HASH_GROUPING_OPTIMZE
			SubmitMeshObjectToHash(meshSingle._MeshSingle, meshSingle._Material, modelMatrix, entityID, objectRendere);
#else
			SubmitShadeMeshObject(meshSingle, shader, modelMatrix, entityID, objectRendere);
#endif

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
			uint32_t countNow = objectRendere.size();
			uint32_t countBevor = copyObjectRendereIndexVec.size();

			uint32_t count = countNow < countBevor ? countNow : countBevor;
			bool fistTimeChanged = true;
			for (uint32_t j = 0; j < count; j++)
			{
				const ObjectRendereIndex& curentObjectRendereIndex = objectRendere.at(j);
				const ObjectRendereIndex& beforObjectRendereIndex = copyObjectRendereIndexVec.at(j);

				if (curentObjectRendereIndex != beforObjectRendereIndex)
				{
					if(fistTimeChanged)
					{
						RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, j, meshSingle.NodeName);
						fistTimeChanged = false;
					}
					RY_CORE_FATAL(
						"Changes on ObjectRendereIndex bevor (Batch: {}, Pipline: {}) / After (Batch: {}, Pipline: {})"
						, beforObjectRendereIndex.BatchIndex, beforObjectRendereIndex.PiplineIndex
						, curentObjectRendereIndex.BatchIndex, curentObjectRendereIndex.PiplineIndex
					);
				}
			}

#endif // RY_RENDER_3D_SEARCH_FOR_CHANGE

			i++;
		}
	}

	void Renderer3D::SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData2D<ObjectRendereIndex>& objectRendereVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		uint32_t count = meshSingleVec.size();
#ifdef RY_RENDER_3D_CACHING
		if (objectRendereVec.SizeDY() < count)
#endif
		{
			objectRendereVec.ResizeY(count );

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {} Reset Caching", entityID);
#endif

		}




		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			Memory::VectorData<ObjectRendereIndex> objectRendere = objectRendereVec.At(i);
			glm::mat4 modelMatrix = model * meshSingle.LocaleCildrenMatrix;

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
			std::vector<ObjectRendereIndex> copyObjectRendereIndexVec = objectRendere;
#endif


#if RY_HASH_GROUPING_OPTIMZE
			SubmitMeshObjectToHash(meshSingle._MeshSingle, meshSingle._Material, modelMatrix, entityID, objectRendere);
#else
			SubmitShadeMeshObject(meshSingle, shader, modelMatrix, entityID, objectRendere);
#endif

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif
			i++;
		}
	}

	void Renderer3D::SubmitShadeMeshStaticObjectMain(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRendereVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		uint32_t count = meshSingleVec.size();
#ifdef RY_RENDER_3D_CACHING
		if (objectRendereVec.size() < count)
#endif
		{
			objectRendereVec.resize(count);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {} Reset Caching", entityID);
#endif

		}




		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			ObjectRendereIndex& objectRendere = objectRendereVec.at(i);
			glm::mat4 modelMatrix = model * meshSingle.LocaleCildrenMatrix;

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
			std::vector<ObjectRendereIndex> copyObjectRendereIndexVec = objectRendere;
#endif


#if RY_HASH_GROUPING_OPTIMZE
			SubmitMeshObjectToHash(meshSingle._MeshSingle, meshSingle._Material, modelMatrix, entityID, objectRendere);
#else
			SubmitShadeMeshObjectMain(meshSingle, shader, modelMatrix, entityID, objectRendere);
#endif

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
			uint32_t countNow = objectRendere.size();
			uint32_t countBevor = copyObjectRendereIndexVec.size();

			uint32_t count = countNow < countBevor ? countNow : countBevor;
			bool fistTimeChanged = true;
			for (uint32_t j = 0; j < count; j++)
			{
				const ObjectRendereIndex& curentObjectRendereIndex = objectRendere.at(j);
				const ObjectRendereIndex& beforObjectRendereIndex = copyObjectRendereIndexVec.at(j);

				if (curentObjectRendereIndex != beforObjectRendereIndex)
				{
					if (fistTimeChanged)
					{
						RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, j, meshSingle.NodeName);
						fistTimeChanged = false;
					}
					RY_CORE_FATAL(
						"Changes on ObjectRendereIndex bevor (Batch: {}, Pipline: {}) / After (Batch: {}, Pipline: {})"
						, beforObjectRendereIndex.BatchIndex, beforObjectRendereIndex.PiplineIndex
						, curentObjectRendereIndex.BatchIndex, curentObjectRendereIndex.PiplineIndex
					);
				}
			}

#endif // RY_RENDER_3D_SEARCH_FOR_CHANGE

			i++;
		}
	}

	void Renderer3D::SubmitShadeMeshStaticObjectMain(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRendereIndex>& objectRendereVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		uint32_t count = meshSingleVec.size();
#ifdef RY_RENDER_3D_CACHING
		if (objectRendereVec.Size() < count)
#endif
		{
			objectRendereVec.Resize2D(count);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {} Reset Caching", entityID);
#endif

		}




		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			ObjectRendereIndex& objectRendere = objectRendereVec.At(i);
			glm::mat4 modelMatrix = model * meshSingle.LocaleCildrenMatrix;

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
			std::vector<ObjectRendereIndex> copyObjectRendereIndexVec = objectRendere;
#endif


#if RY_HASH_GROUPING_OPTIMZE
			SubmitMeshObjectToHash(meshSingle._MeshSingle, meshSingle._Material, modelMatrix, entityID, objectRendere);
#else
			SubmitShadeMeshObjectMain(meshSingle, shader, modelMatrix, entityID, objectRendere);
#endif

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
			uint32_t countNow = objectRendere.size();
			uint32_t countBevor = copyObjectRendereIndexVec.size();

			uint32_t count = countNow < countBevor ? countNow : countBevor;
			bool fistTimeChanged = true;
			for (uint32_t j = 0; j < count; j++)
			{
				const ObjectRendereIndex& curentObjectRendereIndex = objectRendere.at(j);
				const ObjectRendereIndex& beforObjectRendereIndex = copyObjectRendereIndexVec.at(j);

				if (curentObjectRendereIndex != beforObjectRendereIndex)
				{
					if (fistTimeChanged)
					{
						RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, j, meshSingle.NodeName);
						fistTimeChanged = false;
					}
					RY_CORE_FATAL(
						"Changes on ObjectRendereIndex bevor (Batch: {}, Pipline: {}) / After (Batch: {}, Pipline: {})"
						, beforObjectRendereIndex.BatchIndex, beforObjectRendereIndex.PiplineIndex
						, curentObjectRendereIndex.BatchIndex, curentObjectRendereIndex.PiplineIndex
					);
				}
			}

#endif // RY_RENDER_3D_SEARCH_FOR_CHANGE

			i++;
		}

	}

	void Renderer3D::SubmitShadeMeshStaticObjectCurent(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID,std::vector<ObjectRendereIndex>& objectRendereVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		uint32_t count = meshSingleVec.size();
#ifdef RY_RENDER_3D_CACHING
		if (objectRendereVec.size() < count)
#endif
		{
			objectRendereVec.resize(count);

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {} Reset Caching", entityID);
#endif

		}




		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			ObjectRendereIndex& objectRendere = objectRendereVec.at(i);
			glm::mat4 modelMatrix = model * meshSingle.LocaleCildrenMatrix;

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
			std::vector<ObjectRendereIndex> copyObjectRendereIndexVec = objectRendere;
#endif


#if RY_HASH_GROUPING_OPTIMZE
			SubmitMeshObjectToHash(meshSingle._MeshSingle, meshSingle._Material, modelMatrix, entityID, objectRendere);
#else
			SubmitShadeMeshObjectCurent(meshSingle, shader, modelMatrix, entityID, objectRendere);
			
#endif

#ifdef RY_RENDER_3D_PRINT
			RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, i, meshSingle.NodeName);
#endif

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
			uint32_t countNow = objectRendere.size();
			uint32_t countBevor = copyObjectRendereIndexVec.size();

			uint32_t count = countNow < countBevor ? countNow : countBevor;
			bool fistTimeChanged = true;
			for (uint32_t j = 0; j < count; j++)
			{
				const ObjectRendereIndex& curentObjectRendereIndex = objectRendere.at(j);
				const ObjectRendereIndex& beforObjectRendereIndex = copyObjectRendereIndexVec.at(j);

				if (curentObjectRendereIndex != beforObjectRendereIndex)
				{
					if (fistTimeChanged)
					{
						RY_CORE_TRACE("Render3D: Caching Index Single Mesh Entity {}, Mesh(Index: {}, Name {})", entityID, j, meshSingle.NodeName);
						fistTimeChanged = false;
					}
					RY_CORE_FATAL(
						"Changes on ObjectRendereIndex bevor (Batch: {}, Pipline: {}) / After (Batch: {}, Pipline: {})"
						, beforObjectRendereIndex.BatchIndex, beforObjectRendereIndex.PiplineIndex
						, curentObjectRendereIndex.BatchIndex, curentObjectRendereIndex.PiplineIndex
					);
				}
			}

#endif // RY_RENDER_3D_SEARCH_FOR_CHANGE

			i++;
		}
	}

	void Renderer3D::SubmitShadeMeshStaticObjectCurent(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRendereIndex>& objectRendereVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		uint32_t count = meshSingleVec.size();

		if (objectRendereVec.Size() < count)
		{
			objectRendereVec.Resize2D(count);
		}

		objectRendereVec.ValueBegin();
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			ObjectRendereIndex& objectRendere = objectRendereVec.Get();
			glm::mat4 modelMatrix = model * meshSingle.LocaleCildrenMatrix;

			SubmitShadeMeshObjectCurent(meshSingle, shader, modelMatrix, entityID, objectRendere);
			objectRendereVec.Increase();
		}

	}

	void Renderer3D::SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<SingleMeshRender>& singleMeshRendereVec)
	{


		if (singleMeshRendereVec.empty())
		{
			const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
			uint32_t count = meshSingleVec.size();

			singleMeshRendereVec.reserve(count);
			uint32_t i = 0u;
			for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
			{
				const glm::mat4& localeMatrix = meshSingle.LocaleCildrenMatrix;
				glm::mat4 modelMatrix = model * localeMatrix;
				const Ref<MeshSingle>& singleMesh = meshSingle._MeshSingle;
				const Ref<Material>& material = meshSingle._Material;
				SingleMeshRender& single = singleMeshRendereVec.emplace_back<SingleMeshRender>(
					SingleMeshRender{ singleMesh, material, localeMatrix }
				);

				std::vector<ObjectRendereIndex>& renderIndex = single.IndexVec;
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
				


#if RY_HASH_GROUPING_OPTIMZE
				SubmitMeshObjectToHash(single._MeshSingle, single._Material, modelMatrix, entityID, renderIndex);
#else
				SubmitShadeMeshObject(single, shader, modelMatrix, entityID, renderIndex);
#endif

#ifdef RY_RENDER_3D_PRINT
				std::string indexVecString = "";
				for (const ObjectRendereIndex& index : renderIndex)
				{
					std::string batchIndexString = 
						"Batching: " +std::to_string(index.BatchIndex)
						+ "Pipline: " + std::to_string(index.PiplineIndex);
					indexVecString += batchIndexString;
				}
				RY_CORE_TRACE("Render3D: Last Mesh {}, used in {}", entityID, i, meshSingle.NodeName, indexVecString);
#endif
				i++;
			}
		}
		else
		{
			uint32_t i = 0u;
			for (SingleMeshRender& single : singleMeshRendereVec)
			{
				glm::mat4 modelMatrix = model * single.LocaleCildrenMatrix;
				std::vector<ObjectRendereIndex>& renderIndex = single.IndexVec;
#if RY_HASH_GROUPING_OPTIMZE
				const Ref<MeshSingle>& singleMesh = single._MeshSingle;
				const Ref<Material>& material = single._Material;
				SubmitMeshObjectToHash(singleMesh, material, modelMatrix, entityID, objectRendere);


#else

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
				std::vector<ObjectRendereIndex> copyObjectRendereIndexVec = renderIndex;
#endif

				SubmitShadeMeshObject(single, shader, modelMatrix, entityID, renderIndex);

#ifdef RY_RENDER_3D_PRINT
				std::string indexVecString = "";
				for (const ObjectRendereIndex& index : renderIndex)
				{
					std::string batchIndexString =
						"Batching: " + std::to_string(index.BatchIndex)
						+ "Pipline: " + std::to_string(index.PiplineIndex);
					indexVecString += batchIndexString;
				}
				RY_CORE_TRACE("Render3D: Enity Submite Mesh {}, used in {}", entityID, i, indexVecString);

#endif  // RY_RENDER_3D_PRINT

#ifdef RY_RENDER_3D_SEARCH_FOR_CHANGE
				uint32_t i = 0;
				uint32_t countNow = renderIndex.size();
				uint32_t countBevor = copyObjectRendereIndexVec.size();

				uint32_t count = countNow < countBevor ? countNow : countBevor;

				for (uint32_t i = 0; i < count; i++)
				{
					const ObjectRendereIndex& curentObjectRendereIndex = renderIndex.at(i);
					const ObjectRendereIndex& beforObjectRendereIndex = copyObjectRendereIndexVec.at(i);

					if (curentObjectRendereIndex != beforObjectRendereIndex)
					{
						RY_CORE_FATAL(
							"Changes on ObjectRendereIndex bevor (Batch: {}, Pipline:{}) / After (Batch: {}, Pipline:{})"
							, beforObjectRendereIndex.BatchIndex, beforObjectRendereIndex.PiplineIndex
							, curentObjectRendereIndex.BatchIndex, curentObjectRendereIndex.PiplineIndex
						);

					}
				}

#endif // RY_RENDER_3D_SEARCH_FOR_CHANGE

#endif
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




	void Renderer3D::SubmitShapeMeshStaticObjectDirekt(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			glm::mat4 modelMatrix = model * meshSingle.LocaleCildrenMatrix;

			SubmitShapeMeshObjectDirekt(meshSingle, shader, modelMatrix, entityID);

		}
	}

	void Renderer3D::SubmitMeshObjectToHash(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRendereVec)
	{
#if !RY_STATIC_OPTIMZE
		std::vector<RenderEnitityObject>& vec = s_Storarage3D.RenderEntityFrame[meshSingle];
		vec.emplace_back(RenderEnitityObject{ model, entityID, material, &objectRendereVec });
#endif
	}

	

	void Renderer3D::SubmitHashMeshesToPipline()
	{
		for (auto& [meshSingle, vec] : s_Storarage3D.RenderEntityFrame)
		{
			for (RenderEnitityObject& e : vec)
			{
#if RY_STATIC_OPTIMZE
				ObjectRendereIndexStaticArray& objectRendere = *e.ObjectRendereIndexArray;
#else
				std::vector<ObjectRendereIndex>& objectRendere = *e.ObjectRendereIndexVec;
#endif
				SubmitMeshObjectToPipline(meshSingle, e.MaterielRef, e.Matrix, e.EntityID, objectRendere);
			}
		}
	}

	void Renderer3D::SubmitMeshObjectToPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRendere)
	{
		SingleMeshObject singleMesh{ material, meshSingle };
		SubmitShadeMeshObject(singleMesh, s_Storarage3D.InstencShaderShade, model, entityID, objectRendere);
	}

	

	
	void Renderer3D::SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, Memory::VectorData<ObjectRendereIndex>& objectRenderePiplineVec)
	{
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
#if !RY_STATIC_SCREEN_DRAW


		CamerRenderPackages& cameraPackege = Renderer::GetMainCamerRenderPackagesRef();


#if 1
		uint32_t piplineIndex = 0;
		if (objectRenderePiplineVec.size() <= piplineIndex)
			// if (piplineIndex <= objectRenderePiplineVec.size())
			objectRenderePiplineVec.emplace_back<ObjectRendereIndex>(ObjectRendereIndex{});

		Ref<PiplineRenderBase> piplineMain = SubmitMeshObjectToRenderTargetShade(cameraPackege, singleMesh, shader, model, entityID, objectRenderePiplineVec.at(piplineIndex));
		piplineIndex++;
#else	
		Ref<PiplineRenderBase> piplineMain = nullptr;
#endif

#if 1
		if (nullptr == piplineMain)
			return;

		ForEchViewPassType("Shadow"
			, [&](ViewPassStorage& viewPassShadow)
			{

				if (objectRenderePiplineVec.size() <= piplineIndex)
					// if (piplineIndex <= objectRenderePiplineVec.size())
					objectRenderePiplineVec.emplace_back<ObjectRendereIndex>(ObjectRendereIndex{});
#if RY_RENDER_PIPLINE_INSTANCE_SHADOW
				Ref<PiplineRenderBase> piplineShadow = SubmitMeshObjectToRenderTargetDepth(viewPassShadow.CameraPackege, singleMesh, s_Storarage3D.InstencShaderDepth, model, entityID, objectRenderePiplineVec.at(piplineIndex));
#else
				Ref<PiplineRenderBase> piplineShadow = SubmitMeshObjectToRenderTargetDepth(viewPassShadow.CameraPackege, singleMesh, s_Storarage3D.SingleShaderDepth, model, entityID, objectRenderePiplineVec.at(piplineIndex));
#endif
				if (nullptr != piplineMain)
				{
					piplineMain->SubmitRenderTargetResurces(viewPassShadow);
				}
				piplineIndex++;
			}
		);

		if (nullptr != piplineMain && piplineIndex == 1)
		{
			Ref<Texture> tex = Texture::White();
			piplineMain->SubmitRenderTargetResurcesReadImg(tex);
			CamerRenderPackages::CamerPackage& packeg = cameraPackege.GetCamerPackage();
			Ref<UniformBuffer> ub = packeg.GetBuffer();

			piplineMain->SubmitRenderTargetResurcesReadUB(ub);
		}
#else
		if (piplineMain)
		{
			Ref<Texture> tex = Texture::White();
			CamerRenderPackages::DisplayPackage& displayPackage = cameraPackege.GetDisplayPackage();
			Ref<UniformBuffer> ub = displayPackage.GetBuffer();
			piplineMain->SubmitRenderTargetResurcesReadImg(tex);
			piplineMain->SubmitRenderTargetResurcesReadUB(ub);

		}
#endif
#endif
#else
		uint32_t piplineIndex = 0;
		if (objectRenderePiplineVec.Size() <= piplineIndex)
			objectRenderePiplineVec.Push();

		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetMain(singleMesh, shader, model, entityID, objectRenderePiplineVec.At(0));
		
		Ref<Texture> tex = Texture::White();
		Ref<UniformBuffer> ub = Renderer::GetPackegeCamerUniformMain();
		pipline->SubmitRenderTargetResurcesReadImg(tex);
		pipline->SubmitRenderTargetResurcesReadUB(ub);

		RY_REMBER_FUNC_CHANGE("");
#endif
	}

	void Renderer3D::SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRenderePiplineVec)
	{
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
#if !RY_STATIC_SCREEN_DRAW


		CamerRenderPackages& cameraPackege = Renderer::GetMainCamerRenderPackagesRef();


#if 1
		uint32_t piplineIndex = 0;
		if (objectRenderePiplineVec.size() <= piplineIndex)
			// if (piplineIndex <= objectRenderePiplineVec.size())
			objectRenderePiplineVec.emplace_back<ObjectRendereIndex>(ObjectRendereIndex{});

		Ref<PiplineRenderBase> piplineMain = SubmitMeshObjectToRenderTargetShade(cameraPackege, singleMesh, shader, model, entityID, objectRenderePiplineVec.at(piplineIndex));
		piplineIndex++;
#else	
		Ref<PiplineRenderBase> piplineMain = nullptr;
#endif

#if 1
		if (nullptr == piplineMain)
			return;

		ForEchViewPassType("Shadow"
			, [&](ViewPassStorage& viewPassShadow)
			{

				if (objectRenderePiplineVec.size() <= piplineIndex)
					// if (piplineIndex <= objectRenderePiplineVec.size())
					objectRenderePiplineVec.emplace_back<ObjectRendereIndex>(ObjectRendereIndex{});
#if RY_RENDER_PIPLINE_INSTANCE_SHADOW
				Ref<PiplineRenderBase> piplineShadow = SubmitMeshObjectToRenderTargetDepth(viewPassShadow.CameraPackege, singleMesh, s_Storarage3D.InstencShaderDepth, model, entityID, objectRenderePiplineVec.at(piplineIndex));
#else
				Ref<PiplineRenderBase> piplineShadow = SubmitMeshObjectToRenderTargetDepth(viewPassShadow.CameraPackege, singleMesh, s_Storarage3D.SingleShaderDepth, model, entityID, objectRenderePiplineVec.at(piplineIndex));
#endif
				if (nullptr != piplineMain)
				{
					piplineMain->SubmitRenderTargetResurces(viewPassShadow);
				}
				piplineIndex++;
			}
		);

		if (nullptr != piplineMain && piplineIndex == 1)
		{
			Ref<Texture> tex = Texture::White();
			piplineMain->SubmitRenderTargetResurcesReadImg(tex);
			CamerRenderPackages::CamerPackage& packeg = cameraPackege.GetCamerPackage();
			Ref<UniformBuffer> ub = packeg.GetBuffer();

			piplineMain->SubmitRenderTargetResurcesReadUB(ub);
		}
#else
		if (piplineMain)
		{
			Ref<Texture> tex = Texture::White();
			CamerRenderPackages::DisplayPackage& displayPackage = cameraPackege.GetDisplayPackage();
			Ref<UniformBuffer> ub = displayPackage.GetBuffer();
			piplineMain->SubmitRenderTargetResurcesReadImg(tex);
			piplineMain->SubmitRenderTargetResurcesReadUB(ub);

		}
#endif
#endif
#else
		uint32_t piplineIndex = 0;
		if (objectRenderePiplineVec.size() <= piplineIndex)
			objectRenderePiplineVec.emplace_back<ObjectRendereIndex>(ObjectRendereIndex{});

		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetMain(singleMesh, shader, model, entityID, objectRenderePiplineVec.at(0));

		Ref<Texture> tex = Texture::White();
		Ref<UniformBuffer> ub = Renderer::GetPackegeCamerUniformMain();
		pipline->SubmitRenderTargetResurcesReadImg(tex);
		pipline->SubmitRenderTargetResurcesReadUB(ub);

		RY_REMBER_FUNC_CHANGE("");
#endif
	}


	void Renderer3D::SubmitShadeMeshObjectMain(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRenderePipline)
	{
#ifdef RY_RENERER_DESIGN_CURENT_MAIN		

		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetMain(singleMesh, shader, model, entityID, objectRenderePipline);
		
#if 1
		uint32_t count = 0;
		RY_REMBER_FUNC_CHANGE("Move the folwing logic somewhere, to don't call per entity as per Pipline!");
		Renderer::ForEchStoredPassedRenderPass(
			[pipline, &countRef = count](const RenderPass& renderPass)
			{
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

				pipline->SubmitRenderTargetResurcesReadImg(tex);
				pipline->SubmitRenderTargetResurcesReadUB(ub);
				countRef++;
			}
		);
		if (count == 0)
		{
			Ref<Texture> tex = Texture::White();
			Ref<UniformBuffer>& ub = Renderer::GetPackegeCamerUniformMain();

			pipline->SubmitRenderTargetResurcesReadImg(tex);
			pipline->SubmitRenderTargetResurcesReadUB(ub);

		}
#elif 1
		RY_REMBER_FUNC_CHANGE("The folwing code need to remove!");
		Ref<Texture> tex = Texture::White();
		Ref<UniformBuffer>& ub = Renderer::GetPackegeCamerUniformMain();

		pipline->SubmitRenderTargetResurcesReadImg(tex);
		pipline->SubmitRenderTargetResurcesReadUB(ub);

#endif
		

		RY_REMBER_FUNC_CHANGE();
#endif
	}

	void Renderer3D::SubmitShadeMeshObjectCurent(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRenderePipline)
	{
#ifdef RY_RENERER_DESIGN_CURENT_MAIN
		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetCurent(singleMesh, shader, model, entityID, objectRenderePipline);

		

		RY_REMBER_FUNC_CHANGE();
#endif
	}



	

	void Renderer3D::SubmitShapeMeshObjectDirekt(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID)
	{
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
		CamerRenderPackages& cameraPackege = Renderer::GetMainCamerRenderPackagesRef();
		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetShape(cameraPackege, singleMesh, shader, model, entityID);
#else
		Ref<PiplineRenderBase> pipline = SubmitMeshObjectToRenderTargetCurentDirekt(singleMesh, shader, model, entityID);
#endif
	}
	
#ifdef RY_RENERER_DESIGN_CURENT_MAIN
	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTargetMain(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere)
	{
		Ref<PiplineRenderBase> pipline = nullptr;
#if 0
#if 1
		const CameraPackege& cameraData = Renderer::GetCameraPackegeMain();
		const glm::mat4& viewProjtion = cameraData.ViewProjectionMatrix;
		const Ref<MeshSingle>& singleMesh = meshObject._MeshSingle;

		if (!singleMesh->IsViewFustrum(model, viewProjtion))
		{
			objectRendere.Reset();
			return pipline;
		}

#else

		const Ref<MeshSingle>& singleMesh = meshObject._MeshSingle;
		const AABB& aabb = singleMesh->GetAABB();
		if (!Renderer::IsInsideCurentViewFustrem(aabb, model))
		{
			objectRendere.Reset();
			return pipline;
		}
#endif

#endif


#if 0
		PiplineRefBaseVec& piplineBaseVec = s_Storarage3D.InstencPiplineBaseVec;

#elif 1
		PiplineRefBaseVec& piplineBaseVec = Renderer::GetRenderPiplinesMain();
#else		
		Ref<RenderTarget>& target = Renderer::GetRenderTargetMain();
		PiplineRefBaseVec& piplineBaseVec = target->GetPiplineBaseVec();
#endif

		uint32_t& pilineIndex = objectRendere.PiplineIndex;
		uint32_t& instenceIndex = objectRendere.BatchIndex;

		PiplineResultState result = PiplineResultState::Result_None;
		int checkResult = (result & PiplineResultState::Result_Success);
		const uint32_t countPiplines = piplineBaseVec.GetIndexSize();


		if (pilineIndex <= countPiplines)
		{
#if 1
			pipline = piplineBaseVec.GetPiplineType<InstenceMeshPiplineRenderShade>(pilineIndex);
			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);
#else

			pipline = piplineBaseVec.GetPiplineTypeRef<InstenceMeshPiplineRenderShade>(pilineIndex);

			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			pipline->GetFrameCountNotUpdate();
			checkResult = (result & PiplineResultState::Result_Success);
#endif
		}

		uint32_t i = 0;
		while (checkResult == 0 && i <= countPiplines)
		{
#if 1
			pipline = piplineBaseVec.GetPiplineType<InstenceMeshPiplineRenderShade>(i);
			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);
#else
			Ref<PiplineRenderBase>& piplineRef = piplineBaseVec.GetPiplineTypeRef<InstenceMeshPiplineRenderShade>(i);
			if (piplineRef->IsToLongNotUpdated())
			{
				RY_DESTROY_REF(piplineRef);
				piplineRef = CreateRef<InstenceMeshPiplineRenderShade>();
			}

			result = piplineRef->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);
			pipline = piplineRef;
#endif

			i++;
		}
		if (i > 0)
			pilineIndex = i - 1;
		RY_CORE_ASSERT(checkResult != 0, "we dident find a Pipline or/and even a new did not work!");


		Ref<UniformBuffer>& camerPackedUB = Renderer::GetPackegeCamerUniformMain();
		pipline->SetCameraUniformBuffer(camerPackedUB);


		return pipline;
	}

	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTargetCurent(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere)
	{
		Ref<PiplineRenderBase> pipline = nullptr;
#if 0
#if 0
		const CameraPackege& cameraData = Renderer::GetCameraPackegeCurent();
		const glm::mat4& viewProjtion = cameraData.ViewProjectionMatrix;
		const Ref<MeshSingle>& singleMesh = meshObject._MeshSingle;			
		
		uint32_t& pilineIndex = objectRendere.PiplineIndex;

		if (!singleMesh->IsViewFustrum(model, viewProjtion))
		{		
			pilineIndex = 0xFFFFFFFFui32;
			return pipline;
		}

		Ref<RenderTarget>& target = Renderer::GetRenderTargetCurent();
		uint32_t& instenceIndex = objectRendere.BatchIndex;
#else

		const Ref<MeshSingle>& singleMesh = meshObject._MeshSingle;
		const AABB& aabb = singleMesh->GetAABB();
		if (!Renderer::IsInsideCurentViewFustrem(aabb, model))
		{
			objectRendere.Reset();
			return pipline;
		}

#endif

#endif

#if 0
		PiplineRefBaseVec& piplineBaseVec = s_Storarage3D.InstencPiplineBaseVec;
#else
		PiplineRefBaseVec& piplineBaseVec = Renderer::GetRenderPiplinesCurent();
#endif


		PiplineResultState result = PiplineResultState::Result_None;
		int checkResult = (result & PiplineResultState::Result_Success);
		const uint32_t countPiplines = piplineBaseVec.GetIndexSize();

		uint32_t& pilineIndex = objectRendere.PiplineIndex;
		uint32_t& instenceIndex = objectRendere.BatchIndex;

		if (pilineIndex <= countPiplines)
		{
#if 0
			pipline = piplineBaseVec.GetPiplineType<InstenceMeshPiplineRenderDepth>(pilineIndex, target);

			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			pipline->GetFrameCountNotUpdate();
			checkResult = (result & PiplineResultState::Result_Success);
#else
			pipline = piplineBaseVec.GetPiplineType<InstenceMeshPiplineRenderDepth>(pilineIndex);

			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			pipline->GetFrameCountNotUpdate();
			checkResult = (result & PiplineResultState::Result_Success);
#endif
		}

		uint32_t i = 0;
		while (checkResult == 0 && i <= countPiplines)
		{
#if 0
#if 1
			pipline = piplineBaseVec.GetPiplineType<InstenceMeshPiplineRenderDepth>(i, target);	
			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);
#else
			Ref<PiplineRenderBase>& piplineRef = target->GetPiplineIndexOrPush(i);
			if (nullptr == piplineRef)
				piplineRef = CreateRef<InstenceMeshPiplineRenderDepth>();
			result = piplineRef->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			pipline = piplineRef;
#endif
		
#else
			Ref<PiplineRenderBase> piplineRef = piplineBaseVec.GetPiplineType<InstenceMeshPiplineRenderDepth>(i);
			if (nullptr == piplineRef)
				piplineRef = CreateRef<InstenceMeshPiplineRenderDepth>();
			result = piplineRef->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);
			pipline = piplineRef;

#endif
			i++;
		}
		if (i > 0)
			pilineIndex = i - 1;
		RY_CORE_ASSERT(checkResult != 0, "we dident find a Pipline or/and even a new did not work!");


		Ref<UniformBuffer>& camerPackedUB = Renderer::GetPackegeCamerUniformCurent();
		pipline->SetCameraUniformBuffer(camerPackedUB);


		return pipline;
	}

	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTargetCurentDirekt(const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID)
	{
		Ref<PiplineRenderBase> pipline = nullptr;
		Ref<RenderTarget>& target = Renderer::GetRenderTargetCurent();
		PiplineRefBaseVec& piplineBaseVec = s_Storarage3D.InstencPiplineBaseVec;

		PiplineResultState result = PiplineResultState::Result_None;
		int checkResult = (result & PiplineResultState::Result_Success);
		const uint32_t countPiplines = piplineBaseVec.GetIndexSize();


		uint32_t instenceIndex = 0xFFFFFFFFu;
		uint32_t i = 0;
		while (checkResult == 0 && i <= countPiplines)
		{
			pipline = piplineBaseVec.GetPiplineType<InstenceMeshPiplineRenderShape>(i, target);
			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);
			i++;
		}
	
		RY_CORE_ASSERT(checkResult != 0, "we dident find a Pipline or/and even a new did not work!");
		Ref<UniformBuffer>& camerPackedUB = Renderer::GetPackegeCamerUniformCurent();
		pipline->SetCameraUniformBuffer(camerPackedUB);


		return pipline;
	}


#else

	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTargetShade(CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere)
	{
#if 0
#if !RY_STATIC_SCREEN_DRAW

#if RY_PIPLINE_VEC_CLASS_DISABLE
		Ref<SingleMeshPiplineRenderShade> pipline = nullptr;
		uint32_t& index = s_Storarage3D.PiplineIndex;
		std::vector<Ref<SingleMeshPiplineRenderShade>>& pilineBaseVec = s_Storarage3D.PiplineRenderBaseVec;

		if (index < s_Storarage3D.PiplineRenderBaseVec.size())
		{
			pipline = pilineBaseVec.at(index);
		}
		else
		{
			pipline = CreateRef<SingleMeshPiplineRenderShade>();
			pilineBaseVec.emplace_back(pipline);
		}
#else
		Ref<PiplineRenderBase> pipline = Ref<PiplineRenderBase>(nullptr);
		CamerRenderPackages::CamerPackage& packeg = cameraPackege.GetCamerPackage();
#if 1
		const CameraData& cameraData = packeg.GetData();
		const glm::mat4& viewProjtion = cameraData.ViewProjectionMatrix;
		const Ref<MeshSingle>& singleMesh = meshObject._MeshSingle;
		if (!singleMesh->IsViewFustrum(model, viewProjtion))
		{
			return pipline;
		}
#endif

#if RY_ENABELE_LIST_STYSTEM
		PiplineRefVec<SingleMeshPiplineRenderShade>& pilineBaseVec = s_Storarage3D.PiplineShadeVec;
		PiplineResultState result = PiplineResultState::Result_None;
		int checkResult = (result & PiplineResultState::Result_Success);
		const uint32_t countPiplines = pilineBaseVec.GetPiplineCount();

		for (uint32_t i = 0; checkResult == 0 && i <= countPiplines; i++)
		{
			pipline = pilineBaseVec.GetPipline(i);
			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);

		}
		RY_CORE_ASSERT(checkResult != 0, "we dident find a Pipline or/and even a new did not work!");
#else

		pipline = s_Storarage3D.PiplineShadeVec.GetNextPipline();
		PiplineResultState result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
		int checkResult = (result & PiplineResultState::Result_Success);
		RY_CORE_ASSERT(checkResult != 0, "the new Pipline dident work!");


#endif

#endif
		RenderTarget& target = cameraPackege.GetRenderTarget();


		Ref<UniformBuffer> uniformBuffer = packeg.GetBuffer();

		pipline->SetCameraUniformBuffer(uniformBuffer);

		// PiplineRenderBase::PrintPlineResult(result);

		target.AddPipline(pipline);
#if RY_PIPLINE_VEC_CLASS_DISABLE
		index++;
#endif
		return pipline;
#else
		return Ref<PiplineRenderBase>(nullptr);
#endif
#else
		Ref<PiplineRenderBase> pipline = nullptr;

		CamerRenderPackages::CamerPackage& packeg = cameraPackege.GetCamerPackage();
		const CameraData& cameraData = packeg.GetData();
		const glm::mat4& viewProjtion = cameraData.ViewProjectionMatrix;


		const Ref<MeshSingle>& singleMesh = meshObject._MeshSingle;
		SingleMeshObject meshObjectCopy = meshObject;
#if RY_CHECK_FUSTREM_SHADE_ONLY
		if (!singleMesh->IsViewFustrum(model, viewProjtion))
		{
			meshObjectCopy._Material = Renderer3D::GetMaterilNotInFustrem();
			return nullptr;
			return pipline;
		}
#endif

#if RY_RENDER_PIPLINE_INSTANCE
		PiplineRefVec<InstenceMeshPiplineRenderShade>* pilineVecShadePtr = &s_Storarage3D.InstencPiplineShadeVec;
		PiplineBaseVec* pilineBaseVecPtr = reinterpret_cast<PiplineRefBaseVec*>(pilineVecShadePtr);

#if 1
		pipline = SubmitMeshObjectToRenderTarget(pilineBaseVecPtr, cameraPackege, meshObjectCopy, shader, model, entityID, objectRendere);
#endif
#elif !RY_STATIC_SCREEN_DRAW
		PiplineRefVec<SingleMeshPiplineRenderShade>* pilineVecShadePtr = &s_Storarage3D.PiplineShadeVec;
		PiplineRefBaseVec* pilineBaseVecPtr = reinterpret_cast<PiplineRefBaseVec*>(pilineVecShadePtr);

#if 1
		pipline = SubmitMeshObjectToRenderTarget(pilineBaseVecPtr, cameraPackege, meshObjectCopy, shader, model, entityID, objectRendere);
#endif
#endif

		return pipline;
#endif
	}

	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTargetDepth(CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere)
	{


		Ref<PiplineRenderBase> pipline = nullptr;
#if 0

		CamerRenderPackages::CamerPackage& packeg = cameraPackege.GetCamerPackage();
		const CameraData& cameraData = packeg.GetData();
		const glm::mat4& viewProjtion = cameraData.ViewProjectionMatrix;
		const Ref<MeshSingle>& singleMesh = meshObject._MeshSingle;
		if (!singleMesh->IsViewFustrum(model, viewProjtion))
		{

			return pipline;
		}
#endif
#if !RY_STATIC_SCREEN_DRAW
#if RY_RENDER_PIPLINE_INSTANCE_SHADOW
		PiplineRefVec<InstenceMeshPiplineRenderDepth>* pilineVecDepthPtr = &s_Storarage3D.InstencPiplineDepthVec;
#else
		PiplineRefVec<SingleMeshPiplineRenderDepth>* pilineVecDepthPtr = &s_Storarage3D.PiplineDepthVec;
#endif
		PiplineRefBaseVec* pilineBaseVecPtr = reinterpret_cast<PiplineRefBaseVec*>(pilineVecDepthPtr);


		pipline = SubmitMeshObjectToRenderTarget(pilineBaseVecPtr, cameraPackege, meshObject, shader, model, entityID, objectRendere);
#endif

		return pipline;

	}

	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTargetShape(CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID)
	{
		Ref<PiplineRenderBase> pipline = nullptr;

		CamerRenderPackages::CamerPackage& packeg = cameraPackege.GetCamerPackage();
		const CameraData& cameraData = packeg.GetData();
		const glm::mat4& viewProjtion = cameraData.ViewProjectionMatrix;


		const Ref<MeshSingle>& singleMesh = meshObject._MeshSingle;
		SingleMeshObject meshObjectCopy = meshObject;
#if RY_CHECK_FUSTREM_SHADE_ONLY
		if (!singleMesh->IsViewFustrum(model, viewProjtion))
		{
			meshObjectCopy._Material = Renderer3D::GetMaterilNotInFustrem();
			return nullptr;
			return pipline;
		}
#endif
		PiplineRefVec<InstenceMeshPiplineRenderShape>* pilineVecShadePtr = &s_Storarage3D.InstencPiplineShapeVec;
		PiplineRefBaseVec* pilineBaseVecPtr = reinterpret_cast<PiplineRefBaseVec*>(pilineVecShadePtr);

		pipline = SubmitMeshObjectToRenderTarget(pilineBaseVecPtr, cameraPackege, meshObject, shader, model, entityID, Rynex::ObjectRendereIndex());

		return pipline;
	}


	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTarget(PiplineRefBaseVec* piplineBaseVecPtr, CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere)
	{
#if 0
		return Ref<PiplineRenderBase>(nullptr);
#endif
#if !RY_STATIC_SCREEN_DRAW

#if RY_PIPLINE_VEC_CLASS_DISABLE
		Ref<SingleMeshPiplineRenderShade> pipline = nullptr;
		uint32_t& index = s_Storarage3D.PiplineIndex;
		std::vector<Ref<SingleMeshPiplineRenderShade>>& pilineBaseVec = s_Storarage3D.PiplineRenderBaseVec;

		if (index < s_Storarage3D.PiplineRenderBaseVec.size())
		{
			pipline = pilineBaseVec.at(index);
		}
		else
		{
			pipline = CreateRef<SingleMeshPiplineRenderShade>();
			pilineBaseVec.emplace_back(pipline);
		}
#else
		Ref<PiplineRenderBase> pipline = Ref<PiplineRenderBase>(nullptr);
		CamerRenderPackages::CamerPackage& packeg = cameraPackege.GetCamerPackage();
#if RY_CHECK_FUSTREM
		const CameraData& cameraData = packeg.GetData();
		const glm::mat4& viewProjtion = cameraData.ViewProjectionMatrix;
		const Ref<MeshSingle>& singleMesh = meshObject._MeshSingle;

		if (!singleMesh->IsViewFustrum(model, viewProjtion))
		{
			return pipline;
		}
#endif
		uint32_t& instenceIndex = objectRendere.BatchIndex;

#if RY_ENABELE_LIST_STYSTEM
		uint32_t& pilineIndex = objectRendere.PiplineIndex;
		PiplineResultState result = PiplineResultState::Result_None;
		int checkResult = (result & PiplineResultState::Result_Success);
		const uint32_t countPiplines = piplineBaseVecPtr->GetIndexSize();
		RenderTarget& target = cameraPackege.GetRenderTarget();

#if RY_ENABELE_LIST_STYSTEM_OPTIMIZE
		if (pilineIndex <= countPiplines)
		{
			pipline = piplineBaseVecPtr->GetPipline(pilineIndex, target);
			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);
		}

#endif
		uint32_t i = 0;
		while (checkResult == 0 && i <= countPiplines)
		{
			pipline = piplineBaseVecPtr->GetPipline(i, target);
			result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
			checkResult = (result & PiplineResultState::Result_Success);
			i++;
		}
		if (i > 0)
			pilineIndex = i - 1;
		RY_CORE_ASSERT(checkResult != 0, "we dident find a Pipline or/and even a new did not work!");
#else

		pipline = piplineBaseVecPtr->GetNextPipline();
		PiplineResultState result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
		int checkResult = (result & PiplineResultState::Result_Success);
		RY_CORE_ASSERT(checkResult != 0, "the new Pipline dident work!");
		RenderTarget& target = cameraPackege.GetRenderTarget();

#endif

#endif

		Ref<UniformBuffer> uniformBuffer = packeg.GetBuffer();

		pipline->SetCameraUniformBuffer(uniformBuffer);

		// PiplineRenderBase::PrintPlineResult(result);
#if !RY_ENABELE_LIST_STYSTEM
		const Ref<Material>& materiel = meshObject._Material;
		float alpha = materiel->GetAlpha();
		if (alpha == 1.0f)
		{

			target.AddPipline(pipline);
		}
		else
		{
			const CameraData& data = packeg.GetData();
			const glm::vec3& postionCamera = data.Position;
			glm::vec3 postionModel = model[3];
			glm::vec3 distenzVec = postionCamera - postionModel;
			float distenz = glm::length(distenzVec);
			target.AddPiplineAlpha(pipline, distenz);
		}
#endif
#if RY_PIPLINE_VEC_CLASS_DISABLE
		index++;
#endif
		return pipline;
#else
		return Ref<PiplineRenderBase>(nullptr);

#endif


		return Ref<PiplineRenderBase>();
	}
#endif


	void Renderer3D::PrepairMainScene()
	{
		RY_CORE_NOT_IMPL();
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer!");
	}



	void Renderer3D::ClearMeshObjects()
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");

		s_Storarage3D.BatchedMesh.Clear();
	}

	Ref<Material> Renderer3D::GetMaterilNotInFustrem()
	{
		if (nullptr == s_Storarage3D.MaterilNotInFustrem)
		{
			MaterielShaderData materielData = MaterielShaderData();
			materielData.Color = glm::vec3(1.0f, 0.0f, 0.0f);
			materielData.Alpha = 0.15f;
			materielData.AmbientLigthe = 1.0f;
			Ref<Texture> texture = Texture::White();
			Ref<Shader> instenceShaderShade = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMesh.glsl"));
			Ref<Shader> instenceShaderShape = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShape.glsl"));
			Ref<Shader> instenceShaderDepth = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShadow.glsl"));

			s_Storarage3D.MaterilNotInFustrem = CreateRef<DefaultMaterial>(materielData, texture, instenceShaderShade, instenceShaderDepth);
		}
		return s_Storarage3D.MaterilNotInFustrem;
	}

#ifndef RY_RENERER_DESIGN_CURENT_MAIN
	void Renderer3D::ForEchViewPass(const std::function<void(const std::string name, ViewPassStorage& viewPass)>& func)
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering off getting other passes like that!)");

		std::map<std::string, ElementViewPassStorage>& viewPassMap = Renderer::GetViewPassMapRef();
		for (std::pair<const std::string, ElementViewPassStorage>& element : viewPassMap)
		{
			const std::string& typePass = element.first;
			ElementViewPassStorage elementViewPassStorage = element.second;
			RefVectorViewPassStorage& refVectorViewPassStorage = elementViewPassStorage.Vec;
			VectorViewPassStorage& vectorViewPassStorage = *refVectorViewPassStorage;
			for (ViewPassStorage& viewPass : vectorViewPassStorage)
			{
				func(typePass, viewPass);
			}
		}
	}

	void Renderer3D::ForEchViewPassType(const std::string& name, const std::function<void(ViewPassStorage& viewPass)>& func)
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering off getting other passes like that!)");

		std::map<std::string, ElementViewPassStorage>& viewPassMap = Renderer::GetViewPassMapRef();
		for (std::pair<const std::string, ElementViewPassStorage>& element : viewPassMap)
		{
			const std::string& typePass = element.first;
			if (name != typePass)
				continue;

			ElementViewPassStorage elementViewPassStorage = element.second;
			RefVectorViewPassStorage& refVectorViewPassStorage = elementViewPassStorage.Vec;

			if (nullptr == refVectorViewPassStorage)
				continue;

			VectorViewPassStorage& vectorViewPassStorage = *refVectorViewPassStorage;
			for (ViewPassStorage& viewPass : vectorViewPassStorage)
			{
				func(viewPass);
			}
		}
	}
#endif // !RY_RENERER_DESIGN_CURENT_MAIN

#pragma region MeshArrayVertexArray


	glm::uvec2 Renderer3D::GetIndriectOffsetsFromMeshArray(const Ref<MeshStatic>& meshStatic)
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");

		CheckMesh(meshStatic);

		DrawContent content = s_Storarage3D.BatchedMesh.MeshArray->GetData(meshStatic);
		return glm::uvec2{ content.BaseVertex, content.FirstIndex };
	}

	const Ref<VertexArray>& Renderer3D::GetMeshArrayVAO()
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");

		return s_Storarage3D.BatchedMesh.vao;
	}




	void Renderer3D::InitBatchedMeshArray(const std::vector<uint8_t>& vertices, const std::vector<uint32_t>& indices)
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");

		InitBatchedMeshArrayVAO();
		InitBatchedMeshArrayVB(vertices);
		InitBatchedMeshArrayIB(indices);
		s_Storarage3D.BatchedMesh.vao->AddVertexBuffer(s_Storarage3D.BatchedMesh.VB);
		s_Storarage3D.BatchedMesh.vao->SetIndexBuffer(s_Storarage3D.BatchedMesh.IB);
	}

	void Renderer3D::InitBatchedMeshArrayVAO()
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");

		s_Storarage3D.BatchedMesh.vao = VertexArray::Create();
		s_Storarage3D.BatchedMesh.vao->SetPrimitv(VertexArray::Primitv::Traingle);
	}

	void Renderer3D::InitBatchedMeshArrayVB(const std::vector<uint8_t>& vertices)
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");

#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		s_Storarage3D.BatchedMesh.VB = VertexBuffer::Create(
			vertices.data(), vertices.size(),
			BufferDataUsage::StaticDraw,
			{
				{ShaderDataType::Float3, "a_Postion"},
				{ShaderDataType::Float2, "a_UV"},
				{ShaderDataType::Float3, "a_Normals"},
			});
#else
		s_Storarage3D.BatchedMesh.VB = VertexBuffer::Create(
			vertices.data(), vertices.size(),
			BufferFlag::None,
			{
				{ShaderDataType::Float3, "a_Postion"},
				{ShaderDataType::Float2, "a_UV"},
				{ShaderDataType::Float3, "a_Normals"},
			});
#endif
	}

	void Renderer3D::InitBatchedMeshArrayIB(const std::vector<uint32_t>& indices)
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		s_Storarage3D.BatchedMesh.IB = IndexBuffer::Create(indices.data(), indices.size(), BufferDataUsage::StaticDraw);
#else
		s_Storarage3D.BatchedMesh.IB = IndexBuffer::Create(indices.data(), indices.size(), BufferFlag::None);
#endif

	}

	void Renderer3D::CheckMesh(const Ref<MeshStatic>& meshStatic)
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");

		if (!s_Storarage3D.BatchedMesh.MeshArray->Has(meshStatic))
		{
			AddMeshData(meshStatic);
		}
	}

	void Renderer3D::BatcheMeshData(const Ref<VertexBuffer>& vb, const Ref<IndexBuffer>& ib)
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");

		if (s_Storarage3D.BatchedMesh.vao)
		{
			s_Storarage3D.BatchedMesh.VB->AddCopyData(vb);
			s_Storarage3D.BatchedMesh.IB->AddCopyData(ib);
		}
		else
		{
			s_Storarage3D.BatchedMesh.OffsetCount = glm::uvec2{ 0u, 0u };
			const std::vector<uint8_t>& indiciesVec = ib->GetBufferData();

			std::vector<uint32_t> indices32ByteVec;

			const uint32_t elementByteSize = ib->GetElementByte();
			const uint32_t byteSizeModuleElementByteSize = indiciesVec.size() % elementByteSize;
			const uint32_t indicesCount = indiciesVec.size() / elementByteSize;
			RY_CORE_ASSERT(0u == byteSizeModuleElementByteSize, "not Viald BytsSize Found!");
			RY_CORE_ASSERT(sizeof(uint32_t) == elementByteSize, "not expexted Default count like 4 bytes = 32 bit!");
			indices32ByteVec.insert(indices32ByteVec.begin(), indiciesVec.begin(), indiciesVec.end());

			const std::vector<uint8_t>& verticies = vb->GetBufferData();
			InitBatchedMeshArray(verticies, indices32ByteVec);
		}
	}

	Ref<PiplineRenderBase> Renderer3D::GetPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel)
	{
		RY_REMBER_FUNC_CHANGE("Add at befor using somthing to selkect the rigth renderPipline class!")
		uint64_t hashNumber = 0ull;
		uint64_t meshNumber = reinterpret_cast<uint64_t>(meshSingle.get());
		uint64_t materielNumber = reinterpret_cast<uint64_t>(meshSingle.get());
		constexpr uint64_t hashBitsOffset = 16ull;
		constexpr uint64_t meshHashBitsOffset = 1 * hashBitsOffset;
		constexpr uint64_t materielHashBitsOffset = 2 * hashBitsOffset;
		hashNumber |= meshNumber << meshHashBitsOffset;
		hashNumber |= materielNumber << materielHashBitsOffset;

		uint32_t index = Renderer::GetCurentIndex();
		std::unordered_map<uint64_t, Weak<PiplineRenderBase>>& hashMapPipline = s_Storarage3D.RenderPiplinesHashMap.at(index);

		Weak<PiplineRenderBase>& renderPiplineWeak = hashMapPipline[hashNumber];
		Ref<PiplineRenderBase> renderPiplineRef = renderPiplineWeak.lock();
		if (nullptr == renderPiplineRef)
		{
			renderPiplineRef = CreateRef<SingleMeshPiplineRenderShade>();
			renderPiplineWeak = renderPiplineRef;
		}
		return renderPiplineRef;
	}

	void Renderer3D::AddMeshData(const Ref<MeshStatic>& meshStatic)
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");

		const std::vector<Ref<VertexBuffer>>& vabVec = meshStatic->GetVertexBufferVec();
		const std::vector<Ref<IndexBuffer>>& ibVec = meshStatic->GetShadeIndexBufferVec();
		uint32_t countVAB = vabVec.size();
		uint32_t countIB = ibVec.size();
		RY_CORE_ASSERT(countVAB == countIB);
		for (uint32_t i = 0; i < countVAB; i++)
		{
			const Ref<VertexBuffer>& vab = vabVec.at(i);
			const Ref<IndexBuffer>& ib = ibVec.at(i);
			s_Storarage3D.BatchedMesh.MeshArray->Add(
				meshStatic,
				DrawContent{
					vab, ib,
					s_Storarage3D.BatchedMesh.OffsetCount.x,
					s_Storarage3D.BatchedMesh.OffsetCount.y
				}
			);

			BatcheMeshData(vab, ib);
			SetOffsetBatchedMesh();
		}
	}



	void Renderer3D::SetOffsetBatchedMesh()
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");

		s_Storarage3D.BatchedMesh.OffsetCount.x = s_Storarage3D.BatchedMesh.VB->GetVertexCount();
		s_Storarage3D.BatchedMesh.OffsetCount.y = s_Storarage3D.BatchedMesh.IB->GetCount();
	}



	Ref<Texture> Renderer3D::GetDefoultChekebordTex()
	{

		if (nullptr == s_Storarage3D.CheckebordTex)
		{
			s_Storarage3D.CheckebordTex = Texture::Create({ 2, 2, 1, TexTar::Texture2D, TexFrom::S_RGBA8, 1, TexFilter::Nearest });
			uint32_t a = 0xFFCCCCCCu;
			uint32_t b = 0xFF555555u;
			uint32_t c = 0xFF000000u;

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
			s_Storarage3D.CheckebordTex->SetData(data, sizeof(data));
		}
		return s_Storarage3D.CheckebordTex;
	}

	Ref<Texture> Renderer3D::GetErrorTex()
	{
		if (nullptr == s_Storarage3D.ErrorTex)
		{
			s_Storarage3D.ErrorTex = TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ErrorTex.png");
		}
		return s_Storarage3D.ErrorTex;
	}
#pragma endregion
	void Renderer3D::ResetTargetRenderPtr()
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (I have no idear for the purpes off this function curently)");


	}

	void Renderer3D::ResetMeshObject()
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off Rendering Mesh Batches for indrect Rendering)");


		s_Storarage3D.BatchedMesh.Clear();
		s_Storarage3D.BatchedMesh.MeshArray = CreateRef<BatchingMeshArray>();
	}

	void Renderer3D::FrameFinshed()
	{
		RY_REMBER_FUNC_CHANGE("Change Funtion! ");

#if !RY_STATIC_SCREEN_DRAW

#if RY_PIPLINE_VEC_CLASS_DISABLE
		uint32_t& piplineIndex = s_Storarage3D.PiplineIndex;
		std::vector<Ref<SingleMeshPiplineRenderShade>>& pilineBaseVec = s_Storarage3D.PiplineRenderBaseVec;
		uint32_t count = pilineBaseVec.size();
		RY_CORE_ASSERT(piplineIndex <= count, "PilineIndex shoud be not be greater then the stack at max only equel!");

		for (uint32_t i = 0; i < piplineIndex; i++)
		{
			Ref<SingleMeshPiplineRenderShade>& piplineBaseVec = pilineBaseVec.at(i);
			piplineBaseVec->Clear();
		}
		piplineIndex = 0;
#else
#ifndef RY_RENERER_DESIGN_CURENT_MAIN

		s_Storarage3D.PiplineShadeVec.ResetFramePipline();
		s_Storarage3D.PiplineDepthVec.ResetFramePipline();
		s_Storarage3D.PiplineNotInFustremVec.ResetFramePipline();
		s_Storarage3D.InstencPiplineShadeVec.ResetFramePipline();
		s_Storarage3D.InstencPiplineDepthVec.ResetFramePipline();
		s_Storarage3D.InstencPiplineShapeVec.ResetFramePipline();
#else
		s_Storarage3D.InstencPiplineBaseVec.ResetFramePipline();
#endif 


#if RY_HASH_MEMORY_CLEAR
		s_Storarage3D.RenderEntityFrame.clear();
#elif RY_HASH_VEC_MEMORY_CLEAR
		for (auto& [key, vec] : s_Storarage3D.RenderEntityFrame)
		{
			vec.clear();
		}
#endif

#endif
#endif
		
	}

	void Renderer3D::SubmitRenderIndrectDrawList()
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off indrect Rendering we dont use enymore like that)");
	}

	void Renderer3D::SubmitRenderSingleIndrectDrawList()
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off indrect Rendering we dont use enymore like that)");
	}

	void Renderer3D::SubmitRenderMeshDrawList()
	{
		RY_REMBER_FUNC_CHANGE("Remove the function, maybe we don't need anmory in futer! (The Concept off indrect Rendering we dont use enymore like that)");
	}



#undef R3

}
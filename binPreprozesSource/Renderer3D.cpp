#include "rypch.h"
#include "Renderer3D.h"

#include <Rynex/Core/MapVector.h>
#include <Rynex/Core/UnorderDoubleMap.h>

#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Asset/Import/TextureImporter.h>

#include <Rynex/Renderer/Rendering/Render3D/RenderResourceList.h>
#include <Rynex/Renderer/Rendering/Render3D/IndirectDrawMap.h>
#include <Rynex/Renderer/Rendering/RenderObjects/ShaderDrawEntityList.h>

#include <Rynex/Renderer/Rendering/RenderObjects/AllEntitys/DefaultShaderDrawEntityList.h>
#include <Rynex/Renderer/Rendering/RenderObjects/AllEntitys/ShadowShaderDrawEntityList.h>
#include <Rynex/Renderer/Rendering/RenderObjects/AllEntitys/BoundingVolumeShaderDrawEntityList.h>
#include <Rynex/Renderer/Rendering/RenderObjects/SingleEntity/DefaultShaderDrawSingleEntityList.h>

#include <Rynex/Renderer/Rendering/RenderObjects/MeshSingleDraw/DefaultMeshDraw.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/SinglePiplineRender.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/InstenceMeshPiplineRender.h>

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
#define RY_RENDER_PIPLINE_INSTANCE_SHADOW 0

#define RY_STATIC_OPTIMZE 0
#define RY_ENABLE_CHECK_FUSTREM 1
#define RY_CHECK_FUSTREM_SHADE_ONLY 1
#define RY_CHECK_FUSTREM_SHADOW_ONLY 0
#define RY_CHECK_FUSTREM (!RY_CHECK_FUSTREM_SHADE_ONLY && !RY_CHECK_FUSTREM_SHADOW_ONLY) RY_ENABLE_CHECK_FUSTREM 




#define RY_HASH_GROUPING_OPTIMZE 0
#define RY_HASH_MEMORY_CLEAR 0
#define RY_HASH_VEC_MEMORY_CLEAR 0

namespace Rynex {

	using MapVectorShadersDrawEntityList = typename MapVector<UUID, typename Ref<typename ShaderDrawEntityList>>;
	using MapVectorShadersDrawEntityShadowList = typename MapVector<UUID, typename ShadowShaderDrawEntityList>;
	using MapVectorShadersDrawEntityDefaultList = typename MapVector<UUID, typename DefaultShaderDrawEntityList>;
	using MapVectorShadersDrawEntityDefaultList2 = typename MapVector<UUID, typename DefaultShaderDrawSingleEntityList>;
	using MapVectorShadersDrawEntityAABBList = typename MapVector<UUID, typename BoundingVolumeShaderDrawEntityList>;

	struct ShaderDrawEntityList2
	{
		Ref<Shader>			Shader;
		Ref<StorageBuffer>	OffsetBuffer;
		Ref<StorageBuffer>	ObjectBuffer;
		Ref<IndriectBuffer>	DrawBuffer;

		IndirectDrawMap<MeshRenderObjectTransform> DrawMap;
	};

	struct ShaderDrawEntityListShadow
	{
		Ref<Shader>			Shader;
		Ref<StorageBuffer>	OffsetBuffer;
		Ref<StorageBuffer>	ObjectBuffer;
		Ref<IndriectBuffer>	DrawBuffer;

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

		static void SubmitMeshObjectToShaderDrawEntityListShadow(MapVectorShadersDrawEntityShadowList& mapVec, const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const std::vector<MeshEntity>& entityMeshes, int entityID, const glm::mat4& model)
		{
			const UUID& id = shader->Handle;

			uint32_t size = mesh->GetSingleMeshObjectCount();

			if (!mapVec.HasKey(id))
			{
				ShadowShaderDrawEntityList& shaderDrawList = mapVec.AddData(id, ShadowShaderDrawEntityList(shader, 1, 0, 0, -1));

				shaderDrawList.SubmiteDrawList(mesh, entityID, model);
			}
			else
			{
				// ShaderDrawEntityList2& list2 = FindShaderDrawList2(shader, entityID);
				ShadowShaderDrawEntityList& shaderDrawList = mapVec.GetDataFromKey(id);
				shaderDrawList.SubmiteDrawList(mesh, entityID, model);
			}
		}

		static void UpdateMeshObjectToShaderDrawEntityListShadow(MapVectorShadersDrawEntityShadowList& mapVec, const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, int entityID, const glm::mat4& model)
		{
			const UUID& id = shader->Handle;

			uint32_t size = mesh->GetSingleMeshObjectCount();

			RY_CORE_ASSERT(mapVec.HasKey(id));
			{
				// ShaderDrawEntityList2& list2 = FindShaderDrawList2(shader, entityID);
				ShadowShaderDrawEntityList& shaderDrawList = mapVec.GetDataFromKey(id);
				shaderDrawList.UpdateObjectDrawList(mesh, entityID, model);
			}
		}

		static void RemoveMeshObjectToShaderDrawEntityListShadow(MapVectorShadersDrawEntityShadowList& mapVec, const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, int entityID)
		{
			uint32_t size = mesh->GetSingleMeshObjectCount();

			const UUID& id = shader->Handle;
			RY_CORE_ASSERT(mapVec.HasKey(id));
			{
				// ShaderDrawEntityList2& list2 = FindShaderDrawList2(shader, entityID);
				ShadowShaderDrawEntityList& shaderDrawList = mapVec.GetDataFromKey(id);
				shaderDrawList.RemoveEnitiyDrawList(mesh, entityID);
			}
		}


		static void SubmitMeshObjectToShaderDrawEntityListDefault(MapVectorShadersDrawEntityDefaultList& mapVec, const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, int entityID, const glm::mat4& model)
		{
			uint32_t size = mesh->GetSingleMeshObjectCount();

			const UUID& id = shader->Handle;
			if (!mapVec.HasKey(id))
			{
				DefaultShaderDrawEntityList& shaderDrawList = mapVec.AddData(id, DefaultShaderDrawEntityList(shader, 1, 0, 0, -1));
				shaderDrawList.SubmiteDrawList(mesh, entityID, model);
			}
			else
			{
				// ShaderDrawEntityList2& list2 = FindShaderDrawList2(shader, entityID);
				DefaultShaderDrawEntityList& shaderDrawList = mapVec.GetDataFromKey(id);
				shaderDrawList.SubmiteDrawList(mesh, entityID, model);
			}
		}

		static void SubmitMeshObjectToShaderDrawEntityListDefault2(MapVectorShadersDrawEntityDefaultList2& mapVec, const Ref<MeshStatic>& mesh, const Ref<Shader>& shaderShade, const Ref<Shader>& shaderPosition, int entityID, const glm::mat4& model)
		{
			uint32_t size = mesh->GetSingleMeshObjectCount();

			const UUID& id = shaderShade->Handle;
			if (!mapVec.HasKey(id))
			{
				DefaultShaderDrawSingleEntityList& shaderDrawList = mapVec.AddData(id, DefaultShaderDrawSingleEntityList(shaderShade, shaderPosition));
				shaderDrawList.SubmiteDrawList(mesh, entityID, model);
			}
			else
			{
				// ShaderDrawEntityList2& list2 = FindShaderDrawList2(shader, entityID);
				DefaultShaderDrawSingleEntityList& shaderDrawList = mapVec.GetDataFromKey(id);
				shaderDrawList.SubmiteDrawList(mesh, entityID, model);
			}
		}


		static void UpdateMeshObjectToShaderDrawEntityListDefault(MapVectorShadersDrawEntityDefaultList& mapVec, const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, int entityID, const glm::mat4& model)
		{
			uint32_t size = mesh->GetSingleMeshObjectCount();

			const UUID& id = shader->Handle;
			RY_CORE_ASSERT(mapVec.HasKey(id));
			{
				// ShaderDrawEntityList2& list2 = FindShaderDrawList2(shader, entityID);
				DefaultShaderDrawEntityList& shaderDrawList = mapVec.GetDataFromKey(id);
				shaderDrawList.UpdateObjectDrawList(mesh, entityID, model);
			}
		}

		static void RemoveMeshObjectToShaderDrawEntityListDefault(MapVectorShadersDrawEntityDefaultList& mapVec, const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, int entityID)
		{
			uint32_t size = mesh->GetSingleMeshObjectCount();

			const UUID& id = shader->Handle;
			RY_CORE_ASSERT(mapVec.HasKey(id));
			{
				// ShaderDrawEntityList2& list2 = FindShaderDrawList2(shader, entityID);
				DefaultShaderDrawEntityList& shaderDrawList = mapVec.GetDataFromKey(id);
				shaderDrawList.RemoveEnitiyDrawList(mesh, entityID);
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

	using BatchingMeshArray = typename RenderResourceList<MeshStatic, DrawContent>;

	struct BatchingMeshArrayStorage
	{
		Ref<BatchingMeshArray> MeshArray;
		Ref<VertexArray> VAO;
		Ref<VertexBuffer> VB;
		Ref<IndexBuffer> IB;

		glm::uvec2 OffsetCount = { 0u,0u };

		void Clear()
		{
			RY_DESTROY_REF(MeshArray);
			if (VAO)
				VAO->ClearVertexBuffers();
			RY_DESTROY_REF(VAO);
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
		std::vector<ObjectRendereIndex>* ObjectRendereIndexVec;
	};

	struct Renderer3DStorage
	{

		BatchingMeshArrayStorage BatchedMesh;

		DefaultMeshDraw ShaderMeshDraw;											// DefaultMeshDraw <- ShaderDrawEntityList
		MapVectorShadersDrawEntityShadowList ShaderDrawEnitiyVectorShadow;		// MapVector<UUID, { ShadowShaderDrawEntityList <- ShaderDrawEntityIndrectList <- ShaderDrawEntityList }> 
		MapVectorShadersDrawEntityDefaultList ShaderDrawEnitiyVectorDefault;	// MapVector<UUID, { DefaultShaderDrawEntityList <- ShaderDrawEntityIndrectList <- ShaderDrawEntityList }> 
		MapVectorShadersDrawEntityDefaultList2 ShaderDrawEnitiyVectorDefault2;	// MapVector<UUID, { DefaultShaderDrawSingleEntityList <- ShaderDrawEntityList }> 

		PiplineVec<SingleMeshPiplineRenderShade> PiplineShadeVec;
		PiplineVec<SingleMeshPiplineRenderShade> PiplineNotInFustremVec;

		PiplineVec<SingleMeshPiplineRenderDepth> PiplineDepthVec;
		Ref<Shader> SingleShaderShade;
		Ref<Shader> SingleShaderDepth;

		PiplineVec<InstenceMeshPiplineRenderShade> InstencPiplineShadeVec;
		PiplineVec<InstenceMeshPiplineRenderDepth> InstencPiplineDepthVec;
		Ref<Shader> InstencShaderShade;
		Ref<Shader> InstencShaderDepth;

		std::unordered_map<Ref<MeshSingle>, std::vector<RenderEnitityObject>> RenderEntityFrame;


		Ref<RenderTarget> MainTarget;
		Ref<RenderTarget> ViewPortTarget;
		Ref<Shader> MeshDefaultShader;
		Ref<Texture> CheckebordTex;
		Ref<Texture> ErrorTex;
		Ref<DefaultMaterial> MaterilNotInFustrem;
		Ref<Shader> IndrectMultyShadowShader;
	};

	static Renderer3DStorage s_Storarage3D;




	void Renderer3D::Init()
	{
		s_Storarage3D.BatchedMesh.MeshArray = CreateRef<BatchingMeshArray>();
		s_Storarage3D.MeshDefaultShader = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShader.glsl"));
		s_Storarage3D.IndrectMultyShadowShader = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShadowShader.glsl"));




		Ref<Shader> shaderDepth = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/SingelMeshShadow.glsl"));
		Ref<Shader> shaderShade = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/SingelMesh.glsl"));
		Ref<Shader> instenceShaderShade = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMesh.glsl"));
		Ref<Shader> instenceShaderDepth = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/InstenceMeshShadow.glsl"));

		s_Storarage3D.SingleShaderDepth = shaderDepth;
		s_Storarage3D.SingleShaderShade = shaderShade;
		s_Storarage3D.InstencShaderShade = instenceShaderShade;
		s_Storarage3D.InstencShaderDepth = instenceShaderDepth;

	}

	void Renderer3D::InitEditor()
	{
	}

	void Renderer3D::Shutdown()
	{


		s_Storarage3D.ShaderDrawEnitiyVectorShadow.Clear();
		s_Storarage3D.ShaderDrawEnitiyVectorDefault.Clear();
		s_Storarage3D.ShaderDrawEnitiyVectorDefault2.Clear();
		s_Storarage3D.ShaderMeshDraw.Clear();



		s_Storarage3D.PiplineShadeVec.Destroy();
		s_Storarage3D.PiplineDepthVec.Destroy();
		s_Storarage3D.InstencPiplineShadeVec.Destroy();
		s_Storarage3D.InstencPiplineDepthVec.Destroy();
		s_Storarage3D.PiplineNotInFustremVec.Destroy();



		ClearMeshObjects();
		RY_DESTROY_REF(s_Storarage3D.SingleShaderDepth);
		RY_DESTROY_REF(s_Storarage3D.SingleShaderShade);

		RY_DESTROY_REF(s_Storarage3D.InstencShaderShade);
		RY_DESTROY_REF(s_Storarage3D.InstencShaderDepth);
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



	void Renderer3D::MeshCompont(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		const Ref<MeshStatic>& mesh = comp.MeshStatic;
		if (nullptr != mesh)
		{

			SubmitShadeMeshStaticObject(mesh, s_Storarage3D.InstencShaderShade, model, entityID, comp.ObjectRendereIndexPiplineVec2);
		}
	}

	void Renderer3D::MeshCompont(const glm::mat4& model, StaticSingleComponetsMeshComponent& comp, int entityID)
	{
		SingleMeshObject singleMeshObject;
		singleMeshObject._Material = comp._Material;
		singleMeshObject._MeshSingle = comp._MeshSingle;

		SubmitShadeMeshObject(singleMeshObject, s_Storarage3D.InstencShaderShade, model
			, entityID, comp.ObjectRendereIndexPiplineVec);
	}

	void Renderer3D::SubmitMeshCompont(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		const Ref<MeshStatic>& mesh = comp.MeshStatic;
		comp.Stage = StaticMeshComponent::Submited;
		Ref<Shader>& shader = s_Storarage3D.MeshDefaultShader;
		SubmitMeshObject(mesh, shader, model, entityID);
	}

	void Renderer3D::UpdateMeshCompont(const glm::mat4& model, StaticMeshComponent& comp, int entityID)
	{
		const Ref<MeshStatic>& mesh = comp.MeshStatic;
		comp.Stage &= ~(StaticMeshComponent::UpdateMesh | StaticMeshComponent::UpdateTransform);
		UpdateMeshObject(mesh, s_Storarage3D.MeshDefaultShader, model, entityID);
	}

	void Renderer3D::DeleateMeshCompont(int entityID)
	{
		Ref<Shader>& shader = s_Storarage3D.MeshDefaultShader;


	}


	void Renderer3D::SubmitMeshObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID)
	{

		MapVectorShadersDrawEntityDefaultList& mapVecMain = s_Storarage3D.ShaderDrawEnitiyVectorDefault;
		std::vector<MeshEntity> entityMeshes;
		Utils::SubmitMeshObjectToShaderDrawEntityListDefault(mapVecMain, mesh, shader, entityID, model);

		MapVectorShadersDrawEntityDefaultList2& mapVecMain2 = s_Storarage3D.ShaderDrawEnitiyVectorDefault2;
		Utils::SubmitMeshObjectToShaderDrawEntityListDefault2(mapVecMain2, mesh, shader, s_Storarage3D.IndrectMultyShadowShader, entityID, model);

		MapVectorShadersDrawEntityShadowList& mapVecShadow = s_Storarage3D.ShaderDrawEnitiyVectorShadow;
		Utils::SubmitMeshObjectToShaderDrawEntityListShadow(mapVecShadow, mesh, s_Storarage3D.IndrectMultyShadowShader, entityMeshes, entityID, model);

		DefaultMeshDraw& defaultMeshDraw = s_Storarage3D.ShaderMeshDraw;
		defaultMeshDraw.SubmiteDrawList(mesh, entityID, model);
	}

	void Renderer3D::UpdateMeshObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID)
	{

		MapVectorShadersDrawEntityDefaultList& mapVecMain = s_Storarage3D.ShaderDrawEnitiyVectorDefault;
		Utils::UpdateMeshObjectToShaderDrawEntityListDefault(mapVecMain, mesh, shader, entityID, model);

		MapVectorShadersDrawEntityShadowList& mapVecShadow = s_Storarage3D.ShaderDrawEnitiyVectorShadow;
		Utils::UpdateMeshObjectToShaderDrawEntityListShadow(mapVecShadow, mesh, s_Storarage3D.IndrectMultyShadowShader, entityID, model);

		DefaultMeshDraw& defaultMeshDraw = s_Storarage3D.ShaderMeshDraw;
		defaultMeshDraw.UpdateObjectDrawList(mesh, entityID, model);

	}

	void Renderer3D::RemoveMeshObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, int entityID)
	{
		MapVectorShadersDrawEntityDefaultList& mapVecMain = s_Storarage3D.ShaderDrawEnitiyVectorDefault;
		std::vector<MeshEntity> entityMeshes;
		Utils::RemoveMeshObjectToShaderDrawEntityListDefault(mapVecMain, mesh, shader, entityID);

		MapVectorShadersDrawEntityShadowList& mapVecShadow = s_Storarage3D.ShaderDrawEnitiyVectorShadow;
		Utils::RemoveMeshObjectToShaderDrawEntityListShadow(mapVecShadow, mesh, s_Storarage3D.IndrectMultyShadowShader, entityID);

		DefaultMeshDraw& defaultMeshDraw = s_Storarage3D.ShaderMeshDraw;
		defaultMeshDraw.RemoveEnitiyDrawList(mesh, entityID);

	}


	void Renderer3D::SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<std::vector<ObjectRendereIndex>>& objectRendereVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		uint32_t count = meshSingleVec.size();
		if (objectRendereVec.size() != count)
		{
			objectRendereVec.resize(count);
		}
		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			std::vector<ObjectRendereIndex>& objectRendere = objectRendereVec.at(i);
			glm::mat4 modelMatrix = model * meshSingle.LocaleCildrenMatrix;
			SubmitShadeMeshObject(meshSingle, shader, modelMatrix, entityID, objectRendere);
			i++;
		}
	}

	void Renderer3D::SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<SingleMeshRender>& singleMeshRendereVec)
	{


		if (singleMeshRendereVec.empty())
		{
			const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
			uint32_t count = meshSingleVec.size();

			singleMeshRendereVec.clear();
			singleMeshRendereVec.reserve(count);

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
				SubmitShadeMeshObject(single, shader, modelMatrix, entityID, renderIndex);

			}
		}
		else
		{
			for (SingleMeshRender& single : singleMeshRendereVec)
			{
				glm::mat4 modelMatrix = model * single.LocaleCildrenMatrix;
				std::vector<ObjectRendereIndex>& renderIndex = single.IndexVec;

				SubmitShadeMeshObject(single, shader, modelMatrix, entityID, renderIndex);
			}
		}
	}

	void Renderer3D::SubmitShadeMeshStaticObject(const Ref<MeshStatic>& mesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndexStaticArray>& objectRendereVec)
	{
		const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = mesh->GetSingleObjectMesDataVec();
		uint32_t count = meshSingleVec.size();
		if (objectRendereVec.size() != count)
		{
			objectRendereVec.resize(count);
		}
		uint32_t i = 0;
		for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
		{
			ObjectRendereIndexStaticArray& objectRendere = objectRendereVec.at(i);
			glm::mat4 modelMatrix = model * meshSingle.LocaleCildrenMatrix;
			SubmitShadeMeshObject(meshSingle, shader, modelMatrix, entityID, objectRendere);
			i++;
		}
	}



	void Renderer3D::SubmitMeshObjectToHash(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRendereVec)
	{
		std::vector<RenderEnitityObject>& vec = s_Storarage3D.RenderEntityFrame[meshSingle];
		vec.emplace_back(RenderEnitityObject{ model, entityID, material, &objectRendereVec });
	}

	void Renderer3D::SubmitMeshObjectToHash(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, ObjectRendereIndexStaticArray& objectRendereArray)
	{
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

	void Renderer3D::SubmitMeshObjectToPipline(const Ref<MeshSingle>& meshSingle, const Ref<Material>& material, const glm::mat4& model, int entityID, ObjectRendereIndexStaticArray& objectRendere)
	{
		SingleMeshObject singleMesh{ material, meshSingle };
		SubmitShadeMeshObject(singleMesh, s_Storarage3D.InstencShaderShade, model, entityID, objectRendere);
	}

	void Renderer3D::SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndexStaticArray& objectRenderePiplineVec)
	{
#if !RY_STATIC_SCREEN_DRAW
		CamerRenderPackages& cameraPackege = Renderer::GetMainCamerRenderPackagesRef();
#if 1
		uint32_t piplineIndex = 0;
		RY_CORE_ASSERT(piplineIndex < objectRenderePiplineVec.size(), "Overflow in static Array");
		ObjectRendereIndex& objectRendereIndex = objectRenderePiplineVec.at(piplineIndex);
		Ref<PiplineRenderBase> piplineMain = SubmitMeshObjectToRenderTargetShade(cameraPackege, singleMesh, shader, model, entityID, objectRendereIndex);
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

				RY_CORE_ASSERT(piplineIndex < objectRenderePiplineVec.size(), "Overflow in static Array");
				ObjectRendereIndex& objectRendereIndexShadow = objectRenderePiplineVec.at(piplineIndex);
#if RY_RENDER_PIPLINE_INSTANCE_SHADOW
				Ref<PiplineRenderBase> piplineShadow = SubmitMeshObjectToRenderTargetDepth(viewPassShadow.CameraPackege, singleMesh, s_Storarage3D.InstencShaderDepth, model, entityID, objectRendereIndexShadow);
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

	}


	void Renderer3D::SubmitShadeMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, int entityID, std::vector<ObjectRendereIndex>& objectRenderePiplineVec)
	{
#if !RY_STATIC_SCREEN_DRAW
		CamerRenderPackages& cameraPackege = Renderer::GetMainCamerRenderPackagesRef();
#if 1
		uint32_t piplineIndex = 0;
		// if (objectRenderePiplineVec.size() <= piplineIndex)
		if (piplineIndex <= objectRenderePiplineVec.size())
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

	}




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
		PiplineVec<SingleMeshPiplineRenderShade>& pilineBaseVec = s_Storarage3D.PiplineShadeVec;
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
		PiplineVec<InstenceMeshPiplineRenderShade>* pilineVecShadePtr = &s_Storarage3D.InstencPiplineShadeVec;
		PiplineBaseVec* pilineBaseVecPtr = reinterpret_cast<PiplineBaseVec*>(pilineVecShadePtr);

#if 1
		pipline = SubmitMeshObjectToRenderTarget(pilineBaseVecPtr, cameraPackege, meshObjectCopy, shader, model, entityID, objectRendere);
#endif
#elif !RY_STATIC_SCREEN_DRAW
		PiplineVec<SingleMeshPiplineRenderShade>* pilineVecShadePtr = &s_Storarage3D.PiplineShadeVec;
		PiplineBaseVec* pilineBaseVecPtr = reinterpret_cast<PiplineBaseVec*>(pilineVecShadePtr);

#if 1
		pipline = SubmitMeshObjectToRenderTarget(pilineBaseVecPtr, cameraPackege, meshObjectCopy, shader, model, entityID, objectRendere);
#endif
#endif
		return pipline;
#endif
	}

	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTargetDepth(CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere)
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
		Ref<PiplineRenderBase> pipline = nullptr;


#if RY_ENABELE_LIST_STYSTEM
		PiplineVec<SingleMeshPiplineRenderDepth>& pilineBaseVec = s_Storarage3D.PiplineDepthVec;
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

		pipline = s_Storarage3D.PiplineDepthVec.GetNextPipline();
		PiplineResultState result = pipline->SubmitEntityMeshObject(meshObject, shader, model, instenceIndex, entityID);
		int checkResult = (result & PiplineResultState::Result_Success);
		RY_CORE_ASSERT(checkResult != 0, "the new Pipline dident work!");


#endif

#endif
		RenderTarget& target = cameraPackege.GetRenderTarget();

		CamerRenderPackages::CamerPackage& packeg = cameraPackege.GetCamerPackage();
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
		PiplineVec<InstenceMeshPiplineRenderDepth>* pilineVecDepthPtr = &s_Storarage3D.InstencPiplineDepthVec;
#else
		PiplineVec<SingleMeshPiplineRenderDepth>* pilineVecDepthPtr = &s_Storarage3D.PiplineDepthVec;
#endif
		PiplineBaseVec* pilineBaseVecPtr = reinterpret_cast<PiplineBaseVec*>(pilineVecDepthPtr);


		pipline = SubmitMeshObjectToRenderTarget(pilineBaseVecPtr, cameraPackege, meshObject, shader, model, entityID, objectRendere);
#endif

		return pipline;
#endif
	}

	Ref<PiplineRenderBase> Renderer3D::SubmitMeshObjectToRenderTarget(PiplineBaseVec* piplineBaseVecPtr, CamerRenderPackages& cameraPackege, const SingleMeshObject& meshObject, const Ref<Shader>& shader, const glm::mat4& model, int entityID, ObjectRendereIndex& objectRendere)
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
		pilineIndex = i;
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



	void Renderer3D::PrepairMainScene()
	{
		RenderTarget& target = Renderer::GetMainRenderTargetRef();
	}



	void Renderer3D::ClearMeshObjects()
	{
		s_Storarage3D.BatchedMesh.Clear();
	}

	Ref<Material> Renderer3D::GetMaterilNotInFustrem()
	{
		if (nullptr == s_Storarage3D.MaterilNotInFustrem)
		{
			MaterielShaderData materiel = MaterielShaderData();
			materiel.Color = glm::vec3(1.0f, 0.0f, 0.0f);
			materiel.Alpha = 0.15f;
			materiel.AmbientLigthe = 1.0f;
			Ref<Texture> texture = Texture::White();
			s_Storarage3D.MaterilNotInFustrem = CreateRef<DefaultMaterial>(materiel, texture);
		}
		return s_Storarage3D.MaterilNotInFustrem;
	}

	void Renderer3D::ForEchViewPass(const std::function<void(const std::string name, ViewPassStorage& viewPass)>& func)
	{
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

#pragma region MeshArrayVertexArray


	glm::uvec2 Renderer3D::GetIndriectOffsetsFromMeshArray(const Ref<MeshStatic>& meshStatic)
	{
		CheckMesh(meshStatic);
		DrawContent& content = s_Storarage3D.BatchedMesh.MeshArray->GetData(meshStatic);
		return glm::uvec2{ content.BaseVertex, content.FirstIndex };
	}

	const Ref<VertexArray>& Renderer3D::GetMeshArrayVAO()
	{
		return s_Storarage3D.BatchedMesh.VAO;
	}




	void Renderer3D::InitBatchedMeshArray(const std::vector<uint8_t>& vertices, const std::vector<uint32_t>& indices)
	{
		InitBatchedMeshArrayVAO();
		InitBatchedMeshArrayVB(vertices);
		InitBatchedMeshArrayIB(indices);
		s_Storarage3D.BatchedMesh.VAO->AddVertexBuffer(s_Storarage3D.BatchedMesh.VB);
		s_Storarage3D.BatchedMesh.VAO->SetIndexBuffer(s_Storarage3D.BatchedMesh.IB);
	}

	void Renderer3D::InitBatchedMeshArrayVAO()
	{
		s_Storarage3D.BatchedMesh.VAO = VertexArray::Create();
		s_Storarage3D.BatchedMesh.VAO->SetPrimitv(VertexArray::Primitv::Traingle);
	}

	void Renderer3D::InitBatchedMeshArrayVB(const std::vector<uint8_t>& vertices)
	{
		s_Storarage3D.BatchedMesh.VB = VertexBuffer::Create(
			vertices.data(), vertices.size(),
			BufferDataUsage::StaticDraw,
			{
				{ShaderDataType::Float3, "a_Postion"},
				{ShaderDataType::Float2, "a_UV"},
				{ShaderDataType::Float3, "a_Normals"},
			});
	}

	void Renderer3D::InitBatchedMeshArrayIB(const std::vector<uint32_t>& indices)
	{
		s_Storarage3D.BatchedMesh.IB = IndexBuffer::Create(indices.data(), indices.size(), BufferDataUsage::StaticDraw);
	}

	void Renderer3D::CheckMesh(const Ref<MeshStatic>& meshStatic)
	{
		if (!s_Storarage3D.BatchedMesh.MeshArray->Has(meshStatic))
		{
			AddMeshData(meshStatic);
		}
	}

	void Renderer3D::BatcheMeshData(const Ref<VertexBuffer>& vb, const Ref<IndexBuffer>& ib)
	{
		if (s_Storarage3D.BatchedMesh.VAO)
		{
			s_Storarage3D.BatchedMesh.VB->AddCopyData(vb);
			s_Storarage3D.BatchedMesh.IB->AddCopyData(ib);
		}
		else
		{
			s_Storarage3D.BatchedMesh.OffsetCount = glm::uvec2{ 0u, 0u };
			const std::vector<uint32_t>& indicies = ib->GetBufferData();
			const std::vector<uint8_t>& verticies = vb->GetBufferData();
			InitBatchedMeshArray(verticies, indicies);
		}
	}

	void Renderer3D::AddMeshData(const Ref<MeshStatic>& meshStatic)
	{
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
		s_Storarage3D.BatchedMesh.OffsetCount.x = s_Storarage3D.BatchedMesh.VB->GetVertexCount();
		s_Storarage3D.BatchedMesh.OffsetCount.y = s_Storarage3D.BatchedMesh.IB->GetCount();
	}



	Ref<Texture> Renderer3D::GetDefoultChekebordTex()
	{
		if (nullptr == s_Storarage3D.CheckebordTex)
		{
			s_Storarage3D.CheckebordTex = Texture::Create({ 2, 2, TexTar::Texture2D, TexFrom::S_RGBA8, 1, TexFilter::Nearest });
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
			s_Storarage3D.ErrorTex = TextureImporter::LoadTexture("../Rynex-Editor/Resources/Icons/ErrorTex.png", false);
		}
		return s_Storarage3D.ErrorTex;
	}
#pragma endregion
	void Renderer3D::ResetTargetRenderPtr()
	{
#if RY_RENDERER_3D_HARDCODED_PIPLINES

		MapVectorShadersDrawEntityDefaultList& mapVecDefault = s_Storarage3D.ShaderDrawEnitiyVectorDefault;
		for (ShaderDrawEntityList& listdraw : mapVecDefault)
			listdraw.ResetSubmitRenderTargetConction();

		MapVectorShadersDrawEntityShadowList& mapVecShadow = s_Storarage3D.ShaderDrawEnitiyVectorShadow;
		for (ShaderDrawEntityList& listdraw : mapVecShadow)
			listdraw.ResetSubmitRenderTargetConction();

		MapVectorShadersDrawEntityDefaultList2& mapVec2Default = s_Storarage3D.ShaderDrawEnitiyVectorDefault2;
		for (DefaultShaderDrawSingleEntityList& listdraw : mapVec2Default)
			listdraw.ResetSubmitRenderTargetConction();

		DefaultMeshDraw& defaultMeshDraw = s_Storarage3D.ShaderMeshDraw;
		defaultMeshDraw.ResetSubmitRenderTargetConction();

#if 0
		for (uint32_t i = 0; i < RY_SHADOW_COUNT; i++)
		{
			RenderTarget& targetShadow = Renderer::GetRenderTargetRef("Shadow", i);
			targetShadow.ClearShaderDrawList();
		}
#else
		ForEchViewPass([&](const std::string& name, ViewPassStorage& viewPass)
			{
				CamerRenderPackages& viewPassPackege = viewPass.CameraPackege;
				RenderTarget& targetShadow = viewPassPackege.GetRenderTarget();
				targetShadow.ClearShaderDrawList();
			}
		);
#endif
		RenderTarget& targetMain = Renderer::GetMainRenderTargetRef();
		targetMain.ClearShaderDrawList();

		RY_CORE_INFO("Reset Renderer3D All RenderTarget Conctions!");
#elif RY_RENDERER_3D_PROTOYPE_PIPLINES


		RenderTarget& targetMain = Renderer::GetMainRenderTargetRef();
		targetMain.ClearShaderDrawList();


		MapRefVectorViewPassStorage& map = Renderer::GetViewPassMapRef();
		for (std::pair<const std::string, ElementViewPassStorage>& keyElement : map)
		{
			ElementViewPassStorage& element = keyElement.second;
			RefVectorViewPassStorage& refVec = element.Vec;
			VectorViewPassStorage& vec = *refVec;

			for (ViewPassStorage& viewPas : vec)
			{
				CamerRenderPackages& cameraPackeg = viewPas.CameraPackege;
				RenderTarget& renderTarget = cameraPackeg.GetRenderTarget();
				renderTarget.ClearShaderDrawList();
			}
		}
#else
#endif






	}

	void Renderer3D::ResetMeshObject()
	{
		MapVectorShadersDrawEntityDefaultList& mapVecMain = s_Storarage3D.ShaderDrawEnitiyVectorDefault;
		mapVecMain.Clear();

		MapVectorShadersDrawEntityDefaultList2& mapVecMain2 = s_Storarage3D.ShaderDrawEnitiyVectorDefault2;
		mapVecMain2.Clear();

		MapVectorShadersDrawEntityShadowList& mapVecShadow = s_Storarage3D.ShaderDrawEnitiyVectorShadow;
		mapVecShadow.Clear();

		DefaultMeshDraw& defaultMeshDraw = s_Storarage3D.ShaderMeshDraw;
		defaultMeshDraw.Clear();

		s_Storarage3D.BatchedMesh.Clear();
		s_Storarage3D.BatchedMesh.MeshArray = CreateRef<BatchingMeshArray>();
	}

	void Renderer3D::FrameFinshed()
	{
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
		s_Storarage3D.PiplineShadeVec.ResetFramePipline();
		s_Storarage3D.PiplineDepthVec.ResetFramePipline();
		s_Storarage3D.PiplineNotInFustremVec.ResetFramePipline();
		s_Storarage3D.InstencPiplineShadeVec.ResetFramePipline();
		s_Storarage3D.InstencPiplineDepthVec.ResetFramePipline();
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
		RenderTarget& targetMain = Renderer::GetMainRenderTargetRef();

#if 1
		MapVectorShadersDrawEntityDefaultList& mapVecDefault = s_Storarage3D.ShaderDrawEnitiyVectorDefault;
		for (ShaderDrawEntityList& listdraw : mapVecDefault)
		{
			if (listdraw.UpdateDrawList())
			{
				const Ref<UniformBuffer>& camerBuffer = Renderer::GetMainPassViewCamerUniform();
				const Ref<UniformBuffer>& displayBuffer = Renderer::GetMainPassViewCamerUniform();

				listdraw.AddShadeDrawEntityListToRenderTarget(targetMain, camerBuffer, displayBuffer);
			}

		}
#if 1

#if 0
		std::map<std::string, ElementViewPassStorage>& viewPassMap = Renderer::GetViewPassMapRef();
		for (std::pair<const std::string, ElementViewPassStorage>& element : viewPassMap)
		{
			const std::string& typePass = element.first;
			if ("Shadow" != typePass)
				continue;

			ElementViewPassStorage elementViewPassStorage = element.second;
			RefVectorViewPassStorage& refVectorViewPassStorage = elementViewPassStorage.Vec;
			VectorViewPassStorage& vectorViewPassStorage = *refVectorViewPassStorage;
			for (ViewPassStorage& viewPass : vectorViewPassStorage)
			{
				CamerRenderPackages& viewPassPackege = viewPass.CameraPackege;
				RenderTarget& targetShadow = viewPassPackege.GetRenderTarget();
				CamerRenderPackages::CamerPackage camerPackage = viewPassPackege.GetCamerPackage();
				CamerRenderPackages::DisplayPackage displayPackage = viewPassPackege.GetDisplayPackage();

				const Ref<UniformBuffer>& camerBuffer = camerPackage.GetBuffer();
				const Ref<UniformBuffer>& displayBuffer = displayPackage.GetBuffer();

				MapVectorShadersDrawEntityShadowList& mapVecShadow = s_Storarage3D.ShaderDrawEnitiyVectorShadow;
				for (ShaderDrawEntityList& listdraw : mapVecShadow)
				{
					if (listdraw.UpdateDrawList())
					{
						listdraw.AddShadeDrawEntityListToRenderTarget(targetShadow, camerBuffer, displayBuffer);
					}

				}
			}
		}
#else
		ForEchViewPassType("Shadow"
			, [&](ViewPassStorage& viewPass)
			{
				CamerRenderPackages& viewPassPackege = viewPass.CameraPackege;
				RenderTarget& targetShadow = viewPassPackege.GetRenderTarget();
				CamerRenderPackages::CamerPackage camerPackage = viewPassPackege.GetCamerPackage();
				CamerRenderPackages::DisplayPackage displayPackage = viewPassPackege.GetDisplayPackage();

				const Ref<UniformBuffer>& camerBuffer = camerPackage.GetBuffer();
				const Ref<UniformBuffer>& displayBuffer = displayPackage.GetBuffer();

				MapVectorShadersDrawEntityShadowList& mapVecShadow = s_Storarage3D.ShaderDrawEnitiyVectorShadow;
				for (ShaderDrawEntityList& listdraw : mapVecShadow)
				{
					if (listdraw.UpdateDrawList())
					{
						listdraw.AddShadeDrawEntityListToRenderTarget(targetShadow, camerBuffer, displayBuffer);
					}

				}
			}
		);
#endif

#endif

#else

		MapVectorShadersDrawEntityDefaultList2& mapVec2Default = s_Storarage3D.ShaderDrawEnitiyVectorDefault2;
		for (DefaultShaderDrawSingleEntityList& listdraw : mapVec2Default)
		{
			if (listdraw.UpdateDrawList())
			{
				const Ref<UniformBuffer>& camerBuffer = Renderer::GetMainPassViewCamerUniform();
				const Ref<UniformBuffer>& displayBuffer = Renderer::GetMainPassViewCamerUniform();

				listdraw.AddShadeDrawEntityListToRenderTarget(targetMain, camerBuffer, displayBuffer);
			}
		}
		for (uint32_t i = 0; i < RY_SHADOW_COUNT; i++)
		{
			RenderTarget& targetShadow = Renderer::GetRenderTargetRef(i);

			for (DefaultShaderDrawSingleEntityList& listdraw : mapVec2Default)
			{
				if (listdraw.UpdateDrawList())
				{
					const Ref<UniformBuffer>& camerBuffer = Renderer::GetPassViewCamerUniform(i);
					const Ref<UniformBuffer>& displayBuffer = Renderer::GetPassViewDisblayUniform(i);

					listdraw.AddPostionDrawEntityListToRenderTarget(targetShadow, camerBuffer, displayBuffer);
				}
			}
		}

#endif



	}

	void Renderer3D::SubmitRenderSingleIndrectDrawList()
	{
		RenderTarget& targetMain = Renderer::GetMainRenderTargetRef();

		MapVectorShadersDrawEntityDefaultList2& mapVec2Default = s_Storarage3D.ShaderDrawEnitiyVectorDefault2;
		for (DefaultShaderDrawSingleEntityList& listdraw : mapVec2Default)
		{
			if (listdraw.UpdateDrawList())
			{
				const Ref<UniformBuffer>& camerBuffer = Renderer::GetMainPassViewCamerUniform();
				const Ref<UniformBuffer>& displayBuffer = Renderer::GetMainPassViewCamerUniform();

				listdraw.AddShadeDrawEntityListToRenderTarget(targetMain, camerBuffer, displayBuffer);
			}
		}
#if 0
		for (uint32_t i = 0; i < RY_SHADOW_COUNT; i++)
		{
			RenderTarget& targetShadow = Renderer::GetRenderTargetRef("Shadow", i);

			for (DefaultShaderDrawSingleEntityList& listdraw : mapVec2Default)
			{
				if (listdraw.UpdateDrawList())
				{
					const Ref<UniformBuffer>& camerBuffer = Renderer::GetPassViewCamerUniform("Shadow", i);
					const Ref<UniformBuffer>& displayBuffer = Renderer::GetPassViewDisblayUniform("Shadow", i);

					listdraw.AddPostionDrawEntityListToRenderTarget(targetShadow, camerBuffer, displayBuffer);
				}
			}
		}
#else
		ForEchViewPassType("Shadow"
			, [&](ViewPassStorage& viewPass)
			{
				CamerRenderPackages& viewPassPackege = viewPass.CameraPackege;
				RenderTarget& targetShadow = viewPassPackege.GetRenderTarget();
				CamerRenderPackages::CamerPackage camerPackage = viewPassPackege.GetCamerPackage();
				CamerRenderPackages::DisplayPackage displayPackage = viewPassPackege.GetDisplayPackage();

				const Ref<UniformBuffer>& camerBuffer = camerPackage.GetBuffer();
				const Ref<UniformBuffer>& displayBuffer = displayPackage.GetBuffer();

				MapVectorShadersDrawEntityShadowList& mapVecShadow = s_Storarage3D.ShaderDrawEnitiyVectorShadow;
				for (ShaderDrawEntityList& listdraw : mapVecShadow)
				{
					if (listdraw.UpdateDrawList())
					{
						listdraw.AddShadeDrawEntityListToRenderTarget(targetShadow, camerBuffer, displayBuffer);
					}

				}
			}
		);
#endif

	}

	void Renderer3D::SubmitRenderMeshDrawList()
	{
		RenderTarget& targetMain = Renderer::GetMainRenderTargetRef();
		DefaultMeshDraw& defaultMeshDraw = s_Storarage3D.ShaderMeshDraw;
		if (defaultMeshDraw.UpdateDrawList())
		{
			const Ref<UniformBuffer>& camerBufferMain = Renderer::GetMainPassViewCamerUniform();
			const Ref<UniformBuffer>& displayBufferMain = Renderer::GetMainPassViewCamerUniform();

			defaultMeshDraw.AddShadeDrawEntityListToRenderTarget(targetMain, camerBufferMain, displayBufferMain);

			for (uint32_t i = 0; i < RY_SHADOW_COUNT; i++)
			{
				const Ref<UniformBuffer>& camerBufferDepth = Renderer::GetPassViewCamerUniform("Shadow", i);
				const Ref<UniformBuffer>& displayBufferDepth = Renderer::GetPassViewDisblayUniform("Shadow", i);

				RenderTarget& targetShadow = Renderer::GetRenderTargetRef("Shadow", i);
				defaultMeshDraw.AddPostionDrawEntityListToRenderTarget(targetShadow, camerBufferDepth, displayBufferDepth);
			}
		}
	}



#undef R3

}
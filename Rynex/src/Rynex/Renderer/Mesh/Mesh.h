#pragma once
#include <Rynex/Asset/Base/Asset.h>

#include <Rynex/Renderer/API/ProtypeAPI.h>

#define RY_OLD_INDRECT 1

#define RY_MESH_LOADING 1
#define RY_MESH_OPTIMIZE 1


#if RY_MESH_OPTIMIZE
	#define RY_MESH_OPTIMIZE_TRINGLE			1
	#define RY_MESH_OPTIMIZE_TRINGLE_STRIPS		0
	
	#define RY_MESH_OPTIMIZE_DUBLICATION		1
	#define RY_MESH_OPTIMIZE_SHADOW				0
	#define RY_MESH_OPTIMIZE_OVERDRAW			1
	#define RY_MESH_OPTIMIZE_FATCH				1
	#define RY_MESH_OPTIMIZE_CHACHE				1
	#define RY_MESH_OPTIMIZE_OVERDRAW_VALUE		1.05f
#else
	#define RY_MESH_OPTIMIZE_OVERDRAW_VALUE		0.0f
#endif
namespace Rynex {

	struct MeshVerteices
	{
		glm::vec3 Postion;
		glm::vec2 TexCorrds; // UV
		glm::vec3 Normale;


		MeshVerteices() = default;
		MeshVerteices(const MeshVerteices& mv) = default;

		
	};

	constexpr bool operator==(const MeshVerteices& left, const MeshVerteices& rigth)
	{
		return left.Postion == rigth.Postion
			&& left.TexCorrds == rigth.TexCorrds
			&& left.Normale == rigth.Normale;
	}

	constexpr bool operator!=(const MeshVerteices& left, const MeshVerteices& rigth)
	{
		return left.Postion != rigth.Postion
			|| left.TexCorrds != rigth.TexCorrds
			|| left.Normale != rigth.Normale;
	}

	class MeshStatic;
	class MeshSource;
	class MeshDynamic;

	class MeshSingle;
	class Material;

	typedef int ObjectMeshIndexVec;
	typedef int MaterialIndex;
	typedef int EnitiytID;
	

	struct MeshEntity
	{
		Ref<Material> _Material;
		uint32_t LocaleMeshIndex;
	};

	

	class RYNEX_API Mesh : public Asset
	{
	public:
		enum class Type
		{
			None = 0,
			Static,
			Dynamic,
			Source,
			Single
		};
		
		struct PerDrawObject
		{
			uint32_t Count;
			uint32_t InstancesCount;
			uint32_t FirstIndex;
			int BaseVertex;
			uint32_t BaseInstance;
		};

		struct MeshMaterielIndex
		{
			int MaterielIndex = -1;
		};

		struct MeshMateriel
		{
			uint32_t MaterielIndex;
		};

		struct MeshRenderObject
		{
			Ref<MeshSingle>	Mesh;
			Ref<Material>	Material;
			std::string		NodeName;

			glm::mat4		Matrix;

			uint32_t		LocaleIndexMesh;
			uint32_t		LocaleIndexMateriel;

		};

	public:
		Mesh() = default;
		Mesh(const UUID& handdle)
			: Asset(handdle)
		{ }
		virtual ~Mesh() {}

		
		virtual Mesh::Type GetMeshType() const = 0;
		static AssetType GetStaticType() { return AssetType::Mesh; }
		AssetType GetType() const override { return GetStaticType(); }

		static Ref<MeshDynamic> CreateDynamicMesh(const std::filesystem::path& path);
		static Ref<MeshStatic> CreateStaticMesh(Ref<MeshSource> source);
		static Ref<MeshStatic> CreateStaticMeshOld(Ref<MeshSource> source);
		static Ref<MeshStatic> CreateStaticMesh(const std::filesystem::path& path);
		
	};

}



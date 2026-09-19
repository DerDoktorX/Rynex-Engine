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

	struct MeshVertices
	{
		glm::vec3 m_Position;
		glm::vec2 m_TextureCords; // UV
		glm::vec3 m_Normale;


		MeshVertices() = default;
		MeshVertices(const MeshVertices& mv) = default;

        MeshVertices(glm::vec3 position, glm::vec2 textureCords, glm::vec3 normale)
            : m_Position(position)
            , m_TextureCords(textureCords)
            , m_Normale(normale)
        {
        }
    };

	constexpr bool operator==(const MeshVertices& left, const MeshVertices& rigth)
	{
		return left.m_Position == rigth.m_Position
			&& left.m_TextureCords == rigth.m_TextureCords
			&& left.m_Normale == rigth.m_Normale;
	}

	constexpr bool operator!=(const MeshVertices& left, const MeshVertices& rigth)
	{
		return left.m_Position != rigth.m_Position
			|| left.m_TextureCords != rigth.m_TextureCords
			|| left.m_Normale != rigth.m_Normale;
	}

	class MeshStatic;
	class MeshSource;
	class MeshDynamic;

	class MeshSingle;
	class Material;

	typedef int ObjectMeshIndexVec;
	typedef int MaterialIndex;
	typedef int EntityID;
	

	struct MeshEntity
	{
		Ref<Material> m_Material;
		uint32_t m_LocaleMeshIndex;
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
			uint32_t m_Count;
			uint32_t m_InstancesCount;
			uint32_t m_FirstIndex;
			int m_BaseVertex;
			uint32_t m_BaseInstance;
		};

		struct MeshMaterielIndex
		{
			int m_MaterielIndex = -1;
		};

		struct MeshMateriel
		{
			uint32_t m_MaterielIndex;
		};

		struct MeshRenderObject
		{
			Ref<MeshSingle>	m_Mesh;
			Ref<Material>	m_Material;
			std::string		m_NodeName;

			glm::mat4		m_Matrix;

			uint32_t		m_LocaleIndexMesh;
			uint32_t		m_LocaleIndexMateriel;

		};

	// public member methode --------------------------------------------------------------------------------------------------
		Mesh() = default;

        explicit Mesh(const UUID& handle)
			: Asset(handle)
		{ }
        virtual ~Mesh() = default;

		
		virtual Mesh::Type GetMeshType() const = 0;
		static AssetType GetStaticType() { return AssetType::Mesh; }
		AssetType GetType() const override { return GetStaticType(); }

		static Ref<MeshDynamic> CreateDynamicMesh(const FileSystem::Path& path);
		static Ref<MeshStatic> CreateStaticMesh(Ref<MeshSource> source);
		static Ref<MeshStatic> CreateStaticMeshOld(Ref<MeshSource> source);
		static Ref<MeshStatic> CreateStaticMesh(const FileSystem::Path& path);
		
	};

}



#pragma once
#include <Rynex/Renderer/Mesh/Mesh.h>
#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/API/Buffer.h>

namespace Rynex {
	class MeshSource;

	class MeshSingle : public Mesh
	{
	public:
		MeshSingle(uint32_t modelLocaleIndex, const UUID& handle, const UUID& sourceHandle);
		MeshSingle(uint32_t modelLocaleIndex, const UUID& handle, const UUID& sourceHandle, const std::string& name, const BoundingVolume& volume);

		~MeshSingle();

		virtual Mesh::Type GetMeshType() const override { return Mesh::Type::Single; }

		const Ref<VertexBuffer>& GetVertexBuffer()const { return m_VAB; }

		const Ref<IndexBuffer>& GetShadeIndexBuffer() const { RY_REMBER_FUNC_CHANGE("Change to sellect a LOD level"); return m_ShadeVIB; }
		const Ref<IndexBuffer>& GetDepthIndexBuffer() const { RY_REMBER_FUNC_CHANGE("Change to sellect a LOD level"); return m_DepthVIB; }


		const Mesh::PerDrawObject& GetShadePerDrawObjectIndrect() const { return m_ShadePDOIndrect; }
		const Mesh::PerDrawObject& GetDepthPerDrawObjectIndrect() const { return m_DepthPDOIndrect; }

		Ref<MeshSource> GetSourceMesh() const 
		{ 
			Ref<MeshSource> source = m_Source.lock();
			return source;
		}
		uint32_t GetModelLocalMesheIndex() const { return m_ModelLocaleIndex; }

		const std::string& GetName() const { return m_Name; }
		bool IsViewFustrum(const glm::mat4& model, const glm::mat4& viewProjtion);
		
		const BoundingVolume& GetBoundingVolume() const { return m_Bounding; }
		const AABB& GetAABB() const { return m_Bounding.GetAABB(); }
		const Sphere& GetSphere() const { return m_Bounding.GetSphere(); }

	private:
		static void ExtractFrustum(const glm::mat4& viewProj, glm::vec4 planes[6]);

		static bool IsNotPointInFiewFustrem(glm::vec3 max, glm::vec3 min, const glm::vec4& planeSide);
		static bool IsAABBInsideFrustum(const glm::mat4& model, const AABB& box, const glm::mat4& viewProjtion);
		static bool IsAABBInsideFrustum(const glm::mat4& m, const AABB& box);
		static glm::vec4 CaculatePlaneViewFustremPlaneNormilze(const glm::vec3& normale, float constan);
		static glm::vec4 CaculateCorectViewFustremPlane0(const glm::mat4& viewProj);
		static glm::vec4 CaculateCorectViewFustremPlane1(const glm::mat4& viewProj);
		static glm::vec4 CaculateCorectViewFustremPlane2(const glm::mat4& viewProj);
		static glm::vec4 CaculateCorectViewFustremPlane3(const glm::mat4& viewProj);
		static glm::vec4 CaculateCorectViewFustremPlane4(const glm::mat4& viewProj);
		static glm::vec4 CaculateCorectViewFustremPlane5(const glm::mat4& viewProj);
		static void CaculateCorectViewFustremPlane(const glm::mat4& m, const glm::mat4& mTranspose, int row, glm::vec4 plane[2]);
		static glm::vec4 CaculateCorectViewFustremPlaneAdd(const glm::vec4& mC, const glm::vec4& mR);
		static glm::vec4 CaculateCorectViewFustremPlaneSub(const glm::vec4& mC, const glm::vec4& mR);

		void SetupFromSource();

		void Destroy();

		void SetupPDOIndrect(const std::vector<Mesh::PerDrawObject>& indrectPDOVec, Mesh::PerDrawObject& pdoIndrect) const;
	private:
		Ref<VertexBuffer> m_VAB;

		Ref<IndexBuffer> m_ShadeVIB;
		Ref<IndexBuffer> m_DepthVIB;
		Weak<MeshSource> m_Source;

		std::string m_Name;

		BoundingVolume m_Bounding;
		Mesh::PerDrawObject m_ShadePDOIndrect;
		Mesh::PerDrawObject m_DepthPDOIndrect;

		uint32_t m_ModelLocaleIndex;
	};

	struct SingleMeshObject
	{
		Ref<Material> _Material;
		Ref<MeshSingle> _MeshSingle;
		

		SingleMeshObject() = default;
		SingleMeshObject(const SingleMeshObject&) = default;


		bool operator==(const SingleMeshObject& object) const;
		bool operator!=(const SingleMeshObject& object) const;

		uint64_t GetUUID() const;

	};
}

namespace std {

	template<>
	struct hash<Rynex::SingleMeshObject>
	{
		std::size_t operator()(const Rynex::SingleMeshObject& object) const
		{
			uint64_t uuid = object.GetUUID();
			return uuid;
		}
	};

}

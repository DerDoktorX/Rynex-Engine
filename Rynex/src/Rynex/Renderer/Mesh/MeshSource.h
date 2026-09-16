#pragma once
#include <Rynex/Renderer/Mesh/Mesh.h>

#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/Materials/Material.h>


namespace Rynex {
	

	class RYNEX_API MeshSource : public Mesh
	{
	public:
		inline static constexpr const char* s_VertexBufferAttributePositionName = "a_Postion";
		inline static constexpr const char* s_VertexBufferAttributeTextureCoordName = "a_UV";
		inline static constexpr const char* s_VertexBufferAttributeNormaleName = "a_Normals";
	public:
		struct SourceVertex 
		{
			std::vector<MeshVertices> m_MeshVertices;
			std::vector<uint32_t> m_ObjectMeshIndexVec;
		};

		struct SourceMesh
		{
			std::vector<MeshVertices> m_MeshVertices;
			std::vector<uint32_t> m_ObjectMeshIndexVec;

			uint32_t m_MaterielIndex;
			BoundingVolume m_Volume;

			std::string m_MeshName;
			uint32_t m_MeshIndex;
			uint32_t m_MeshDataIndex;
			UUID m_MeshHandle;

		};
		
		struct MaterialMesh
		{
			std::string m_NameMateriel;
			uint32_t MaterielIndex;
			std::vector<std::filesystem::path> m_TexturesMaterial;
			glm::vec4 m_BaseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		};

		struct EntityNodes
		{
			std::vector<EntityNodes> m_Childrens;
			std::string m_NodeName;
			glm::mat4 m_Matrics = glm::mat4(0.0f);
			
			// Mesh Index
			std::vector<uint32_t> m_ObjectMeshIndexVec;

			bool operator == (const EntityNodes& nodes) const
			{
				const bool resultName = this->m_NodeName == nodes.m_NodeName;
				const bool resultChildrensCount = this->m_Childrens.size() == nodes.m_Childrens.size();
				const bool resultMeshCount = this->m_ObjectMeshIndexVec.size() == nodes.m_ObjectMeshIndexVec.size();
				return resultName && resultChildrensCount && resultMeshCount;
			}
		};

		struct OptimizeMeshData
		{
			std::vector<MeshVertices> m_VerticesDataVec;
			std::vector<uint32_t> m_ShadeIndicesDataVec;
			std::vector<uint32_t> m_DepthIndicesDataVec;
		};
	public:

		MeshSource(std::vector<MeshSource::MaterialMesh>&& materiel, std::vector<MeshSource::SourceMesh>&& meshes, std::vector<MeshSource::SourceVertex>&& vertexSourceData,  EntityNodes&& nodes);
		~MeshSource();

		virtual Mesh::Type GetMeshType() const override { return Mesh::Type::Source; }

		void SetVertexBuffer(const Ref<VertexBuffer>& vb, uint32_t i) { m_VABVec.at(i) = vb; }
		void SetIndexBuffer(const Ref<IndexBuffer>& vi, uint32_t i) { m_DepthIABVec.at(i) = vi; }

		void SetStorageBuffer(const Ref<StorageBuffer>& sbboM) { m_StorageBuffer = sbboM; }

		void SetMaterials(const std::vector<Ref<Material>>& m) { m_MaterialsVec = m; }
		void SetTextures(const std::vector<Ref<Texture>>& textures);

		void SetTexturesMap(const MapVector<int64_t, Ref<Texture>>& t) { m_TexturesMap = t; }
		void SetPerDrawObject(const std::vector<Mesh::PerDrawObject>& pdo) { m_ShadePerDrawObjectVec = pdo; }


		const Ref<VertexBuffer>& GetVertexBuffer(uint32_t i) const { return m_VABVec.at(i); }
		const Ref<VertexBuffer>& GetVertexBufferConst(uint32_t i) const { return m_VABVec.at(i); }

		const Ref<IndexBuffer>& GetShadeIndexBuffer(uint32_t i) const { return m_ShadeIABVec.at(i); }
		const Ref<IndexBuffer>& GetDepthIndexBuffer(uint32_t i) const { return m_DepthIABVec.at(i); }

		const Ref<IndexBuffer>& GetShadeIndexBufferConst(uint32_t i) const { return m_ShadeIABVec.at(i); }
		const Ref<IndexBuffer>& GetDepthIndexBufferConst(uint32_t i) const { return m_DepthIABVec.at(i); }


		// ---

		const std::vector<Ref<VertexBuffer>>& GetVertexBufferVecConst() const { return m_VABVec; }

		const std::vector<Ref<IndexBuffer>>& GetShadeIndexBufferVecConst() const { return m_ShadeIABVec; }
		const std::vector<Ref<IndexBuffer>>& GetDepthIndexBufferVecConst() const { return m_DepthIABVec; }


		// ---
		const Ref<StorageBuffer>& GetOffsetBuffer() const { return m_StorageBuffer; }

		const std::vector<Ref<Texture>>& GetTexturesVecConst() const { return m_TexturesMap.GetVector(); }
		const MapVector<int64_t, Ref<Texture>>& GetTexturesMapConst() const { return m_TexturesMap; }
		const std::vector<Mesh::PerDrawObject>& GetPerDrawObjectsVecConst() const { return m_ShadePerDrawObjectVec; }

		const std::vector<Ref<MeshSingle>>& GetMeshSingleVecConst() const { return m_SingleMeshVec; }
		const std::vector<Ref<Material>>& GetMaterialsVecConst() const { return m_MaterialsVec; }


		const std::vector<Ref<Texture>>& GetTexturesVec() const { return GetTexturesVecConst(); }
		const MapVector<int64_t, Ref<Texture>>& GetTexturesMap() const { return GetTexturesMapConst(); }
		const std::vector<Mesh::PerDrawObject>& GetPerDrawObjectsShadeVec() const { return GetPerDrawObjectsVecConst(); }


		const std::vector<PerDrawObject>& GetPerDrawObjectsDepthVec() const { return m_DepthPerDrawObjectVec; }
		const std::vector<Ref<MeshSingle>>& GetMeshSingleVec() const { return GetMeshSingleVecConst(); }
		const std::vector<Ref<Material>>& GetMaterialsVec() const { return GetMaterialsVecConst(); }

		const MeshSource::EntityNodes& GetNodes() const {return m_Nodes;}
		const std::vector<MeshSource::SourceMesh>& GetMeshSourcesConst() const { return m_SourceMeshes; }
		const std::vector<MeshSource::MaterialMesh>& GetMaterialsSourcesConst() const { return m_SourceMateriel; }

		const std::vector<MeshSource::SourceMesh>& GetMeshSources() const { return GetMeshSourcesConst(); }
		const std::vector<MeshSource::MaterialMesh>& GetMaterialsSources() const { return GetMaterialsSourcesConst(); }

		 void SeparateMeshes();
		 static void OptimizeMesh(std::vector<MeshVertices>& verticies, std::vector<uint32_t>& indicies, OptimizeMeshData& optMesh);
		 bool HasMeshDataGenerated();
		 void GenerateMeshesGPUBufferBatched();

		 void GenerateMeshDataIndividuallyBuffer();
		
		 bool IsOneToOne(Ref<MeshStatic> meshStatic) const;
		 
		 static bool CheckVertexArraysIdentical(const std::vector<MeshVertices>* aVec, const std::vector<MeshVertices>* bVec);
		 static bool CheckVertexArraysIdentical(const std::vector<uint32_t>* aVec, const std::vector<uint32_t>* bVec);

		 static uint32_t GetIndexVertexArraysIdentical(const std::vector<MeshSource::SourceVertex>& list, const std::vector<MeshVertices>* verticesVec, const std::vector<uint32_t>* indiciesVec);
	private:
		Ref<VertexBuffer>& GetVertexBuffer(const uint32_t i) { return m_VABVec.at(i); }
		Ref<IndexBuffer>& GetShadeIndexBuffer(const uint32_t i) { return m_ShadeIABVec.at(i); }
		Ref<IndexBuffer>& GetDepthIndexBuffer(const uint32_t i) { return m_DepthIABVec.at(i); }


		std::vector<Ref<VertexBuffer>>& GetVertexBufferVec() { return m_VABVec; }

		std::vector<Ref<IndexBuffer>>& GetShadeIndexBufferVec() { return m_ShadeIABVec; }
		std::vector<Ref<IndexBuffer>>& GetDepthIndexBufferVec() { return m_DepthIABVec; }

		Ref<StorageBuffer>& GetOffsetBuffer() { return m_StorageBuffer; }


		std::vector<Ref<Material>>& GetMaterialsVec() { return m_MaterialsVec; }
		std::vector<Ref<Texture>>& GetTexturesVec() { return m_TexturesMap.GetVector(); }
		std::vector<Ref<Texture>>& GetTexturesMap() { return m_TexturesMap.GetVector(); }
		std::vector<PerDrawObject>& GetPerDrawObjectsShadeVec() { return m_ShadePerDrawObjectVec; }
		std::vector<PerDrawObject>& GetPerDrawObjectsDepthVec() { return m_ShadePerDrawObjectVec; }

		std::vector<Ref<MeshSingle>>& GetMeshSingleVec() { return m_SingleMeshVec; }

		const Ref<MeshStatic>& GetStaticMesh() const { return m_MeshStatic; }
		void SetStaticMesh(const Ref<MeshStatic>& staticMesh) { m_MeshStatic = staticMesh; }		std::vector<MeshSource::SourceMesh>& GetMeshSources() { return m_SourceMeshes; }
		void OptimizeMeshes();

		static void OptimizeMeshVec(std::vector<MeshVertices>& verticies, std::vector<uint32_t>& indicies);

		static OptimizeMeshData OptimizeMeshVec(MeshSource::SourceMesh& m);
		static OptimizeMeshData OptimizeMeshVec(MeshSource::SourceVertex& m);
		static void OptimizeMeshDuplication(OptimizeMeshData& origMesh, OptimizeMeshData& optMesh);
		static void OptimizeMeshDuplicationRemapVertexBuffer(const std::vector<MeshVertices>& origVericesVec, std::vector<MeshVertices>& optVericesVec, const std::vector<uint32_t>& remap, uint32_t optVertexCount);
		static void OptimizeMeshDuplicationRemapIndexBuffer(const std::vector<uint32_t>& origIndiciesVec, std::vector<uint32_t>& optIndiciesVec, const std::vector<uint32_t>& remap);

		static void OptimizeMeshTriangleCache(OptimizeMeshData& meshBuffers);
		static void OptimizeMeshTriangleStripsCache(OptimizeMeshData& meshBuffers);
		static void OptimizeMeshTriangleStripsGenerate(OptimizeMeshData& meshBuffers);
		static void OptimizeMeshOverdraw(OptimizeMeshData& meshBuffers, float efficiently = 1.05f);
		static void OptimizeMeshFetch(OptimizeMeshData& meshBuffers);
		static void OptimizeMeshShadow(OptimizeMeshData& meshBuffers);

		static void OptimizeMeshLOD(OptimizeMeshData& meshBuffers, float prozentigReduseIndices = 0.5f, float errorSimplefly = 1e-2f);
		void ClearVecData();
		void ReisizeMeshVecData(uint32_t count);
		void ReisizeMaterielVecData(uint32_t count);

		void SearchInSourceDataForIdenticalVerteices();
		
		static bool IsIndexPresentInVec(const std::vector<uint32_t>& indexVec, uint32_t index);
	private:
		std::vector<MeshSource::MaterialMesh> m_SourceMateriel;
		std::vector<MeshSource::SourceMesh> m_SourceMeshes;
		std::vector<MeshSource::SourceVertex> m_SourceVertexData;
		EntityNodes m_Nodes;

		std::vector<Ref<VertexBuffer>> m_VABVec;

		std::vector<Ref<IndexBuffer>> m_ShadeIABVec;
		std::vector<Ref<IndexBuffer>> m_DepthIABVec;


		std::vector<Ref<MeshSingle>> m_SingleMeshVec;
		std::vector<Ref<Material>> m_MaterialsVec;
		Ref<StorageBuffer> m_StorageBuffer;
		MapVector<int64_t, Ref<Texture>> m_TexturesMap;
		Ref<MeshStatic> m_MeshStatic;
		std::vector<Mesh::PerDrawObject> m_ShadePerDrawObjectVec;
		std::vector<Mesh::PerDrawObject> m_DepthPerDrawObjectVec;


	private:
		friend Mesh;
		friend MeshSingle;
	};


}



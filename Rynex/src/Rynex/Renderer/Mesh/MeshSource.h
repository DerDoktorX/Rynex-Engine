#pragma once
#include <Rynex/Renderer/Mesh/Mesh.h>

#include <Rynex/Renderer/API/VertexArray.h>
#include <Rynex/Renderer/Materials/Material.h>


namespace Rynex {
	

	class RYNEX_API MeshSource : public Mesh
	{
	public:
		inline static constexpr const char* s_VerexBufferAtributePostionName = "a_Postion";
		inline static constexpr const char* s_VerexBufferAtributeTextureCoordName = "a_UV";
		inline static constexpr const char* s_VerexBufferAtributeNormaleName = "a_Normals";
	public:
		struct SourceVertex 
		{
			std::vector<MeshVerteices> MeshVerteices;
			std::vector<uint32_t> ObjectMeshIndexVec;
		};

		struct SourceMesh
		{
			std::vector<MeshVerteices> MeshVerteices;
			std::vector<uint32_t> ObjectMeshIndexVec;

			uint32_t MaterielIndex;
			BoundingVolume Volume;

			std::string MeshName;
			uint32_t MeshIndex;
			uint32_t MeshDataIndex;
			UUID MeshHandle;

		};
		
		struct _Material
		{
			std::string NameMateriel;
			uint32_t MaterielIndex;
			std::vector<std::filesystem::path> TexturesMatieral;
			glm::vec4 BaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		};

		struct EntityNodes
		{
			std::vector<EntityNodes> Childrens;
			std::string NodeName;
			glm::mat4 Matrics = glm::mat4(0.0f);
			
			// Mesh Index
			std::vector<uint32_t> ObjectMeshIndexVec;

			bool operator == (const EntityNodes& nodes) const
			{
				bool resultName = this->NodeName == nodes.NodeName;
				bool resultChildrensCount = this->Childrens.size() == nodes.Childrens.size();
				bool resultMeshCount = this->ObjectMeshIndexVec.size() == nodes.ObjectMeshIndexVec.size();
				return resultName && resultChildrensCount && resultMeshCount;
			}
		};

		struct OptimizeMeshData
		{
			std::vector<MeshVerteices> VerticesDataVec;
			std::vector<uint32_t> ShadeIndicesDataVec;
			std::vector<uint32_t> DepthIndicesDataVec;
		};
#if 0
		struct MeshData 
		{
			Ref<IndexBuffer> IAB;
			Ref<VertexArray> IAA;
			Mesh::PerDrawObject indrectPDO;
		};
#endif
	public:

		MeshSource(std::vector<MeshSource::_Material>&& materiel, std::vector<MeshSource::SourceMesh>&& meshes, std::vector<MeshSource::SourceVertex>&& vertexSourceData,  EntityNodes&& nodes);
		~MeshSource();

		virtual Mesh::Type GetMeshType() const override { return Mesh::Type::Source; }

		void SetVertexBuffer(const Ref<VertexBuffer>& vb, uint32_t i) { m_VABvec.at(i) = vb; }
		void SetIndexBuffer(const Ref<IndexBuffer>& vi, uint32_t i) { m_DepthIABvec.at(i) = vi; }

#if RY_OLD_INDRECT
		void SetStorageBuffer(const Ref<StorageBuffer>& sbboM) { m_StorageBuffer = sbboM; }
#endif

		void SetMaterials(const std::vector<Ref<Material>>& m) { m_MaterialsVec = m; }
		void SetTextures(const std::vector<Ref<Texture>>& textures);

#if RY_OLD_INDRECT
		void SetTexturesMap(const MapVector<int64_t, Ref<Texture>>& t) { m_TexturesMap = t; }
		void SetPerDrawObject(const std::vector<Mesh::PerDrawObject>& pdo) { m_ShadePerDrawObjectVec = pdo; }
#endif


		const Ref<VertexBuffer>& GetVertexBuffer(uint32_t i) const { return m_VABvec.at(i); }
		const Ref<VertexBuffer>& GetVertexBufferConst(uint32_t i) const { return m_VABvec.at(i); }

		const Ref<IndexBuffer>& GetShadeIndexBuffer(uint32_t i) const { return m_ShadeIABvec.at(i); }
		const Ref<IndexBuffer>& GetDepthIndexBuffer(uint32_t i) const { return m_DepthIABvec.at(i); }

		const Ref<IndexBuffer>& GetShadeIndexBufferConst(uint32_t i) const { return m_ShadeIABvec.at(i); }
		const Ref<IndexBuffer>& GetDepthIndexBufferConst(uint32_t i) const { return m_DepthIABvec.at(i); }


		// ---

		const std::vector<Ref<VertexBuffer>>& GetVertexBufferVecConst() const { return m_VABvec; }

		const std::vector<Ref<IndexBuffer>>& GetShadeIndexBufferVecConst() const { return m_ShadeIABvec; }
		const std::vector<Ref<IndexBuffer>>& GetDepthIndexBufferVecConst() const { return m_DepthIABvec; }


		// ---
#if RY_OLD_INDRECT
		const Ref<StorageBuffer>& GetOffsetBuffer() const { return m_StorageBuffer; }

		const std::vector<Ref<Texture>>& GetTexturesVecConst() const { return m_TexturesMap.GetVector(); }
		const MapVector<int64_t, Ref<Texture>>& GetTexturesMapConst() const { return m_TexturesMap; }
		const std::vector<Mesh::PerDrawObject>& GetPerDrawObjectsVecConst() const { return m_ShadePerDrawObjectVec; }
#endif

		const std::vector<Ref<MeshSingle>>& GetMeshSingleVecConst() const { return m_SingleMeshVec; }
		const std::vector<Ref<Material>>& GetMaterialsVecConst() const { return m_MaterialsVec; }

#if RY_OLD_INDRECT

		const std::vector<Ref<Texture>>& GetTexturesVec() const { return GetTexturesVecConst(); }
		const MapVector<int64_t, Ref<Texture>>& GetTexturesMap() const { return GetTexturesMapConst(); }
		const std::vector<Mesh::PerDrawObject>& GetPerDrawObjectsShadeVec() const { return GetPerDrawObjectsVecConst(); }
#endif


#if RY_OLD_INDRECT
		const std::vector<PerDrawObject>& GetPerDrawObjectsDepthVec() const { return m_DepthPerDrawObjectVec; }
#endif
		const std::vector<Ref<MeshSingle>>& GetMeshSingleVec() const { return GetMeshSingleVecConst(); }
		const std::vector<Ref<Material>>& GetMaterialsVec() const { return GetMaterialsVecConst(); }

		const MeshSource::EntityNodes& GetNodes() const {return m_Nodes;}
		const std::vector<MeshSource::SourceMesh>& GetMeshSourcesConst() const { return m_SourceMeshes; }
		const std::vector<MeshSource::_Material>& GetMaterialsSourcesConst() const { return m_SourceMateriel; }

		const std::vector<MeshSource::SourceMesh>& GetMeshSources() const { return GetMeshSourcesConst(); }
		const std::vector<MeshSource::_Material>& GetMaterialsSources() const { return GetMaterialsSourcesConst(); }
		
		 void SepareteMeshes();
		 static void OptimizeMesh(std::vector<MeshVerteices>& verticies, std::vector<uint32_t>& indicies, OptimizeMeshData& optMesh);
		 bool HasMeshDataGerated();
		 void GenarteMeshesGPUBufferBatched();

		 void GenarteMeshDataIndevdiuelBuffer();
		
		 bool IsOneToOne(Ref<MeshStatic> meshStatic) const;
		 
		 static bool CheckVertexArraysIdentical(const std::vector<MeshVerteices>* aVec, const std::vector<MeshVerteices>* bVec);
		 static bool CheckVertexArraysIdentical(const std::vector<uint32_t>* aVec, const std::vector<uint32_t>* bVec);

		 static uint32_t GetIndexVertexArraysIdentical(const std::vector<MeshSource::SourceVertex>& list, const std::vector<MeshVerteices>* verticesVec, const std::vector<uint32_t>* indiciesVec);
	private:
		Ref<VertexBuffer>& GetVertexBuffer(uint32_t i) { return m_VABvec.at(i); }
		Ref<IndexBuffer>& GetShadeIndexBuffer(uint32_t i) { return m_ShadeIABvec.at(i); }
		Ref<IndexBuffer>& GetDepthIndexBuffer(uint32_t i) { return m_DepthIABvec.at(i); }


		std::vector<Ref<VertexBuffer>>& GetVertexBufferVec() { return m_VABvec; }

		std::vector<Ref<IndexBuffer>>& GetShadeIndexBufferVec() { return m_ShadeIABvec; }
		std::vector<Ref<IndexBuffer>>& GetDepthIndexBufferVec() { return m_DepthIABvec; }

#if RY_OLD_INDRECT
		Ref<StorageBuffer>& GetOffsetBuffer() { return m_StorageBuffer; }
#endif


		std::vector<Ref<Material>>& GetMaterialsVec() { return m_MaterialsVec; }
#if RY_OLD_INDRECT
		std::vector<Ref<Texture>>& GetTexturesVec() { return m_TexturesMap.GetVector(); }
		std::vector<Ref<Texture>>& GetTexturesMap() { return m_TexturesMap.GetVector(); }
		std::vector<PerDrawObject>& GetPerDrawObjectsShadeVec() { return m_ShadePerDrawObjectVec; }
		std::vector<PerDrawObject>& GetPerDrawObjectsDepthVec() { return m_ShadePerDrawObjectVec; }
#endif

		std::vector<Ref<MeshSingle>>& GetMeshSingleVec() { return m_SingleMeshVec; }

#if RY_OLD_INDRECT
		const Ref<MeshStatic>& GetStaticMesh() const { return m_MeshStatic; }
		void SetStaticMesh(const Ref<MeshStatic>& staticMesh) { m_MeshStatic = staticMesh; }
#endif
		std::vector<MeshSource::SourceMesh>& GetMeshSources() { return m_SourceMeshes; }
		void OptimizeMeshes();

		static void OptimizeMeshVec(std::vector<MeshVerteices>& verticies, std::vector<uint32_t>& indicies);

		static OptimizeMeshData OptimizeMeshVec(MeshSource::SourceMesh& m);
		static OptimizeMeshData OptimizeMeshVec(MeshSource::SourceVertex& m);
		static void OptimizeMeshDublication(OptimizeMeshData& origMesh, OptimizeMeshData& optMesh);
		static void OptimizeMeshDublicationRemapVertexBuffer(const std::vector<MeshVerteices>& origVericesVec, std::vector<MeshVerteices>& optVericesVec, const std::vector<uint32_t>& remap, uint32_t optVertexCount);
		static void OptimizeMeshDublicationRemapIndexBuffer(const std::vector<uint32_t>& origIndiciesVec, std::vector<uint32_t>& optIndiciesVec, const std::vector<uint32_t>& remap);

		static void OptimizeMeshTrinagleChache(OptimizeMeshData& meshBuffers);
		static void OptimizeMeshTrinagleStripsChache(OptimizeMeshData& meshBuffers);
		static void OptimizeMeshTrinagleStripsGenerate(OptimizeMeshData& meshBuffers);
		static void OptimizeMeshOverdraw(OptimizeMeshData& meshBuffers, float efficiently = 1.05f);
		static void OptimizeMeshFatch(OptimizeMeshData& meshBuffers);
		static void OptimizeMeshShadow(OptimizeMeshData& meshBuffers);

		static void OptimizeMeshLOD(OptimizeMeshData& meshBuffers, float prozentigReduseIndices = 0.5f, float errorSimplefly = 1e-2f);
		void ClearVecData();
		void ReisizeMeshVecData(uint32_t count);
		void ReisizeMaterielVecData(uint32_t count);

		void SearchInSourceDataForIdenticalVerteices();
		
		static bool IsIndexPresentInVec(const std::vector<uint32_t>& indexVec, uint32_t index);
	private:
		std::vector<MeshSource::_Material> m_SourceMateriel;
		std::vector<MeshSource::SourceMesh> m_SourceMeshes;
		std::vector<MeshSource::SourceVertex> m_SourceVertexData;
		EntityNodes m_Nodes;

		std::vector<Ref<VertexBuffer>> m_VABvec;

		std::vector<Ref<IndexBuffer>> m_ShadeIABvec;
		std::vector<Ref<IndexBuffer>> m_DepthIABvec;


		std::vector<Ref<MeshSingle>> m_SingleMeshVec;
		std::vector<Ref<Material>> m_MaterialsVec;
#if RY_OLD_INDRECT
		Ref<StorageBuffer> m_StorageBuffer;
		MapVector<int64_t, Ref<Texture>> m_TexturesMap;
		Ref<MeshStatic> m_MeshStatic;
#endif
		std::vector<Mesh::PerDrawObject> m_ShadePerDrawObjectVec;
		std::vector<Mesh::PerDrawObject> m_DepthPerDrawObjectVec;


	private:
		friend Mesh;
		friend MeshSingle;
	};


}



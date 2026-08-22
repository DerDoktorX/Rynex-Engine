#pragma once
#include <Rynex/Renderer/Mesh/Mesh.h>
#include <Rynex/Renderer/Mesh/MeshSingle.h>
#include <Rynex/Renderer/Mesh/MeshSource.h>

#include <Rynex/Renderer/API/VertexArray.h>

#include <Rynex/Renderer/Materials/DefaultMaterial.h>



namespace Rynex {

	
	class StaticMeshSerialzation;

	class RYNEX_API MeshStatic : public Mesh
	{
	public:
		struct SingleObjectMeshData : public SingleMeshObject
		{
			SingleObjectMeshData() = default;
			SingleObjectMeshData(const SingleObjectMeshData&) = default;
			SingleObjectMeshData(const Ref<MeshSingle>& meshSingle, const Ref<Material>& materiel, const glm::mat4& localeMatrix, const std::string& nodeName, uint32_t localeMeshIndex, uint32_t localeMaterielIndex)
				: SingleMeshObject({ materiel, meshSingle })
				, LocaleCildrenMatrix(localeMatrix)
				, NodeName(nodeName)
				, LocaleIndexMesh(localeMeshIndex)
				, LocaleIndexMateriel(localeMaterielIndex)
			{
			}

			glm::mat4		LocaleCildrenMatrix;
			std::string		NodeName;
			uint32_t		LocaleIndexMesh;
			uint32_t		LocaleIndexMateriel;

			const UUID& GetHandle() const;
			const Mesh::PerDrawObject& GetShadePerDrawObjectIndrect() const;
			const Mesh::PerDrawObject& GetDepthPerDrawObjectIndrect() const;

			bool operator==(const SingleObjectMeshData& data) const
			{
				bool rMateriel = this->_Material == data._Material;
				bool rMesh = this->_MeshSingle == data._MeshSingle;
				bool rMatrix = this->LocaleCildrenMatrix == data.LocaleCildrenMatrix;
				bool rNodeName = this->NodeName == data.NodeName;
				bool rIndexMesh = this->LocaleIndexMesh == data.LocaleIndexMesh;
				bool rIndexMateriel = this->LocaleIndexMateriel == data.LocaleIndexMateriel;
				return rMateriel && rMesh && rMatrix && rNodeName && rIndexMesh && rIndexMateriel;
			}

			bool operator!=(const SingleObjectMeshData& data) const
			{
				bool rMateriel = this->_Material == data._Material;
				bool rMesh = this->_MeshSingle == data._MeshSingle;
				bool rMatrix = this->LocaleCildrenMatrix == data.LocaleCildrenMatrix;
				bool rNodeName = this->NodeName != data.NodeName;
				bool rIndexMesh = this->LocaleIndexMesh != data.LocaleIndexMesh;
				bool rIndexMateriel = this->LocaleIndexMateriel != data.LocaleIndexMateriel;
				return rMateriel || rMesh || rMatrix || rNodeName || rIndexMesh || rIndexMateriel;
			}
		};
	public:
		MeshStatic();
		MeshStatic(const Ref<MeshSource>& source);
		MeshStatic(const Ref<MeshSource>& source, const std::vector<SingleObjectMeshData>& singleMeshDatas);
		virtual ~MeshStatic();

		virtual Mesh::Type GetMeshType() const override { return Mesh::Type::Static; }

		bool NeedUpdating() const { return m_Change; }
		const std::vector<Ref<VertexBuffer>>& GetVertexBufferVec() const { return m_VABvec; }
		const std::vector<Ref<IndexBuffer>>& GetShadeIndexBufferVec() const { return m_ShadeIABvec; }
		const std::vector<Ref<IndexBuffer>>& GetDepthIndexBufferVec() const { return m_DepthIABvec; }

		const Ref<StorageBuffer>& GetDefaultMaterialBuffer() const { return m_DefaultMaterialPackage->GetBuffer(); }
		const Ref<BindlesTextureArray>& GetBindlesTexureArray() const { return m_BindlesAlbedoTextureArray; }

		const std::vector<MeshStatic::SingleObjectMeshData>& GetSingleObjectMesDataVec() const { return m_SingleObjectDataVec; }
		const MeshStatic::SingleObjectMeshData& GetSingleObjectData(uint32_t index) const { RY_CORE_ASSERT(index < m_SingleObjectDataVec.size(), "Buffer Overflow"); return m_SingleObjectDataVec.at(index); }
		const Ref<MeshSingle>& GetMeshFromObject(uint32_t index) const { return GetSingleObjectData(index)._MeshSingle; }
		const Ref<Material>& GetMaterialFromObject(uint32_t index) const { return GetSingleObjectData(index)._Material; }
		const glm::mat4& GetChildrenMatriceFromObject(uint32_t index) const { return GetSingleObjectData(index).LocaleCildrenMatrix; }
		const UUID& GetMeshesIDFromMeshObject(uint32_t index) const { return GetSingleObjectData(index).GetHandle(); }
		const Mesh::PerDrawObject& GetPerDrawObjectFromMeshObject(uint32_t index) const { return GetSingleObjectData(index).GetShadePerDrawObjectIndrect(); }



		uint32_t GetSingleMeshObjectCount() const { return m_SingleObjectDataVec.size(); }

		template<typename T, typename N>
		inline void UpdateDataStruct(uint32_t offset, uint32_t size, uint32_t steps, std::vector<T>& globleChildrenMat, const N& dataBuffer, std::function<void(uint32_t, const N&, T&)> func) const
		{
			uint32_t sizeChi = m_LocaleChildrenMat.size();
			RY_CORE_ASSERT(size == sizeChi);

			int32_t givenSize = static_cast<int32_t>(globleChildrenMat.size()) - static_cast<int32_t>(offset);
			int32_t requayerSize = static_cast<int32_t>(size * steps);
			if (givenSize <= requayerSize)
			{
				uint32_t resizeSize = (offset)+(size * steps);
				RY_CORE_WARN("Need Resizeing Vector to {}", resizeSize);
				globleChildrenMat.resize(resizeSize);
			}
			uint32_t sizeGlo = globleChildrenMat.size();
			T* itBuffer = globleChildrenMat.data() + offset;
			T* endBuffer = globleChildrenMat.data() + sizeGlo;
			uint32_t index = 0u;
			for (const glm::mat4& matL : m_LocaleChildrenMat)
			{
				RY_CORE_ASSERT(itBuffer != endBuffer)
				func(index, dataBuffer, *itBuffer);
				itBuffer->Matrix = glm::mat4{ dataBuffer.Matrix * matL };
				itBuffer += steps;
				index++;
			}
		};
		
		template<typename T, typename N>
		inline void UpdateDataStruct(uint32_t offset, uint32_t size, uint32_t steps, std::vector<T>& globleChildrenMat, const N& dataBuffer) const
		{
			UpdateDataStruct<T, N>(offset, size, steps, globleChildrenMat, dataBuffer, [](uint32_t index, const N& dataBuffer, T& globleElment){});
		};

		
		void UpdateData(uint32_t offset, uint32_t size, uint32_t steps, std::vector<glm::mat4>& globleChildrenMat, const glm::mat4& loaleMat) const;

		void UpdateBuffer(Ref<StorageBuffer>& buffer, std::vector<glm::mat4>& globleChildrenMat, const glm::mat4& loaleMat) const;
		void GenarteBuffer(Ref<StorageBuffer>& buffer, std::vector<glm::mat4>& globleChildrenMat, const glm::mat4& glloaleMatobleMat) const;
		void UpdateOffsetData(uint32_t curentOffset, uint32_t offset, uint32_t size, uint32_t steps, std::vector<int>& offsetData)const;
		void AddMeshHandles(std::vector<UUID>& handles, const std::vector<uint32_t>& MeshIndicices)const;
		void AddMeshMateriels(std::vector<Ref<Material>>& materiels, const std::vector<uint32_t>& MeshIndicices)const;
		void AddMeshPerDrawObject(std::vector<Mesh::PerDrawObject>& perDrawObjects, const std::vector<uint32_t>& MeshIndicices)const;
		void AddMeshLocaleChildrensMatrices(std::vector<glm::mat4>& matrices, const std::vector<uint32_t>& MeshIndicices)const;

		Ref<MeshSource> GetMeshSource() const 
		{ 
			Ref<MeshSource> source = m_Source.lock();
			return source; 
		}
	private:
		using MaterielShaderDataMap = typename MapVector<typename UUID, typename MaterielShaderData>;
	private:
		void Generator();

		void SetupShadeVAA();
		void SetupDepthVAA();

		void SetDefaultMateriel(SingleObjectMeshData& singleMeshData, MapVector<UUID, MaterielShaderData>& defaultMaterielMap);
		void SetOffsetObject(const SingleObjectMeshData& singleMeshData, MapVector<UUID, int>& offsetObjectMap, int&  );
		void SetDrawObject(const SingleObjectMeshData& singleMeshData, MapVector<UUID, Mesh::PerDrawObject>& drawObjectMap);
		
		void SetubeOffsetObjectsPackage(MapVector<UUID, int>& offsetObjectMap);
		void SetubeDefaultMaterialPackage(MapVector<UUID, MaterielShaderData>& defaultMaterielMap);
		void SetubePerDrawObjectPackage(MapVector<UUID, Mesh::PerDrawObject>& drawObjectMap);


		void DestroyPackeges();
		

		template<typename T, typename N>
		void SetMeshData(const SingleObjectMeshData& singleMeshData, MapVector<T, N>& map);

		void Create(const Ref<MeshSource>& source, const std::vector<SingleObjectMeshData>& singleMeshDatas);
		void Cear();
		void GenarteMeshDataVec();
		void GenartaSingleMeshVecItrativ(const MeshSource::EntityNodes& sourceNodes, const glm::mat4& parentMatrix);
	private:
		std::vector<Ref<VertexBuffer>>					m_VABvec;
		std::vector<Ref<IndexBuffer>>					m_DepthIABvec;
		std::vector<Ref<IndexBuffer>>					m_ShadeIABvec;
		std::vector<SingleObjectMeshData>	m_SingleObjectDataVec;

		Ref<DefaultMaterial::PackageArrayDynamic>	m_DefaultMaterialPackage;
		Ref<BindlesTextureArray>					m_BindlesAlbedoTextureArray;


		Weak<MeshSource>						m_Source;
		bool									m_Change;
	private:

		friend StaticMeshSerialzation;
	};
}

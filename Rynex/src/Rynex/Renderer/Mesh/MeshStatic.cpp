#include "rypch.h"
#include "MeshStatic.h"

#include <Rynex/Renderer/Rendering/Render3D/Renderer3D.h>
#include <Rynex/Core/Application.h>

namespace Rynex {

	const UUID& MeshStatic::SingleObjectMeshData::GetHandle() const
	{
		RY_CORE_ASSERT(nullptr != _MeshSingle, "Nullptr Created Object!");
		return _MeshSingle->Handle;
	}

	const Mesh::PerDrawObject& MeshStatic::SingleObjectMeshData::GetShadePerDrawObjectIndrect() const
	{
		RY_CORE_ASSERT(nullptr != _MeshSingle, "Nullptr Created Object!");
		return _MeshSingle->GetShadePerDrawObjectIndrect();
	}

	const Mesh::PerDrawObject& MeshStatic::SingleObjectMeshData::GetDepthPerDrawObjectIndrect() const
	{
		RY_CORE_ASSERT(nullptr != _MeshSingle, "Nullptr Created Object!");
		return _MeshSingle->GetDepthPerDrawObjectIndrect();
	}

	MeshStatic::MeshStatic()
		: m_SingleObjectDataVec()
		, m_VABvec()
		, m_ShadeIABvec()
		, m_DepthIABvec()
		, m_Source()
		, m_Change(false)
	{
	}

	MeshStatic::MeshStatic(const Ref<MeshSource>& source, const std::vector<SingleObjectMeshData>& singleMeshDatas)
		: m_SingleObjectDataVec(singleMeshDatas)
		, m_VABvec(source->GetVertexBufferVecConst())
		, m_ShadeIABvec(source->GetShadeIndexBufferVecConst())
		, m_DepthIABvec(source->GetDepthIndexBufferVecConst())
		, m_Source(source)
		, m_Change(false)
	{
		Generator();
	}

	MeshStatic::MeshStatic(const Ref<MeshSource>& source)
		: m_SingleObjectDataVec()
		, m_VABvec(source->GetVertexBufferVecConst())
		, m_ShadeIABvec(source->GetShadeIndexBufferVecConst())
		, m_DepthIABvec(source->GetDepthIndexBufferVecConst())
		, m_Source(source)
		, m_Change(false)
	{
		Generator();
	}


	MeshStatic::~MeshStatic()
	{
		Cear();
	}

	void MeshStatic::GenarteBuffer(Ref<StorageBuffer>& buffer, std::vector<glm::mat4>& globleChildrenMat, const glm::mat4& globleMat) const
	{
		globleChildrenMat.reserve(m_SingleObjectDataVec.size());
		for (const SingleObjectMeshData& meshSingle : m_SingleObjectDataVec)
		{
			const glm::mat4& matL = meshSingle.LocaleCildrenMatrix;
			globleChildrenMat.emplace_back<glm::mat4>(glm::mat4{ globleMat * matL });
		}
		buffer = StorageBuffer::Create(globleChildrenMat.data(), globleChildrenMat.size() * sizeof(glm::mat4), BufferFlag::Dynamic);
	}

	void MeshStatic::UpdateOffsetData(uint32_t curentOffset,uint32_t offset, uint32_t size, uint32_t steps, std::vector<int>& offsetData) const
	{
		uint32_t sizeChi = m_SingleObjectDataVec.size();
		RY_CORE_ASSERT(size == sizeChi);

		int givenSize = static_cast<int>(offsetData.size()) - static_cast<int>(offset);
		if (givenSize <= static_cast<int>(size))
		{
			uint32_t resizeSize = (offset) + (size * steps);
			RY_CORE_WARN("Need Resizeing Offset Vector to {}", resizeSize);
			offsetData.resize(resizeSize);
		}

		int* it = offsetData.data() + offset;
		for (const SingleObjectMeshData& meshSingle : m_SingleObjectDataVec)
		{
			const Mesh::PerDrawObject& drawIndrect = meshSingle._MeshSingle->GetShadePerDrawObjectIndrect();
			const uint32_t& instaencesCount = drawIndrect.InstancesCount;
			*it = curentOffset; 
			curentOffset += instaencesCount * steps;
			it++;
		}
	}

	void MeshStatic::AddMeshHandles(std::vector<UUID>& handles, const std::vector<uint32_t>& MeshIndicices) const
	{
	}

	void MeshStatic::AddMeshMateriels(std::vector<Ref<Material>>& materiels, const std::vector<uint32_t>& MeshIndicices) const
	{
	}

	void MeshStatic::AddMeshPerDrawObject(std::vector<Mesh::PerDrawObject>& perDrawObjects, const std::vector<uint32_t>& MeshIndicices) const
	{
	}

	void MeshStatic::AddMeshLocaleChildrensMatrices(std::vector<glm::mat4>& matrices, const std::vector<uint32_t>& MeshIndicices) const
	{
	}

	void MeshStatic::Generator()
	{
		DestroyPackeges();
		GenarteMeshDataVec();
	}

	void MeshStatic::SetupDepthVAA()
	{
		Ref<MeshSource> source = m_Source.lock();
		RY_CORE_ASSERT(source != nullptr);
		m_DepthIABvec = source->GetDepthIndexBufferVecConst();
	}

	void MeshStatic::SetupShadeVAA()
	{
		Ref<MeshSource> source = m_Source.lock();
		RY_CORE_ASSERT(source != nullptr);
		m_ShadeIABvec = source->GetShadeIndexBufferVecConst();
	}

	void MeshStatic::SetDefaultMateriel(SingleObjectMeshData& singleMeshData, MapVector<UUID, MaterielShaderData>& defaultMaterielMap)
	{
		const Ref<Material>& materiel = singleMeshData._Material;
		singleMeshData.LocaleIndexMateriel = Material::SetupMaterielObjectMapVector<UUID, MaterielShaderData>(materiel, defaultMaterielMap, m_BindlesAlbedoTextureArray);
	}

	void MeshStatic::SetOffsetObject(const SingleObjectMeshData& singleMeshData, MapVector<UUID, int>& offsetObjectMap, int& offsetValue)
	{
		const UUID& meshHandle = singleMeshData._MeshSingle->Handle;
		if (!offsetObjectMap.HasKey(meshHandle))
		{
			int size = 0;
			offsetObjectMap.AddData(meshHandle, size);
		}
		else
		{
			int& offsetValueRef = offsetObjectMap.GetDataFromKey(meshHandle);
			offsetValueRef++;
		}
	}

	void MeshStatic::SetDrawObject(const SingleObjectMeshData& singleMeshData, MapVector<UUID, Mesh::PerDrawObject>& drawObjectMap)
	{
		const UUID& meshHandle = singleMeshData._MeshSingle->Handle;
		if (!drawObjectMap.HasKey(meshHandle))
		{
			const Mesh::PerDrawObject& cmd = singleMeshData.GetShadePerDrawObjectIndrect();
			
			Mesh::PerDrawObject& cmdRef = drawObjectMap.AddData(meshHandle, cmd);
			cmdRef.InstancesCount = 1;
		}
		else
		{
			Mesh::PerDrawObject& cmdRef = drawObjectMap.GetKeyData(meshHandle);
			cmdRef.InstancesCount++;
		}
	}

	void MeshStatic::SetubeOffsetObjectsPackage(MapVector<UUID, int>& offsetObjectMap)
	{
		int offset = 0;
		for (int& offsetRef : offsetObjectMap)
		{
			int copyoffset = offset + offsetRef;
			offsetRef = offset;
			offset = copyoffset + 1;
		}
		std::vector<int>& vecOffsetObject = offsetObjectMap.GetVector();

		uint32_t count = vecOffsetObject.size();
		uint32_t byteSize = count * sizeof(int);
		const void* dataPtr = vecOffsetObject.data();

	}
#if 0
	void MeshStatic::SetubeDefaultMaterialPackage(MapVector<UUID, MaterielShaderData>& defaultMaterielMap)
	{
		std::vector<MaterielShaderData>& vecDefaultMateriel = defaultMaterielMap.GetVector();

		uint32_t count = vecDefaultMateriel.size();
		uint32_t byteSize = count * sizeof(MaterielShaderData);
		const void* dataPtr = vecDefaultMateriel.data();

		Ref<StorageBuffer> materielBuffer = StorageBuffer::Create(dataPtr, byteSize, BufferFlag::None);
		m_DefaultMaterialPackage = CreateRef<DefaultMaterial::PackageArrayDynamic>(materielBuffer, vecDefaultMateriel);
	}
#endif
	void MeshStatic::SetubePerDrawObjectPackage(MapVector<UUID, Mesh::PerDrawObject>& drawObjectMap)
	{
		std::vector<Mesh::PerDrawObject>& vecDrawObjectMap = drawObjectMap.GetVector();

		uint32_t count = vecDrawObjectMap.size();
		uint32_t byteSize = count * sizeof(Mesh::PerDrawObject);
		const void* dataPtr = vecDrawObjectMap.data();

	}

	void MeshStatic::DestroyPackeges()
	{
		RY_DESTROY_REF(m_BindlesAlbedoTextureArray);
#if 0
		RY_DESTROY_REF(m_DefaultMaterialPackage);
#endif
	}

	void MeshStatic::Create(const Ref<MeshSource>& source, const std::vector<SingleObjectMeshData>& singleMeshDatas)
	{
		m_SingleObjectDataVec = singleMeshDatas;
		m_VABvec = source->GetVertexBufferVecConst();
		m_ShadeIABvec = source->GetShadeIndexBufferVecConst();
		m_DepthIABvec = source->GetDepthIndexBufferVecConst();
		m_Source = source;
		
	}

	void MeshStatic::Cear()
	{
		m_VABvec.clear();
		m_ShadeIABvec.clear();
		m_DepthIABvec.clear();
		uint32_t i = 0;
		for (SingleObjectMeshData& meshData : m_SingleObjectDataVec)
		{
			RY_DESTROY_REF(meshData._Material);
			i++;
		}
		m_SingleObjectDataVec.clear();

		DestroyPackeges();

	}

	void MeshStatic::GenarteMeshDataVec()
	{
		Ref<MeshSource> source = m_Source.lock();
		RY_CORE_ASSERT(source != nullptr);

		glm::mat4 matrix = glm::mat4(1.0f);
		const MeshSource::EntityNodes& sourceNodes = source->GetNodes();
		GenartaSingleMeshVecItrativ(sourceNodes, matrix);
	}

	void MeshStatic::GenartaSingleMeshVecItrativ(const MeshSource::EntityNodes& sourceNodes, const glm::mat4& parentMatrix)
	{
		Ref<MeshSource> source = m_Source.lock();
		RY_CORE_ASSERT(source != nullptr);

		const std::vector<MeshSource::EntityNodes>& childrens = sourceNodes.Childrens;
		glm::mat4 globleMatrix = parentMatrix * sourceNodes.Matrics;
		const std::string& nameNode = sourceNodes.NodeName;
		const std::vector<MeshSource::SourceMesh>& sourceMeshesVec = source->GetMeshSourcesConst();
		const std::vector<Ref<MeshSingle>>& singleMeshVec = source->GetMeshSingleVecConst();
		const std::vector<Ref<Material>>& materielVec = source->GetMaterialsVecConst();


		for (const uint32_t& meshIndex : sourceNodes.ObjectMeshIndexVec)
		{
			const MeshSource::SourceMesh& meshSource = sourceMeshesVec.at(meshIndex);
			uint32_t indexMateriel = meshSource.MaterielIndex;
			uint32_t meshDataIndex = meshSource.MeshDataIndex;
			RY_CORE_ASSERT(meshIndex == meshSource.MeshIndex, "Index Are not expexted Equel!");

			const Ref<Material>& materiel = materielVec.at(indexMateriel);
			const Ref<MeshSingle>& meshSingle = singleMeshVec.at(meshDataIndex);
			m_SingleObjectDataVec.emplace_back(meshSingle, materiel, globleMatrix, nameNode, meshDataIndex, indexMateriel);
		}
		for (const MeshSource::EntityNodes& node : childrens)
		{
			GenartaSingleMeshVecItrativ(node, globleMatrix);
		}
	}

	template<typename T, typename N>
	void MeshStatic::SetMeshData(const SingleObjectMeshData& singleMeshData, MapVector<T, N>& map)
	{
		static_assert(false);
	}

	template<>
	void MeshStatic::SetMeshData<UUID, MaterielShaderData>(const SingleObjectMeshData& singleMeshData, MapVector<UUID, MaterielShaderData>& defaultMaterielMap)
	{
		const Ref<Material>& materiel = singleMeshData._Material;
		Material::SetupMaterielObjectMapVector<UUID, MaterielShaderData>(materiel, defaultMaterielMap, m_BindlesAlbedoTextureArray);
	}

	void MeshStatic::UpdateBuffer(Ref<StorageBuffer>& buffer, std::vector<glm::mat4>& globleChildrenMat, const glm::mat4& globleMat) const
	{
		globleChildrenMat.clear();
		for (const SingleObjectMeshData& meshSingle : m_SingleObjectDataVec)
		{
			const glm::mat4& matL = meshSingle.LocaleCildrenMatrix;
			globleChildrenMat.emplace_back<glm::mat4>(glm::mat4{ globleMat * matL });
		}
		buffer->SetData(globleChildrenMat.data(), globleChildrenMat.size() * sizeof(glm::mat4));
	}


	void MeshStatic::UpdateData(uint32_t offset, uint32_t size, uint32_t steps, std::vector<glm::mat4>& globleChildrenMat, const glm::mat4& loaleMat) const
	{
		uint32_t sizeChi = m_SingleObjectDataVec.size();
		RY_CORE_ASSERT(size == sizeChi);
		
		int32_t givenSize = static_cast<int32_t>(globleChildrenMat.size()) - static_cast<int32_t>(offset);
		if (givenSize <= static_cast<int32_t>(size))
		{
			uint32_t resizeSize = (offset) + (size * steps);
			RY_CORE_WARN("Need Resizeing Vector to {}", resizeSize);
			globleChildrenMat.resize(resizeSize);
		}

		glm::mat4* it = globleChildrenMat.data() + offset;

		for (const SingleObjectMeshData& meshSingle : m_SingleObjectDataVec)
		{
			const glm::mat4& matL = meshSingle.LocaleCildrenMatrix;
			*it = glm::mat4{ loaleMat * matL };
			it += steps;
		}
	}	

	

}
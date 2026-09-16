#pragma once
#include <rypch.h>
#include <Rynex/Renderer/Mesh/MeshSingle.h>
#include <Rynex/Renderer/Materials/Material.h>
namespace Rynex{
	// #define RY_USE_SINGLE_DATA_STRUCTS_FOR_DATA


	using RenderProxyKey = uint64_t;
	struct RenderMeshBatch
	{
		Ref<MeshSingle> mesh = nullptr;
		Ref<Material> materiel = nullptr;

		void Check() const
		{
			RY_CORE_ASSERT(nullptr != mesh, "Invaild Proxy Mesh!");
			RY_CORE_ASSERT(nullptr != materiel, "Invaild Proxy Mesh!");
		}
		
		RenderProxyKey GetKey() const
		{
			Ref<Texture> texture = materiel->GetAlbedoTextures();
			uint64_t keyTextur = robin_hood::hash<void*>{}(texture.get());
			uint64_t keyMateril = robin_hood::hash<void*>{}(materiel.get());
			uint64_t keyMesh = robin_hood::hash<void*>{}(mesh.get());
			uint64_t key = keyTextur;
			key ^= keyMateril;
			key ^= keyMesh;
			return key;
		}

	};

	struct StaticMeshInstanceBatchRenderProxy
	{
		RenderMeshBatch m_MeshBatch;

		std::vector<glm::mat4> n_Model;
		std::vector<glm::mat4> m_Normale;
		int m_Entity;
		
	};

	struct RenderProxy
	{
		RenderMeshBatch m_MeshBatch;
#ifndef RY_USE_SINGLE_DATA_STRUCTS_FOR_DATA
		glm::mat4 m_Model;
#endif
		int m_Entity;
		uint32_t m_SubMesh;

		void Check() const
		{
			m_MeshBatch.Check();
			RY_CORE_ASSERT(-1 != m_Entity, "Invaild Proxy Mesh!");
			RY_CORE_ASSERT(std::numeric_limits<uint32_t>::max() != m_SubMesh, "Invaild Proxy Mesh!");
		}

		

		RenderProxyKey GetKey() const { return m_MeshBatch.GetKey(); }

		Ref<Material>& GetMaterial() { return m_MeshBatch.materiel; }
		Ref<MeshSingle>& GetMesh() { return m_MeshBatch.mesh; }

		const Ref<Material>& GetMaterial() const { return m_MeshBatch.materiel; }
		const Ref<MeshSingle>& GetMesh() const { return m_MeshBatch.mesh; }
	};

	struct RenderProxyRef
	{
		int& m_EntityRef;
		uint32_t& m_SubMeshRef;
		Ref<MeshSingle>& m_MeshRef;
		Ref<Material>& m_MaterielRef;
	};

	struct ProxyGroupViewConst
	{
		RenderProxy const* m_DataPtr = nullptr;
		const uint32_t    m_Count = 0;
		const RenderMeshBatch m_RenderMeshBatch;
		

		const RenderProxy* begin() const
		{
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			return m_DataPtr;
		}

		const RenderProxy* end()   const
		{
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			return m_DataPtr + m_Count;
		}

		bool     Empty() const { return 0 ==  m_Count; }
		uint32_t Size()  const { return m_Count; }

		const RenderProxy& operator[](uint32_t i) const
		{
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			RY_CORE_ASSERT(i < m_Count);

			return m_DataPtr[i];
		}

		const RenderProxy& At(uint32_t i) const
		{
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			return m_DataPtr[i];
		}
	};

	struct ProxyGroupView
	{
		RenderProxy* m_DataPtr = nullptr;
		const uint32_t     m_Count = 0;
		RenderMeshBatch& m_RenderMeshBatch;
		ProxyGroupView(RenderProxy* data, const uint32_t count)
			: m_DataPtr(data)
			, m_Count(count)
			, m_RenderMeshBatch(data->m_MeshBatch)
		{
		}

		ProxyGroupView(RenderProxy* data, const uint32_t count, RenderMeshBatch& meshBatch)
			: m_DataPtr(data)
			, m_Count(count)
			, m_RenderMeshBatch(meshBatch)
		{
		}
		RenderProxy* begin() 
		{ 
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			return m_DataPtr;
		}

		const RenderProxy* begin() const 
		{
			RY_CORE_ASSERT(nullptr == m_DataPtr);
			return m_DataPtr;
		}

		RenderProxy* end() 
		{
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			return m_DataPtr + m_Count;
		}

		const RenderProxy* end()   const 
		{ 
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			return m_DataPtr + m_Count;
		}

		bool     Empty() const { return 0 == m_Count; }
		uint32_t Size()  const { return m_Count; }

		RenderProxy& First()
		{
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			RY_CORE_ASSERT(0 < m_Count);
			return m_DataPtr[0];
		}

		const RenderProxy& First() const
		{
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			RY_CORE_ASSERT(0 < m_Count);
			return m_DataPtr[0];
		}
		RenderProxy& Last()
		{
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			uint32_t lastIndex = m_Count - 1;
			return m_DataPtr[lastIndex];
		}

		const RenderProxy& Last() const
		{
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			uint32_t lastIndex = m_Count - 1;
			return m_DataPtr[lastIndex];
		}

		RenderProxy& operator[](uint32_t i) 
		{ 
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			RY_CORE_ASSERT(i < m_Count);

			return m_DataPtr[i];
		}

		const RenderProxy& operator[](uint32_t i) const
		{ 
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			RY_CORE_ASSERT(i < m_Count);

			return m_DataPtr[i];
		}

		RenderProxy& At(uint32_t i) 
		{ 
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			return m_DataPtr[i];
		}
		const RenderProxy& At(uint32_t i) const 
		{ 
			RY_CORE_ASSERT(nullptr != m_DataPtr);
			return m_DataPtr[i];
		}
	};

	
}
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
		RenderMeshBatch meshBatch;

		std::vector<glm::mat4> model;
		std::vector<glm::mat4> normale;
		int entity;
		
	};

	struct RenderProxy
	{
		RenderMeshBatch meshBatch;
#ifndef RY_USE_SINGLE_DATA_STRUCTS_FOR_DATA
		glm::mat4 model;	
#endif
		int entity;
		uint32_t subMesh;

		void Check() const
		{
			meshBatch.Check();
			RY_CORE_ASSERT(-1 != entity, "Invaild Proxy Mesh!");
			RY_CORE_ASSERT(MAXUINT32 != subMesh, "Invaild Proxy Mesh!");
		}

		

		RenderProxyKey GetKey() const { return meshBatch.GetKey(); }

		Ref<Material>& GetMaterial() { return meshBatch.materiel; }
		Ref<MeshSingle>& GetMesh() { return meshBatch.mesh; }

		const Ref<Material>& GetMaterial() const { return meshBatch.materiel; }
		const Ref<MeshSingle>& GetMesh() const { return meshBatch.mesh; }
	};

	struct RenderProxyRef
	{
		int& entityRef;
		uint32_t& subMesh;
		Ref<MeshSingle>& meshRef;
		Ref<Material>& materielRef;
	};

	struct ProxyGroupViewConst
	{
		RenderProxy const* data = nullptr;
		const uint32_t    count = 0;
		const RenderMeshBatch renderMeshBatch;
		

		const RenderProxy* begin() const
		{
			RY_CORE_ASSERT(nullptr != data);
			return data;
		}

		const RenderProxy* end()   const
		{
			RY_CORE_ASSERT(nullptr != data);
			return data + count;
		}

		bool     Empty() const { return 0 ==  count; }
		uint32_t Size()  const { return count; }

		const RenderProxy& operator[](uint32_t i) const
		{
			RY_CORE_ASSERT(nullptr != data);
			RY_CORE_ASSERT(i < count);

			return data[i];
		}

		const RenderProxy& At(uint32_t i) const
		{
			RY_CORE_ASSERT(nullptr != data);
			return data[i];
		}
	};

	struct ProxyGroupView
	{
		RenderProxy* data = nullptr;
		const uint32_t     count = 0;
		RenderMeshBatch& renderMeshBatch;
		ProxyGroupView(RenderProxy* data, const uint32_t count)
			: data(data)
			, count(count)
			, renderMeshBatch(data->meshBatch)
		{
		}

		ProxyGroupView(RenderProxy* data, const uint32_t count, RenderMeshBatch& meshBatch)
			: data(data)
			, count(count)
			, renderMeshBatch(meshBatch)
		{
		}
		RenderProxy* begin() 
		{ 
			RY_CORE_ASSERT(nullptr != data);
			return data;
		}

		const RenderProxy* begin() const 
		{
			RY_CORE_ASSERT(nullptr == data);
			return data; 
		}

		RenderProxy* end() 
		{
			RY_CORE_ASSERT(nullptr != data);
			return data + count;
		}

		const RenderProxy* end()   const 
		{ 
			RY_CORE_ASSERT(nullptr != data);
			return data + count; 
		}

		bool     Empty() const { return 0 == count; }
		uint32_t Size()  const { return count; }

		RenderProxy& First()
		{
			RY_CORE_ASSERT(nullptr != data);
			RY_CORE_ASSERT(0 < count);
			return data[0];
		}

		const RenderProxy& First() const
		{
			RY_CORE_ASSERT(nullptr != data);
			RY_CORE_ASSERT(0 < count);
			return data[0];
		}
		RenderProxy& Last()
		{
			RY_CORE_ASSERT(nullptr != data);
			uint32_t lastIndex = count - 1;
			return data[lastIndex];
		}

		const RenderProxy& Last() const
		{
			RY_CORE_ASSERT(nullptr != data);
			uint32_t lastIndex = count - 1;
			return data[lastIndex];
		}

		RenderProxy& operator[](uint32_t i) 
		{ 
			RY_CORE_ASSERT(nullptr != data);
			RY_CORE_ASSERT(i < count);

			return data[i]; 
		}

		const RenderProxy& operator[](uint32_t i) const
		{ 
			RY_CORE_ASSERT(nullptr != data);
			RY_CORE_ASSERT(i < count);

			return data[i]; 
		}

		RenderProxy& At(uint32_t i) 
		{ 
			RY_CORE_ASSERT(nullptr != data);
			return data[i]; 
		}
		const RenderProxy& At(uint32_t i) const 
		{ 
			RY_CORE_ASSERT(nullptr != data);
			return data[i];
		}
	};

	
}
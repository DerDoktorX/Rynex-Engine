#pragma once
#include <Rynex/Memory/DynamicDataStruct.h>

#include <Rynex/Renderer/Rendering/StoreSubmite.h>
#include <Rynex/Renderer/Rendering/Batch3DKey.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchDataStructs.h>

#define RY_USE_BATCH_RENDERER_POOL
#define RY_BATCHING_UPDATE_BUFFER_RANGE_BASED
namespace Rynex {

	enum class RenderProxyDynamicEllmenenttData
	{
		None = 0,
		EntityID,
		TransformMatrix,
		NormaleMatrix,

		MaterilIndex,
		Texture_0_Index,
		Texture_1_Index,
		Texture_2_Index,
		Texture_3_Index,
		Texture_4_Index,
		Texture_5_Index,
		Texture_6_Index,
		Texture_7_Index,

		Empty_Int_1,
		Empty_Int_2,
		Empty_Int_3,
		Empty_Int_4,

		Empty_Float_1,
		Empty_Float_2,
		Empty_Float_3,
		Empty_Float_4,

		Empty_Uint_1,
		Empty_Uint_2,
		Empty_Uint_3,
		Empty_Uint_4

	};

	struct BatchedRenderObjectVAO
	{
		Ref<VertexArray> vertexArray;
		Ref<VertexBuffer> instanceObjectBuffer;
		Ref<UniformBuffer> materielUB;
#if 0	
		std::vector<Batch::Render3DMeshObject> rendeObjectVec;
#else
		std::vector<Batch::Render3DMeshObject> rendeObjectVec;
		std::vector<Batch::Render3DMeshObjectTrasform> rendeObjectTransformVec;

		Memory::DynamicDataStruct rendeObjectDynamicElementStruct;
		std::vector<RenderProxyDynamicEllmenenttData> renderProxyDynamicDataTypeVec;
#endif
		uint32_t curentIndex;
		bool needUpdate;



		void Reset()
		{
			curentIndex = 0u;
			needUpdate = false;
#ifndef RY_BATCHING_UPDATE_BUFFER_RANGE_BASED
			rendeObjectVec.clear();
#endif

		}

		void AddRenderObject(const Batch::Render3DMeshObject& renderObject)
		{
#ifdef RY_BATCHING_UPDATE_BUFFER_RANGE_BASED
			uint32_t count = rendeObjectVec.size();
			if (count <= curentIndex)
			{
				rendeObjectVec.emplace_back(renderObject);
				needUpdate = true;
				count = rendeObjectVec.size();
				RY_CORE_ASSERT(curentIndex < count, "Index is not samler then count! buffer overflow");

			}
			else
			{
				Batch::Render3DMeshObject& object = rendeObjectVec.at(curentIndex);
				if(object != renderObject)
				{
					object = renderObject;
					needUpdate = true;
				}
			}
#else
			rendeObjectVec.emplace_back(renderObject);
#endif
			curentIndex++;
		}

		void AddRenderObject(const Batch::Render3DMeshObjectTrasform& renderObjectTrasform)
		{
#ifdef RY_BATCHING_UPDATE_BUFFER_RANGE_BASED
			uint32_t count = rendeObjectTransformVec.size();
			if (count <= curentIndex)
			{
				rendeObjectTransformVec.emplace_back(renderObjectTrasform);
				needUpdate = true;
				count = rendeObjectTransformVec.size();

				RY_CORE_ASSERT(curentIndex < count, "Index is not samler then count! buffer overflow");
			}
			else
			{
				Batch::Render3DMeshObjectTrasform& objectTrasform = rendeObjectTransformVec.at(curentIndex);
				if (objectTrasform != renderObjectTrasform)
				{
					objectTrasform = renderObjectTrasform;
					needUpdate = true;
				}

			}
#else
			rendeObjectVec.emplace_back(renderObject);
#endif
			curentIndex++;
		}


	};

	struct BatchedTexture
	{
		Ref<LinkedTextureArray> linkedTextureArray;
		uint32_t curentIndex;

		void Reset()
		{
			curentIndex = 0u;
		}

		void AddTexture(const Ref<Texture>& texture)
		{
#ifdef RY_BATCHING_UPDATE_BUFFER_RANGE_BASED
			if (nullptr == linkedTextureArray)
			{
				RY_CORE_ASSERT(nullptr != texture);
				TextureSpecification spec = texture->GetSpecification();
				spec.Target = TextureTarget::Texture2D_Array;
				linkedTextureArray = LinkedTextureArray::Create(spec);
			}
			if (linkedTextureArray->GetTextureCount() <= curentIndex)
			{
				linkedTextureArray->ResizeTextureArray(curentIndex+1);
			}
			linkedTextureArray->SetTextureToArray(curentIndex, texture);
#else
			rendeObjectVec.emplace_back(renderObject);
#endif
			curentIndex++;
		}


	};


#ifdef RY_USE_BATCH_RENDERER_POOL
	using BatchVAO = Memory::StoreSubmite<BatchedRenderObjectVAO>;
	using BatchTexture = Memory::StoreSubmite<BatchedTexture>;
#else
	using BatchVAO = BatchedRenderObjectVAO;
	using BatchVAO = BatchedTexture;
#endif


	class BatchManager
	{
	public:
		using HashMapVAO = HashMapFlat<Batch3DKey, BatchVAO>;
		using ItHashMapVAO = HashMapVAO::iterator;

		using HashMapTexture = HashMapFlat<Batch3DKey, BatchTexture>;
		using ItHashMapTexture = HashMapTexture::iterator;
	public:
		BatchManager();
		~BatchManager();

		BatchVAO* GetBatchPtrVAO(const Batch3DKey& key);
		BatchVAO& GetBatchVAO(const Batch3DKey& key);
#if 0
		BatchVAO* GetBatchPtrVAO(const Batch3DKey& key);
		BatchVAO& GetBatchVAO(const Batch3DKey& key);

		BatchTexture* GetBatchPtrTexture(const Batch3DKey& key);
		BatchTexture& GetBatchTexture(const Batch3DKey& key);
#endif
		BatchTexture* GetBatchPtrTexture(const Batch3DKey& key);
		BatchTexture& GetBatchTexture(const Batch3DKey& key);


		void Clear();
	private:
		HashMapVAO m_VertexAtributeObjectHashMap;
		HashMapTexture m_LinkedTextureArrayHashMap;
	};
}
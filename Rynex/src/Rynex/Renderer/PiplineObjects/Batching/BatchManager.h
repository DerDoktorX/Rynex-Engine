#pragma once
#include <Rynex/Memory/DynamicDataStruct.h>

#include <Rynex/Renderer/Rendering/StoreSubmite.h>
#include <Rynex/Renderer/Rendering/Batch3DKey.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchDataStructs.h>

#define RY_USE_BATCH_RENDERER_POOL
#define RY_BATCHING_UPDATE_BUFFER_RANGE_BASED
namespace Rynex {

	enum class RenderProxyDynamicElenenttData
	{
		None = 0,
		EntityID,
		TransformMatrix,
		NormaleMatrix,

		MaterielIndex,
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
		std::vector<Batch::Render3DMeshObject> renderObjectVec;
		std::vector<Batch::Render3DMeshObjectTransform> renderObjectTransformVec;

		Memory::DynamicDataStruct renderObjectDynamicElementStruct;
		std::vector<RenderProxyDynamicElenenttData> renderProxyDynamicDataTypeVec;
		uint32_t currentIndex;
		bool needUpdate;



		void Reset()
		{
			currentIndex = 0u;
			needUpdate = false;
		}

		void AddRenderObject(const Batch::Render3DMeshObject& renderObject)
		{
			uint32_t count = renderObjectVec.size();
			if (count <= currentIndex)
			{
				renderObjectVec.emplace_back(renderObject);
				needUpdate = true;
				count = renderObjectVec.size();
				RY_CORE_ASSERT(currentIndex < count, "Index is not sampler then count! buffer overflow");

			}
			else
			{
				Batch::Render3DMeshObject& object = renderObjectVec.at(currentIndex);
				if(object != renderObject)
				{
					object = renderObject;
					needUpdate = true;
				}
			}
			currentIndex++;
		}

		void AddRenderObject(const Batch::Render3DMeshObjectTransform& renderObjectTransform)
		{
			uint32_t count = renderObjectTransformVec.size();
			if (count <= currentIndex)
			{
				renderObjectTransformVec.emplace_back(renderObjectTransform);
				needUpdate = true;
				count = renderObjectTransformVec.size();

				RY_CORE_ASSERT(currentIndex < count, "Index is not sampler then count! buffer overflow");
			}
			else
			{
				Batch::Render3DMeshObjectTransform& objectTransform = renderObjectTransformVec.at(currentIndex);
				if (objectTransform != renderObjectTransform)
				{
					objectTransform = renderObjectTransform;
					needUpdate = true;
				}

			}
			currentIndex++;
		}


	};

	struct BatchedTexture
	{
		Ref<LinkedTextureArray> linkedTextureArray;
		uint32_t currentIndex;

		void Reset()
		{
			currentIndex = 0u;
		}

		void AddTexture(const Ref<Texture>& texture)
		{
			if (nullptr == linkedTextureArray)
			{
				RY_CORE_ASSERT(nullptr != texture);
				TextureSpecification spec = texture->GetSpecification();
				spec.m_Target = TextureTarget::Texture2D_Array;
				linkedTextureArray = LinkedTextureArray::Create(spec);
			}
			if (linkedTextureArray->GetTextureCount() <= currentIndex)
			{
				linkedTextureArray->ResizeTextureArray(currentIndex+1);
			}
			linkedTextureArray->SetTextureToArray(currentIndex, texture);
			currentIndex++;
		}


	};


	using BatchVAO = Memory::StoreSubmite<BatchedRenderObjectVAO>;
	using BatchTexture = Memory::StoreSubmite<BatchedTexture>;


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

		BatchTexture* GetBatchPtrTexture(const Batch3DKey& key);
		BatchTexture& GetBatchTexture(const Batch3DKey& key);


		void Clear();
	private:
		HashMapVAO m_VertexAttributeObjectHashMap;
		HashMapTexture m_LinkedTextureArrayHashMap;
	};
}
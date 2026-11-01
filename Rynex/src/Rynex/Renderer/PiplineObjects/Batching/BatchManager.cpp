#include <rypch.h>
#include "BatchManager.h"


namespace Rynex {
	BatchManager::BatchManager()
	{
	}

	BatchManager::~BatchManager()
	{
	}

	BatchVAO* BatchManager::GetBatchPtrVAO(const Batch3DKey& key)
	{
		BatchVAO* storeSubmite = nullptr;
		try 
		{
			storeSubmite = &(m_VertexAttributeObjectHashMap[key]);
		}
		catch (std::overflow_error e) 
		{
			RY_CORE_FATAL("Exception: hit in render loop! {}", e.what());
			m_VertexAttributeObjectHashMap = std::move(HashMapVAO());
			RY_CORE_INFO("We Replaced the hash map and about Rendering!");
			return nullptr;
		}
		return storeSubmite;
	}

	BatchVAO& BatchManager::GetBatchVAO(const Batch3DKey& key)
	{
		return m_VertexAttributeObjectHashMap[key];
	}

	BatchTexture* BatchManager::GetBatchPtrTexture(const Batch3DKey& key)
	{
		BatchTexture* storeSubmite = nullptr;
		try
		{
			storeSubmite = &(m_LinkedTextureArrayHashMap[key]);
		}
		catch (std::overflow_error e) 
		{
			RY_CORE_FATAL("Exaption: hit in render loop! {}", e.what());
			m_LinkedTextureArrayHashMap = std::move(HashMapTexture());
			RY_CORE_INFO("We Replaced the hash map and abourt Rendering!");
			return nullptr;
		}
		return storeSubmite;
	}

	BatchTexture& BatchManager::GetBatchTexture(const Batch3DKey& key)
	{
		return m_LinkedTextureArrayHashMap[key];
	}

	void BatchManager::Clear()
	{
		m_VertexAttributeObjectHashMap.clear();
		m_LinkedTextureArrayHashMap.clear();
	}
}
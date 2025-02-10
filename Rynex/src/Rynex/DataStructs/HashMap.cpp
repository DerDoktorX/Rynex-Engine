#include "rypch.h"
#include "HashMap.h"

namespace Rynex {

	HashMap::HashMap()
	{
	}

	HashMap::~HashMap()
	{
	}

	void HashMap::Resize(uint32_t size)
	{
		if (m_DataSize < size)
		{
			uint32_t nextDataSize = size;
			uint32_t nextMapSize = nextDataSize * 10;

			MapKeyPtr* heshMapPtr = m_HeshMap;
			int* mapDataPtr = m_MapData;
			m_HeshMap = nullptr;
			m_MapData = nullptr;

			m_HeshMap = new MapKeyPtr[nextMapSize];
			m_MapData = new int[nextDataSize];
			
			if (heshMapPtr != nullptr)
			{
				MapKeyPtr* baseMapData = heshMapPtr;
				for (uint32_t i = 0; i < m_MapSize; i++)
				{
					m_HeshMap[i].DataPtr = m_MapData + (mapDataPtr[i] - m_DataSize);
				}
				delete[] heshMapPtr;
			}
			
			
			if (mapDataPtr != nullptr)
			{
				std::memcpy(m_MapData, mapDataPtr, m_ActiveSizeData);
				delete[] mapDataPtr;
			}
			else
			{
				for (uint32_t i = 0; i < nextDataSize; i++)
				{
					m_MapData[i] = 0;
				}
			}
			m_DataSize = nextDataSize;
			m_MapSize = nextMapSize;

		}
	}

	int HashMap::GetData(UUID key)
	{
		int index = key % m_MapSize;
		MapKeyPtr& keyData = m_HeshMap[index];

		switch (keyData.KeyMatch(key))
		{
		case KeyResults::None:
			break;
		case KeyResults::KeyMatch:
			break;

		case KeyResults::KeysNotMatch:
			break;

		case KeyResults::NoKeySet:
			break;

		default:
			RY_CORE_ASSERT(false);
			break;
		}
			

		int data = *keyData.DataPtr;
		return data;
	}

	void HashMap::AddData(UUID key, int data)
	{
	}

	void HashMap::SetData(UUID key, int data)
	{
	}

	void HashMap::RemoveData(UUID key)
	{
	}

	void HashMap::MapData(int* keyDataPtr)
	{
	}

	
	const HashMap::KeyResults HashMap::MapKeyPtr::KeyMatch(UUID key) const
	{
		
			if (DataPtr == nullptr && Key == 0)
				return KeyResults::NoKeySet;
			else if (DataPtr != nullptr && Key == key)
				return KeyResults::KeyMatch;
			else if (DataPtr != nullptr && Key == key)
				return KeyResults::KeysNotMatch;

			int ptrAdresse = int(DataPtr);
			uint64_t origKey = Key;
			uint64_t compareKey = key;
			RY_CORE_FATAL("PTR-Data-Adresse: {} / Defined Key is {} / Compare Key is {}", ptrAdresse, origKey, compareKey);
			RY_CORE_ASSERT(false);
			return KeyResults::None;
		
	}

}
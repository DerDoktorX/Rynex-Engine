#include "rypch.h"
#include "HashPtrTable.h"

#pragma once
namespace Rynex {

	HashPtrTable::HashPtrTable()
		: m_UseSize(0)
		, m_Count(0)
	{
	}

	HashPtrTable::HashPtrTable(uint32_t size)
		: m_UseSize(0)
		, m_Count(0)
		, m_Table(size)
	{
		for (auto& list : m_Table)
		{
			for (auto& index : list)
			{
				index.first = MAXUINT16;
				index.second = 0;
			}
		}
	}

	HashPtrTable::~HashPtrTable()
	{ 
		m_Table.clear();
		m_Table.shrink_to_fit();
		
	}

	MeshInstences& HashPtrTable::AddItem(const Ref<Mesh>& key, MeshInstences& value)
	{
		if (m_Table.size() == 0 || m_Count >= m_Table.size())
			ResizeTable();
		uint32_t keyR = (uint32_t)key.get();
		uint32_t hash = HashFunktion(keyR);
		
		for (auto& index : m_Table[hash])
		{
			if (index.second == 0 && index.first == MAXUINT16 )
			{
				index.second = keyR;
				
				index.first = m_Count++;
				RY_CORE_ASSERT(index.first < 255);
				m_Values[index.first] = value;
				return m_Values[index.first];
				
			}
		}
	}

	void HashPtrTable::RemoveItem(const Ref<Mesh>& key)
	{
		
		uint32_t keyR = (uint32_t)key.get();
		uint32_t hash = HashFunktion(keyR);
		for (auto& index : m_Table[hash])
		{
			if (index.second == keyR && index.first != MAXUINT16 && index.first > m_Values.size())
			{
				m_Values[index.first] = MeshInstences();
				std::partition(m_Values.begin(), m_Values.end(),
					[](const MeshInstences& meshIn) {
						return meshIn.EntityIDs.size() != 0;
					});
				index.second = 0;
				index.first = MAXUINT16;
				
				m_Count--;
				return;
			}
		}
		
	}

	

	MeshInstences& HashPtrTable::GetItem(const Ref<Mesh>& key)
	{
		
		uint32_t keyR = (uint32_t)key.get();
		uint32_t hash = HashFunktion(keyR);
		uint32_t count = 0;
		for (auto& index : m_Table[hash])
		{
			if (index.second == keyR && index.first != MAXUINT16 )
			{
				MeshInstences& inst = m_Values[index.first];
				return inst;
			}
			count++;
		}

		if(count == s_ListSizeMax)
			return AddItem(key, MeshInstences());

		return MeshInstences();
		
	}

	void HashPtrTable::Clear()
	{
		
	}

	void HashPtrTable::CheckTable()
	{
		std::array<uint32_t, 255> checkArray;
		for (uint32_t& value : checkArray)
			value = 0;

		uint32_t emptyCount = 0;
		uint32_t size = m_Table.size();
		for (uint32_t x = 0; x < size; x++)
		{
			for (uint32_t y = 0; y < s_ListSizeMax; y++)
			{
				const std::pair<uint16_t, uint32_t>& element = m_Table[x][y];
				if (element.first != 0)
					checkArray[element.first]++;
				else
					emptyCount++;
			}
		}
		
		
	}

	uint32_t HashPtrTable::HashFunktion(uint32_t key) const
	{
		uint32_t keyV = key;
		return keyV % m_Table.size();
	}

	void HashPtrTable::ResizeTable()
	{
		RY_CORE_WARN("need Resize!");
		std::vector<std::array<std::pair<uint16_t, uint32_t>, s_ListSizeMax>> table = std::move(m_Table);
		m_Table = std::vector<std::array<std::pair<uint16_t, uint32_t>, s_ListSizeMax>>((table.size() + 1) * (1 / 3));
		
		uint32_t sizeOld = m_Table.size();
		uint32_t size = m_Table.size();
		for(uint32_t x = 0; x < size; x++)
		{
			if (x < sizeOld)
			{
				for (uint32_t y = 0; y < s_ListSizeMax; y++)
				{
					m_Table[x][y] = table[x][y];
				}
			}
			else
			{
				for (uint32_t y = 0; y < s_ListSizeMax; y++)
				{
					m_Table[x][y].first = MAXUINT16;
					m_Table[x][y].second = 0;
				}
			}
		}
	}

	

}
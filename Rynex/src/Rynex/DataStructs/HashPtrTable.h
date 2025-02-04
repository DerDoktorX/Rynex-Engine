#pragma once
#include <Rynex/Renderer/Objects/Mesh.h>
#include <Rynex/Renderer/Rendering/Renderer3D.h>

namespace Rynex {
#if RY_OLD_RENDER_SYSTEM
	class HashPtrTable
	{
	public:
		HashPtrTable();
		HashPtrTable(uint32_t size);
		~HashPtrTable();

		MeshInstences& AddItem(const Ref<Mesh>& key, MeshInstences& value);
		void RemoveItem(const Ref<Mesh>& key);
		MeshInstences& GetItem(const Ref<Mesh>& key);
		const MeshInstences& At(const Ref<Mesh>& key) { return GetItem(key); };
		void Clear();
		void CheckTable();

		std::array<MeshInstences, 255>::const_iterator begin() const { return m_Values.begin(); }
		std::array<MeshInstences, 255>::const_iterator end() const { return m_Values.end(); }


		std::array<MeshInstences, 255>::iterator begin() { return m_Values.begin(); }
		std::array<MeshInstences, 255>::iterator end() { return m_Values.end(); }

		const MeshInstences& operator [](const Ref<Mesh>& key)
		{
			return At(key);
		}
	private:
		uint32_t HashFunktion(uint32_t key) const;
		void ResizeTable();
	private:

		inline const static uint8_t s_ListSizeMax = 8;
		uint16_t m_UseSize;
		uint8_t m_Count;
		std::vector<std::array<std::pair<uint16_t, uint32_t>, s_ListSizeMax>>	m_Table;
		std::array<MeshInstences, 255> m_Values;
	};
#else
#endif
}


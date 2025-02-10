#pragma once
#include <Rynex/Asset/Base/Asset.h>

namespace Rynex {

	

	class HashMap
	{
	private:
		enum class KeyResults : uint8_t
		{
			None = 0,
			KeyMatch,
			NoKeySet,
			KeysNotMatch
		};

		struct MapKeyPtr
		{
			int* DataPtr = nullptr;
			UUID Key = 0;

			

			void Clear()
			{
				Key = 0;
				DataPtr = nullptr;
			}



			const KeyResults KeyMatch(UUID key) const;
			

			
		};
	public:
	

		HashMap();
		~HashMap();

		void Resize(uint32_t size);
		int GetData(UUID key);
		void AddData(UUID key, int data);
		void SetData(UUID key, int data);
		void RemoveData(UUID key);

		uint32_t Size() const { return m_ActiveSizeData; }
		uint32_t AloacSize() const { return m_DataSize; }
		uint32_t MapSize() const { return m_DataSize; }
	private:
		void MapData(int* keyDataPtr);
	private:

		MapKeyPtr* m_HeshMap = nullptr;
		int* m_MapData = nullptr;

		MapKeyPtr* m_KeyMap = nullptr;

		uint32_t m_DataSize = 0;
		uint32_t m_MapSize = 0;
		uint32_t m_ActiveSizeData = 0;
	};

}

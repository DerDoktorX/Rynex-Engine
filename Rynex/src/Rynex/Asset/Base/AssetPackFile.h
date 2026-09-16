#pragma once


namespace Rynex {

	struct AssetPackFile
	{
		struct AssetInfo
		{
			uint64_t m_PackedOffset;
			uint64_t m_PackedSize;
			uint16_t m_Type;
			uint16_t m_Flags;
		};

		struct SceneInfo
		{
			uint64_t m_PackedOffset = 0;
			uint64_t m_PackedSize = 0;
			uint16_t m_Flags = 0;
			std::map<uint64_t, SceneInfo> m_Scenes;
		};

		struct IndexTable
		{
			uint64_t m_PackedAppBinaryOffset = 0;
			uint64_t m_PackedAppSize = 0;
			std::map<uint64_t, SceneInfo> m_Scenes;
		};


		struct FileHeader
		{
			const char m_Header[4] = {'R','Z','A','P'};
			uint32_t m_Version = 0;
			uint64_t m_BuildVersion = 0;
		};

		FileHeader m_Header;
		IndexTable m_Index;
	};
	

}
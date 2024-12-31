#pragma once

#include "Rynex/Asset/Base/Asset.h"
#include <map>

namespace Rynex {

	struct AssetPackFile
	{
		struct AssetInfo
		{
			uint64_t PackedOffset; 
			uint64_t PackedSize;
			uint16_t Type;
			uint16_t Flags;
		};

		struct SceneInfo
		{
			uint64_t PackedOffset = 0;
			uint64_t PackedSize = 0;
			uint16_t Flags = 0;
			std::map<uint64_t, SceneInfo> Scenes;
		};

		struct IndexTable
		{
			uint64_t PackedAppBinaryOffset = 0;
			uint64_t PackedAppSize = 0;
			std::map<uint64_t, SceneInfo> Scenes;
		};


		struct FileHeader
		{
			const char HEADER[4] = {'R','Z','A','P'};
			uint32_t Version = 0;
			uint64_t BuilDVersion = 0;
		};

		FileHeader Header;
		IndexTable Index;
	};
	

}
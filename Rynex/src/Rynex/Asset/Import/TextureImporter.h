#pragma once
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Asset/Base/AssetMetadata.h"
#include "Rynex/Renderer/API/Texture.h"

namespace Rynex {

	class TextureImporter
	{
	public:
		static Ref<Texture> ImportTexture(AssetHandle handle, const AssetMetadata& metadata, bool async);
		static Ref<Texture> LoadTexture(const std::filesystem::path& path, bool async);
		static void ReLoadeTexture(AssetHandle handle, const std::filesystem::path& path, bool async);
	};

}


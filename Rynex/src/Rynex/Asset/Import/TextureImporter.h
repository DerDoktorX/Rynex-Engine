#pragma once

#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Asset/Base/AssetMetadata.h"
#include "Rynex/Renderer/Texture.h"

namespace Rynex {

	class TextureImporter
	{
	public:
		static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& path);

	};

}


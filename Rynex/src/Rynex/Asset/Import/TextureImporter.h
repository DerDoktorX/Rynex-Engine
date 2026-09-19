#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMetadata.h>
#include <Rynex/Renderer/API/Texture.h>

namespace Rynex {

	class TextureImporter
	{
	public:
		static Ref<Texture> ImportTexture(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Texture> LoadTexture(const  FileSystem::Path& path);
		static bool ReloadTexture(AssetHandle handle, const FileSystem::Path& path);
		static bool SaveTexture(const Ref<Texture>& tex, const std::filesystem::path& path);

	};

}


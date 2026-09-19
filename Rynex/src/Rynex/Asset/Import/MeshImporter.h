#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMetadata.h>
#include <Rynex/Renderer/Mesh/MeshStatic.h>

namespace Rynex {
	class MeshImporter
	{
	public:
		static Ref<MeshStatic> ImportMesh(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<MeshStatic> LoadMesh(const FileSystem::Path& path);
		static bool ReloadMesh(AssetHandle handle, const FileSystem::Path& path);
	};
}



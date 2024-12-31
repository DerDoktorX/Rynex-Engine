#pragma once

#include <Rynex/Renderer/Objects/Mesh.h>
#include <Rynex/Asset/Base/AssetMetadata.h>

namespace Rynex {

	class MeshImporter
	{
	public:
		static Ref<Mesh> ImportMesh(AssetHandle handle, const AssetMetadata& metadata, bool async);
		static Ref<Mesh> LoadMesh(const std::filesystem::path& path, bool async);
		static void ReLoadingMesh(AssetHandle handle, const std::filesystem::path& path, bool async);
	};

}

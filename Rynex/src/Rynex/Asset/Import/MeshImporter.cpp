#include "rypch.h"
#include "MeshImporter.h"

namespace Rynex {

    Ref<Mesh> MeshImporter::ImportMesh(AssetHandle handle, const AssetMetadata& metadata, bool async)
    {
        return Ref<Mesh>();
    }

    Ref<Mesh> MeshImporter::LoadMesh(const std::filesystem::path& path, bool async)
    {
        return Ref<Mesh>();
    }

    void MeshImporter::ReLoadingMesh(AssetHandle handle, const std::filesystem::path& path, bool async)
    {
    }

}
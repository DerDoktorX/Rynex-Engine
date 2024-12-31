#pragma once
#include "Rynex/Asset/Base/AssetMetadata.h"
#include "Rynex/Renderer/API/VertexArray.h"

namespace Rynex {
	class VertexArrayImporter
	{
	public:
		static Ref<VertexArray> ImportVertexArray(AssetHandle handle, const AssetMetadata& metadata, bool async = false);
		static Ref<VertexArray> LoadVertexArry(const std::filesystem::path& path, bool async = false);
		static void ReLoadeVertexArray(AssetHandle handle, const std::filesystem::path& path, bool async = false);
	};
}

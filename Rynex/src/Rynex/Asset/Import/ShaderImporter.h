#pragma once
#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Asset/Base/AssetMetadata.h"


namespace Rynex {

	class ShaderImporter
	{
	public:
		static Ref<Shader> ImportShader(AssetHandle handle, const AssetMetadata& metadata, bool async);
		static Ref<Shader> LoadShader(const std::filesystem::path& path, const std::string& name, bool async);
		static void ReLoadeShader(AssetHandle handle, const std::filesystem::path& path, bool async);
	};
}


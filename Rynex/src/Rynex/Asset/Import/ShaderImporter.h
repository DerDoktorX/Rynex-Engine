#pragma once
#include <Rynex/Renderer/Shader.h>
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMetadata.h>


namespace Rynex {

	class ShaderImporter
	{
	public:
		static Ref<Shader> ImportShader(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Shader> LoadShader(const std::filesystem::path& path, const std::string& name);
	};
}


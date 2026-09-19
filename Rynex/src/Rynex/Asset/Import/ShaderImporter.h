#pragma once
#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMetadata.h>


namespace Rynex {

	class ShaderImporter
	{
	public:
		static Ref<Shader> ImportShader(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Shader> LoadShader(const FileSystem::Path& path, const std::string& name);
		static bool ReloadShader(AssetHandle handle, const FileSystem::Path& path);
	};
}


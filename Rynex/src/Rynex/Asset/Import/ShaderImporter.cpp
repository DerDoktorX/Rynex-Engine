#include "rypch.h"
#include "ShaderImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"

namespace Rynex {

    Ref<Shader> ShaderImporter::ImportShader(AssetHandle handle, const AssetMetadata& metadata)
    {
        //CreateRef<Shader>()
        return LoadShader(metadata.FilePath, metadata.Name);
    }

	Ref<Shader> ShaderImporter::LoadShader(const std::filesystem::path& path, const std::string& name)
	{
		std::string result;
		std::ifstream in(path, std::ios::in, std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
			RY_CORE_INFO("Sucesfull open and close file'{0}'! (LoadShader)", path.string());
		}
		else
		{
			RY_CORE_ERROR("Coud not open file '{0}' filepath. (LoadShader)", path.string());
			return nullptr;
		}
		Ref<Shader> shader = Shader::Create(result, name);
		return shader;
	}

	void ShaderImporter::ReLoadeShader(AssetHandle handle, const std::filesystem::path& path)
	{
		RY_CORE_WARN("In Dev Funktion: ReLoadeShader!");
		std::string result;
		std::ifstream in(path, std::ios::in, std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
			RY_CORE_INFO("Sucesfull open and close file'{0}'! (ReLoadeShader)", path.string());
		}
		else
		{
			RY_CORE_ERROR("Coud not open file '{0}' filepath. (ReLoadeShader)", path.string());
			return;
		}
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		shader->ReganrateShader(result);
	}

}

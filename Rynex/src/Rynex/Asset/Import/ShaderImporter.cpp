#include "rypch.h"
#include "ShaderImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"

namespace Rynex {

    Ref<Shader> ShaderImporter::ImportShader(AssetHandle handle, const AssetMetadata& metadata, bool async)
    {
        //CreateRef<Shader>()
		return LoadShader((metadata.FilePath), metadata.Name, async);
    }

	Ref<Shader> ShaderImporter::LoadShader(const std::filesystem::path& path, const std::string& name, bool async)
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
		Ref<Shader> shader;

		if(async)
			shader = Shader::CreateAsync(std::move(result));
		else
			shader = Shader::Create(result, name);

		return shader;
	}

	void ShaderImporter::ReLoadeShader(AssetHandle handle, const std::filesystem::path& path, bool async)
	{
		RY_CORE_WARN("In Dev Funktion: ReLoadeShader!");
		std::string result;
		std::ifstream in((Project::GetActiveProjectDirectory()/path), std::ios::in, std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
			RY_CORE_INFO("Sucesfull open and close file'{0}'! (ReLoadeShader)", (Project::GetActiveProjectDirectory()/path).string());
		}
		else
		{
			RY_CORE_ERROR("Coud not open file '{0}' filepath. (ReLoadeShader)", (Project::GetActiveProjectDirectory()/path).string());
			return;
		}
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		shader->ReganrateShader(result);
	}

}

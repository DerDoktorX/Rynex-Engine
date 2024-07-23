#include "rypch.h"
#include "ShaderImporter.h"

namespace Rynex {

    Ref<Shader> ShaderImporter::ImportShader(AssetHandle handle, const AssetMetadata& metadata)
    {
        //CreateRef<Shader>()
		RY_PROFILE_FUNCTION();
        return LoadShader(metadata.FilePath, metadata.Name);
    }

	Ref<Shader> ShaderImporter::LoadShader(const std::filesystem::path& path, const std::string& name)
	{
		RY_PROFILE_FUNCTION();
		std::string result;
		std::ifstream in(path, std::ios::in, std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			RY_CORE_ERROR("Coud not open file '{0}' filepath", path.string());
			return nullptr;
		}
		Ref<Shader> shader = Shader::Create(result, name);
		return shader;
	}

}

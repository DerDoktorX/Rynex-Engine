#include "rypch.h"
#include "ShaderImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"

namespace Rynex {

    Ref<Shader> ShaderImporter::ImportShader(AssetHandle handle, const AssetMetadata& metadata, bool async)
    {
        //CreateRef<Shader>()
		return LoadShader(metadata.FilePath, metadata.Name, async);
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

		shader = Shader::CreateAsync(std::move(result));

		return shader;
	}

	void ShaderImporter::ReLoadeShader(AssetHandle handle, const std::filesystem::path& path, bool async)
	{
		RY_CORE_WARN("In Dev Funktion: ReLoadeShader!");
		
		std::string result;

		try {
			
			std::ifstream in(path, std::ios::in, std::ios::binary);
			// in.open(path, std::ios::in, std::ios::binary);
			// in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			
		
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
				in.close();
				std::string mes = std::strerror(errno);
				if (in.fail())
					RY_CORE_FATAL("Logical error on i/o operation '{}'", mes);
				if(in.bad())
					RY_CORE_FATAL("Read/write error on i/o operation '{}'", mes);
				RY_CORE_ERROR("Coud not open file '{0}' filepath. (ReLoadeShader)  Error Flage: {1}", path.string(), in.rdstate());
				return;
			}
		}
		catch (const std::ios_base::failure& e) {
			RY_CORE_FATAL("Exception caught: {}", e.what());
			return;
		}
		catch (std::exception e)
		{
			RY_CORE_FATAL("Exception caught: {}", e.what());
			return;
		}
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		shader->ReganrateShader(std::move(result));
	}

}

#include "rypch.h"
#include "ShaderImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"

namespace Rynex {

    Ref<Shader> ShaderImporter::ImportShader(AssetHandle handle, const AssetMetadata& metadata)
    {
#if 0
		std::filesystem::path filePath = (Project::GetActiveProjectDirectory() / metadata.FilePath).string();
#else
		std::filesystem::path filePath = metadata.AbsolutePath;
#endif
		return LoadShader(filePath, metadata.Name);
    }

	Ref<Shader> ShaderImporter::LoadShader(const std::filesystem::path& path, const std::string& name)
	{
		std::string result;
		try
		{		
			std::ifstream in(path, std::ios::in, std::ios::binary);

			if (in)
			{
				in.seekg(0, std::ios::end);
				uint32_t sizeByte = in.tellg();
				RY_CORE_ASSERT(sizeByte != 0, "File Found But Empty!");
				result.resize(sizeByte);
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
		}
		catch (const std::ios_base::failure& e) {
			RY_CORE_FATAL("Exception caught: {}", e.what());
			return nullptr;
		}
		catch (std::exception e)
		{
			RY_CORE_FATAL("Exception caught: {}", e.what());
			return nullptr;
		}

		RY_CORE_ASSERT(!result.empty(), "File Found But Empty!");
		Ref<Shader> shader;

		shader = Shader::CreateAsync(std::move(result));

		return shader;
	}

	bool ShaderImporter::ReLoadeShader(AssetHandle handle, const std::filesystem::path& path)
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
				return false;
			}
		}
		catch (const std::ios_base::failure& e) {
			RY_CORE_FATAL("Exception caught: {}", e.what());
			return false;
		}
		catch (std::exception e)
		{
			RY_CORE_FATAL("Exception caught: {}", e.what());
			return false;
		}
		Ref<Shader> shader = AssetManager::GetAsset<Shader>(handle);
		shader->ReganrateShader(std::move(result));
		return true;

	}

}

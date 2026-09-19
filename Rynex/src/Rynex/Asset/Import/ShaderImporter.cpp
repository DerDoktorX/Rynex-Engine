#include "rypch.h"
#include "ShaderImporter.h"

#include <Rynex/Asset/Base/AssetManager.h>

namespace Rynex {

    Ref<Shader> ShaderImporter::ImportShader(AssetHandle handle, const AssetMetadata& metadata)
    {
		const FileSystem::Path& filePath = metadata.m_Path;
		return LoadShader(filePath, metadata.m_Name);
    }

	Ref<Shader> ShaderImporter::LoadShader(const FileSystem::Path& path, const std::string& name)
	{
		std::string result;
		try
		{
			std::string filePath = path.GetPathString();
			auto fileFlag = std::ios::binary | std::ios::in;
			std::ifstream in(filePath, fileFlag);

			if (in)
			{
				in.seekg(0, std::ios::end);
				uint32_t sizeByte = in.tellg();
				RY_CORE_ASSERT(sizeByte != 0, "File Found But Empty!");
				result.resize(sizeByte);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], result.size());
				in.close();
				RY_CORE_INFO("Successful open and close file'{0}'! (LoadShader)", path);

			}
			else
			{
				RY_CORE_ERROR("Could not open file '{0}' filepath. (LoadShader)", path);
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

	bool ShaderImporter::ReloadShader(AssetHandle handle, const FileSystem::Path& path)
	{
		RY_CORE_WARN("In Dev Funktion: ReloadShader!");
		
		std::string result;

		try {
			std::string filePath = path.GetPathString();
			auto fileFlag = std::ios::binary | std::ios::in;
			std::ifstream in(filePath, fileFlag);
		
			if (in)
			{
				in.seekg(0, std::ios::end);
				result.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&result[0], result.size());
				in.close();
				RY_CORE_INFO("Successful open and close file'{0}'! (ReLoadShader)", path);
			}
			else
			{
				in.close();
				std::string mes = std::strerror(errno);
				if (in.fail())
					RY_CORE_FATAL("Logical error on i/o operation '{}'", mes);
				if(in.bad())
					RY_CORE_FATAL("Read/write error on i/o operation '{}'", mes);
				std::string mesStr = path.GetPathString();
				std::ios_base::iostate rdState = in.rdstate();

				RY_CORE_WARN_IF(std::ios_base::goodbit == rdState, "Could not open file '{0}' filepath. (ReloadShader)  Error Flag: goodbit", mesStr);
				RY_CORE_ERROR_IF(std::ios_base::badbit == rdState, "Could not open file '{0}' filepath. (ReloadShader)  Error Flag: badbit", mesStr);
				RY_CORE_FATAL_IF(std::ios_base::failbit == rdState,"Could not open file '{0}' filepath. (ReloadShader)  Error Flag: failbit", mesStr);
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

#include "rypch.h"
#include "TextureImporter.h"

#include <Rynex/Asset/Base/AssetManager.h>

#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

namespace Rynex{
	namespace Utils {

	}


	Ref<Texture> TextureImporter::ImportTexture(AssetHandle handle, const AssetMetadata& metadata)
	{
		const FileSystem::Path& filePath = metadata.m_Path;
		return LoadTexture(filePath);
	}

	Ref<Texture> TextureImporter::LoadTexture(const FileSystem::Path& path)
	{
		std::filesystem::path extension = path.GetExtensionPath();
		if(".png" != extension
			&& ".jpeg" != extension
			&& ".jpg" != extension
			&& ".hdr"  != extension)
		{
			RY_CORE_ASSERT(false, "ERROR: TextureImporter::LoadTexture! not supported extension {}", extension);
			return nullptr;
		}
		int width, height, channels, req_comp;
		stbi_set_flip_vertically_on_load(1);
		std::string pathStr = path.GetPathString();
		const char* pathChar = pathStr.c_str();
		
		stbi_uc* dataBytePtr = stbi_load(pathChar, &width, &height, &channels, STBI_default); // STBI_rgb_alpha
		

		if (nullptr == dataBytePtr)
		{
			RY_CORE_ERROR("Could not Load Image! {}", pathChar);
			return nullptr;
		}


		TextureSpecification spec;
		spec.m_Width = static_cast<uint32_t>(width);
		spec.m_Height = static_cast<uint32_t>(height);
		spec.m_Target = TextureTarget::Texture2D;
		spec.m_FilteringMode = TextureFilteringMode::Linear;
		spec.WrappingSpec = {
			TextureWrappingMode::Repeat,
			TextureWrappingMode::Repeat,
		};
		if(stbi_is_hdr(pathChar))
		{
			switch (channels)
			{
				case 1:
				{
					spec.m_Format = TextureFormat::R8;
					break;
				}
				case 2:
				{
					spec.m_Format = TextureFormat::RG8;
					break;
				}
				case 3:
				{
					spec.m_Format = TextureFormat::RGB8;
					break;
				}
				case 4:
				{
					spec.m_Format = TextureFormat::RGBA16F;
					break;
				}
				default:
					RY_CORE_ASSERT(false);
			}
		}
		else
		{
			switch (channels)
			{
				case 1:
				{
					spec.m_Format = TextureFormat::R8;
					break;
				}
				case 2:
				{
					spec.m_Format = TextureFormat::RG8;
					break;
				}
				case 3:
				{
					spec.m_Format = TextureFormat::S_RGB8;
					break;
				}
				case 4:
				{
					spec.m_Format = TextureFormat::S_RGBA8;
					break;
				}
				default:
					RY_CORE_ASSERT(false);
			}
		}
		Ref<Texture> texture;
		
			
		uint32_t byteSize = width * height * channels;
		std::vector<unsigned char> vData(byteSize);
		std::memcpy(vData.data(), dataBytePtr, byteSize);
		texture = Texture::CreateAsync(spec, std::move(vData));
		RY_CORE_INFO("Successful Load A-Sync Texture {}/{} withe {} Channels from Path {}", width, height, channels, pathChar);
		
		stbi_image_free(dataBytePtr);
		return texture;
	}

	bool TextureImporter::ReloadTexture(AssetHandle handle, const FileSystem::Path& path)
	{
		std::filesystem::path extension = path.GetExtensionPath();
		if (".png" != extension
			&& ".jpeg" != extension
			&& ".jpg" != extension
			&& ".hdr" != extension)
		{
			RY_CORE_ASSERT(false, "ERROR: TextureImporter::LoadTexture! not sepoted extension {}", extension);
			return false;
		}

		RY_CORE_WARN("In Dev Funktion: ReLoadeTexture2D!");
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		std::string pathStr = path.GetPathString();
		const char* pathChar = pathStr.c_str();
		stbi_uc* dataBytePtr = stbi_load(pathChar, &width, &height, &channels, STBI_default);

		if (nullptr == dataBytePtr)
		{
			RY_CORE_ERROR("Could not Load Image! {}", pathChar);
			return false;
		}

		Ref<Texture> texture = AssetManager::GetAsset<Texture>(handle);
		uint32_t bytesSize = width * height * channels;
		texture->SetData(dataBytePtr, bytesSize);
		stbi_image_free(dataBytePtr);
		return true;
	}

	bool TextureImporter::SaveTexture(const Ref<Texture>& tex, const std::filesystem::path& path)
	{
		RY_CORE_ASSERT(nullptr != tex, "no valid Texture!");
		RY_CORE_ASSERT(path.has_parent_path(), "this Path dont Exist!");

		int width = static_cast<int>(tex->GetWidth());
		int height = static_cast<int>(tex->GetHeight());
		const TextureSpecification& spec = tex->GetSpecification();
		int channels = 0;
		std::vector<uint8_t> dataVec;
		switch (spec.m_Format)
		{
		case TextureFormat::R8:
		{
			channels = 1;
			break;
		}
		case TextureFormat::RG8:
		{
			channels = 2;
			break;
		}
		case TextureFormat::RGB32F:
		case TextureFormat::RGB16F:
		case TextureFormat::S_RGB8:
		case TextureFormat::RGB8:
		{
			dataVec = tex->GetCurrentRenderData(TextureFormat::RGB8);
			channels = 3;
			break;
		}
		case TextureFormat::RGBA32F:
		case TextureFormat::RGBA16F:
		case TextureFormat::S_RGBA8:
		case TextureFormat::RGBA8:
		{
			dataVec = tex->GetCurrentRenderData(TextureFormat::RGBA8);
			channels = 4;
			break;
		}
		default:
			RY_CORE_ASSERT(false);
			channels = 0;
		}
		
		if (channels == 0)
			return false;

		void* dataPtr = dataVec.data();

		stbi_flip_vertically_on_write(1);

		const std::string pathStr = path.string();
		const char* pathChar = pathStr.c_str();

		stbi_write_png(pathChar, width, height, channels, dataPtr, width * channels);
		
		
		return true;
	}


}

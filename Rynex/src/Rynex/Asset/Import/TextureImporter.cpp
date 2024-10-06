#include "rypch.h"
#include "TextureImporter.h"

#include "Rynex/Asset/Base/Buffer.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Serializers/TextureSerialiazer.h"

#include <stb_image.h>

namespace Rynex{



	Ref<Texture> TextureImporter::ImportTexture(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadTexture(metadata.FilePath);
	}

	Ref<Texture> TextureImporter::LoadTexture(const std::filesystem::path& path)
	{
		if (path.extension() == ".png" || path.extension() == ".jpeg")
		{
			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);
			std::string strPath = path.string();
			stbi_uc* data = stbi_load(strPath.c_str(), &width, &height, &channels, 0);

			if (data == nullptr)
			{
				RY_CORE_ERROR("Coud not Load Image!");
				return nullptr;
			}


			TextureSpecification spec;
			spec.Width = (uint32_t)width;
			spec.Height = (uint32_t)height;
			spec.Target = TextureTarget::Texture2D;
			spec.FilteringMode = TextureFilteringMode::Linear;
			spec.WrappingSpec = {
				TextureWrappingMode::Repeate,
				TextureWrappingMode::Repeate
			};
			//data.Size = (uint64_t)width * height * channels;
			switch (channels)
			{
			case 1:
			{
				spec.Format = ImageFormat::R8;
				break;
			}
			case 2:
			{
				spec.Format = ImageFormat::RG8;
				break;
			}
			case 3:
			{
				spec.Format = ImageFormat::RGB8;
				break;
			}
			case 4:
			{
				spec.Format = ImageFormat::RGBA8;
				break;
			}
			}

			Ref<Texture> texture = Texture::Create(spec, data, width * height * channels);
			stbi_image_free(data);
			return texture;
		}
		else if (path.extension() == ".rytex2d")
		{
			return Texture2DSerialiazer::Deserlize(path);
		}
		else
		{
			RY_CORE_ASSERT(false, "ERROR: TextureImporter::LoadTexture!");
		}
		RY_CORE_ASSERT(false, "ERROR: TextureImporter::LoadTexture!");
		return nullptr;
	}

	void TextureImporter::ReLoadeTexture(AssetHandle handle, const std::filesystem::path& path)
	{
#if 1
		if (path.extension() == ".png" || path.extension() == ".jpeg")
		{
			RY_CORE_WARN("In Dev Funktion: ReLoadeTexture2D!");
			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);
			std::string strPath = path.string();
			stbi_uc* data = stbi_load(strPath.c_str(), &width, &height, &channels, 0);

			if (data == nullptr)
			{
				RY_CORE_ERROR("Coud not Load Image!");
				return;
			}

			Ref<Texture> texture = AssetManager::GetAsset<Texture>(handle);
			texture->SetData(data, width * height * channels);
#if 1
			stbi_image_free(data);
		}
		else if (path.extension() == ".rytex2d")
		{
			RY_CORE_ASSERT(false, "ERROR: TextureImporter::LoadTexture! '.rytex2d' is not rady");
			Texture2DSerialiazer::Deserlize(path);
		}
		else
		{
			RY_CORE_ASSERT(false, "ERROR: TextureImporter::LoadTexture!");
		}
#endif // IMPORTENT! Becouse we Free the Image prt, at the ende of the conststuctor. We don't use the stbi Funktion for this because thar crated some Isuse! Withe Free meamory that alrdy has Freed.
#endif
	}

}

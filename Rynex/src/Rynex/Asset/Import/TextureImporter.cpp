#include "rypch.h"
#include "TextureImporter.h"
#include <stb_image.h>
#include <Rynex/Asset/Base/Buffer.h>

namespace Rynex{



	Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{

		return LoadTexture2D(metadata.FilePath);
	}

	Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path)
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
		//data.Size = (uint64_t)width * height * channels;
		switch (channels)
		{
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
		Ref<Texture2D> texture = Texture2D::Create(spec, data);
		stbi_image_free(data);
		return texture;
	}

}

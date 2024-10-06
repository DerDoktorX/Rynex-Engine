#include "rypch.h"
#include "Texture.h"

#include "Rynex/Renderer/Rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>

namespace Rynex {
	Ref<Texture> Texture::Create(uint32_t withe, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture>(withe, height);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
		return Ref<Texture>();
	}

	Ref<Texture> Texture::Create(TextureSpecification spec, void* data, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture>(spec, data, size);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<Texture> Texture::Create(TextureSpecification spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture>(spec);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<Texture> Texture::Create(const std::string& path)
	{
		RY_CORE_ERROR("Not allowd Funktion(AssetManger)!: Texture::Create");
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	
			{
				int width, height, channels;
				stbi_set_flip_vertically_on_load(1);
				stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
				if (!data) RY_CORE_ERROR("filePath->{0}", path.c_str());
				RY_CORE_ASSERT(data, "Faild to load Image!");

				return CreateRef<OpenGLTexture>(data, width, height, channels);
				stbi_image_free(data);
			}
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	void Texture::BindTex(uint32_t renderID, uint32_t slot)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return;
			case RendererAPI::API::OpenGL:	OpenGLTexture::BindTex(renderID, slot);
		
		}
	}


	Ref<Texture> Texture::CreateFrame(AssetHandle assetHandle, int index)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureLinking>(assetHandle, index);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}
#include "rypch.h"
#include "Texture.h"

#include "Rynex/Renderer/Rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include "stb_image.h"
namespace Rynex {
	Ref<Texture2D> Texture2D::Create(uint32_t withe, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D>(withe, height);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
		return Ref<Texture2D>();
	}
#if 0
	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D>(path);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
#else

	Ref<Texture2D> Texture2D::Create(TextureSpecification spec, void* data)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTexture2D>(spec, data);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		RY_CORE_ERROR("Not allowd Funktion(AssetManger)!: Texture2D::Create");
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

				return CreateRef<OpenGLTexture2D>(data, width, height, channels);
				stbi_image_free(data);
			}
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
#endif
}
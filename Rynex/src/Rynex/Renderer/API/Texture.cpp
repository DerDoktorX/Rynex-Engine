#include "rypch.h"
#include "Texture.h"

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Platform/OpenGL/OpenGLTexture.h>
#include <Platform/OpenGL/OpenGLRenderCommand.h>

#include <stb_image.h>

namespace Rynex {
	static Ref<Texture> s_DefaultTexture = nullptr;
	static Ref<Texture> s_WhiteTexture = nullptr;



#ifdef RY_OPNEGL_OLD_TEXTURE
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
#endif
	Ref<Texture> Texture::Create(TextureSpecification spec, void* data, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureStorageModern>(spec, data, size);;
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<Texture> Texture::CreateAsync(TextureSpecification spec, std::vector<unsigned char>&& data)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureStorageModern>(spec, std::move(data));;
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<Texture> Texture::Create(TextureSpecification spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLTextureStorageModern>(spec);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<Texture> Texture::Default()
	{

		
		if (!s_DefaultTexture && Renderer::IsInit())
		{
			constexpr uint32_t withe = 6u, higth = 6u, depth = 1u;
			s_DefaultTexture = Texture::Create({ withe, higth , depth, TextureTarget::Texture2D, TextureFormat::S_RGBA8, 1,});
			uint32_t w = 0xffffffff;
			uint32_t p = 0xff00ffff;
			uint32_t b = 0x00000000;

			uint32_t notDefinedTexData[36] = { 
				p, w, p, w, p, w,
				w, p, w, p, w, p,
				p, w, b, w, p, w,
				w, p, w, b, w, p,
				p, w, p, w, p, w,
				w, p, w, p, w, p,
			};
			s_DefaultTexture->SetData(&notDefinedTexData, sizeof(uint32_t) * 36);
		}
		return s_DefaultTexture;
	}

	Ref<Texture> Texture::White()
	{


		if (!s_WhiteTexture && Renderer::IsInit())
		{
			constexpr uint32_t withe = 1u, higth = 1u, depth = 1u;

			s_WhiteTexture = Texture::Create({ withe, higth , depth,TextureTarget::Texture2D, TextureFormat::S_RGBA8, 1, });
			uint32_t whitheTexData = 0xffffffff;
			s_WhiteTexture->SetData(&whitheTexData, sizeof(uint32_t));
		}
		return s_WhiteTexture;
	}

	void Texture::Shutdown()
	{
		s_DefaultTexture.reset();
		s_DefaultTexture = nullptr;

		s_WhiteTexture.reset();
		s_WhiteTexture = nullptr;

	}

#ifdef RY_OPNEGL_OLD_TEXTURE
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
#endif

	void Texture::BindTex(uint32_t renderID, uint32_t slot)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	
				RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet");
				break;
			case RendererAPI::API::OpenGL:	
				OpenGLRenderCommand::BindTextureSlot(slot, renderID); 
				break;
			default:
				RY_CORE_ASSERT(false, "Unknown RenderAPI!");
				break;
		}
	}


	Ref<LinkedTextureArray> LinkedTextureArray::Create(TextureSpecification spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
		case RendererAPI::API::OpenGL:	return CreateRef<OpenGLLinkedTextureArray>(spec);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


}
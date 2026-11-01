#include <rypch.h>
#include "BindlessTextureArray.h"

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Platform/OpenGL/OpenGLBindlessTextureArray.h>

namespace Rynex {


	Ref<BindlessTextureArray> BindlessTextureArray::CreateBindlesTextureArray(uint32_t initCount)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLBindlessTextureArray>(initCount);
				
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<BindlessTextureArray> BindlessTextureArray::CreateBindlesTextureArray(std::vector<Ref<Texture>>&& tex)
	{ 
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLBindlessTextureArray>(std::move(tex));
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<BindlessTextureArray> BindlessTextureArray::CreateBindlesTextureArray(std::initializer_list<Ref<Texture>> tex)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLBindlessTextureArray>(tex);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;

	}
	Ref<BindlessTextureArray> BindlessTextureArray::CreateBindlesTextureArray()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLBindlessTextureArray>();
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}
#include "rypch.h"
#include "BindlesTextureArray.h"

#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Platform/OpenGL/OpenGLBindlesTextureArray.h>

namespace Rynex {


	Ref<BindlesTextureArray> BindlesTextureArray::CreateBindlesTextureArray(uint32_t initCount)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLBindlesTextureArray>(initCount);
				
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<BindlesTextureArray> BindlesTextureArray::CreateBindlesTextureArray(std::vector<Ref<Texture>>&& tex)
	{ 
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLBindlesTextureArray>(std::move(tex));
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	Ref<BindlesTextureArray> BindlesTextureArray::CreateBindlesTextureArray(std::initializer_list<Ref<Texture>> tex)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLBindlesTextureArray>(tex);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;

	}
	Ref<BindlesTextureArray> BindlesTextureArray::CreateBindlesTextureArray()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Curently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLBindlesTextureArray>();
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
}
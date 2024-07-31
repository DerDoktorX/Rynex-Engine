#include "rypch.h"
#include "Framebuffer.h"

#include "Rynex/Renderer/Rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Rynex {
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
			case RendererAPI::API::OpenGL: return  CreateRef<OpenGLFramebuffer>(spec);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;

	}
}
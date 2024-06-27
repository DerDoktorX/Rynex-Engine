#include "rypch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Rynex {
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	RY_CORE_ASSERT(false, "RendererAPI::None is Cuurently not supportet"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexArray>();
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	
}
#include "rypch.h"
#include "Buffer.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Rynex {

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t byteSize)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLVertexBuffer>(byteSize);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
		return nullptr;
	}
	Ref<VertexBuffer> VertexBuffer::Create(float* indices, uint32_t byteSize)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
			case RendererAPI::API::OpenGL: return  CreateRef<OpenGLVertexBuffer>(indices, byteSize);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
			case RendererAPI::API::OpenGL: return  CreateRef<OpenGLIndexBuffer>(indices, count);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}
	

}
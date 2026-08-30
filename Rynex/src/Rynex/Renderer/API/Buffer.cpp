#include "rypch.h"
#include "Buffer.h"


#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Platform/OpenGL/OpenGLBuffer.h>


namespace Rynex {


	Ref<VertexBuffer> VertexBuffer::Create(uint32_t byteSize)
	{
		
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(byteSize);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const void* vextices, uint32_t byteSize)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLVertexBuffer>(vextices, byteSize);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, uint32_t byteSize, BufferFlagGPU flag)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLVertexBuffer>(vertices, byteSize, flag);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, uint32_t byteSize, BufferFlagGPU flag, const BufferLayout& layout)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLVertexBuffer>(vertices, byteSize, flag, layout);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::CreateAsync(std::vector<unsigned char>&& data, uint32_t byteSize, BufferFlagGPU flag, const BufferLayout& layout)
	{

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexBuffer>(std::move(data), byteSize, flag, layout);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Default()
	{
		static Ref<VertexBuffer> DefaultVertexBuffer;
		if (!DefaultVertexBuffer)
		{
			float vertecies[] = {
				-1.0f, -1.0f, 
				 1.0f,  1.0f, 
				 1.0f, -1.0f
			};
			DefaultVertexBuffer = Create(
				vertecies, sizeof(float) * 6, 
				BufferFlag::None,
				BufferLayout({
					{ShaderDataType::Float2,	"a_Postion"}
				}, 0u)
			);
		} 
		return DefaultVertexBuffer;
	}

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count, BufferFlagGPU flag)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndexBuffer>(indices, count, flag);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(const uint16_t* indices, uint32_t count, BufferFlagGPU flag)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLIndexBuffer>(indices, count, flag);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::CreateAsync(std::vector<uint32_t>&& data, uint32_t size, BufferFlagGPU flag)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLIndexBuffer>(std::move(data), size, flag);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<StorageBuffer> StorageBuffer::Create(const void* data, uint32_t byteSize, BufferType bufferType, BufferFlagGPU flag)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLStorageBuffer>(data, byteSize, bufferType, flag);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<StorageBuffer> StorageBuffer::Create(uint32_t byteSize, BufferFlagGPU flag)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLStorageBuffer>(byteSize, flag);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<StorageBuffer> StorageBuffer::Create(const void* data, uint32_t byteSize, BufferFlagGPU flag)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLStorageBuffer>(data,byteSize, flag);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create( uint32_t byteSize)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(byteSize);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(const void* data, uint32_t byteSize)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(data, byteSize);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(const void* data, uint32_t byteSize, const BufferLayout& layout)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(data, byteSize, layout);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<UniformBuffer> UniformBuffer::Create(const void* data, uint32_t byteSize, const BufferLayout& layout, BufferFlagGPU flag)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(data, byteSize, layout, flag);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::CreateAsync(std::vector<unsigned char>&& data, const BufferLayout& layout, BufferFlagGPU flag)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(std::move(data), layout, flag);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<UniformBuffer> UniformBuffer::CreateCopy(const Ref<UniformBuffer>& uniformBuffer)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: 
		{
			Ref<OpenGLUniformBuffer> uniformBufferOpenGL = std::static_pointer_cast<OpenGLUniformBuffer, UniformBuffer>(uniformBuffer);
			return CreateRef<OpenGLUniformBuffer>(uniformBufferOpenGL);
		}
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	
	
	Ref<IndirectBuffer> IndirectBuffer::Create(const void* data, uint32_t byteSize)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndriectBuffer>(data, byteSize);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<IndirectBuffer> IndirectBuffer::Create(const DrawElementsIndirectCommand* data, uint32_t count)
	{
		uint32_t byteSize = count * sizeof(DrawElementsIndirectCommand);
		const void* ptr = reinterpret_cast<const void*>(data);
		return Create(ptr, byteSize);
	}

	Ref<IndirectBuffer> IndirectBuffer::Create(uint32_t byteSize)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLIndriectBuffer>(byteSize);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<IndirectBuffer> IndirectBuffer::Create(const void* data, uint32_t byteSize, const BufferLayout& layout)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLIndriectBuffer>(data, byteSize, layout);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<IndirectBuffer> IndirectBuffer::Create(uint32_t byteSize, const BufferLayout& layout)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLIndriectBuffer>(byteSize, layout);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}



	

	

}
#include "rypch.h"
#include "Buffer.h"

#include "Rynex/Renderer/Rendering/Renderer.h"
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

	Ref<VertexBuffer> VertexBuffer::Create(const void* vextices, uint32_t byteSize)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
			case RendererAPI::API::OpenGL: return  CreateRef<OpenGLVertexBuffer>(vextices, byteSize);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const void* vertices, uint32_t byteSize, BufferDataUsage usage)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
			case RendererAPI::API::OpenGL: return  CreateRef<OpenGLVertexBuffer>(vertices, byteSize, usage);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}


	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, uint32_t count, BufferDataUsage usage)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
			case RendererAPI::API::OpenGL: return  CreateRef<OpenGLIndexBuffer>(indices, count, usage);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(const uint16_t* indices, uint32_t count, BufferDataUsage usage)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLIndexBuffer>(indices, count, usage);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	struct DataBuffer
	{
		std::map<std::string, ShaderDataType>  StringShaderDataType;
		std::map< ShaderDataType, std::string> ShaderDataTypeString;
	};

	static DataBuffer s_Data;
	void BufferAPI::Init()
	{
		s_Data.ShaderDataTypeString[ShaderDataType::Float4x4]	= "Float4x4";
		s_Data.ShaderDataTypeString[ShaderDataType::Float3x3]	= "Float3x3";
		s_Data.ShaderDataTypeString[ShaderDataType::Float	]	= "Float"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Int		]	= "Int"		;
		s_Data.ShaderDataTypeString[ShaderDataType::Float2	]	= "Float2"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Float3	]	= "Float3"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Float4	]	= "Float4"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Texture ]	= "Texture"	;

		s_Data.StringShaderDataType["mat4"		]	= ShaderDataType::Float4x4;
		s_Data.StringShaderDataType["mat3"		]	= ShaderDataType::Float3x3;
		s_Data.StringShaderDataType["float"		]	= ShaderDataType::Float	  ;
		s_Data.StringShaderDataType["int"		]	= ShaderDataType::Int	  ;
		s_Data.StringShaderDataType["vec2"		]	= ShaderDataType::Float2  ;
		s_Data.StringShaderDataType["vec3"		]	= ShaderDataType::Float3  ;
		s_Data.StringShaderDataType["vec4"		]	= ShaderDataType::Float4  ;
		s_Data.StringShaderDataType["sampler2D"	]	= ShaderDataType::Texture ;
	}

	void BufferAPI::Shutdown()
	{
		s_Data.ShaderDataTypeString.clear();
		s_Data.StringShaderDataType.clear();
	}

	ShaderDataType BufferAPI::GetShaderDataTypeFromString(const std::string& type)
	{
		return s_Data.StringShaderDataType[type];
	};

	std::string BufferAPI::GetStringFromShaderData(ShaderDataType type)
	{
		return s_Data.ShaderDataTypeString[type];
	};


	

	Ref<StorageBuffer> StorageBuffer::Create(uint32_t byteSize, Type type)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
			case RendererAPI::API::OpenGL: return  CreateRef<OpenGLStorageBuffer>(byteSize, type);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<StorageBuffer> StorageBuffer::Create(void* data, uint32_t byteSize, Type type)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return  CreateRef<OpenGLStorageBuffer>(data,byteSize, type);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}
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
		std::map<std::string, ShaderResourceType>  StringShaderResourceType;

		std::map<ShaderDataType, std::string> ShaderDataTypeString;
		std::map<ShaderResourceType, std::string> ShaderResourceTypeString;
	};

	static DataBuffer s_Data;

	void BufferAPI::Init()
	{
#if 1
		s_Data.ShaderDataTypeString[ShaderDataType::Float4x4]	= "Float4x4";
		s_Data.ShaderDataTypeString[ShaderDataType::Float3x3]	= "Float3x3";
		s_Data.ShaderDataTypeString[ShaderDataType::Float	]	= "Float"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Int		]	= "Int"		;
		s_Data.ShaderDataTypeString[ShaderDataType::Float2	]	= "Float2"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Float3	]	= "Float3"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Float4	]	= "Float4"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Int2	]	= "ivec2"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Int3	]	= "ivec3"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Int4	]	= "ivec4"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Uint2	]	= "uvec2"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Uint3	]	= "uvec3"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Uint4	]	= "uvec4"	;
		s_Data.ShaderDataTypeString[ShaderDataType::Texture ]	= "Texture"	;

		s_Data.ShaderResourceTypeString[ShaderResourceType::None							] = "None";
		s_Data.ShaderResourceTypeString[ShaderResourceType::EnitiyID						] = "EnitiyID";
		s_Data.ShaderResourceTypeString[ShaderResourceType::GlobleResource					] = "GlobleResource";
		s_Data.ShaderResourceTypeString[ShaderResourceType::LocalModel						] = "LocalModel";
		s_Data.ShaderResourceTypeString[ShaderResourceType::LocalColor						] = "LocalColor";
		s_Data.ShaderResourceTypeString[ShaderResourceType::MainCameraViewMatrix			] = "MainCameraViewMatrix";
		s_Data.ShaderResourceTypeString[ShaderResourceType::MainCameraViewProjectionMatrix	] = "MainCameraViewProjectionMatrix";
		s_Data.ShaderResourceTypeString[ShaderResourceType::MainCamerPos					] = "MainCamerPos";
		s_Data.ShaderResourceTypeString[ShaderResourceType::MainCamerProjectionMatrix		] = "MainCamerProjectionMatrix";
		s_Data.ShaderResourceTypeString[ShaderResourceType::AmbientLigths					] = "AmbientLigths";
		s_Data.ShaderResourceTypeString[ShaderResourceType::PointLigths						] = "PointLigths";
		s_Data.ShaderResourceTypeString[ShaderResourceType::SpotLigths						] = "SpotLigths";
		s_Data.ShaderResourceTypeString[ShaderResourceType::DrirektionLigths				] = "DrirektionLigths";
		s_Data.ShaderResourceTypeString[ShaderResourceType::ShadowTexture					] = "ShadowTexture";
		

		s_Data.StringShaderDataType["mat4"		]	= ShaderDataType::Float4x4;
		s_Data.StringShaderDataType["mat3"		]	= ShaderDataType::Float3x3;
		s_Data.StringShaderDataType["float"		]	= ShaderDataType::Float	  ;
		s_Data.StringShaderDataType["uint"		]	= ShaderDataType::Uint	  ;
		s_Data.StringShaderDataType["int"		]	= ShaderDataType::Int	  ;
		s_Data.StringShaderDataType["vec2"		]	= ShaderDataType::Float2  ;
		s_Data.StringShaderDataType["vec3"		]	= ShaderDataType::Float3  ;
		s_Data.StringShaderDataType["vec4"		]	= ShaderDataType::Float4  ;
		s_Data.StringShaderDataType["ivec2"		]   = ShaderDataType::Int2	  ;
		s_Data.StringShaderDataType["ivec3"		]   = ShaderDataType::Int3	  ;
		s_Data.StringShaderDataType["ivec4"		]   = ShaderDataType::Int4	  ;
		s_Data.StringShaderDataType["uvec2"		]   = ShaderDataType::Uint2	  ;
		s_Data.StringShaderDataType["uvec3"		]   = ShaderDataType::Uint3	  ;
		s_Data.StringShaderDataType["uvec4"		]   = ShaderDataType::Uint4	  ;
		s_Data.StringShaderDataType["sampler2D"	]	= ShaderDataType::Texture ;

		s_Data.StringShaderResourceType["None"							] = ShaderResourceType::None;
		s_Data.StringShaderResourceType["EnitiyID"						] = ShaderResourceType::EnitiyID;
		s_Data.StringShaderResourceType["GlobleResource"				] = ShaderResourceType::GlobleResource;
		s_Data.StringShaderResourceType["LocalModel"					] = ShaderResourceType::LocalModel;
		s_Data.StringShaderResourceType["LocalColor"					] = ShaderResourceType::LocalColor;
		s_Data.StringShaderResourceType["MainCameraViewMatrix"			] = ShaderResourceType::MainCameraViewMatrix;
		s_Data.StringShaderResourceType["MainCameraViewProjectionMatrix"] = ShaderResourceType::MainCameraViewProjectionMatrix;
		s_Data.StringShaderResourceType["MainCamerPos"					] = ShaderResourceType::MainCamerPos;
		s_Data.StringShaderResourceType["MainCamerProjectionMatrix"		] = ShaderResourceType::MainCamerProjectionMatrix;
		s_Data.StringShaderResourceType["AmbientLigths"					] = ShaderResourceType::AmbientLigths;
		s_Data.StringShaderResourceType["PointLigths"					] = ShaderResourceType::PointLigths;
		s_Data.StringShaderResourceType["SpotLigths"					] = ShaderResourceType::SpotLigths;
		s_Data.StringShaderResourceType["DrirektionLigths"				] = ShaderResourceType::DrirektionLigths;
		s_Data.StringShaderResourceType["ShadowTexture"					] = ShaderResourceType::ShadowTexture;
#endif
	}

	void BufferAPI::Shutdown()
	{
#if 1
		s_Data.ShaderDataTypeString.clear();
		s_Data.StringShaderDataType.clear();

		s_Data.StringShaderResourceType.clear();
		s_Data.StringShaderDataType.clear();
#endif
	}

	ShaderDataType BufferAPI::GetShaderDataTypeFromString(const std::string& type)
	{
		RY_CORE_ASSERT(s_Data.StringShaderDataType.find(type) != s_Data.StringShaderDataType.end(), "Critle Error: This '{0}' string, is not a defined Type!", type.c_str());
		return s_Data.StringShaderDataType[type];
	};

	std::string BufferAPI::GetStringFromShaderData(ShaderDataType type)
	{
		return s_Data.ShaderDataTypeString[type];
	}
	std::map<ShaderDataType, std::string>& BufferAPI::GetShaderDataStringMap()
	{
		return s_Data.ShaderDataTypeString;
	}

	std::map<std::string, ShaderDataType>& BufferAPI::GetShaderDataFromStringMap()
	{
		return s_Data.StringShaderDataType;
	}


	std::map<ShaderResourceType, std::string>& BufferAPI::GetShaderResourceTypeStringMap()
	{
		return s_Data.ShaderResourceTypeString;
	}
	std::map<std::string, ShaderResourceType>& BufferAPI::GetShaderResourceTypeFromStringMap()
	{
		return s_Data.StringShaderResourceType;
	}


	

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

	Ref<UniformBuffer> UniformBuffer::Create( uint32_t byteSize, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(byteSize, binding);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(const void* data, uint32_t byteSize, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(data, byteSize, binding);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(const void* data, uint32_t byteSize, const BufferLayout& layout, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: RY_CORE_ASSERT(false, "RendererAPI::None is not supported!") return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(data, byteSize, layout, binding);
		}
		RY_CORE_ASSERT(false, "Unknown RenderAPI!");
		return nullptr;
	}

}
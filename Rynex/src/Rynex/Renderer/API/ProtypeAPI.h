#pragma once
namespace Rynex{
	// BindlesTextureArray.h
	class BindlesTextureArray;

	// Buffer.h
	enum class DataType : uint8_t;
	enum class ShaderDataType : uint8_t;
	enum class BufferDataUsage : uint8_t;
	enum class BufferType : uint8_t;

	namespace BufferFlag {
		enum BufferFlagGPU : uint8_t;
	}

	class BufferLayout;

	struct BufferElement;
	struct DrawElementsIndirectCommand;
	class BufferLayout;

	class BufferAPI;

	class VertexBuffer;
	class IndexBuffer;
	class StorageBuffer;
	class IndirectBuffer;
	class UniformBuffer;


	// Framebuffer.h
	struct FramebufferTextureSpecification;
	struct FramebufferAttachmentSpecification;
	struct FramebufferSpecification;
	class Framebuffer;

	// GraphicsContext.h
	class GraphicsContext;


	// Shader.h
	enum class ShaderResourceType;
	enum class ShaderBufferType;

	class Shader;
	class ShaderLibary;

	// Texture.h
	enum class TextureFormat : uint8_t;
	enum class TextureTarget : uint8_t;
	enum class TextureFilteringMode : uint8_t;
	enum class TextureCompareModes : uint8_t;
	enum class TextureWrappingMode : uint8_t;
	enum class Acces : uint8_t;

	struct TextureWrappingSpecification;
	struct TextureSpecification;

	class Texture;

	class Texture1D;
	class Texture2D;
	class Texture3D;
	class LinkedTextureArray;
	class TextureCubeMap;

	// ThreadContext.h
	class Window;
	class ThreadContext;

	// VertexArray.h
	class VertexArray;

}
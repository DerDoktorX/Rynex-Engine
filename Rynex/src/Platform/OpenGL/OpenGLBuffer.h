#pragma once

#include "Rynex/Renderer/API/Buffer.h"
#include <glad/glad.h>

namespace Rynex {
	

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const void* vertices, uint32_t size);
		OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage);
		OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage, const BufferLayout& layout);
		
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void SetData(const void* data, uint32_t byteSize) override;
		virtual uint32_t GetByteSize() override { return m_Size; }

		virtual const std::vector<unsigned char>& GetBufferData() override;
		virtual void FreeBufferData() override;
		

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

		// virtual const RendererAPI::API GetRendererAPI() const override { return RendererAPI::API::OpenGL; };

	private: 
		uint32_t m_RendererID;
		BufferLayout m_Layout;
		uint32_t m_Size = 0;
		std::vector<unsigned char> m_Data;
		GLenum m_Target;
		BufferDataUsage m_Usage = BufferDataUsage::None;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, BufferDataUsage usage = BufferDataUsage::None);
		OpenGLIndexBuffer(const uint16_t* indices, uint32_t count, BufferDataUsage usage = BufferDataUsage::None);
		virtual ~OpenGLIndexBuffer();

		virtual uint32_t GetCount() const { return m_Count; }
		virtual uint32_t GetElementByte() { return m_EllementByte; }
		virtual void SetData(const uint32_t* indices, uint32_t byteSize) override;
		virtual void SetData(const uint16_t* indices, uint32_t byteSize) override;

		virtual uint32_t GetRenderID() override { return m_RendererID; };

		virtual const std::vector<unsigned char> GetBufferData() override;
		virtual void FreeBufferData() override;

		virtual void Bind() const;
		virtual void UnBind() const;

		// virtual const RendererAPI::API GetRendererAPI() const override { return RendererAPI::API::OpenGL; };
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
		std::vector<unsigned char> m_Data;
		GLenum m_Target;
		uint32_t m_EllementByte = 0;
		BufferDataUsage m_Usage = BufferDataUsage::None;
	};

	class OpenGLStorageBuffer : public StorageBuffer
	{
	public:
		OpenGLStorageBuffer(uint32_t byteSize, Type type);
		OpenGLStorageBuffer(void* data, uint32_t byteSize, Type type);

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void UnBind() const override;

		virtual void SetData(const void* data, uint32_t byteSize) override;

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
		uint32_t m_ByteSize;
		Type m_PolicieType;
	};

}



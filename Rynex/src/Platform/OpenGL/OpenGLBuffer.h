#pragma once
#include "Rynex/Renderer/API/Buffer.h"


namespace Rynex {
	

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const void* vertices, uint32_t size);
		OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage);
		OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage, const BufferLayout& layout);
		OpenGLVertexBuffer(std::vector<unsigned char>&& data, uint32_t size, BufferDataUsage usage, const BufferLayout& layout);
		
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void InitAsync() override;
		virtual void SetData(const void* data, uint32_t byteSize) override;
		virtual uint32_t GetByteSize() override { return m_Size; }

		virtual const std::vector<unsigned char>& GetBufferData() override;
		virtual void FreeBufferData() override;
		

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

		
	private: 
		uint32_t m_RendererID = 0;
		BufferLayout m_Layout;
		uint32_t m_Size = 0;
		std::vector<unsigned char> m_Data;
		uint32_t m_Target = 0;
		BufferDataUsage m_Usage = BufferDataUsage::None;
		
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, BufferDataUsage usage = BufferDataUsage::StaticDraw);
		OpenGLIndexBuffer(const uint16_t* indices, uint32_t count, BufferDataUsage usage = BufferDataUsage::StaticDraw);
		OpenGLIndexBuffer(std::vector<uint32_t>&& data, uint32_t size, BufferDataUsage usage = BufferDataUsage::StaticDraw);
		virtual ~OpenGLIndexBuffer();

		virtual uint32_t GetCount() const { return m_Count; }
		virtual uint32_t GetElementByte() { return m_EllementByte; }

		virtual void InitAsync() override;
		virtual void SetData(const uint32_t* indices, uint32_t byteSize) override;
		virtual void SetData(const uint16_t* indices, uint32_t byteSize) override;

		virtual uint32_t GetRenderID() override { return m_RendererID; };

		virtual const std::vector<uint32_t> GetBufferData() override;
		virtual void FreeBufferData() override;

		virtual void Bind() const;
		virtual void UnBind() const;

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Count = 0;
		std::vector<uint32_t> m_Data;
		uint32_t m_Target = 0;
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

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(std::vector<unsigned char>&& data, const BufferLayout& layout, BufferDataUsage usage, uint32_t binding);
		OpenGLUniformBuffer(uint32_t byteSize, uint32_t binding);
		OpenGLUniformBuffer(const void* data, uint32_t byteSize, uint32_t binding);
		OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout, uint32_t binding);
		OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout, BufferDataUsage usage, uint32_t binding);

		virtual ~OpenGLUniformBuffer();
		virtual void SetOnDelete(std::function<void()> onDelete) override;

		virtual void InitAsync() override;

		virtual void Bind(uint32_t slot = 15) override;

		virtual void SetData(const void* data, uint32_t byteSize) override;
		virtual void SetData(const void* data, uint32_t offset, uint32_t byteSize) override;
		virtual void SetLocelData(const BufferElement& ellement, const void* data, uint32_t byteSize) override;

		virtual uint32_t GetByteSize() override { return m_Size; }

		virtual const std::vector<unsigned char>& GetBufferData() override;
		virtual void FreeBufferData() override;


		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	
		// virtual const RendererAPI::API GetRendererAPI() const override { return RendererAPI::API::OpenGL; };
	private:
		void CheckLayout();
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Binding = 0;
		
		uint32_t m_Size = 0;
		
		uint32_t m_Target = 0;
		BufferDataUsage m_Usage = BufferDataUsage::None;
		
		uint32_t m_ChangeOffset = 0, m_ChangeSize = 0;
		BufferLayout m_Layout = BufferLayout();
		std::vector<unsigned char> m_Data;
		std::function<void()> m_OnDelete = nullptr;
	};
}



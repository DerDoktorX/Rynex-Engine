#pragma once
#include <Rynex/Renderer/API/Buffer.h>


#include <Platform/OpenGL/OpenGLPrototypes.h>

#include <Platform/OpenGL/OpenGLArrayBuffer.h>
#include <Platform/OpenGL/OpenGLShaderStorageBuffer.h>

#define RY_OPENGL_INDRECT_API_SHADER_STORAGE_BUFFER_USE


namespace Rynex {
	

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		using Parent = std::variant<Weak<OpenGLStorageBuffer>, Weak<OpenGLVertexArray>>;
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const void* vertices, uint32_t size);
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage);
		OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage, const BufferLayout& layout);
		OpenGLVertexBuffer(std::vector<uint8_t>&& data, uint32_t size, BufferDataUsage usage, const BufferLayout& layout);
#else
		OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferFlagGPU flag);
		OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferFlagGPU flag, const BufferLayout& layout);
		OpenGLVertexBuffer(std::vector<uint8_t>&& data, uint32_t size, BufferFlagGPU flag, const BufferLayout& layout);

#endif
		virtual ~OpenGLVertexBuffer();

		virtual bool IsTransferd() override;
		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void SetData(const void* data, uint32_t byteSize) override;
		virtual void ResizeBuffer(const void* data, uint32_t byteSize) override;


		virtual uint32_t GetByteSize() const override { return m_Buffer.GetByteSize(); }
		virtual uint32_t GetRenderID() const override { return m_Buffer.GetRenderID(); }

		virtual void CopyData(uint32_t fromPoint, uint32_t toPoint, uint32_t byteSize, Ref<VertexBuffer> vb) override;
		virtual void AddCopyData(Ref<VertexBuffer> vb) override;
		
		virtual const std::vector<uint8_t>& GetBufferData() const override;
		virtual void FreeBufferData() override;
		
		virtual uint32_t GetVertexCount()const override;
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		
		void AddParent(OpenGLStorageBuffer* storageBufferPtr);
		void AddParent(OpenGLVertexArray* vertexArrayPtr);

		void RemoveParent(OpenGLStorageBuffer* storageBufferPtr);
		void RemoveParent(OpenGLVertexArray* vertexArrayPtr);

		void OnSpecifcationChangeCall();
		void OnDataChangeCall();
		void OnDestroyCall();
	private:
		static constexpr const uint32_t s_Target = GL_ARRAY_BUFFER;
	// --- private member varibles --------------------------------------------------------------------------------------------
		BufferLayout m_Layout;
		std::vector<Parent> m_ParentVec;

#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		OpenGLArrayBuffer m_Buffer;
		BufferDataUsage m_Usage;
#else
		OpenGLShaderStorageBuffer m_Buffer;
		BufferFlagGPU m_Flag;
#endif
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, BufferDataUsage usage = BufferDataUsage::StaticDraw);
		OpenGLIndexBuffer(const uint16_t* indices, uint32_t count, BufferDataUsage usage = BufferDataUsage::StaticDraw);
		OpenGLIndexBuffer(std::vector<uint32_t>&& data, uint32_t size, BufferDataUsage usage = BufferDataUsage::StaticDraw);
#else
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, BufferFlagGPU flag = BufferFlag::None);
		OpenGLIndexBuffer(const uint16_t* indices, uint32_t count, BufferFlagGPU usage = BufferFlag::None);
		OpenGLIndexBuffer(std::vector<uint32_t>&& data, uint32_t size, BufferFlagGPU usage = BufferFlag::None);
#endif


		virtual ~OpenGLIndexBuffer();


		virtual uint32_t GetCount() const { return (m_Buffer.GetByteSize() / m_EllementByte); }
		virtual uint32_t GetByteSize() const { return (m_Buffer.GetByteSize()); }

		virtual uint32_t GetElementByte() const { return m_EllementByte; }

		virtual void SetData(const uint32_t* indices, uint32_t byteSize) override;
		virtual void SetData(const uint16_t* indices, uint32_t byteSize) override;

		virtual uint32_t GetRenderID() const override { return m_Buffer.GetRenderID(); };

		virtual void CopyData(uint32_t fromPoint, uint32_t toPoint, uint32_t byteSize, Ref<IndexBuffer> ib) override;
		virtual void AddCopyData(Ref<IndexBuffer> ib) override;

		virtual const std::vector<uint8_t>& GetBufferData() const override;
		virtual void FreeBufferData() override;

		virtual bool IsTransferd() override;
		virtual void Bind() const;
		virtual void UnBind() const;

		void AddParent(OpenGLVertexArray* vertexArrayPtr);
		void RemoveParent(OpenGLVertexArray* vertexArrayPtr);

		void OnSpecifcationChangeCall();
		void OnDataChangeCall();
		void OnDestroyCall();
	private:
		static constexpr const uint32_t s_Target = GL_ELEMENT_ARRAY_BUFFER;
	// --- private member varibles --------------------------------------------------------------------------------------------
		std::vector<Weak<OpenGLVertexArray>> m_ParentVec;
		uint32_t m_EllementByte;
#ifdef RY_OPENGL_USE_ARRAY_BUFFER
		OpenGLArrayBuffer m_Buffer;
		BufferDataUsage m_Usage;

#else
		OpenGLShaderStorageBuffer m_Buffer;
		BufferFlagGPU m_Flag;
#endif
		

	};

	class OpenGLStorageBuffer : public StorageBuffer
	{
	public:

		OpenGLStorageBuffer(uint32_t byteSize, BufferFlagGPU flag);
		OpenGLStorageBuffer(const void* data, uint32_t byteSize, BufferFlagGPU flag);
		OpenGLStorageBuffer(const void* data, uint32_t byteSize, BufferType buffertype, BufferFlagGPU flag);
		virtual ~OpenGLStorageBuffer();

		virtual bool IsTransferd() override;
		virtual void Bind(uint32_t slot = 0) override;
		virtual void UnBind(uint32_t slot) override;
		virtual uint32_t GetByteSize() const override { return m_Buffer.GetByteSize(); }

		virtual void SetData(const void* data, uint32_t byteSize) override;
		virtual void SetData(const void* data, uint32_t offset, uint32_t byteSize) override;
		virtual void ResizeBuffer(uint32_t byteSize)override;
		virtual void ResizeBuffer(const void* data, uint32_t byteSize) override;

		void OnChildeSpecifcationChange(OpenGLUniformBuffer* uniformBufferPtr);
		void OnChildeDataChange(OpenGLUniformBuffer* uniformBufferPtr);
		void OnChildeDestroy(OpenGLUniformBuffer* uniformBufferPtr);

		void OnChildeSpecifcationChange(OpenGLVertexBuffer* vertexBufferPtr);
		void OnChildeDataChange(OpenGLVertexBuffer* vertexBufferPtr);
		void OnChildeDestroy(OpenGLVertexBuffer* vertexBufferPtr);

	private:
		OpenGLShaderStorageBuffer m_Buffer;
		BufferLayout m_Layout;

		uint32_t m_Target;
		BufferFlagGPU m_FlagTypes;
	};

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		using Parent = Weak<OpenGLStorageBuffer>;
	// --- public member funktions --------------------------------------------------------------------------------------------
#ifdef RY_OPENGL_OLD_UNIFORM
		OpenGLUniformBuffer(std::vector<unsigned char>&& data, const BufferLayout& layout, BufferDataUsage usage);
#else	
		OpenGLUniformBuffer(std::vector<unsigned char>&& data, const BufferLayout& layout, BufferFlagGPU flag);
#endif

		OpenGLUniformBuffer(uint32_t byteSize);
		OpenGLUniformBuffer(const void* data, uint32_t byteSize);
		OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout);
#ifdef RY_OPENGL_OLD_UNIFORM
		OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout, BufferDataUsage usage);
#else	
		OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout, BufferFlagGPU flag);
#endif


		OpenGLUniformBuffer(const Ref<OpenGLUniformBuffer>& uniformBufferOpenGL);

		virtual ~OpenGLUniformBuffer();


		virtual bool IsTransferd() override;
		virtual void Bind(uint32_t slot = 15) override;
		virtual void UnBind(uint32_t slot) override;

		virtual void SetData(const void* data, uint32_t byteSize) override;
		virtual void SetData(const void* data, uint32_t offset, uint32_t byteSize) override;

		virtual uint32_t GetByteSize() const override { return m_Buffer.GetByteSize(); }
		virtual uint32_t GetRenderID() const override { return m_Buffer.GetRenderID(); }

		virtual const std::vector<uint8_t>& GetBufferData() const override;
		virtual void FreeBufferData() override;


		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	
		void AddParent(OpenGLStorageBuffer* storageBufferPtr);
		void RemoveParent(OpenGLStorageBuffer* storageBufferPtr);
	
	private:
		void CheckLayout();
	// --- private static varibles --------------------------------------------------------------------------------------------
		static constexpr const uint32_t s_Target = GL_UNIFORM_BUFFER;
	// --- private member varibles --------------------------------------------------------------------------------------------

#ifdef RY_OPENGL_OLD_UNIFORM
		BufferLayout m_Layout;
		OpenGLArrayBuffer m_Buffer;
		std::vector<parent> m_ParentVec;

		uint32_t m_ChangeOffset;
		uint32_t m_ChangeSize;

		BufferDataUsage m_Usage;
#else		
		BufferLayout m_Layout;
		std::vector<Parent> m_ParentVec;

		OpenGLShaderStorageBuffer m_Buffer;

		BufferFlagGPU m_FlagTypes;


#endif
	};
	class OpenGLIndriectBuffer : public IndirectBuffer
	{
	public:
		OpenGLIndriectBuffer(uint32_t byteSize);
		OpenGLIndriectBuffer(const void* data, uint32_t byteSize);
		OpenGLIndriectBuffer(uint32_t byteSize, const BufferLayout& layout);
		OpenGLIndriectBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout);
		virtual ~OpenGLIndriectBuffer();

		virtual bool IsTransferd() override;
		virtual void Bind() const override;
		virtual void UnBind() const override;
#ifdef RY_OPENGL_INDRECT_API_SHADER_STORAGE_BUFFER_USE
		virtual uint32_t GetStrideSize() override { return m_Layout.GetStride(); }
		virtual uint32_t GetCount() override { uint32_t count = m_Buffer.GetByteSize() / GetStrideSize(); return count; }
		virtual uint32_t GetByteSize()override { return m_Buffer.GetByteSize(); }
		virtual const void* GetDataPtr() const override { return m_Buffer.GetData().data(); }
#else
		virtual uint32_t GetStrideSize() override { return m_StrideSize; }
		virtual uint32_t GetCount() override { return m_Count; }
		virtual uint32_t GetByteSize()override { return m_ByteSize; }
		virtual const void* GetDataPtr() const override { return m_Data.data(); }

#endif
		virtual void SetData(const void* data, uint32_t byteSize) override;
		virtual void ResizeBuffer(uint32_t byteSize)override;
		virtual void ResizeBuffer(const void* data, uint32_t byteSize)override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override;
	private:
		// s_Target = GL_DRAW_INDIRECT_BUFFER;
		static constexpr const uint32_t s_Target = GL_DRAW_INDIRECT_BUFFER;

		// s_FlagsOptimz = 0; 0 is the flag for no optimice to cahnge Data
		static constexpr const uint32_t s_FlagsOptimz = 0;
	// --- private member member ----------------------------------------------------------------------------------------------

#ifdef RY_OPENGL_INDRECT_API_SHADER_STORAGE_BUFFER_USE
		OpenGLShaderStorageBuffer m_Buffer;
		BufferLayout m_Layout;
		BufferFlagGPU m_Flag;
#else
		std::vector<unsigned char> m_Data;
		BufferLayout m_Layout;

		uint32_t m_RendererID;
		uint32_t m_ByteSize;
		uint32_t m_Target;
		uint32_t m_StrideSize;
		uint32_t m_Count;
		BufferDataUsage m_Usage;
#endif
	};
	
}



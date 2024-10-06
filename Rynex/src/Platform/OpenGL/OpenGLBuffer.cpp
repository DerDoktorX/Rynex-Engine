#include "rypch.h"
#include "OpenGLBuffer.h"

namespace Rynex {
	
	namespace Utils {
		GLenum static GetBufferDataUsage(BufferDataUsage usage)
		{

			switch (usage)
			{
			case BufferDataUsage::None:			return GL_STATIC_DRAW;
			case BufferDataUsage::DynamicDraw:	return GL_DYNAMIC_DRAW;
			case BufferDataUsage::StaticDraw:	return GL_STATIC_DRAW;
			default:
				RY_CORE_ASSERT(false, "BufferData, GL_STATIC_DRAW");
				return GL_STATIC_DRAW;
			}
			return GL_STATIC_DRAW;
		}

		static GLenum GetPoliciyType(StorageBuffer::Type type)
		{
			return GL_DYNAMIC_STORAGE_BIT;
		}

	}


	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
		: m_Size(size)
		, m_Target(GL_ARRAY_BUFFER)
		, m_Usage(BufferDataUsage::None)
	{
		
		RY_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, size, nullptr, Utils::GetBufferDataUsage(m_Usage));

		

	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage)
		: m_Usage(usage)
		, m_Size(size)
		, m_Target(GL_ARRAY_BUFFER)
	{
		RY_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, size, vertices, Utils::GetBufferDataUsage(m_Usage));

	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage, const BufferLayout& layout)
		: m_Usage(usage)
		, m_Size(size)
		, m_Target(GL_ARRAY_BUFFER)
		
	{
		RY_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, size, vertices, Utils::GetBufferDataUsage(m_Usage));

	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size)
		: m_Size(size)
		, m_Target(GL_ARRAY_BUFFER)
		, m_Usage(BufferDataUsage::None)
	{
		RY_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, Utils::GetBufferDataUsage(m_Usage));

	}

	
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
		FreeBufferData();
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

	}

	void OpenGLVertexBuffer::UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t byteSize)
	{
		m_Size = byteSize;
		RY_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, byteSize, data);



	}

	const std::vector<unsigned char>& OpenGLVertexBuffer::GetBufferData()
	{
		FreeBufferData();
		Bind();
		m_Data.resize(m_Size);
		glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_Data.size(), m_Data.data());

		return m_Data;
	}

	void OpenGLVertexBuffer::FreeBufferData()
	{
		if (m_Data.size())
		{
			m_Data.clear();
		}
	}

	


	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, BufferDataUsage usage)
		: m_Count(count)
		, m_Usage(usage)
		, m_Target(GL_ELEMENT_ARRAY_BUFFER)
		, m_EllementByte(sizeof(uint32_t))
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count * m_EllementByte, indices, Utils::GetBufferDataUsage(m_Usage));
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint16_t* indices, uint32_t count, BufferDataUsage usage)
		: m_Count(count)
		, m_Usage(usage)
		, m_Target(GL_ELEMENT_ARRAY_BUFFER)
		, m_EllementByte(sizeof(uint16_t))
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count * m_EllementByte, indices, Utils::GetBufferDataUsage(m_Usage));
	}

	

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::SetData(const uint32_t* indices, uint32_t count)
	{ 
		RY_PROFILE_FUNCTION();
		m_EllementByte = sizeof(uint32_t);
		
		m_Count = count;
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count * m_EllementByte, indices, Utils::GetBufferDataUsage(m_Usage));
		
		
	}

	void OpenGLIndexBuffer::SetData(const uint16_t* indices, uint32_t count)
	{
		m_EllementByte = sizeof(uint16_t);
		m_Count = count;
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count * m_EllementByte, indices, Utils::GetBufferDataUsage(m_Usage));
	}

	const std::vector<unsigned char> OpenGLIndexBuffer::GetBufferData()
	{

		FreeBufferData();
		m_Data.resize(m_Count * m_EllementByte);
		Bind();
		glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_Data.size(), m_Data.data());

		return m_Data;
	}

	void OpenGLIndexBuffer::FreeBufferData()
	{
		if (m_Data.size())
		{
			m_Data.clear();
		}
	}


	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::UnBind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t byteSize, Type type)
		: m_PolicieType(type)
	{
		RY_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, byteSize, nullptr, Utils::GetPoliciyType(m_PolicieType));
	}

	OpenGLStorageBuffer::OpenGLStorageBuffer(void* data, uint32_t byteSize, Type type)
		: m_PolicieType(type)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, byteSize, data, Utils::GetPoliciyType(m_PolicieType));
	}

	void OpenGLStorageBuffer::Bind(uint32_t slot) const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot,m_RendererID);
		
	}

	void OpenGLStorageBuffer::UnBind() const
	{
		glBindBuffer(0, m_RendererID);
	}

	void OpenGLStorageBuffer::SetData(const void* data, uint32_t byteSize)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, byteSize, data, Utils::GetPoliciyType(m_PolicieType));
	}
}
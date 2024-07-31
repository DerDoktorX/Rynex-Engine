#include "rypch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Rynex {
	
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		RY_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
#if RY_CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)");
#endif
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		RY_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
#if RY_CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)");
#endif
	}

	
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		RY_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		RY_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
#if RY_CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLVertexBuffer::Bind() const");
#endif
	}

	void OpenGLVertexBuffer::UnBind() const
	{
		RY_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#if RY_CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLVertexBuffer::UnBind() const");
#endif
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t byteSize)
	{
		RY_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, byteSize, data);

#if RY_CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)");
#endif
	}



	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
		: m_Count(count)
	{
		RY_PROFILE_FUNCTION();
#if RY_CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)");
#endif
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		RY_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
#if RY_CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)");
#endif		
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		RY_PROFILE_FUNCTION();
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::SetData(const uint32_t* indices, uint32_t count)
	{
		RY_PROFILE_FUNCTION();
		m_Count = count;
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
#if RY_CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLIndexBuffer::SetData(const uint32_t* indices, uint32_t count)");
#endif

	}

	void OpenGLIndexBuffer::Bind() const
	{
		RY_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
#if RY_CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLIndexBuffer::Bind()");
#endif
	}

	void OpenGLIndexBuffer::UnBind() const
	{
		RY_PROFILE_FUNCTION();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#if RY_CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLIndexBuffer::UnBind()");
#endif
	}
}
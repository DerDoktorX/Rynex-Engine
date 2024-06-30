#include "rypch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>
#define CONSOLE_LOG_FUNKTION_OPENGL 0
namespace Rynex {
	
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)");
#endif
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)");
#endif
	}

	
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);

	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLVertexBuffer::Bind() const");
#endif
	}

	void OpenGLVertexBuffer::UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLVertexBuffer::UnBind() const");
#endif
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)");
#endif
	}



	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
		: m_Count(count)
	{
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)");
#endif
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)");
#endif		
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);

	}

	void OpenGLIndexBuffer::SetData(const uint32_t* indices, uint32_t count)
	{
		m_Count = count;
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLIndexBuffer::SetData(const uint32_t* indices, uint32_t count)");
#endif

	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLIndexBuffer::Bind()");
#endif
	}

	void OpenGLIndexBuffer::UnBind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLIndexBuffer::UnBind()");
#endif
	}
}
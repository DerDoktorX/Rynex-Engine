#include "rypch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Rynex {

	static GLenum ShaderDaterTyToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:			return GL_FLOAT;
		case ShaderDataType::Float2:		return GL_FLOAT;
		case ShaderDataType::Float3:		return GL_FLOAT;
		case ShaderDataType::Float4:		return GL_FLOAT;
		case ShaderDataType::Float3x3:		return GL_FLOAT;
		case ShaderDataType::Float4x4:		return GL_FLOAT;
		//caseShaderDataType::FloatArray:	return 4;
		case ShaderDataType::Int:			return GL_INT;
		case ShaderDataType::Int2:			return GL_INT;
		case ShaderDataType::Int3:			return GL_INT;
		case ShaderDataType::Int4:			return GL_INT;
		case ShaderDataType::Int3x3:		return GL_INT;
		case ShaderDataType::Int4x4:		return GL_INT;
		//case ShaderDataType::IntArray:	return 4;
		//case ShaderDataType::Uint:		return 4;
		//case ShaderDataType::Uint2:		return 4;
		//case ShaderDataType::Uint3:		return 4;
		//case ShaderDataType::Uint4:		return 4;
		//case ShaderDataType::Uint3x3:		return 4;
		//case ShaderDataType::Uint4x4:		return 4;
		//case ShaderDataType::UintArray	return 4;
		case ShaderDataType::Bool:			return GL_BOOL;
		//case ShaderDataType::Struct:		return 4;
		}
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}



	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::UnBind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		RY_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer Has no Layoute!");
		
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();
		
		uint32_t index = 0;
		for (const auto& elements : vertexBuffer->GetLayout())
		{
			switch (elements.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glEnableVertexAttribArray(index);
				glVertexAttribPointer(index,
					elements.GetCompontsCount(),
					ShaderDaterTyToOpenGLBaseType(elements.Type),
					elements.Normilized ? GL_TRUE : GL_FALSE,
					vertexBuffer->GetLayout().GetStride(),
					(const void*)elements.Offset);
				index++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			{
				glEnableVertexAttribArray(index);
				glVertexAttribIPointer(index,
					elements.GetCompontsCount(),
					ShaderDaterTyToOpenGLBaseType(elements.Type),
					vertexBuffer->GetLayout().GetStride(),
					(const void*)elements.Offset);
				index++;
				break;
			}
			case ShaderDataType::Bool:
			{
				glEnableVertexAttribArray(index);
				glVertexAttribIPointer(index,
					elements.GetCompontsCount(),
					ShaderDaterTyToOpenGLBaseType(elements.Type),
					vertexBuffer->GetLayout().GetStride(),
					(const void*)elements.Offset);
				index++;
				break;
			}
			}

			
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}
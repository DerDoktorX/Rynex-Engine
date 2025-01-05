#include "rypch.h"
#include "OpenGLVertexArray.h"

#include <Platform/OpenGL/OpenGLThreadContext.h>
#include "Rynex/Core/Application.h"

#include <glad/glad.h>

namespace Rynex {

	namespace Utils{

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
				//case ShaderDataType::Bool:		return GL_BOOL;
				//case ShaderDataType::Struct:		return 4;
			}
		}

	}

	OpenGLVertexArray::OpenGLVertexArray()
		: m_Primitv(VertexArray::Primitv::Traingle)
	{
		RY_PROFILE_FUNCTION();
		if(OpenGLThreadContext::IsActive())
		{
			InitAsync();
		}
		else
		{
			Application::Get().SubmiteToMainThreedQueue([this]() {
				InitAsync();
				});
		}

	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		RY_PROFILE_FUNCTION();
		glDeleteVertexArrays(1, &m_RendererID);
	}



	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
		if (m_IndexBuffer)
			m_IndexBuffer->Bind();

		for (auto& vertexBuffer : m_VertexBuffers)
			vertexBuffer->Bind();
	}

	void OpenGLVertexArray::UnBind() const
	{
		glBindVertexArray(0);
		if (m_IndexBuffer)
			m_IndexBuffer->UnBind();

		for (auto& vertexBuffer : m_VertexBuffers)
			vertexBuffer->UnBind();
	}

	void OpenGLVertexArray::SetPrimitv(Primitv primitv)
	{
		m_Primitv = primitv;
	}

	VertexArray::Primitv OpenGLVertexArray::GetPrimitv()
	{
		return m_Primitv;
	}

	

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		if(m_RendererID && OpenGLThreadContext::IsActive())
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
						Utils::ShaderDaterTyToOpenGLBaseType(elements.Type),
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
						Utils::ShaderDaterTyToOpenGLBaseType(elements.Type),
						vertexBuffer->GetLayout().GetStride(),
						(const void*)elements.Offset);
					index++;
					break;
				}
#if 0
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
#endif
				}


			}
			m_VertexBuffers.push_back(vertexBuffer);
		}
		else
		{
			Ref<VertexBuffer> vertexBufferC = vertexBuffer;
			Application::Get().SubmiteToMainThreedQueue([this, vertexBufferC]() {
				AddVertexBuffer(vertexBufferC);
			});
		}
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		RY_PROFILE_FUNCTION();
		if (m_RendererID && OpenGLThreadContext::IsActive())
		{
			glBindVertexArray(m_RendererID);
			indexBuffer->Bind();

			m_IndexBuffer = indexBuffer;
		}
		else
		{
			Ref<IndexBuffer> indexBufferC = indexBuffer;
			Application::Get().SubmiteToMainThreedQueue([this, indexBufferC]() {
				SetIndexBuffer(indexBufferC);
				});
		}

	}

	void OpenGLVertexArray::SetBoxAABB(BufferElement postionElement)
	{
		if(m_RendererID && OpenGLThreadContext::IsActive())
		{
			for (auto& buffer : m_VertexBuffers)
			{
				m_Box.SetBoxAABB(buffer, postionElement);
			}
		}
		else
		{
			
			Application::Get().SubmiteToMainThreedQueue([this, postionElement]() {
				SetBoxAABB(postionElement);
			});
		}
	}

	

	const char* OpenGLVertexArray::GetPrimitvChar() const
	{
		switch (m_Primitv)
		{
			case VertexArray::Primitv::Traingle:		return {"Traingle"};
			case VertexArray::Primitv::TraingleStrips:	return {"TraingleStrips"};
			case VertexArray::Primitv::TraingleFan:		return {"TraingleFan"};
			case VertexArray::Primitv::Line:			return {"Line"};
			case VertexArray::Primitv::LineLoop:		return {"LineLoop"};
			case VertexArray::Primitv::LineStrips:		return {"LineStrips"};
			case VertexArray::Primitv::Points:			return {"Points"};
			default:
				break;
		}
		RY_CORE_ASSERT(false, "Not known Type! OpenGLVertexArray::GetPrimitvChar()");

		return nullptr;
	}

	void OpenGLVertexArray::InitAsync()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}
}
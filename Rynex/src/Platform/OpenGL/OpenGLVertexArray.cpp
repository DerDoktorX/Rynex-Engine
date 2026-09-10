#include "rypch.h"
#include "OpenGLVertexArray.h"

#include <Platform/OpenGL/OpenGLBase.h>
#include <Platform/OpenGL/OpenGLBuffer.h>

#include <glad/glad.h>

namespace Rynex {
#define RY_OPENGL_VERTEX_OLD 0
#define RY_OPENGL_VAO_ABSTRACTION_DISABLE 0
	namespace Utils{

		static GLenum ShaderDaterTyToOpenGLBaseType(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				case ShaderDataType::Float3x3:
				case ShaderDataType::Float4x4:
					return GL_FLOAT;
					//caseShaderDataType::FloatArray:	return 4;
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Int3x3:	
				case ShaderDataType::Int4x4:	
					return GL_INT;
				//case ShaderDataType::IntArray:	return 4;
				case ShaderDataType::Uint:
				case ShaderDataType::Uint2:
				case ShaderDataType::Uint3:
				case ShaderDataType::Uint4:
					return GL_UNSIGNED_INT;
			}
			RY_CORE_ASSERT(false, "No, Valid enum Type!");
			return GL_UNSIGNED_INT;
		}

		static bool ShaderDaterTypeSameType(ShaderDataType typeA, ShaderDataType typeB)
		{
			return ShaderDaterTyToOpenGLBaseType(typeA) == ShaderDaterTyToOpenGLBaseType(typeB);
		}

		static uint32_t PrimitvVertxCount( const VertexArray::Primitv& primitv, uint32_t vertexCount)
		{
			switch (primitv)
			{
			case VertexArray::Primitv::Traingle:		return vertexCount / 3u;
			case VertexArray::Primitv::TraingleStrips:	return vertexCount - 2u;
			case VertexArray::Primitv::TraingleFan:		return vertexCount - 2u;
			case VertexArray::Primitv::Line:			return vertexCount / 2u;
			case VertexArray::Primitv::LineLoop:		return vertexCount - 2u;
			case VertexArray::Primitv::LineStrips:		return vertexCount - 2u;
			case VertexArray::Primitv::Points:			return vertexCount;
			case VertexArray::Primitv::Patches:			return vertexCount;
			default: return 0u;
			}
		}
#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
		static uint32_t VertexCount(const std::vector<Ref<VertexBuffer>>& vertexElements, const Ref<IndexBuffer>& index, const VertexArray::Primitv& primitv)
#else
		static uint32_t VertexCount(const std::vector<OpenGLVertexArray::VertexElements>& vertexElements, const Ref<IndexBuffer>& index, const VertexArray::Primitv& primitv)
#endif
		{
			if (!vertexElements.empty() && index)
			{
				return PrimitvVertxCount(primitv, index->GetCount());
			}
			return 0;
		}

		static GLenum PrimtivGL(VertexArray::Primitv primtiv)
		{
			switch (primtiv)
			{
				RY_INTERNLE_GET_OPENGL_MACRO_CASE(VertexArray::Primitv::Traingle, GL_TRIANGLES);
				RY_INTERNLE_GET_OPENGL_MACRO_CASE(VertexArray::Primitv::TraingleStrips, GL_TRIANGLE_STRIP);
				RY_INTERNLE_GET_OPENGL_MACRO_CASE(VertexArray::Primitv::TraingleFan, GL_TRIANGLE_FAN);
				RY_INTERNLE_GET_OPENGL_MACRO_CASE(VertexArray::Primitv::Line, GL_LINE);
				RY_INTERNLE_GET_OPENGL_MACRO_CASE(VertexArray::Primitv::LineLoop, GL_LINE);
				RY_INTERNLE_GET_OPENGL_MACRO_CASE(VertexArray::Primitv::LineStrips, GL_LINE_STRIP);
				RY_INTERNLE_GET_OPENGL_MACRO_CASE(VertexArray::Primitv::Points, GL_LINE_STRIP);
				RY_INTERNLE_GET_OPENGL_MACRO_CASE(VertexArray::Primitv::Patches, GL_PATCHES);
				RY_INTERNLE_GET_OPENGL_MACRO_DEFAULT(VertexArray::Primitv);
			}
		}
	}

	uint32_t OpenGLVertexArray::s_LastBindVAO = 0u;

	OpenGLVertexArray::OpenGLVertexArray()
		: m_Primitv(VertexArray::Primitv::Traingle)
		, m_PrimitvGL(GL_TRIANGLES)
	{
		RY_PROFILE_FUNCTION();
#if 0
		if (!OpenGLThreadContext::IsActive())
		{
			std::function<void()> func = std::bind(&OpenGLVertexArray::InitAsync, this);
#if RY_GRAFIC_SUBMIT_TO_MAIN_THREAD_WITHE_OUT_WAIT
			Application::Get().SubmiteToMainThreedQueue(func);
#else
			Application::Get().SubmiteToMainThreedQueueWait(func);
#endif			
			return;
		}
#endif
		RY_EXE_ON_MAIN_THREAD_FUNCTION(OpenGLVertexArray::CreateID);
		

	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());



		DestroyID();

#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
		for (auto& vertexBuffer : m_VertexBuffers)
		{
			vertexBuffer.reset();
			vertexBuffer = nullptr;
		}
#else
		for (VertexElements& vertexElements : m_VertexElements)
		{
			RY_DESTROY_REF(vertexElements.Buffer);
		}
#endif
		m_VertexElements.clear();

		RY_DESTROY_REF(m_IndexBuffer);

		
	}



	void OpenGLVertexArray::Bind() const
	{
		RY_CORE_ASSERT(0u != m_RendererID);
#if RY_OPENGL_BIND_RENDER_COMAND
		OpenGLRenderCommand::BindVertexArray(m_RendererID);
#else
		if(s_LastBindVAO != m_RendererID)
		{
			glBindVertexArray(m_RendererID);
			s_LastBindVAO = m_RendererID;
		}
#endif

		// m_IndexBuffer->Bind();
		// for (auto& indexBuffer : m_IndexBuffers)
		// 	indexBuffer->UnBind();
		// 
		// for (auto& vertexBuffer : m_VertexBuffers)
		// 	vertexBuffer->Bind();
	}

	void OpenGLVertexArray::UnBind() const
	{
		RY_CORE_ASSERT(0u != m_RendererID);
#if RY_OPENGL_BIND_RENDER_COMAND
		OpenGLRenderCommand::BindVertexArray(0);	
#else
		if (s_LastBindVAO != 0)
		{
			glBindVertexArray(0);
			s_LastBindVAO = m_RendererID;
		}
#endif
	}

	void OpenGLVertexArray::SetPrimitv(Primitv primitv)
	{
		m_Primitv = primitv;
		m_PrimitvGL = Utils::PrimtivGL(m_Primitv);
	}

	VertexArray::Primitv OpenGLVertexArray::GetPrimitv()
	{
		return m_Primitv;
	}

	void OpenGLVertexArray::ClearVertexBuffers()
	{
#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
		glBindVertexArray(m_RendererID);
		uint32_t index = 0;
		for (const auto& vertexBuffer : m_VertexBuffers)
		{
			vertexBuffer->Bind();
			uint32_t size = vertexBuffer->GetLayout().GetLength();
			for (uint32_t i = 0; i < size; i++)
			{
				glDisableVertexAttribArray(index);
				index++;
			}
			vertexBuffer->UnBind();
			
		}
		glBindVertexArray(0);
		m_VertexBuffers.clear();
#else
		uint32_t index = 0;
#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
		for (auto& vertexBuffer : m_VertexBuffers)
		{
			uint32_t size = vertexBuffer->GetLayout().GetLength();
			for (uint32_t i = 0; i < size; i++)
			{
				glDisableVertexArrayAttrib(m_RendererID, index);
				index++;
			}
		}
#else
		uint32_t i = 0;
		for (VertexElements& vertexElements : m_VertexElements)
		{
			uint32_t size = vertexElements.UseLayout.GetLength();
			glDisableVertexArrayAttrib(m_RendererID, i);
			Ref<OpenGLVertexBuffer> vertexBufferOpenGL = std::static_pointer_cast<OpenGLVertexBuffer>(vertexElements.Buffer);
			vertexBufferOpenGL->RemoveParent(this);
			RY_DESTROY_REF(vertexElements.Buffer);
			i++;
		}
#endif


#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
		m_VertexBuffers.clear();
#endif 
		m_VertexElements.clear();
#endif
	}

	

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		Ref<VertexBuffer> vb = vertexBuffer;
		BufferLayout layout = vb->GetLayout();

#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			std::function<void()> func = std::bind(&OpenGLVertexArray::AddVertexBufferToList, this, vb, layout);
#if RY_GRAFIC_SUBMIT_TO_MAIN_THREAD_WITHE_OUT_WAIT
			Application::Get().SubmiteToMainThreedQueue(func);
#else
			Application::Get().SubmiteToMainThreedQueueWait(func);
#endif
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME_ARGS(OpenGLVertexArray::AddVertexBufferToList, vb, layout);
#endif
		
		RY_CORE_ASSERT(0u != m_RendererID);
		
		AddVertexBufferToList(vb, layout);

#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
			const BufferLayout& layout = vertexBuffer->GetLayout();
			const std::vector<BufferElement>& ellements = layout.GetElements();
			uint32_t ellemntsCount = ellements.size();
			uint32_t strideSize = layout.GetStride();

			RY_CORE_ASSERT(0 != ellemntsCount, "Vertex Buffer Has no Layoute!");
#if RY_OPENGL_VERTEX_OLD
			glBindVertexArray(m_RendererID);
			vertexBuffer->Bind();
#else
			uint32_t size = m_VertexBuffers.size();
			Ref<OpenGLVertexBuffer> vertexBufferOpengGL = std::static_pointer_cast<OpenGLVertexBuffer>(vertexBuffer);
			uint32_t vertexBufferRenderID = vertexBufferOpengGL->GetRenderID();
			glVertexArrayVertexBuffer(m_RendererID, size, vertexBufferRenderID, 0, strideSize);
#endif

			uint32_t index = 0;
			for (const Ref<VertexBuffer>& vbo : m_VertexBuffers)
			{
				index += vbo->GetLayout().GetLength();
			}

			for (const BufferElement& elements : layout)
			{
				switch (elements.Type)
				{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
#if RY_OPENGL_VERTEX_OLD
					glEnableVertexAttribArray(index);

					glVertexAttribPointer(index,
						elements.GetCompontsCount(),
						Utils::ShaderDaterTyToOpenGLBaseType(elements.Type),
						elements.Normilized ? GL_TRUE : GL_FALSE,
						vertexBuffer->GetLayout().GetStride(),
						(const void*)elements.offset);
					
#else
					glEnableVertexArrayAttrib(m_RendererID, index);

					GLenum glType = Utils::ShaderDaterTyToOpenGLBaseType(elements.Type);
					GLint elmenCount = elements.GetCompontsCount();
					GLboolean normalize = elements.Normilized ? GL_TRUE : GL_FALSE;
					GLint realtivOffset = elements.offset;

					glVertexArrayAttribFormat(m_RendererID, index, elmenCount, glType, normalize, realtivOffset);
					glVertexArrayAttribBinding(m_RendererID, index, 0);
#endif
					index++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				{
#if RY_OPENGL_VERTEX_OLD
					glEnableVertexAttribArray(index);

					glVertexAttribPointer(index,
						elements.GetCompontsCount(),
						Utils::ShaderDaterTyToOpenGLBaseType(elements.Type),
						elements.Normilized ? GL_TRUE : GL_FALSE,
						vertexBuffer->GetLayout().GetStride(),
						(const void*)elements.offset);
					
#else
					glEnableVertexArrayAttrib(m_RendererID, index);

					GLenum glType = Utils::ShaderDaterTyToOpenGLBaseType(elements.Type);
					GLint elmenCount = elements.GetCompontsCount();
					GLboolean normalize = elements.Normilized ? GL_TRUE : GL_FALSE;
					GLint realtivOffset = elements.offset;

					glVertexArrayAttribIFormat(m_RendererID, index, elmenCount, glType, realtivOffset);
					glVertexArrayAttribBinding(m_RendererID, index, 0);
#endif

					index++;
					break;
				}
				case ShaderDataType::Uint:
				case ShaderDataType::Uint2:
				case ShaderDataType::Uint3:
				case ShaderDataType::Uint4:
				{
#if RY_OPENGL_VERTEX_OLD
					glEnableVertexAttribArray(index);

					glVertexAttribPointer(index,
						elements.GetCompontsCount(),
						Utils::ShaderDaterTyToOpenGLBaseType(elements.Type),
						elements.Normilized ? GL_TRUE : GL_FALSE,
						vertexBuffer->GetLayout().GetStride(),
						(const void*)elements.offset);

#else
					glEnableVertexArrayAttrib(m_RendererID, index);

					GLenum glType = Utils::ShaderDaterTyToOpenGLBaseType(elements.Type);
					GLint elmenCount = elements.GetCompontsCount();
					GLboolean normalize = elements.Normilized ? GL_TRUE : GL_FALSE;
					GLint realtivOffset = elements.offset;

					glVertexArrayAttribIFormat(m_RendererID, index, elmenCount, glType, realtivOffset);
					glVertexArrayAttribBinding(m_RendererID, index, 0);
#endif
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
						(const void*)elements.offset);
					index++;
					break;
				}
#endif
				}


			}

			m_VertexBuffers.emplace_back(vertexBuffer);
			m_RenderResterRiezer = Utils::VertexCount(m_VertexBuffers, m_DepthIAB, m_Primitv);
#if RY_OPENGL_VERTEX_OLD
			glBindVertexArray(0);
#endif
#else

		
#endif
		
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer, const BufferLayout& layout)
	{
		const Ref<VertexBuffer>& vb = vertexBuffer;
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			std::function<void()> func = std::bind(&OpenGLVertexArray::AddVertexBufferToList, this, vb, layout);
#if RY_GRAFIC_SUBMIT_TO_MAIN_THREAD_WITHE_OUT_WAIT
			Application::Get().SubmiteToMainThreedQueue(func);
#else
			Application::Get().SubmiteToMainThreedQueueWait(func);
#endif
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME_ARGS(OpenGLVertexArray::AddVertexBufferToList, vb, layout);
#endif

		RY_CORE_ASSERT(0u != m_RendererID);

		AddVertexBufferToList(vb, layout);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			std::function<void()> func = std::bind(&OpenGLVertexArray::SetIndexBuffer, this, indexBuffer);
#if RY_GRAFIC_SUBMIT_TO_MAIN_THREAD_WITHE_OUT_WAIT
			Application::Get().SubmiteToMainThreedQueue(func);
#else
			Application::Get().SubmiteToMainThreedQueueWait(func);
#endif
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME_ARGS(OpenGLVertexArray::SetIndexBuffer, indexBuffer);
#endif

		RY_CORE_ASSERT(0u != m_RendererID);
		

		if (nullptr == m_IndexBuffer)
		{
			Ref<OpenGLIndexBuffer> indexBufferOpengGLOld = std::static_pointer_cast<OpenGLIndexBuffer>(indexBuffer);
			indexBufferOpengGLOld->RemoveParent(this);
		}
		m_IndexBuffer = indexBuffer;
#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
		m_RenderResterRiezer = Utils::VertexCount(m_VertexBuffers, m_DepthIAB, m_Primitv);
#else
		m_RenderResterRiezer = Utils::VertexCount(m_VertexElements, m_IndexBuffer, m_Primitv);

#endif

#if RY_OPENGL_VERTEX_OLD
		glBindVertexArray(m_RendererID);
		m_DepthIAB->Bind();
		glBindVertexArray(0);
#else
		Ref<OpenGLIndexBuffer> indexBufferOpengGL = std::static_pointer_cast<OpenGLIndexBuffer>(indexBuffer);
		indexBufferOpengGL->AddParent(this);
		uint32_t indexBufferRenderID = indexBufferOpengGL->GetRenderID();
		glVertexArrayElementBuffer(m_RendererID, indexBufferRenderID);

#endif
		
	}

#if 1
	const Ref<IndexBuffer>& OpenGLVertexArray::GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}
#endif

	void OpenGLVertexArray::SetVertexBufferNew(const Ref<VertexBuffer>& vertexBuffer)
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueue(std::bind(&OpenGLVertexArray::SetVertexBufferNew, this, vertexBuffer));
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME_ARGS(OpenGLVertexArray::SetVertexBufferNew, vertexBuffer);
#endif
		RY_CORE_ASSERT(0u != m_RendererID);


#if 1
		uint32_t bindIndex = 0u;
		uint32_t elementsCount = 0u;
		GLint absolutOffset = 0u;
		for (VertexElements& vElemt : m_VertexElements)
		{
			BufferLayout& layout = vElemt.UseLayout;
			Ref<VertexBuffer> vab = vElemt.Buffer;
			
			RY_CORE_ASSERT(nullptr != vab);
			if (vertexBuffer == vab)
			{
				Ref<OpenGLVertexBuffer> vertexBufferOpenGL = std::static_pointer_cast<OpenGLVertexBuffer>(vab);
				if (!SetupVertexBuffersToVAO(bindIndex, layout, vertexBufferOpenGL))
				{
					bindIndex++;
					continue;
				}
				if(IsLayouteEqual(layout, vertexBufferOpenGL))
				{
					bindIndex++;
					continue;
				}

				SetupVertexBuffersToLayoutVAO(bindIndex, elementsCount, layout, vertexBufferOpenGL);
				return;
			}
			else
			{
				elementsCount += layout.GetLength();
			}
			bindIndex++;
		}
		RY_CORE_ASSERT(false, "We diden't found this vertex buffer in the curen List!");
#else
		Invalidate();
#endif
	}

	uint32_t OpenGLVertexArray::GetVertexBuffersCount() const
	{
		uint32_t count = static_cast<uint32_t>(m_VertexElements.size());
		return count;
	}

	

	

	void OpenGLVertexArray::SetBoxAABB(BufferElement postionElement)
	{		
		
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueue(std::bind(&OpenGLVertexArray::SetVertexBufferNew, this, vertexBuffer));
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME_ARGS(OpenGLVertexArray::SetBoxAABB, postionElement);
#endif
		RY_CORE_ASSERT(0u != m_RendererID);

#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
		for (auto& buffer : m_VertexBuffers)
		{
			m_Box.SetBoxAABB(buffer, postionElement);
		}
#else
		for (VertexElements& bufferEllements : m_VertexElements)
		{
			m_Box.SetBoxAABB(bufferEllements.Buffer, m_IndexBuffer, postionElement);
		}
#endif
	}

	


	uint32_t OpenGLVertexArray::GetTringleCount() const
	{
		return m_RenderResterRiezer;
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

	

	void OpenGLVertexArray::OnChildeSpecifcationChange(OpenGLVertexBuffer* vertexBufferPtr)
	{
		for (const VertexElements& vElemt : m_VertexElements)
		{
			Ref<VertexBuffer> vab = vElemt.Buffer;
			Ref<OpenGLVertexBuffer> vertexBufferOpenGL = std::static_pointer_cast<OpenGLVertexBuffer>(vab);

			if (vertexBufferOpenGL.get() == vertexBufferPtr)
			{
				Invalidate();
				return;
			}
		}
		RY_CORE_ASSERT(false, "VertexBuffer Not found!");

	}

	void OpenGLVertexArray::OnChildeDataChange(OpenGLVertexBuffer* vertexBufferPtr)
	{
#if 0
		for (const VertexElements& vElemt : m_VertexElements)
		{
			Ref<VertexBuffer> vab = vElemt.Buffer;
			Ref<OpenGLVertexBuffer> vertexBufferOpenGL = std::static_pointer_cast<OpenGLVertexBuffer>(vab);

			if (vertexBufferOpenGL.get() == vertexBufferPtr)
			{
				Invalidate();
				return;
			}
		}
		RY_CORE_ASSERT(false, "VertexBuffer Not found!");
#endif
		// we are oly interested in specifcation change, like resizing.
	}

	void OpenGLVertexArray::OnChildeDestroy(OpenGLVertexBuffer* vertexBufferPtr)
	{
		RY_CORE_ASSERT("This funktion in this class shound never be to able get called!");
	}



	void OpenGLVertexArray::OnChildeSpecifcationChange(OpenGLIndexBuffer* indexBufferPtr)
	{
		Ref<OpenGLIndexBuffer> indexBufferOpenGL = std::static_pointer_cast<OpenGLIndexBuffer>(m_IndexBuffer);

		if (indexBufferOpenGL.get() == indexBufferPtr)
		{
			Invalidate();
			return;
		}
		RY_CORE_ASSERT(false, "VertexBuffer Not found!");

	}

	void OpenGLVertexArray::OnChildeDataChange(OpenGLIndexBuffer* indexBufferPtr)
	{
		Ref<OpenGLIndexBuffer> indexBufferOpenGL = std::static_pointer_cast<OpenGLIndexBuffer>(m_IndexBuffer);

		if (indexBufferOpenGL.get() == indexBufferPtr)
		{
			Invalidate();
			return;
		}
		RY_CORE_ASSERT(false, "VertexBuffer Not found!");

	}

	void OpenGLVertexArray::OnChildeDestroy(OpenGLIndexBuffer* vertexBufferPtr)
	{
		RY_CORE_ASSERT("This funktion in this class shound never be to able get called!");
	}

	void OpenGLVertexArray::AddVertexBufferToList(const Ref<VertexBuffer>& vb, const BufferLayout& layout)
	{
#if RY_OPENGL_VAO_ABSTRACTION_DISABLE
		m_VertexBuffers.emplace_back(vb);
#endif
		VertexElements& vertexElemelts = m_VertexElements.emplace_back<VertexElements>(VertexElements{ layout, vb });
		Ref<OpenGLVertexBuffer> openGLbuffer = std::static_pointer_cast<OpenGLVertexBuffer>(vertexElemelts.Buffer);
		openGLbuffer->AddParent(this);
		m_RenderResterRiezer = Utils::VertexCount(m_VertexElements, m_IndexBuffer, m_Primitv);
		
		Invalidate();
	}

	void OpenGLVertexArray::CreateID()
	{ 
		DestroyID();

		RY_CORE_ASSERT(0u == m_RendererID);
		RY_OPENGL_VERTEX_ARRAY_ID_SCOPE_LOCK();

#if RY_OPENGL_VERTEX_OLD
		glGenVertexArrays(1, &m_RendererID);
#else
		glCreateVertexArrays(1, &m_RendererID);
#endif
	
	}

	void OpenGLVertexArray::DestroyID()
	{
		if (0u == m_RendererID)
			return;
		RY_OPENGL_VERTEX_ARRAY_ID_SCOPE_LOCK();
		glDeleteVertexArrays(1, &m_RendererID);
		m_RendererID = 0u;
	}

	void OpenGLVertexArray::Invalidate()
	{
#ifndef RY_OPENGL_MAIN_THREADE
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLVertexArray::Invalidate);
#endif
		if (0u == m_RendererID)
		{
			CreateID();
		}

		uint32_t bindIndex = 0;
		uint32_t elementIndex = 0;
		
		
		for (VertexElements& vElemt : m_VertexElements)
		{
			BufferLayout& layout = vElemt.UseLayout;
			Ref<VertexBuffer> vab = vElemt.Buffer;

			RY_CORE_ASSERT(nullptr != vab);
			Ref<OpenGLVertexBuffer> vertexBufferOpenGL = std::static_pointer_cast<OpenGLVertexBuffer>(vab);
			
			if (!SetupVertexBuffersToVAO(bindIndex, layout, vertexBufferOpenGL))
			{			
				bindIndex++;
				continue;
			}

			SetupVertexBuffersToLayoutVAO(bindIndex, elementIndex, layout, vertexBufferOpenGL);
			bindIndex++;
		}

		if(nullptr != m_IndexBuffer)
		{
			Ref<OpenGLIndexBuffer> indexBufferOpengGL = std::static_pointer_cast<OpenGLIndexBuffer>(m_IndexBuffer);
			uint32_t indexBufferRenderID = indexBufferOpengGL->GetRenderID();
			glVertexArrayElementBuffer(m_RendererID, indexBufferRenderID);
		}

	}

	uint32_t OpenGLVertexArray::AddFloatBufferEllementToVAO(uint32_t& indexElemment, uint32_t bindIndex, const BufferElement& element, uint32_t ellementReserverdComponts)
	{
		constexpr GLuint bytesPerCompent = sizeof(float);
		constexpr GLint maxCompontsCount = 4;
		const GLenum glType = Utils::ShaderDaterTyToOpenGLBaseType(element.type);
		const GLint elmenCount = element.GetCompontsCount() + ellementReserverdComponts;
		const GLboolean normalize = element.normilized ? GL_TRUE : GL_FALSE;
		const GLuint ellementReserverdCompontsBytes = ellementReserverdComponts * bytesPerCompent;
		const GLuint realtivOffset = element.offset - ellementReserverdCompontsBytes;
		const GLuint expextedOffsetPos = element.size + element.offset;

		GLint usedElementCount = 0;
		GLint notSetElementCount = elmenCount;

		GLuint curentRealtivElementOffset = realtivOffset;
		uint32_t indexElemmentCount = 0u;
		GLint unUsedComponetCount = 0;
#if 1
		while (0 < notSetElementCount)
		{
			GLint setElementCount = maxCompontsCount < notSetElementCount ? maxCompontsCount : notSetElementCount;
			GLuint ofsetBytesCount = setElementCount * bytesPerCompent;

			glEnableVertexArrayAttrib(m_RendererID, indexElemment);
			glVertexArrayAttribFormat(m_RendererID, indexElemment, setElementCount, glType, normalize, curentRealtivElementOffset);
			glVertexArrayAttribBinding(m_RendererID, indexElemment, bindIndex);

			notSetElementCount -= setElementCount;
			usedElementCount += setElementCount;
			curentRealtivElementOffset += ofsetBytesCount;
			indexElemmentCount++;

			GLint elmentCountUseAndUnusedAdded = notSetElementCount + usedElementCount;
			unUsedComponetCount = maxCompontsCount - setElementCount;
			RY_CORE_ASSERT(elmentCountUseAndUnusedAdded == elmenCount);
			indexElemment++;
		}
		curentRealtivElementOffset += ellementReserverdCompontsBytes;

		RY_CORE_ASSERT(curentRealtivElementOffset == expextedOffsetPos);
		RY_CORE_ASSERT(0 == notSetElementCount);
		RY_CORE_ASSERT(usedElementCount == elmenCount);
		RY_CORE_ASSERT(0u != indexElemmentCount);
#else
		glEnableVertexArrayAttrib(m_RendererID, indexElemment);
		glVertexArrayAttribFormat(m_RendererID, indexElemment, elmenCount, glType, normalize, realtivOffset);
		glVertexArrayAttribBinding(m_RendererID, indexElemment, bindIndex);
		indexElemment++;
#endif

		return unUsedComponetCount;


	}

	uint32_t OpenGLVertexArray::AddIntBufferEllementToVAO(uint32_t& indexElemment, uint32_t bindIndex, const BufferElement& element, uint32_t ellementReserverdComponts)
	{
		constexpr GLuint bytesPerCompent = sizeof(int);
		constexpr GLint maxCompontsCount = 4;
		const GLenum glType = Utils::ShaderDaterTyToOpenGLBaseType(element.type);
		const GLint elmenCount = element.GetCompontsCount() + ((maxCompontsCount - ellementReserverdComponts) % maxCompontsCount);
		const GLboolean normalize = element.normilized ? GL_TRUE : GL_FALSE;
		const GLuint ellementReserverdCompontsBytes = ((maxCompontsCount - ellementReserverdComponts) % maxCompontsCount ) * bytesPerCompent;
		const GLuint realtivOffset = element.offset - ellementReserverdCompontsBytes;
		const GLuint expextedOffsetPos = element.size + element.offset;

		GLint usedElementCount = 0;
		GLint notSetElementCount = elmenCount;

		GLuint curentRealtivElementOffset = realtivOffset;
		uint32_t indexElemmentCount = 0u;
		GLint unUsedComponetCount = 0;
#if 1
		while (0 < notSetElementCount)
		{
			GLint setElementCount = maxCompontsCount < notSetElementCount ? maxCompontsCount : notSetElementCount;
			GLuint ofsetBytesCount = setElementCount * bytesPerCompent;
			glEnableVertexArrayAttrib(m_RendererID, indexElemment);
			glVertexArrayAttribIFormat(m_RendererID, indexElemment, setElementCount, glType, curentRealtivElementOffset);
			glVertexArrayAttribBinding(m_RendererID, indexElemment, bindIndex);

			notSetElementCount -= setElementCount;
			usedElementCount += setElementCount;
			curentRealtivElementOffset += ofsetBytesCount;
			indexElemmentCount++;

			GLint elmentCount = notSetElementCount + usedElementCount;
			unUsedComponetCount = maxCompontsCount - setElementCount;

			RY_CORE_ASSERT(elmentCount == elmenCount);
			indexElemment++;

		}
		RY_CORE_ASSERT(curentRealtivElementOffset == expextedOffsetPos);
		RY_CORE_ASSERT(0 == notSetElementCount);
		RY_CORE_ASSERT(usedElementCount == elmenCount);
		RY_CORE_ASSERT(0u != indexElemmentCount);
#else
		glEnableVertexArrayAttrib(m_RendererID, indexElemment);
		glVertexArrayAttribIFormat(m_RendererID, indexElemment, elmenCount, glType, realtivOffset);
		glVertexArrayAttribBinding(m_RendererID, indexElemment, bindIndex);
		indexElemment++;
#endif
		return unUsedComponetCount;
	}

	uint32_t OpenGLVertexArray::AddUintBufferEllementToVAO(uint32_t& indexElemment, uint32_t bindIndex, const BufferElement& element, uint32_t ellementReserverdComponts)
	{
		constexpr GLuint bytesPerCompent = sizeof(uint32_t);
		constexpr GLint maxCompontsCount = 4;
		const GLenum glType = Utils::ShaderDaterTyToOpenGLBaseType(element.type);
		const GLint elmenCount = element.GetCompontsCount();
		const GLboolean normalize = element.normilized ? GL_TRUE : GL_FALSE;
		const GLuint ellementReserverdCompontsBytes = ellementReserverdComponts * bytesPerCompent;
		const GLuint realtivOffset = element.offset - ellementReserverdCompontsBytes;
		const GLuint expextedOffsetPos = element.size + element.offset;

		GLint usedElementCount = 0;
		GLint notSetElementCount = elmenCount;

		GLuint curentRealtivElementOffset = realtivOffset;
		uint32_t indexElemmentCount = 0u;
		GLint unUsedComponetCount = 0;
#if 1

		while (0 < notSetElementCount)
		{
			GLint setElementCount = maxCompontsCount < notSetElementCount ? maxCompontsCount : notSetElementCount;
			GLuint ofsetBytesCount = setElementCount * bytesPerCompent;
			

			glEnableVertexArrayAttrib(m_RendererID, indexElemment);
			glVertexArrayAttribLFormat(m_RendererID, indexElemment, setElementCount, glType, curentRealtivElementOffset);
			glVertexArrayAttribBinding(m_RendererID, indexElemment, bindIndex);

			notSetElementCount -= setElementCount;
			usedElementCount += setElementCount;
			curentRealtivElementOffset += ofsetBytesCount;
			indexElemmentCount++;

			GLint elmentCount = notSetElementCount + usedElementCount;
			unUsedComponetCount = maxCompontsCount - setElementCount;

			RY_CORE_ASSERT(elmentCount == elmenCount);
			indexElemment++;
		}
		curentRealtivElementOffset += ellementReserverdCompontsBytes;

		RY_CORE_ASSERT(curentRealtivElementOffset == expextedOffsetPos);
		RY_CORE_ASSERT(0 == notSetElementCount);
		RY_CORE_ASSERT(usedElementCount == elmenCount);
		RY_CORE_ASSERT(0u != indexElemmentCount);
#else
		glEnableVertexArrayAttrib(m_RendererID, indexElemment);
		glVertexArrayAttribLFormat(m_RendererID, indexElemment, elmenCount, glType, realtivOffset);
		glVertexArrayAttribBinding(m_RendererID, indexElemment, bindIndex);
		indexElemment++;
#endif

		return unUsedComponetCount;


	}


	bool OpenGLVertexArray::IsLayouteEqual(const BufferLayout& layout, const Ref<OpenGLVertexBuffer>& vertexBufferOpenGL)
	{
		const BufferLayout& layoutVertexBuffer = vertexBufferOpenGL->GetLayout();
		bool result = layoutVertexBuffer == layout;
		return result;
	}

	bool OpenGLVertexArray::SetupVertexBuffersToVAO(uint32_t bindIndex, const BufferLayout& layout, const Ref<OpenGLVertexBuffer>& vertexBufferOpenGL)
	{
		uint32_t vertexBufferRenderID = vertexBufferOpenGL->GetRenderID();

		if (vertexBufferRenderID == 0)
		{
			RY_CORE_ASSERT(false, "not vaild RenderID");
			return false;
		}
		uint32_t strideSize = layout.GetStride();
		uint32_t offestPtr = 0;
		glVertexArrayVertexBuffer(m_RendererID, bindIndex, vertexBufferRenderID, offestPtr, strideSize);
		return true;
	}

	void OpenGLVertexArray::SetupVertexBuffersToLayoutVAO(uint32_t bindIndex, uint32_t& offsetEllment, const BufferLayout& layout, const Ref<OpenGLVertexBuffer>& vertexBufferOpenGL)
	{
		

		uint32_t layoutIndex = 0u;
#ifdef RY_CHNAGE_OF_ORGNISE_LAYOUTE_INSTANC_INCREASE
		uint32_t divisor = 0u;
#else
		uint32_t divisor = layout.GetInstastancIncreas();
#endif
		uint32_t unUsedElementsPalces = 0u;
		ShaderDataType lastShaderDataType = ShaderDataType::None;
		bool compress = layout.AutoCompress();
		for (const BufferElement& elements : layout)
		{
			if (!elements.active)
			{
				offsetEllment++;
				continue;
			}

			if (compress && 0u != unUsedElementsPalces 
				&& Utils::ShaderDaterTypeSameType(elements.type, lastShaderDataType)
				&& elements.GetCompontsCount() <= unUsedElementsPalces)
			{
				offsetEllment--;
			}
			else
			{
				unUsedElementsPalces = 0;
			}

			switch (elements.type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Float3x3:
			case ShaderDataType::Float4x4:
				unUsedElementsPalces = AddFloatBufferEllementToVAO(offsetEllment, bindIndex, elements, unUsedElementsPalces);
				break;
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Int3x3:
			case ShaderDataType::Int4x4:
				unUsedElementsPalces = AddIntBufferEllementToVAO(offsetEllment, bindIndex, elements, unUsedElementsPalces);
				break;
			case ShaderDataType::Uint:
			case ShaderDataType::Uint2:
			case ShaderDataType::Uint3:
			case ShaderDataType::Uint4:
			case ShaderDataType::Uint3x3:
			case ShaderDataType::Uint4x4:
				unUsedElementsPalces = AddUintBufferEllementToVAO(offsetEllment, bindIndex, elements, unUsedElementsPalces);
				break;
			default:
				RY_CORE_ASSERT(false);
				break;
			}
			lastShaderDataType = elements.type;

#ifdef RY_CHNAGE_OF_ORGNISE_LAYOUTE_INSTANC_INCREASE
			divisor = elements.instanceIncreas;
#endif
		}
		glVertexArrayBindingDivisor(m_RendererID, bindIndex, divisor);
	}
}
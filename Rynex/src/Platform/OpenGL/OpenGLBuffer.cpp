#include "rypch.h"
#include "OpenGLBuffer.h"

#include "Rynex/Core/Application.h"

#include <glad/glad.h>

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
		
		static GLint GetMaxUniforms()
		{
			GLint maxUniformBlockSize = 0;
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);
			return maxUniformBlockSize;
		}

	}

#pragma region VertexBuffer

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
		// glCreateBuffers(1, &m_RendererID);
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, size, vertices, Utils::GetBufferDataUsage(m_Usage));

	}



	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size, BufferDataUsage usage, const BufferLayout& layout)
		: m_Usage(usage)
		, m_Size(size)
		, m_Target(GL_ARRAY_BUFFER)
		, m_Layout(layout)
	{
		RY_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, size, vertices, Utils::GetBufferDataUsage(m_Usage));

	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(std::vector<unsigned char>&& data, uint32_t size, BufferDataUsage usage, const BufferLayout& layout)
		: m_Usage(usage)
		, m_Size(size)
		, m_Target(GL_ARRAY_BUFFER)
		, m_Layout(layout)
		, m_Data(std::move(data))
		
	{
		
		RY_CORE_ASSERT(m_Data.size() == m_Size);
		Application::Get().SubmiteToMainThreedQueue([this]() {
			InitAsync();
		});
	}

	

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices, uint32_t size)
		: m_Size(size)
		, m_Target(GL_ARRAY_BUFFER)
		, m_Usage(BufferDataUsage::None)
	{
		RY_PROFILE_FUNCTION();
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, size, vertices, Utils::GetBufferDataUsage(m_Usage));
	}

	
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
		FreeBufferData();
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(m_Target, m_RendererID);

	}

	void OpenGLVertexBuffer::UnBind() const
	{
		glBindBuffer(m_Target, 0);
	}

	void OpenGLVertexBuffer::InitAsync()
	{
		RY_CORE_ASSERT(m_Size != 0 && m_Data.size() != 0);
		if(m_RendererID)
			glDeleteBuffers(1, &m_RendererID);
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, m_Data.size(), m_Data.data(), Utils::GetBufferDataUsage(m_Usage));
		
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t byteSize)
	{
		if(m_RendererID)

		m_Size = byteSize;
		RY_PROFILE_FUNCTION();
		glBindBuffer(m_Target, m_RendererID);
		glBufferSubData(m_Target, 0, byteSize, data);
	}

	const std::vector<unsigned char>& OpenGLVertexBuffer::GetBufferData()
	{
		FreeBufferData();
		Bind();
		m_Data.resize(m_Size);
		glGetBufferSubData(m_Target, 0, m_Data.size(), m_Data.data());

		return m_Data;
	}

	void OpenGLVertexBuffer::FreeBufferData()
	{
		if (m_Data.size())
		{
			m_Data.clear();
		}
	}




#pragma endregion

	
#pragma region IndexBuffer




	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices, uint32_t count, BufferDataUsage usage)
		: m_Count(count)
		, m_Usage(usage)
		, m_Target(GL_ELEMENT_ARRAY_BUFFER)
		, m_EllementByte(sizeof(uint32_t))
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, m_Count * m_EllementByte, indices, Utils::GetBufferDataUsage(m_Usage));
		RY_CORE_ASSERT(m_RendererID != 0);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint16_t* indices, uint32_t count, BufferDataUsage usage)
		: m_Count(count)
		, m_Usage(usage)
		, m_Target(GL_ELEMENT_ARRAY_BUFFER)
		, m_EllementByte(sizeof(uint16_t))
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, m_Count * m_EllementByte, indices, Utils::GetBufferDataUsage(m_Usage));
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<uint32_t>&& data, uint32_t count, BufferDataUsage usage)
		: m_Count(count)
		, m_Usage(usage)
		, m_Target(GL_ELEMENT_ARRAY_BUFFER)
		, m_EllementByte(sizeof(uint32_t))
		, m_Data(std::move(data))
	{
		RY_CORE_ASSERT(m_Count != 0 && m_EllementByte != 0 && m_Data.size() != 0);

		Application::Get().SubmiteToMainThreedQueue([this]() {
			InitAsync();
		});
	}

	

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::InitAsync()
	{
		RY_CORE_ASSERT(m_Count != 0 && m_EllementByte != 0 && m_Data.size() != 0);
		if (m_RendererID)
			glDeleteBuffers(1, &m_RendererID);

		uint32_t size = m_Count * m_EllementByte;
		
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, size, m_Data.data(), Utils::GetBufferDataUsage(m_Usage));

		RY_CORE_ASSERT(m_RendererID != 0);
	}

	void OpenGLIndexBuffer::SetData(const uint32_t* indices, uint32_t count)
	{ 
		RY_PROFILE_FUNCTION();
		m_EllementByte = sizeof(uint32_t);
		
		m_Count = count;
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, m_Count * m_EllementByte, indices, Utils::GetBufferDataUsage(m_Usage));
	}

	void OpenGLIndexBuffer::SetData(const uint16_t* indices, uint32_t count)
	{
		m_EllementByte = sizeof(uint16_t);
		m_Count = count;
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(m_Target, m_RendererID);
		glBufferData(m_Target, m_Count * m_EllementByte, indices, Utils::GetBufferDataUsage(m_Usage));
	}

	const std::vector<uint32_t> OpenGLIndexBuffer::GetBufferData()
	{

		FreeBufferData();
		m_Data.resize(m_Count * m_EllementByte);
		Bind();
		glGetBufferSubData(m_Target, 0, m_Data.size(), m_Data.data());

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

		glBindBuffer(m_Target, m_RendererID);
	}

	void OpenGLIndexBuffer::UnBind() const
	{
		glBindBuffer(m_Target, 0);
	}

#pragma endregion


#pragma region StorageBuffer

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

#pragma endregion


#pragma region UniformBuffer


	OpenGLUniformBuffer::OpenGLUniformBuffer(std::vector<unsigned char>&& data, const BufferLayout& layout, BufferDataUsage usage, uint32_t binding)
		: m_Data(std::move(data))
		, m_Binding(binding)
		, m_Layout(layout)
		, m_Target(GL_UNIFORM_BUFFER)
		, m_Usage(usage)
		, m_RendererID(0)
		, m_Size(data.size())
	{
		
		RY_CORE_ASSERT(m_Size % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
		Application::Get().SubmiteToMainThreedQueue([this]() {
			InitAsync();
		});
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t byteSize, uint32_t binding)
		: m_Binding(binding)
		, m_Size(byteSize)
		, m_Target(GL_UNIFORM_BUFFER)
		, m_Data(m_Size)
		, m_Usage(BufferDataUsage::DynamicDraw)
	{
		RY_CORE_ASSERT(m_Size < Utils::GetMaxUniforms(), "to large!");
		RY_CORE_ASSERT(m_Size % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
		CheckLayout();
		
		glCreateBuffers(1, &m_RendererID);
		
		
		
		glNamedBufferData(m_RendererID, m_Size, nullptr, Utils::GetBufferDataUsage(m_Usage));
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize, uint32_t binding)
		: m_Binding(binding)
		, m_Size(byteSize)
		, m_Data(byteSize)
		, m_Target(GL_UNIFORM_BUFFER)
		, m_Usage(BufferDataUsage::DynamicDraw)
	{
		RY_CORE_ASSERT(m_Size < Utils::GetMaxUniforms(), "to large!");
		RY_CORE_ASSERT(m_Size % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
		
		CheckLayout();
		
		std::memcpy(m_Data.data(), data, byteSize);
		glCreateBuffers(1, &m_RendererID);
		
		glNamedBufferData(m_RendererID, m_Size, m_Data.data(), Utils::GetBufferDataUsage(m_Usage));
		
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout, uint32_t binding)
		: m_Binding(binding)
		, m_Size(byteSize)
		, m_Data(byteSize)
		, m_Target(GL_UNIFORM_BUFFER)
		, m_Usage(BufferDataUsage::DynamicDraw)
		, m_Layout(layout)
	{
		RY_CORE_ASSERT(m_Size < Utils::GetMaxUniforms(), "to large!");
		RY_CORE_ASSERT(m_Size % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
		
		CheckLayout();
		std::memcpy(m_Data.data(), data, byteSize);
		glCreateBuffers(1, &m_RendererID);

		glNamedBufferData(m_RendererID, m_Size, m_Data.data(), Utils::GetBufferDataUsage(m_Usage));
	}

	OpenGLUniformBuffer::OpenGLUniformBuffer(const void* data, uint32_t byteSize, const BufferLayout& layout, BufferDataUsage usage, uint32_t binding)
		: m_Binding(binding)
		, m_Size(byteSize)
		, m_Layout(layout)
		, m_Target(GL_UNIFORM_BUFFER)
		, m_Usage(usage)
	{
		RY_CORE_ASSERT(m_Size % 16 == 0, "OpenGL UniformBuffer need to be 16 bytes");
		CheckLayout();
		std::memcpy(m_Data.data(), data, byteSize);
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, m_Size, m_Data.data(), Utils::GetBufferDataUsage(m_Usage));
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		if(m_OnDelete)
			m_OnDelete();

		glDeleteBuffers(1, &m_RendererID);
		m_Data.clear();
	}	
	
	void OpenGLUniformBuffer::SetOnDelete(std::function<void()> onDelete)
	{
		m_OnDelete = onDelete;
	}

	void OpenGLUniformBuffer::InitAsync()
	{
		RY_CORE_ASSERT(m_Size < Utils::GetMaxUniforms());
		
		CheckLayout();
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, m_Size, m_Data.data(), Utils::GetBufferDataUsage(m_Usage));
	}

	void OpenGLUniformBuffer::Bind(uint32_t slot)
	{

#if 1
		if (m_ChangeSize != 0)
		{
			
			glNamedBufferSubData(m_RendererID, m_ChangeOffset, m_ChangeSize, m_Data.data());
			m_ChangeSize = 0;
			m_ChangeOffset = 0;
		}
#endif
		glBindBufferBase(m_Target, slot != 15 ? slot : m_Binding , m_RendererID);
		
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t byteSize)
	{
		RY_CORE_ASSERT(m_Size == byteSize);
		std::memcpy(m_Data.data(), data, m_Size);
		glNamedBufferData(m_RendererID, byteSize, data, Utils::GetBufferDataUsage(m_Usage));
		m_ChangeOffset = 0;
		m_ChangeSize = 0;
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t offset, uint32_t byteSize)
	{
		if ((offset + byteSize) > m_Size)
		{
			RY_CORE_ASSERT(false);
			
		}
		else
		{
			std::memcpy(m_Data.data() + offset, data, byteSize);
			glNamedBufferSubData(m_RendererID, offset, byteSize, m_Data.data());
		}
	}

#if 1
	void OpenGLUniformBuffer::SetLocelData(const BufferElement& ellement, const void* data, uint32_t byteSize)
	{
		
		for(const BufferElement& elem : m_Layout)
		{
			if (elem == ellement)
			{
				if (elem.Size == byteSize)
				{
					std::memcpy(m_Data.data() + elem.Offset, data, elem.Size);
					uint32_t begin, end, endNew;
					begin = m_ChangeOffset;
					end = m_ChangeSize + begin;
					endNew = elem.Offset + elem.Size;
				
					m_ChangeOffset = begin < elem.Offset ? begin : elem.Offset;
					m_ChangeSize = end > endNew ? end - m_ChangeOffset : endNew - m_ChangeOffset;
					
					
				}
				else
					RY_CORE_ASSERT(false);
			}
		}
	}
#endif
	

	const std::vector<unsigned char>& OpenGLUniformBuffer::GetBufferData()
	{
		
		return m_Data;
	}

	void OpenGLUniformBuffer::FreeBufferData()
	{
		m_Data.clear();
	}


	

	void OpenGLUniformBuffer::CheckLayout()
	{
		uint32_t size = 0ul;
		for (const BufferElement& element : m_Layout)
		{
			size += element.Size;
			uint8_t size16 = size % 16;
      		RY_CORE_ASSERT(!(element.Size > 16 && size16 != 0));

		}
	}

#pragma endregion
}
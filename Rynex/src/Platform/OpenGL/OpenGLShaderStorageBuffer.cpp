#include "rypch.h"
#include "OpenGLShaderStorageBuffer.h"
#include <Platform/OpenGL/OpenGLBindlesTextureArray.h>
#include <Platform/OpenGL/OpenGLBase.h>

namespace Rynex {
#pragma region OpenGLShaderStorageBuffer

	OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(uint32_t target, std::nullptr_t, uint32_t byteSize, uint32_t flags)
		: m_RendererID(0u)
		, m_Flags(flags)
		, m_ByteSize(byteSize)
		, m_FanceObject()
	{
#ifdef RY_SET_RESIZE_MEMORY_FUNC
		ResizeMemoryData(byteSize);
#else
		CopyMemoryData(nullptr, byteSize);
#endif
		InvalideData(target);
	}

	OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(uint32_t target, const uint8_t* dataPtr, uint32_t byteSize, uint32_t flags)
		: m_RendererID(0u)
		, m_Flags(flags)
		, m_ByteSize(byteSize)
		, m_FanceObject()
	{
		CopyMemoryData(dataPtr, byteSize);
		InvalideData(target);
	}

	OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(uint32_t target, std::vector<uint8_t>&& data, uint32_t flags)
		: m_RendererID(0u)
		, m_Flags(flags)
		, m_Data(std::move(data))
		, m_FanceObject()
	{
		m_ByteSize = m_Data.size();
		RY_CORE_ASSERT(0u != m_ByteSize, "Size not Set!");
		InvalideData(target);
	}

	OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(uint32_t target, const OpenGLShaderStorageBuffer& buffer)
		: m_RendererID(0u)
		, m_Flags(buffer.m_Flags)
		, m_Data(buffer.m_Data)
		, m_ByteSize(buffer.m_ByteSize)
		, m_FanceObject()
	{
		RY_CORE_ASSERT(0u != m_ByteSize, "Size not Set!");
		InvalideData(target);
	}


	OpenGLShaderStorageBuffer::~OpenGLShaderStorageBuffer()
	{
		DestroyID();
	}

	void OpenGLShaderStorageBuffer::Bind(uint32_t target) const
	{
		RY_CORE_ASSERT(0u != target);
		RY_CORE_ASSERT(0u != m_RendererID);
#if RY_OPENGL_BIND_RENDER_COMAND
		OpenGLRenderCommand::BindBuffer(target, m_RendererID);
#else
		glBindBuffer(target, m_RendererID);
#endif
	}

	void OpenGLShaderStorageBuffer::UnBind(uint32_t target) const
	{
		GL_CHECK_LOOP();

		RY_CORE_ASSERT(0u != target);
		RY_CORE_ASSERT(0u != m_RendererID);
#if RY_OPENGL_BIND_RENDER_COMAND
		OpenGLRenderCommand::BindBuffer(target, 0u);
#else
		glBindBuffer(target, 0);
#endif
		GL_CHECK_LOOP();

	}

	void OpenGLShaderStorageBuffer::BindSlot(uint32_t target, uint32_t slot) const
	{
		GL_CHECK_LOOP();

		RY_CORE_ASSERT(0u != target);
		RY_CORE_ASSERT(0u != m_RendererID);

#if RY_OPENGL_BIND_RENDER_COMAND
		OpenGLRenderCommand::BindBufferSlot(target, slot, m_RendererID);
#else
		glBindBufferBase(target, slot, m_RendererID);
#endif
		GL_CHECK_LOOP();
	}

	void OpenGLShaderStorageBuffer::UnBindSlot(uint32_t target, uint32_t slot) const
	{
		GL_CHECK_LOOP();

		RY_CORE_ASSERT(0u != target);
		RY_CORE_ASSERT(0u != m_RendererID);

#if RY_OPENGL_BIND_RENDER_COMAND
		OpenGLRenderCommand::BindBufferSlot(target, slot, 0u);
#else
		glBindBufferBase(target, slot, 0);
#endif

		GL_CHECK_LOOP();
	}

	void OpenGLShaderStorageBuffer::SetData(uint32_t target, const uint8_t* dataPtr, uint32_t offset, uint32_t byteSize)
	{
		RY_CORE_ASSERT(0u != target);
		RY_CORE_ASSERT(0u != m_RendererID);
		RY_CORE_ASSERT(0u != byteSize, "It snot allwowd to set 0 bytes data!")
		CopyOffsetMemoryData(dataPtr, offset, byteSize);
		GL_CHECK();
		const uint8_t* dataOffsetPtr = m_Data.data() + offset;

		glNamedBufferSubData(m_RendererID, offset, byteSize, dataOffsetPtr);

		GL_CHECK_LOOP();
#ifdef RY_USE_GRAFIC_API_FANCE
		m_FanceObject.SetupFence();
#endif
	}

	void OpenGLShaderStorageBuffer::ResizeData(uint32_t target, uint32_t byteSize)
	{
		

#ifdef RY_SET_RESIZE_MEMORY_FUNC
		ResizeMemoryData(byteSize);
#endif
		InvalideData(target);
	}

	void OpenGLShaderStorageBuffer::ResizeData(uint32_t target, const uint8_t* dataPtr, uint32_t byteSize, uint32_t flags)
	{
		m_Flags = flags;

		CopyMemoryData(dataPtr, byteSize);
		InvalideData(target);


		GL_CHECK();
	}

	bool OpenGLShaderStorageBuffer::IsTransfered()
	{
		return m_FanceObject.IsTransfered();
	}

	void OpenGLShaderStorageBuffer::SetupFance()
	{
#ifdef RY_USE_GRAFIC_API_FANCE
		m_FanceObject.SetupFence();
#else
		if (Asset::CurrentOnMainThread())
			return;

		m_FanceObject.SetupFence();
		uint32_t byteSize = m_Data.size();
		int64_t pastTime = m_FanceObject.StoppThreadUntlieLoded();
		RY_CORE_INFO("Data OpenGLShaderStorageBuffer Transfered! off {} bytesComplet. Time waiting {} Nanosec", byteSize, pastTime);
		m_FanceObject.DestroyID();
		
#endif
	}

	void OpenGLShaderStorageBuffer::CreateID(uint32_t target)
	{
		DestroyID();

		RY_CORE_ASSERT(0u == m_RendererID, "Render-ID Not Removed!");
		RY_CORE_ASSERT(0u != target);

		RY_OPENGL_BUFFER_ID_SCOPE_LOCK();
		glCreateBuffers(1u, &m_RendererID);

		GL_CHECK();

		RY_CORE_ASSERT(0u != m_RendererID, "Render-ID Not Created!");

	}


	void OpenGLShaderStorageBuffer::DestroyID()
	{
		if (m_RendererID != 0)
		{
			RY_OPENGL_BUFFER_ID_SCOPE_LOCK();

			glDeleteBuffers(1, &m_RendererID);
			m_RendererID = 0;
		}
		GL_CHECK();

		RY_CORE_ASSERT(0u == m_RendererID, "Not Removed!");

	}

	void OpenGLShaderStorageBuffer::InvalideData(uint32_t target)
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
#if RY_GRAFIC_SUBMIT_TO_MAIN_THREAD_WITHE_OUT_WAIT
			Application::Get().SubmiteToMainThreedQueue(std::bind(&OpenGLShaderStorageBuffer::InvalideData, this, target));
#else
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLShaderStorageBuffer::InvalideData, this, target));
#endif
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME(OpenGLShaderStorageBuffer::InvalideData, target);
#endif
		RY_CORE_ASSERT(0u != m_ByteSize);

		RY_CORE_ASSERT(m_ByteSize == m_Data.size());

		CreateID(target);

		const uint8_t* dataPtr = m_Data.data();
		uint32_t byteSize = m_Data.size();
		
		if(m_Data.empty())
		{
			RY_CORE_FATAL("To saml Byte Size Alloc buffer withe 1 byte and data as nullptr!");
			byteSize = 1u;
			m_ByteSize = 1u;
			dataPtr = nullptr;
		}
		RY_CORE_ASSERT(0u != m_RendererID);

		glNamedBufferStorage(m_RendererID, byteSize, dataPtr, m_Flags);
		
		GL_CHECK();

		SetupFance();

	}



	void OpenGLShaderStorageBuffer::AddCopyMemoryData(const uint8_t* data, uint32_t byteSize)
	{
		const uint32_t offset = m_ByteSize;
		const uint32_t newByteSize = offset + byteSize;

		m_ByteSize += byteSize;
		m_Data.resize(newByteSize);
		CopyOffsetMemoryData(data, offset, byteSize);
	}

	void OpenGLShaderStorageBuffer::CopyMemoryData(const uint8_t* data, uint32_t byteSize)
	{
		if (m_Data.size() < byteSize)
		{
			m_Data.clear();
			m_Data.resize(byteSize);
			m_ByteSize = byteSize;
		}

		CopyOffsetMemoryData(data, 0u, byteSize);
	}

	void OpenGLShaderStorageBuffer::CopyOffsetMemoryData(const uint8_t* dataPtr, uint32_t offset, uint32_t size)
	{
		uint32_t expextSize = offset + size;
		RY_CORE_ASSERT(expextSize <= m_ByteSize);

		if (nullptr == dataPtr)
			return;

		uint8_t* offsetDataDstPtr = m_Data.data() + offset;
		std::memcpy(offsetDataDstPtr, dataPtr, size);
	}
#ifdef RY_SET_RESIZE_MEMORY_FUNC
	void OpenGLShaderStorageBuffer::ResizeMemoryData(uint32_t byteSize)
	{
		m_Data.clear();
		m_ByteSize = byteSize;
		m_Data.resize(byteSize);

	}
#endif

	void OpenGLShaderStorageBuffer::LoadeGPUDataOnCPU()
	{
		RY_CORE_ASSERT(0u == m_RendererID, "Render-ID not vaild!");

		uint32_t byteSize = m_Data.size();
		uint8_t* dataPtr = m_Data.data();

		glGetNamedBufferSubData(m_RendererID, 0u, byteSize, dataPtr);
		GL_CHECK();
		RY_CORE_WARN("Get GPU Data on CPU is an perforfonmenz intensive action!");
	}

	void OpenGLShaderStorageBuffer::ClearLocaleDataStore()
	{
		m_Data.clear();
		m_Data.shrink_to_fit();
	}

	

#pragma endregion

}
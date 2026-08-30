#include "rypch.h"
#include "OpenGLArrayBuffer.h"

#include <Platform/OpenGL/OpenGLBase.h>


namespace Rynex {

	OpenGLArrayBuffer::OpenGLArrayBuffer(uint32_t target, const uint8_t* data, uint32_t byteSize, uint32_t usage)
		: m_RendererID(0u)
		, m_Usage(usage)
		, m_ByteSize(byteSize)
		, m_FanceObject()
	{
		CopyMemoryData(data, byteSize);
		InvalideData(target);
	}

	OpenGLArrayBuffer::OpenGLArrayBuffer(uint32_t target, std::nullptr_t, uint32_t byteSize, uint32_t usage)
		: m_RendererID(0u)
		, m_Usage(usage)
		, m_ByteSize(byteSize)
		, m_FanceObject()
	{
		CopyMemoryData(nullptr, byteSize);
		InvalideData(target);
	}

	OpenGLArrayBuffer::OpenGLArrayBuffer(uint32_t target, std::vector<uint8_t>&& data, uint32_t usage)
		: m_RendererID(0u)
		, m_Usage(usage)
		, m_Data(std::move(data))
		, m_FanceObject()
	{
		m_ByteSize = m_Data.size();
		RY_CORE_ASSERT(0u != m_ByteSize, "Size not Set!");
		InvalideData(target);
	};

	OpenGLArrayBuffer::OpenGLArrayBuffer(uint32_t target, const OpenGLArrayBuffer& buffer)
		: m_RendererID(0u)
		, m_Usage(buffer.m_Usage)
		, m_Data(buffer.m_Data)
		, m_ByteSize(buffer.m_ByteSize)
		, m_FanceObject()
	{
		RY_CORE_ASSERT(0u != m_ByteSize, "Size not Set!");
		InvalideData(target);
	}

	OpenGLArrayBuffer::~OpenGLArrayBuffer()
	{
		DestroyID();
	}

	void OpenGLArrayBuffer::Bind(uint32_t target) const
	{
		RY_CORE_ASSERT(0u != target);
		RY_CORE_ASSERT(0u != m_RendererID);

		OpenGLRenderCommand::BindBuffer(target, m_RendererID);

		GL_CHECK_LOOP();
	}

	void OpenGLArrayBuffer::UnBind(uint32_t target) const
	{
		RY_CORE_ASSERT(0u != target);
		RY_CORE_ASSERT(0u != m_RendererID);

		OpenGLRenderCommand::BindBuffer(target, 0u);

		GL_CHECK_LOOP();
	}

	void OpenGLArrayBuffer::BindSlot(uint32_t target, uint32_t slot) const
	{
		RY_CORE_ASSERT(0u != target);
		RY_CORE_ASSERT(0u != m_RendererID);

		OpenGLRenderCommand::BindBufferSlot(target, slot, m_RendererID);

		GL_CHECK_LOOP();
	}

	void OpenGLArrayBuffer::UnBindSlot(uint32_t target, uint32_t slot) const
	{
		RY_CORE_ASSERT(0u != target);
		RY_CORE_ASSERT(0u != m_RendererID);

		OpenGLRenderCommand::BindBufferSlot(target, slot, 0u);

		GL_CHECK_LOOP();
	}

	void OpenGLArrayBuffer::SetData(uint32_t target, const uint8_t* data, uint32_t size)
	{
		RY_CORE_ASSERT(size <= m_ByteSize, "Overflow!");
		SetData(target, data, 0u, size);
	}

	void OpenGLArrayBuffer::SetData(uint32_t target, const uint8_t* dataPtr, uint32_t offset, uint32_t byteSize)
	{

		RY_CORE_ASSERT(0u != target);
		RY_CORE_ASSERT(0u != m_RendererID);
		RY_CORE_ASSERT(0u != byteSize, "It snot allwowd to set 0 bytes data!")
		CopyOffsetMemoryData(dataPtr, offset, byteSize);
		GL_CHECK();

		glNamedBufferSubData(m_RendererID, offset, byteSize, m_Data.data());
		GL_CHECK_LOOP();
#ifdef RY_USE_GRAFIC_API_FANCE
		m_FanceObject.SetupFence();
#endif
	}

	void OpenGLArrayBuffer::ResizeData(uint32_t target, const uint8_t* dataPtr, uint32_t size, uint32_t usage)
	{
		RY_CORE_ASSERT(0u != m_ByteSize, "Size not Set!");

		m_Usage = usage;
		CopyMemoryData(dataPtr, size);

		InvalideData(target);
	}

	void OpenGLArrayBuffer::InvalideData(uint32_t target)
	{
#ifdef RY_OPENGL_MAIN_THREADE
		if (!OpenGLThreadContext::IsActive())
		{
			Application::Get().SubmiteToMainThreedQueueWait(std::bind(&OpenGLArrayBuffer::InvalideData, this, target));
			return;
		}
#else
		RY_EXE_ON_MAIN_THREAD_RESUME_ARGS(OpenGLArrayBuffer::InvalideData, target);
#endif
		RY_CORE_ASSERT(0u != m_ByteSize);
		CreateID(target);
		

		RY_CORE_ASSERT(m_ByteSize == m_Data.size());

		const uint8_t* dataPtr = m_Data.data();
		uint32_t byteSize = m_Data.size();

		glNamedBufferData(m_RendererID, byteSize, dataPtr, m_Usage);

		GL_CHECK();
#ifdef RY_USE_GRAFIC_API_FANCE
		m_FanceObject.SetupFence();
#endif
		SetupFance();
	}

	void OpenGLArrayBuffer::AddCopyMemoryData(const uint8_t* dataPtr, uint32_t byteSize)
	{

		const uint32_t offset = m_ByteSize;
		const uint32_t newByteSize = offset + byteSize;

		m_ByteSize += byteSize;
		m_Data.resize(newByteSize);
		CopyOffsetMemoryData(dataPtr, offset, m_ByteSize);
	}

	void OpenGLArrayBuffer::CopyMemoryData(const uint8_t* dataPtr, uint32_t byteSize)
	{
		if(m_Data.size() < byteSize)
		{
			m_Data.clear();
			m_Data.resize(byteSize);
			m_ByteSize = byteSize;
		}
		CopyOffsetMemoryData(dataPtr, 0u, byteSize);
	}

	void OpenGLArrayBuffer::CopyOffsetMemoryData(const uint8_t* dataPtr, uint32_t offset, uint32_t byteSize)
	{
		uint32_t expextSize = offset + byteSize;
		RY_CORE_ASSERT(expextSize <= m_ByteSize);

		if (nullptr == dataPtr)
			return;

		uint8_t* offsetDataDstPtr = m_Data.data() + offset;
		std::memcpy(offsetDataDstPtr, dataPtr, byteSize);
	}

	void OpenGLArrayBuffer::CreateID(uint32_t target)
	{
		DestroyID();

		RY_CORE_ASSERT(0u == m_RendererID, "Render-ID Not Removed!");
		RY_CORE_ASSERT(0u != target);

		RY_OPENGL_BUFFER_ID_SCOPE_LOCK();
		glCreateBuffers(1u, &m_RendererID);

		GL_CHECK();
	}


	void OpenGLArrayBuffer::DestroyID()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());
		if (0u == m_RendererID)
			return;

		RY_OPENGL_BUFFER_ID_SCOPE_LOCK();

		glDeleteBuffers(1u, &m_RendererID);
		m_RendererID = 0u;

		RY_CORE_ASSERT(0u == m_RendererID, "Render-ID Not Removed!");

		GL_CHECK();
	}

	bool OpenGLArrayBuffer::IsTransfered()
	{
		return m_FanceObject.IsTransfered();
	}

	void OpenGLArrayBuffer::SetupFance()
	{
		if (Asset::CurrentOnMainThread())
			return;

		m_FanceObject.SetupFence();
		uint32_t byteSize = m_Data.size();
		int64_t pastTime = m_FanceObject.StoppThreadUntlieLoded();
		RY_CORE_INFO("Data OpenGLArrayBuffer Transfered! off {} bytesComplet. Time waiting {} Nanosec", byteSize, pastTime);
		m_FanceObject.DestroyID();
		ClearLocaleDataStore();
	}

	void OpenGLArrayBuffer::LoadeGPUDataOnCPU()
	{
		RY_CORE_ASSERT(0u == m_RendererID, "Render-ID not vaild!");

		uint32_t byteSize = m_Data.size();
		uint8_t* dataPtr = m_Data.data();

		glGetNamedBufferSubData(m_RendererID, 0u, byteSize, dataPtr);
		GL_CHECK();
		RY_CORE_WARN("Get GPU Data on CPU is an perforfonmenz intensive action!");
	}

	void OpenGLArrayBuffer::ClearLocaleDataStore()
	{
		m_Data.clear();
		m_Data.shrink_to_fit();
	}
}
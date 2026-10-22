#include <rypch.h>
#include "OpenGLContext.h"

#include <Platform/OpenGL/OpenGLBase.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


// Nivade extension Flags
#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B


namespace Rynex {


    OpenGLContext* OpenGLContext::s_Instance = nullptr;

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
        , m_BufferOperationIDLock()
        , m_TextureOperationIDLock()
        , m_ShaderOperationIDLock()
        , m_FramebufferOperationIDLock()
        , m_FancyOperationIDLock()
        , m_VertexArrayOperationIDLock()
		, m_MaxGPUMemory(0ull)
		, m_MaxVideoMemory(0ull)
		, m_CurrentAllocMemory(0ull)
		, m_DifferentBuffer(0ll)
        , m_MainContextThread(std::this_thread::get_id())
	    , m_ActiveMultyThreading(false)
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_ASSERT(windowHandle, "Window hale is null!");
	}

	OpenGLContext::~OpenGLContext()
	{
		RY_CORE_ASSERT(m_ActiveMultyThreading, "Multy threading is already activ!");
		{
			std::scoped_lock<std::mutex> lockBufferID(OpenGLContext::GetBufferOperationLock());
			std::scoped_lock<std::mutex> lockTextureID(OpenGLContext::GetTextureOperationLock());
			std::scoped_lock<std::mutex> lockShaderID(OpenGLContext::GetShaderOperationLock());
			std::scoped_lock<std::mutex> lockFramebufferID(OpenGLContext::GetFramebufferOperationLock());
			std::scoped_lock<std::mutex> lockFenceID(OpenGLContext::GetFancyOperationLock());
			std::scoped_lock<std::mutex> vertexArrayOperationID(OpenGLContext::GetVertexArrayOperationLock());
			m_ActiveMultyThreading = false;
		}

		{
			std::scoped_lock<std::mutex> lockBufferID = std::scoped_lock<std::mutex>(m_BufferOperationIDLock);
			std::scoped_lock<std::mutex> lockTextureID = std::scoped_lock<std::mutex>(m_TextureOperationIDLock);
			std::scoped_lock<std::mutex> lockShaderID = std::scoped_lock<std::mutex>(m_ShaderOperationIDLock);
			std::scoped_lock<std::mutex> lockFramebufferID = std::scoped_lock<std::mutex>(m_FramebufferOperationIDLock);
			std::scoped_lock<std::mutex> lockFenceID = std::scoped_lock<std::mutex>(m_FancyOperationIDLock);
			std::scoped_lock<std::mutex> vertexArrayOperationID = std::scoped_lock<std::mutex>(m_VertexArrayOperationIDLock);
		}


		m_WindowHandle = nullptr;

	    if(s_Instance == this)
	        s_Instance = nullptr;
	}

	void OpenGLContext::Init()
	{
	    if (nullptr == s_Instance)
	        s_Instance = this;

		RY_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_WindowHandle);
		GLADloadproc loadedGlfwProcFuncPtr = reinterpret_cast<GLADloadproc>(glfwGetProcAddress);
		const int status = gladLoadGLLoader(loadedGlfwProcFuncPtr);

		RY_CORE_ASSERT(status , "Failed to initialize Glad!");
		RY_CORE_ASSERT(!m_ActiveMultyThreading, "Multy threading is already activ!");
		m_ActiveMultyThreading = true;
	    m_MainContextThread = std::this_thread::get_id();

#if RY_ENABLE_GARFIC_API_DBUGE_MASSEGES
		GLint glContectFlags = 0;
		glGetIntegerv(GL_CONTEXT_FLAGS, &glContectFlags);
		if (GL_CONTEXT_FLAGS & GL_CONTEXT_FLAG_DEBUG_BIT)
			RY_CORE_INFO("Akitve OpenGL Debuge Context");
		else
			RY_CORE_ERROR("Not Akitve OpenGL Debuge Context");

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGL::DebugOutPut, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif
		

		const GLubyte* glVersion = glGetString(GL_VERSION);
		const std::string_view version(reinterpret_cast<const char*>(glVersion));
		const std::string versionStr(version.data(), version.size());
		if(versionStr.find("NVIDIA") < versionStr.length())
		{
			GLint totalMemoryKB = 0;
			glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemoryKB);
			if (totalMemoryKB <= 0)
				RY_CORE_WARN("We Can't track memory directly!!!");
			m_MaxGPUMemory = static_cast<uint64_t>(totalMemoryKB);

			GLint totalVRamKB = 0;
			glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &totalVRamKB);
			m_MaxVideoMemory = static_cast<uint64_t>(totalVRamKB);


			RY_CORE_INFO("OpenGL version: ({}) / GPU memory {} KB, VRam: {} KB", version.data(), totalMemoryKB, totalVRamKB);
		}
		else
		{
			RY_CORE_INFO("OpenGL version: ({})", version.data());
			m_MaxVideoMemory = 0ull;
			m_MaxGPUMemory = 0ull;
		}
	}

	OpenGLContext& OpenGLContext::Get()
    {
	    return *s_Instance;
    }

	void OpenGLContext::SwapBuffers()
	{
		RY_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);

	}

	std::mutex& OpenGLContext::GetBufferOperationLock()
	{
		RY_CORE_ASSERT(m_ActiveMultyThreading, "Multy threading is not activ!");
		return m_BufferOperationIDLock;
	}

	std::mutex& OpenGLContext::GetVertexArrayOperationLock()
	{
		RY_CORE_ASSERT(m_ActiveMultyThreading, "Multy threading is not active!");
		return m_VertexArrayOperationIDLock;
	}

	std::mutex& OpenGLContext::GetTextureOperationLock()
	{
		RY_CORE_ASSERT(m_ActiveMultyThreading, "Multy threading is not active!");
		return m_TextureOperationIDLock;
	}

	std::mutex& OpenGLContext::GetFramebufferOperationLock()
	{
		RY_CORE_ASSERT(m_ActiveMultyThreading, "Multy threading is not activ!");
		return m_FramebufferOperationIDLock;
	}

	std::mutex& OpenGLContext::GetShaderOperationLock()
	{
		RY_CORE_ASSERT(m_ActiveMultyThreading, "Multy threading is not activ!");
		return m_ShaderOperationIDLock;
	}

	std::mutex& OpenGLContext::GetFancyOperationLock()
	{
		RY_CORE_ASSERT(m_ActiveMultyThreading, "Multy threading is not activ!");
		return m_FancyOperationIDLock;
	}

    bool OpenGLContext::IsMainContextThread() const
    {
	    const std::thread::id threadId = std::this_thread::get_id();
	    return m_MainContextThread == threadId;
    }

    void OpenGLContext::AddCurrentAllocMemory(uint64_t add)
	{
	    m_CurrentAllocMemory += add;
	    m_DifferentBuffer++;

	    RY_CORE_WARN_IF(m_MaxGPUMemory <= FRACTURE_ONE(m_CurrentAllocMemory,4), "Alloc Memory is Circle Close to Max {} KB / {} KB", m_CurrentAllocMemory, m_MaxGPUMemory);

	}


    uint64_t OpenGLContext::GetMemoryUsage() const
	{
		if (0 != m_MaxGPUMemory)
		{
			GLint curAvailVRamMemoryKB = 0;
			glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &curAvailVRamMemoryKB);
			uint64_t memoryUsage = m_MaxVideoMemory - static_cast<uint64_t>(curAvailVRamMemoryKB);

		    const uint64_t memoryFraction = FRACTURE_ONE(m_MaxVideoMemory, 1 / 4);
		    RY_CORE_FATAL_IF(memoryFraction < memoryUsage, "Currently to much V-Ram Memory usage: {} KB available: {} KB max: {} KB", memoryUsage, curAvailVRamMemoryKB, m_MaxVideoMemory);
			return memoryUsage;
		}
		return 0ull;
	}


}

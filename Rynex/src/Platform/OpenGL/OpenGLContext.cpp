#include "rypch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <Platform/OpenGL/OpenGLBase.h>

// Nivade extension Flags
#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B


namespace Rynex {

	struct MultyThreadStorage
	{
		std::mutex bufferOperationIDLock;
		std::mutex textureOperationIDLock;
		std::mutex shaderOperationIDLock;
		std::mutex frambufferOperationIDLock;
		std::mutex fanceOperationIDLock;
		std::mutex vertexArrayOperationIDLock;
		
		bool activeMultyThreading = false;

	};
	static MultyThreadStorage s_ThreadStorage;

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
		, m_CurentAllocMemory(0ull)
		, m_MaxGPUMemory(0ull)
		, m_DifferentBuffer(0ll)
		, m_MaxVidoeMemory(0ull)
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_ASSERT(windowHandle, "Window hale is null!");
	}

	OpenGLContext::~OpenGLContext()
	{
		RY_CORE_ASSERT(s_ThreadStorage.activeMultyThreading, "Mutly threading is already aktiv!");
		{
			std::scoped_lock<std::mutex> lockBufferID = std::scoped_lock<std::mutex>(OpenGLContext::GetBufferOperationLock());
			std::scoped_lock<std::mutex> lockTextureID = std::scoped_lock<std::mutex>(OpenGLContext::GetTextureOperationLock());
			std::scoped_lock<std::mutex> lockShaderID = std::scoped_lock<std::mutex>(OpenGLContext::GetShaderOperationLock());
			std::scoped_lock<std::mutex> lockFrambufferID = std::scoped_lock<std::mutex>(OpenGLContext::GetFrambufferOperationLock());
			std::scoped_lock<std::mutex> lockFanceID = std::scoped_lock<std::mutex>(OpenGLContext::GetFanceOperationLock());
			std::scoped_lock<std::mutex> vertexArrayOperationID = std::scoped_lock<std::mutex>(OpenGLContext::GetVertexArrayOperationLock());
			s_ThreadStorage.activeMultyThreading = false;
		}

		{
			std::scoped_lock<std::mutex> lockBufferID = std::scoped_lock<std::mutex>(s_ThreadStorage.bufferOperationIDLock);
			std::scoped_lock<std::mutex> lockTextureID = std::scoped_lock<std::mutex>(s_ThreadStorage.textureOperationIDLock);
			std::scoped_lock<std::mutex> lockShaderID = std::scoped_lock<std::mutex>(s_ThreadStorage.shaderOperationIDLock);
			std::scoped_lock<std::mutex> lockFrambufferID = std::scoped_lock<std::mutex>(s_ThreadStorage.frambufferOperationIDLock);
			std::scoped_lock<std::mutex> lockFanceID = std::scoped_lock<std::mutex>(s_ThreadStorage.fanceOperationIDLock);
			std::scoped_lock<std::mutex> vertexArrayOperationID = std::scoped_lock<std::mutex>(s_ThreadStorage.vertexArrayOperationIDLock);
		}


		m_WindowHandle = nullptr;
	}

	void OpenGLContext::Init()
	{
		RY_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_WindowHandle);
		GLADloadproc loadeGLFW_ProcFuncPtr = reinterpret_cast<GLADloadproc>(glfwGetProcAddress);
		int status = gladLoadGLLoader(loadeGLFW_ProcFuncPtr);

		RY_CORE_ASSERT(status , "Faild to instalize Glad!");
		RY_CORE_ASSERT(!s_ThreadStorage.activeMultyThreading, "Mutly threading is already aktiv!");
		s_ThreadStorage.activeMultyThreading = true;
#if RY_ENABLE_GARFIC_API_DBUGE_MASSEGES
		GLint glContectFlags = 0;
		glGetIntegerv(GL_CONTEXT_FLAGS, &glContectFlags);
		if (GL_CONTEXT_FLAGS & GL_CONTEXT_FLAG_DEBUG_BIT)
			RY_CORE_INFO("Akitve OpenGL Debuge Context");
		else
			RY_CORE_ERROR("Not Akitve OpenGL Debuge Context");

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGL::DebugeOutPut, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif
		

		const GLubyte* glVersion = glGetString(GL_VERSION);
		std::string_view version(reinterpret_cast<const char*>(glVersion));
		std::string versionStr(version.data(), version.size());
		if(versionStr.find("NVIDIA") < versionStr.length())
		{
			GLint totalMemoryKB = 0;
			glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemoryKB);
			if (totalMemoryKB <= 0)
				RY_CORE_WARN("We Can't track memory directly!!!");
			m_MaxGPUMemory = static_cast<uint64_t>(totalMemoryKB);

			GLint totalVRamKB = 0;
			glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &totalVRamKB);
			m_MaxVidoeMemory = static_cast<uint64_t>(totalVRamKB);


			RY_CORE_INFO("OpenGL version: ({}) / GPU memory {} KB, VRam: {} KB", version.data(), totalMemoryKB, totalVRamKB);
		}
		else
		{
			RY_CORE_INFO("OpenGL version: ({})", version.data());
			m_MaxVidoeMemory = 0ull;
			m_MaxGPUMemory = 0ull;
		}
	}

	

	void OpenGLContext::SwapBuffers()
	{
		RY_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);

	}

	std::mutex& OpenGLContext::GetBufferOperationLock()
	{
		RY_CORE_ASSERT(s_ThreadStorage.activeMultyThreading, "Mutly threading is not aktiv!");
		return s_ThreadStorage.bufferOperationIDLock;
	}

	std::mutex& OpenGLContext::GetVertexArrayOperationLock()
	{
		RY_CORE_ASSERT(s_ThreadStorage.activeMultyThreading, "Mutly threading is not aktiv!");
		return s_ThreadStorage.vertexArrayOperationIDLock;
	}

	std::mutex& OpenGLContext::GetTextureOperationLock()
	{
		RY_CORE_ASSERT(s_ThreadStorage.activeMultyThreading, "Mutly threading is not aktiv!");
		return s_ThreadStorage.textureOperationIDLock;
	}

	std::mutex& OpenGLContext::GetFrambufferOperationLock()
	{
		RY_CORE_ASSERT(s_ThreadStorage.activeMultyThreading, "Mutly threading is not aktiv!");
		return s_ThreadStorage.frambufferOperationIDLock;
	}

	std::mutex& OpenGLContext::GetShaderOperationLock()
	{
		RY_CORE_ASSERT(s_ThreadStorage.activeMultyThreading, "Mutly threading is not aktiv!");
		return s_ThreadStorage.shaderOperationIDLock;
	}

	std::mutex& OpenGLContext::GetFanceOperationLock()
	{
		RY_CORE_ASSERT(s_ThreadStorage.activeMultyThreading, "Mutly threading is not aktiv!");
		return s_ThreadStorage.fanceOperationIDLock;
	}


	uint64_t OpenGLContext::GetMemoryUsage() const
	{
		if (m_MaxGPUMemory != 0)
		{
			GLint curAvailVRamMemoryKB = 0;
			glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &curAvailVRamMemoryKB);
			uint64_t memoryUsage = m_MaxVidoeMemory - static_cast<uint64_t>(curAvailVRamMemoryKB);
			if (FRACTURE_ONE(m_MaxVidoeMemory, 1/4) > memoryUsage)
			{
				RY_CORE_FATAL("Curently to much V-Ram Memory usage: {} KB avible: {} KB max: {} KB", memoryUsage, curAvailVRamMemoryKB, m_MaxVidoeMemory);
			}
			return memoryUsage;
		}
		return 0ull;
	}

}
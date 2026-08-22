#include "rypch.h"
#include "OpenGLFence.h"

#include <Platform/OpenGL/OpenGLBase.h>


namespace Rynex {
	OpenGLFence::OpenGLFence()
		: m_Fance(nullptr)
	{
	}

	OpenGLFence::~OpenGLFence()
	{
		DestroyID();
	}

	void OpenGLFence::SetupFence()
	{
		CreateID();
	}

	bool OpenGLFence::WaitForTransfered(uint64_t timeout, bool flush) const
	{

		constexpr GLbitfield flags[2] = { GL_SYNC_FLUSH_COMMANDS_BIT, 0u };
		constexpr int useFlage = 0;
		GLbitfield useFlags = flush ? flags[0] : flags[1];
		GLenum result = glClientWaitSync(m_Fance, useFlags, timeout);
		GL_CHECK_LOOP();

		switch (result)
		{
		case GL_ALREADY_SIGNALED:
			// GL_ALREADY_SIGNALED indicates that sync was signaled at the time that glClientWaitSync was called. 
			return true;

		case GL_TIMEOUT_EXPIRED:
			// GL_CONDITION_SATISFIED indicates that sync was signaled before the timeout expired.
			return false;

		case GL_CONDITION_SATISFIED:
			RY_CORE_TRACE("GL_CONDITION_SATISFIED indicates that sync was signaled before the timeout expired.");
			return true;
		case GL_WAIT_FAILED:
			RY_CORE_FATAL("GL_WAIT_FAILED indicates that an error occurred. Additionally, an OpenGL error will be generated.");
			return true;
		case GL_INVALID_VALUE:
			RY_CORE_TRACE("GL_INVALID_VALUE is generated if flags contains any unsupported flag.");
			return true;
		default:
			RY_CORE_FATAL("Result Value: {}", result);
			RY_CORE_ASSERT(false, "Not defined Result");
			return false;
		}

		RY_CORE_ASSERT(false);
		return false;
	}

	int64_t OpenGLFence::StoppThreadUntlieLoded()
	{
		return 0ll;
	}

	bool OpenGLFence::IsTransfered()
	{
		if (nullptr == m_Fance)
			return true;

		GLint status = GetState(GL_SYNC_STATUS);
		
	
		if (GL_SIGNALED == status)
		{
			constexpr GLuint64 timeout = 0ull;

			return WaitForTransfered(timeout);
		}
		RY_CORE_ASSERT(status == GL_UNSIGNALED, "The only rigth other return value from GetState(GL_SYNC_STATUS), woude be GL_UNSIGNALED");

		return false;
	}

	void OpenGLFence::CreateID()
	{
		DestroyID();
		RY_CORE_ASSERT(nullptr == m_Fance, "Fance is present!");

		// Both values have the only vaild values, for glFenceSync curently
		// OpenGL this are placeholder for anticipated future extensions of fence sync object capabilities. 
		// But curently in March, 2026 there are no extions present.
		constexpr GLenum condition = GL_SYNC_GPU_COMMANDS_COMPLETE;
		constexpr GLbitfield falg = 0u;
		m_Fance = glFenceSync(condition, falg);
		GL_CHECK();

	}

	void OpenGLFence::DestroyID()
	{
		if (nullptr == m_Fance)
			return;

		glDeleteSync(m_Fance);
		m_Fance = nullptr;

		GL_CHECK();
	}

	// options for (pname)			expted return values    |
	//					                                    V
	//		GL_OBJECT_TYPE			GL_SYNC_FENCE
	//		GL_SYNC_STATUS			GL_SIGNALED		||		GL_UNSIGNALED
	//		GL_SYNC_CONDITION		GL_SYNC_GPU_COMMANDS_COMPLETE
	//		GL_SYNC_FLAGS			
	//	error states (return) 
	//		GL_INVALID_VALUE 
	//		GL_INVALID_ENUM 
	GLint OpenGLFence::GetState(GLenum pname) const
	{
		RY_CORE_ASSERT(nullptr != m_Fance, "no Fance Set!");

		GLint status;
		GLsizei length;
		GLsizei count = sizeof(status);
		glGetSynciv(m_Fance, pname, sizeof(status), &length, &status);
		RY_CORE_ASSERT(length == 1, "Not expexted length Size! we asument simply that it is always 1, but never chenked.");

		return status;
	}

}


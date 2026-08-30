#pragma once
#include <glad/glad.h>

namespace Rynex {

	template<typename T>
	class ParentEventFuncs
	{
	public:
		using WeakPtr = T*;
		using Func = std::function<void(WeakPtr)>;
	// --- public member methods ----------------------------------------------------------------------------------------------
		ParentEventFuncs()
			: m_OnChange(nullptr)
			, m_OnDestroy(nullptr)
		{
		}

		ParentEventFuncs(const ParentEventFuncs& eventsFuncs)
			: m_OnChange(eventsFuncs.m_OnChange)
			, m_OnDestroy(eventsFuncs.m_OnDestroy)
		{
		}
		
		ParentEventFuncs(const Func& onChange, const Func& onDestroy)
			: m_OnChange(onChange)
			, m_OnDestroy(onDestroy)
		{
		}

		void operator =(const ParentEventFuncs& eventsFuncs)
		{
			m_OnChange = eventsFuncs.m_OnChange;
			m_OnDestroy = eventsFuncs.m_OnDestroy;
		}

		void OnChangCall(WeakPtr ptr) const
		{
			this->m_OnChange(ptr);
		}

		void OnDestroyCall(WeakPtr ptr) const
		{
			this->m_OnDestroy(ptr);
		}

	private:
		Func m_OnChange;
		Func m_OnDestroy;
	};

	class OpenGLFence
	{
	public:
		OpenGLFence();
		~OpenGLFence();


		void SetupFence();
		bool WaitForTransfered(uint64_t timeout = 0u, bool flush = false) const;
		bool IsTransfered();
		const GLsync& GetFence() const { return m_Fance; };
		int64_t StoppThreadUntlieLoded();
		void DestroyID();
	private:
		void CreateID();

		GLint GetState(GLenum pname) const;
	private:
		GLsync m_Fance;
	};

}

 
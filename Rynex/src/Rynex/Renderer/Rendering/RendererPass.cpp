#include "rypch.h"
#include "RendererPass.h"

namespace Rynex {

	namespace Utils {

		template<typename Func, typename ...Args>
		inline static void AddDrawCallFunction(std::vector<std::function<void()>>* funcStackPtr, Func&& func, Args && ...args)
		{
			funcStackPtr->emplace_back([f = std::forward<Func>(func), ... params = std::forward<Args>(args)]() {
				f(params...);
			});
		}

		inline static void ExecuteDrawCall(const std::vector<std::function<void()>>& funcStackPtr)
		{
			for (const std::function<void()>& drawFunc : funcStackPtr)
			{
				drawFunc();
			}
		}
	}


	RendererPass::RendererPass()
	{
		m_Framebuffer = nullptr;
	}

	RendererPass::~RendererPass()
	{
		m_DrawCalls.clear();
		m_DrawCalls.shrink_to_fit();

	}

	void RendererPass::SetFrambuffer(const Ref<Framebuffer>& framebuffer)
	{
		m_Framebuffer = framebuffer;
	}

	void RendererPass::ClearDrawCallList()
	{
		m_DrawCalls.clear();
	}

	const Ref<Framebuffer>& RendererPass::GetFrambuffer() const
	{
		return m_Framebuffer;
	}

	template<typename Func, typename ...Args>
	inline void RendererPass::AddDrawCall(Func&& func, Args&& ...args)
	{
		Utils::AddDrawCallFunction(&m_DrawCalls2D ,func, args);

	}

	

	template<typename Func, typename ...Args>
	void RendererPass::DrawObjects(Func&& func, Args && ...args)
	{
		if(m_Framebuffer)
			m_Framebuffer->Bind();

		func(m_Framebuffer, std::forward<Args>(args));

		
		Utils::ExecuteDrawCall(m_DrawCalls);
		

		if(m_Framebuffer)
			m_Framebuffer->UnBind();
	}



}
#pragma once
#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Renderer/Objects/Mesh.h>
#include <Rynex/Renderer/Camera/Camera.h>

namespace Rynex {

	class RendererPass
	{
	public:
		RendererPass();
		~RendererPass();
		void SetFrambuffer(const Ref<Framebuffer>& framebuffer);

		

		template<typename Func, typename... Args>
		void AddDrawCall(Func&& func, Args&&... args);
		
		// the Funktion you can pass wel be exexuted Befor we Render
		template<typename Func, typename... Args>
		void DrawObjects(Func&& func, Args&&... args);


		void ClearDrawCallList();

		const Ref<Framebuffer>& GetFrambuffer() const;
	private:
		enum Flags
		{
			None = 0,
			CheckTraspernt = 1,
			SortTranspernt = 2
		};

	private:
		Ref<Framebuffer> m_Framebuffer;
		std::vector<std::function<void()>> m_DrawCalls;
		int m_Flags;
	};
	
}

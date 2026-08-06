#pragma once

#include "Rynex/Core/Window.h"

#include "Rynex/Events/MouseEvent.h"
#include "Rynex/Events/KeyEvent.h"

#include <GLFW/glfw3.h>

namespace Rynex {
	class ThreadContext;

	class WindowsWindow : public Window 
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;
		virtual void OnSreenRefresh() override;
		virtual unsigned int GetWidth() const override { return m_Data.Width; }
		virtual unsigned int GetHeight() const override { return m_Data.Height; }

		virtual unsigned int GetPosX() const override { return m_Data.PosX; }
		virtual unsigned int GetPosY() const override { return m_Data.PosY; }

		virtual float GetMousePosX() const override { return m_Data.MousePosX; }
		virtual float GetMousePosY() const override { return m_Data.MousePosY; }

		// Window attributes
		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual bool IsFocused() const override;
		virtual bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const override { return m_Window; }
		virtual GraphicsContext* GetGraphicsContext() override { return m_Context.get(); }

		virtual Ref<ThreadContext> CreateThreadeContext() override;
		virtual int64_t GetRenderTime() const override { return m_UpdateScreeTime; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		int64_t m_UpdateScreeTime;
		Ref<GraphicsContext> m_Context;
		
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			unsigned int PosX, PosY;
			float MousePosX, MousePosY;
			bool VSync, Focuse, Hovered;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}
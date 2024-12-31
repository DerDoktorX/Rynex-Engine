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

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		unsigned int GetPosX() const override { return m_Data.PosX; }
		unsigned int GetPosY() const override { return m_Data.PosY; }

		float GetMousePosX() const override { return m_Data.MousePosX; }
		float GetMousePosY() const override { return m_Data.MousePosY; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsFocused() const override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const override { return m_Window; }
		virtual GraphicsContext* GetGraphicsContext() override { return m_Context.get(); }

		virtual Ref<ThreadContext> CreateThreadeContext() override;
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;

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
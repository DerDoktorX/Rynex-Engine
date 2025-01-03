#pragma once

#include "rypch.h"
#include "Rynex/Events/Event.h"
#include "Rynex/Renderer/API/GraphicsContext.h"

namespace Rynex {

	class ThreadContext;

	struct RYNEX_API WindowProps
	{
		std::string Title;
		uint32_t Height;
		uint32_t Width;
		uint32_t PosX;
		uint32_t PosY;


		WindowProps(const std::string& title = "Rynex Engine",
			uint32_t width = 1600 ,
			uint32_t height = 900 )
			: Title(title), Width(width), Height(height)
		{

		}
	};

	class RYNEX_API Window
	{
	public: 
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual uint32_t GetPosX() const = 0;
		virtual uint32_t GetPosY() const = 0;

		virtual float GetMousePosX() const = 0;
		virtual float GetMousePosY() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;
		virtual bool IsFocused() const = 0;
		virtual void* GetNativeWindow() const = 0;

		virtual GraphicsContext* GetGraphicsContext() = 0;

		virtual Ref<ThreadContext> CreateThreadeContext() = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}
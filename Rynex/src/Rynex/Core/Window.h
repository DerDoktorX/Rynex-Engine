#pragma once


#include <Rynex/Events/Event.h>
#include <Rynex/Renderer/API/GraphicsContext.h>
struct GLFWwindow;


namespace Rynex {

	class ThreadContext;
	class WindowsWindow;


	struct RYNEX_API WindowProps
	{
		std::string m_Title;
		uint32_t m_Height;
		uint32_t m_Width;
		uint32_t m_PosX;
		uint32_t m_PosY;

	    WindowProps()
	        : m_Title("Window Rynex-Engine")
            , m_Height(600u)
            , m_Width(400u)
            , m_PosX(0u)
            , m_PosY(0u)
	    {
	    }

        explicit WindowProps(const std::string& title, const uint32_t width = 1600u , const uint32_t height = 900u)
			: m_Title(title)
	        , m_Height(height)
	        , m_Width(width)
	        , m_PosX(0u)
	        , m_PosY(0u)
		{
		}
	};

	class RYNEX_API Window
	{
	public: 
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnScreenRefresh() = 0;
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
		virtual int64_t GetRenderTime() const = 0;

		virtual GraphicsContext* GetGraphicsContext() = 0;
		virtual Ref<ThreadContext> CreateThreadContext() = 0;

		static Scope<Window> Create();
	    static Scope<Window> Create(const WindowProps& props);

	};






}
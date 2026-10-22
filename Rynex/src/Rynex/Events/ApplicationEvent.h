#pragma once

#include <Rynex/Events//Event.h>



namespace Rynex {

	class RYNEX_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int withe, unsigned int height)
			: m_Width(withe), m_Height(height) {}
        ~WindowResizeEvent() { };

		[[nodiscard]] inline unsigned int GetWidth() const { return m_Width; }
		[[nodiscard]] inline unsigned int GetHeight() const { return m_Height; }

		[[nodiscard]] std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)

	private:
		unsigned int m_Width, m_Height;
	};

	class RYNEX_API WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(unsigned int posX, unsigned int posY)
			: m_PosX(posX), m_PosY(posY) {}
        ~WindowMovedEvent() { }

		[[nodiscard]] inline unsigned int GetPosX() const { return m_PosX; }
		[[nodiscard]] inline unsigned int GetPosY() const { return m_PosY; }

		[[nodiscard]] std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowMoved: " << m_PosX << ", " << m_PosY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowMoved)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	private:
		unsigned int m_PosX, m_PosY;

	};

	class RYNEX_API WindowCurserEnterEvent : public Event
	{
	public:
		WindowCurserEnterEvent() = default;
        ~WindowCurserEnterEvent() { }

		[[nodiscard]] std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowCurserEnter!";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowCurserEnter)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};

	class RYNEX_API WindowCurserLeaveEvent : public Event
	{
	public:
		WindowCurserLeaveEvent() = default;
	    ~WindowCurserLeaveEvent() { }
		[[nodiscard]] std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowCurserLeave!";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowCurserLeave)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};

	class RYNEX_API WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() = default;
	    ~WindowFocusEvent() {};

		[[nodiscard]] std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowFocus!";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowFocus)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};

	class RYNEX_API WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() = default;
        ~WindowLostFocusEvent() {}


		[[nodiscard]] std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowLostFocus!";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowLostFocus)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};


	class RYNEX_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;
        ~WindowCloseEvent() { }

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
			
	};

	

	class RYNEX_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() { }
        ~AppTickEvent() { }

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};

	class RYNEX_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}
	    ~AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};

	class RYNEX_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}
        ~AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};
}
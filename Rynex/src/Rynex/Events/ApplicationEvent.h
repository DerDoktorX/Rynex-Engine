#pragma once

#include "Event.h"



namespace Rynex {

	class RYNEX_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int withe, unsigned int heigth)
			: m_Width(withe), m_Height(heigth) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
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

		inline unsigned int GetPosX() const { return m_PosX; }
		inline unsigned int GetPosY() const { return m_PosY; }

		std::string ToString() const override
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
		WindowCurserEnterEvent() {}

		std::string ToString() const override
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
		WindowCurserLeaveEvent() {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowCurserLeave!";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowCurserLeave)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};

	class RYNEX_API WindowFocuseEvent : public Event
	{
	public:
		WindowFocuseEvent() {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowFocuse!";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowFocuse)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};

	class RYNEX_API WindowLostFocuseEvent : public Event
	{
	public:
		WindowLostFocuseEvent() {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowLostFocuse!";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowLostFocuse)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};


	class RYNEX_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
			
	};

	

	class RYNEX_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}


		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};

	class RYNEX_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};

	class RYNEX_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEYGORY(EventCategoryApplication)
	};
}
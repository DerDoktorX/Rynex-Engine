#pragma once
#include "Event.h"


namespace Rynex {

	class RYNEX_API MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMoveEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEYGORY(EventCategoryMouse | EventCategoryInput)

	private: 
		float m_MouseX, m_MouseY;
	};


	class RYNEX_API MouseSrolledEvent : public Event
	{
	public:
		MouseSrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseSrolledEvent: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseSrolled)
		EVENT_CLASS_CATEYGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_XOffset, m_YOffset;
	};

	class RYNEX_API MouseButtenEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEYGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		MouseButtenEvent(int button) 
			: m_Button(button) {}

		int m_Button;
	};

	class RYNEX_API MouseButtenPressedEvent : public MouseButtenEvent
	{
	public:
		MouseButtenPressedEvent(int button)
			: MouseButtenEvent(button) {}


		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtenPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)

	};

	class RYNEX_API MouseButtenRealsedEvent : public MouseButtenEvent
	{
	public:
		MouseButtenRealsedEvent(int button)
			: MouseButtenEvent(button) {}


		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtenRealsedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonRealsed)

	};

}
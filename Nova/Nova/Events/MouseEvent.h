#pragma	once
#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H
#include <Nova/Events/Event.h>
#include <sstream>

namespace Nova
{
	class NOVA_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y) : m_mouseX(x), m_mouseY(y) {}

		inline float GetX() const { return m_mouseX; }
		inline float GetY() const { return m_mouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMoved Event: (" << m_mouseX << ", " << m_mouseY << ")";
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_mouseX;
		float m_mouseY;
	};


	class NOVA_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xoffset, float yoffset) : m_xOffset(xoffset), m_yOffset(yoffset) {}
		inline float GetXOffset() const { return m_xOffset; }
		inline float GetYOffset() const { return m_yOffset; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolled Event: " << "(" << m_xOffset << ", " << m_yOffset << ")";
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)
	private:
		float m_xOffset;
		float m_yOffset;
	};


	class NOVA_API MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButtton() const { return m_button; }
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse)
	protected:
		MouseButtonEvent(int button) : m_button(button) {}

		int m_button;
	};

	class NOVA_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent :" << m_button;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonPressed)
	};
	class NOVA_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent :" << m_button;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}

#endif
#pragma once
#ifndef  APPLICATION_EVENT_H
#define  APPLICATION_EVENT_H
#include <Nova/Events/Event.h>
#include <sstream>

namespace Nova
{
	class NOVA_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}

		inline unsigned int GetWidth() const { return m_width; }
		inline unsigned int GetHeight() const { return m_height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;
			return ss.str();
		}
		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_width;
		unsigned int m_height;

	};
	
	class NOVA_API WindowCLoseEvent : public Event
	{
	public:
		WindowCLoseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class NOVA_API AppTickEvent : public Event 
	{
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class NOVA_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class NOVA_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};


}
#endif
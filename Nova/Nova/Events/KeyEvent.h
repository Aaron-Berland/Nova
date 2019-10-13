#pragma once
#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include <Nova/Events/Event.h>
#include <sstream>
namespace Nova
{
	class NOVA_API KeyEvent : public Event 
	{
	public:
		inline int GetKeyCode() const { return m_keyCode; }
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);
	protected:
		KeyEvent(int keyCode) : m_keyCode(keyCode) {}
		int m_keyCode;

	};
	class NOVA_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keyCode, int repeatCount) : KeyEvent(keyCode) , m_repeatCount(repeatCount) {}
		inline int GetRepeatCount() const { return m_repeatCount; }
		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "KeyPressed Event: " << m_keyCode << " (" << m_repeatCount << " repeats)";
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyPressed)
	
	private:
		int m_repeatCount;
	};

	class NOVA_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}
		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "KeyRelased Event: " << m_keyCode;
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyReleased)
	};

	class NOVA_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keyCode) : KeyEvent(keyCode) {}
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTyped Event: " << m_keyCode;
			return ss.str();
		}
		EVENT_CLASS_TYPE(KeyTyped)
	};
}
#endif
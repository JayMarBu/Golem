#pragma once

#include "Event.h"

namespace golem
{
	// ********************************************************************************************//
	//									  Key Event Class										   //
	// ********************************************************************************************//	
	class KeyEvent : public Event
	{
		// Members ********************************************************************************
	protected:
		int m_keyCode;
		// Methods ********************************************************************************
	protected:
		KeyEvent(int keycode)
			: m_keyCode(keycode) {}

	public:
		inline int GetKeyCode() const {return m_keyCode;}

		EVENT_CLASS_CATEGORY(EventCategory_Keyboard | EventCategory_Input)
	};

	// ********************************************************************************************//
	//								  Key Pressed Event Class									   //
	// ********************************************************************************************//	
	class KeyPressedEvent : public KeyEvent
	{
		// Members ********************************************************************************
	private:
		int m_repeatCount;
		// Methods ********************************************************************************
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), m_repeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_repeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keyCode << "(" << m_repeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	};

	// ********************************************************************************************//
	//								  Key Released Event Class									   //
	// ********************************************************************************************//	
	class KeyReleasedEvent : public KeyEvent
	{
		// Members ********************************************************************************

		// Methods ********************************************************************************
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}
		
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	// ********************************************************************************************//
	//								    Key Typed Event Class									   //
	// ********************************************************************************************//	
	class KeyTypedEvent : public KeyEvent
	{
		// Members ********************************************************************************

		// Methods ********************************************************************************
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}
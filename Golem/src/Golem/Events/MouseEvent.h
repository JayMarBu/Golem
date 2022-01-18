#pragma once

#include "Event.h"

namespace golem
{
	// ********************************************************************************************//
	//									Mouse Moved Event Class									   //
	// ********************************************************************************************//
	class MouseMovedEvent : public Event
	{
		// Members ********************************************************************************
	private:
		float m_mouseX, m_mouseY;
		// Methods ********************************************************************************
	public:
		MouseMovedEvent(float x, float y)
			: m_mouseX(x), m_mouseY(y) {}

		inline float GetX() const {return m_mouseX;}
		inline float GetY() const {return m_mouseY;}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_mouseX << "," << m_mouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategory_Mouse | EventCategory_Input)
	};

	// ********************************************************************************************//
	//								 Mouse Scrolled Event Class									   //
	// ********************************************************************************************//
	class MouseScrolledEvent : public Event
	{
		// Members ********************************************************************************
	private:
		float m_xOffset, m_yOffset;
		// Methods ********************************************************************************
	public:
		MouseScrolledEvent(float x, float y)
			: m_xOffset(x), m_yOffset(y) {}

		inline float GetXOffset() const { return m_xOffset; }
		inline float GetYOffset() const { return m_yOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_xOffset << "," << m_yOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategory_Mouse | EventCategory_Input)
	};

	// ********************************************************************************************//
	//								 Mouse Button Event Class									   //
	// ********************************************************************************************//
	class MouseButtonEvent : public Event
	{
		// Members ********************************************************************************
	protected:
		int m_button;
		// Methods ********************************************************************************
	protected:
		MouseButtonEvent(int button)
			: m_button(button) {}

	public:
		inline int GetMouseButton() const { return m_button; }

		EVENT_CLASS_CATEGORY(EventCategory_Input | EventCategory_Mouse)
	};

	// ********************************************************************************************//
	//								Mouse Button Pressed Event Class							   //
	// ********************************************************************************************//
	class MouseButtonPressedEvent : public MouseButtonEvent
	{
		// Members ********************************************************************************
	private:
		// Methods ********************************************************************************
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button){}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	// ********************************************************************************************//
	//								Mouse Button Released Event Class							   //
	// ********************************************************************************************//
	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
		// Members ********************************************************************************
	private:
		// Methods ********************************************************************************
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
	
}
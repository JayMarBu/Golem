#pragma once

#include "Event.h"

namespace golem
{
	// ********************************************************************************************//
	//								  Window Resize Event Class									   //
	// ********************************************************************************************//
	class WindowResizeEvent : public Event
	{
		// Members ********************************************************************************
	private:
		unsigned int m_width, m_height;
		// Methods ********************************************************************************
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_width(width), m_height(height) {}

		inline unsigned int GetWidth() const { return m_width; }
		inline unsigned int GetHeight() const { return m_height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << "," << m_height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategory_Application)
	};

	// ********************************************************************************************//
	//								  Window Closed Event Class									   //
	// ********************************************************************************************//
	class WindowCloseEvent : public Event
	{
		// Members ********************************************************************************

		// Methods ********************************************************************************
	public:
		WindowCloseEvent(){}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategory_Application)
	};

	// ********************************************************************************************//
	//									App Tick Event Class									   //
	// ********************************************************************************************//
	class AppTickEvent : public Event
	{
		// Members ********************************************************************************

		// Methods ********************************************************************************
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategory_Application)
	};

	// ********************************************************************************************//
	//									App Update Event Class									   //
	// ********************************************************************************************//
	class AppUpdateEvent : public Event
	{
		// Members ********************************************************************************

		// Methods ********************************************************************************
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategory_Application)
	};

	// ********************************************************************************************//
	//									App Render Event Class									   //
	// ********************************************************************************************//
	class AppRenderEvent : public Event
	{
		// Members ********************************************************************************

		// Methods ********************************************************************************
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategory_Application)
	};
}
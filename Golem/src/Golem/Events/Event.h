#pragma once
#include "golpch.h"
#include "Golem/Core.h"

namespace golem
{
	// ********************************************************************************************//
	//									Event Reflection Enums									   //
	// ********************************************************************************************//
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategory_Application	= BIT(0),
		EventCategory_Input			= BIT(1),
		EventCategory_Keyboard		= BIT(2),
		EventCategory_Mouse			= BIT(3),
		EventCategory_MouseButton	= BIT(4)
	};

	// ********************************************************************************************//
	//									Event Reflection Macros									   //
	// ********************************************************************************************//
	#define EVENT_CLASS_TYPE(type)  static EventType GetStaticType() {return EventType::##type;}\
									virtual EventType GetEventType() const override { return GetStaticType(); }\
									virtual const char* GetName() const override {return #type;}

	#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return category;}

	// ********************************************************************************************//
	//										Event Class											   //
	// ********************************************************************************************//
	class GOLEM_API Event
	{
		friend class EventDispatcher;

		// Members ********************************************************************************
	public:
		bool Handled = false;
		// Methods ********************************************************************************
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const {return GetName();}

		inline bool IsInCategory(EventCategory catergory)
		{
			return GetCategoryFlags() & catergory;
		}
	};
	
	// ********************************************************************************************//
	//									Event Dispatcher Class									   //
	// ********************************************************************************************//
	class EventDispatcher
	{
		template <typename T>
		using EventFn = std::function<bool(T&)>;
		// Members ********************************************************************************
	private:
		Event& m_event;
		// Methods ********************************************************************************
	public:
		EventDispatcher(Event& event)
			: m_event(event)
		{	
		}

		template <typename T>
		bool Dispatch(EventFn<T> func)
		{
			if(m_event.GetEventType() == T::GetStaticType())
			{
				m_event.Handled = func(*(T*)&m_event);
				return true;
			}
			return false;
		}
	};
}
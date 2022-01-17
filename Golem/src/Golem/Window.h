#pragma once

#include "golpch.h"

#include "Golem/Core.h"
#include "Golem/Events/Event.h"

namespace golem
{
	struct WindowProps
	{
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowProps(const std::string& _title = "Golem App",	
					unsigned int _width = 1200,
					unsigned int _height = 700)
			: title{_title}, width{_width}, height{_height}
		{
		}
	};

	class GOLEM_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		// Members ********************************************************************************
	protected:

		// Methods ********************************************************************************
	public:
		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;


		virtual bool ShouldClose() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};
}
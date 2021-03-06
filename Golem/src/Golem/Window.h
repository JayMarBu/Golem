#pragma once

#include "golpch.h"

#include "Golem/Core.h"
#include "Golem/Events/Event.h"

namespace golem
{
	struct WindowProps
	{
		std::string title;
		uint32_t width;
		uint32_t height;

		WindowProps(const std::string& _title = "Golem App",	
					uint32_t _width = 1600,
					uint32_t _height = 900)
			: title{_title}, width{_width}, height{_height}
		{
		}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		// Members ********************************************************************************
	public:
		struct WindowExtent
		{
			uint32_t width;
			uint32_t height;

			operator VkExtent2D() const {return {width, height};}
		};

		// Methods ********************************************************************************
	public:
		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) = 0;

		virtual bool ShouldClose() const = 0;
		virtual bool WasResized() const = 0;
		virtual void ResetWindowResizedFlag() = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual WindowExtent GetExtent() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};
}
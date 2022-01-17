#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Golem/Window.h"


namespace golem
{
	class WindowWin32 : public Window
	{
		// Members ********************************************************************************
	private:
		GLFWwindow* m_window;

		struct WindowDate
		{
			std::string title;
			unsigned int width;
			unsigned int height;
			bool VSync;

			EventCallbackFn eventCallback;
		};

		WindowDate m_data;

		// Methods ********************************************************************************
	public:
		WindowWin32(const WindowProps& props);
		virtual ~WindowWin32();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override {return m_data.width;}
		inline unsigned int GetHeight() const override {return m_data.height;}

		inline void SetEventCallback(const EventCallbackFn& callback) override {m_data.eventCallback = callback;}
		void SetVSync(bool enabled);
		bool IsVSync() const;

		inline bool ShouldClose() const override{ return glfwWindowShouldClose(m_window); }
	private:
		virtual void Init(const WindowProps& props);
		virtual void ShutDown();
	};
}


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

		struct WindowData
		{
			std::string title;
			unsigned int width;
			unsigned int height;
			bool VSync;
			bool frameBufferResized;

			EventCallbackFn eventCallback;
		};

		WindowData m_data;

		// Methods ********************************************************************************
	public:
		WindowWin32(const WindowProps& props);
		virtual ~WindowWin32();

		void OnUpdate() override;

		inline uint32_t GetWidth() const override {return m_data.width;}
		inline uint32_t GetHeight() const override {return m_data.height;}

		inline virtual void SetEventCallback(const EventCallbackFn& callback) override {m_data.eventCallback = callback;}
		virtual void SetVSync(bool enabled);
		virtual bool IsVSync() const;

		inline virtual bool ShouldClose() const override{ return glfwWindowShouldClose(m_window); }

		virtual void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) override;

		virtual bool WasResized() const override { return m_data.frameBufferResized;}
		virtual void ResetWindowResizedFlag() override { m_data.frameBufferResized = false; }

		inline virtual void* GetNativeWindow() const { return m_window; };

		WindowExtent GetExtent() const;
	private:
		virtual void Init(const WindowProps& props);
		virtual void ShutDown();
	};
}


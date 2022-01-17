#include "golpch.h"
#include "WindowWin32.h"
#include "Golem/Log.h"

namespace golem
{
	static bool s_GLFWInitialized = false;
	
	Window* Window::Create(const WindowProps& props)
	{
		return new WindowWin32(props);
	}

	WindowWin32::WindowWin32(const WindowProps& props)
	{
		Init(props);
	}

	WindowWin32::~WindowWin32()
	{
		ShutDown();
	}

	void WindowWin32::Init(const WindowProps& props)
	{
		m_data.title = props.title;
		m_data.width = props.width;
		m_data.height = props.height;

		GOL_CORE_INFO("Creating Window {0} ({1}, {2})", m_data.title, m_data.width, m_data.height);

		if(!s_GLFWInitialized)
		{
			int success = glfwInit();
			GOL_CORE_ASSERT(success, "Could not initialize GLFW!");

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

			s_GLFWInitialized = true;
		}

		m_window = glfwCreateWindow((int)m_data.width, (int)m_data.height, m_data.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window,&m_data);
		SetVSync(true);
	}

	void WindowWin32::ShutDown()
	{
		glfwDestroyWindow(m_window);
	}

	void WindowWin32::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_window);
	}

	void WindowWin32::SetVSync(bool enabled)
	{
		if(enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_data.VSync = enabled;
	}

	bool WindowWin32::IsVSync() const
	{
		return m_data.VSync;
	}



}
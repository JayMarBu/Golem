#include "golpch.h"
#include "WindowWin32.h"
#include "Golem/Log.h"
#include "Golem/Events/ApplicationEvent.h"
#include "Golem/Events/KeyEvent.h"
#include "Golem/Events/MouseEvent.h"

namespace golem
{
	static bool s_GLFWInitialized = false;
	
	static void GLFWErrorCallback(int error, const char* description)
	{
		GOL_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

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

			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;
		}

		m_window = glfwCreateWindow((int)m_data.width, (int)m_data.height, m_data.title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_window,&m_data);
		SetVSync(true);

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) 
		{
			WindowData& data =  *(WindowData*)glfwGetWindowUserPointer(window);

			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.eventCallback(event);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.eventCallback(event);
				break;
			}

			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.eventCallback(event);
				break;
			}

			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.eventCallback(event);
				break;
			}
			}
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch(action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.eventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xoffset,(float)yoffset);
			data.eventCallback(event);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xpos,(float)ypos);
			data.eventCallback(event);
		});
	}

	void WindowWin32::ShutDown()
	{
		glfwDestroyWindow(m_window);
	}

	void WindowWin32::OnUpdate()
	{
		glfwPollEvents();
	}

	void WindowWin32::SetVSync(bool enabled)
	{
		m_data.VSync = enabled;
	}

	bool WindowWin32::IsVSync() const
	{
		return m_data.VSync;
	}



}
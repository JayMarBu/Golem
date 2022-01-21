#include "golpch.h"
#include "InputWin32.h"
#include <GLFW/glfw3.h>
#include "Golem/Application.h"

namespace golem
{
	Input* Input::s_instance = nullptr;

	bool InputWin32::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || GLFW_REPEAT;
	}

	bool InputWin32::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	float InputWin32::GetMouseXImpl()
	{
		auto pos = GetMousePosImpl();
		return (float)pos.x;
	}

	float InputWin32::GetMouseYImpl()
	{
		auto pos = GetMousePosImpl();
		return (float)pos.y;
	}

	golem::Input::MousePos InputWin32::GetMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		double x, y;

		glfwGetCursorPos(window, &x, &y);

		return { (float)x,(float)y };
	}

	void InputWin32::SetMousePosImpl(float x, float y)
	{
		const auto window = Application::Get().GetWindow().GetNativeWindow();

		glfwSetCursorPos(static_cast<GLFWwindow*>(window), x, y);
	}

	void InputWin32::SetMousePosCentreImpl()
	{
		const auto window = Application::Get().GetWindow();
		SetMousePosImpl(window.GetWidth()/2,window.GetHeight()/2);
	}

	void InputWin32::SetMouseCursorHiddenImpl(bool hidden)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		glfwSetInputMode(window, GLFW_CURSOR, (hidden == true)? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);
	}

}
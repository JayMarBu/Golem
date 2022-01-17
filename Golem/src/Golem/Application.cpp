#include "golpch.h"
#include "Application.h"

namespace golem
{

	Application::Application()
	{
		m_window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while(!m_window->ShouldClose())
		{
			m_window->OnUpdate();
		}
	}

}
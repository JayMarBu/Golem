#pragma once
#include "Core.h"
#include "Window.h"

namespace golem
{
	class GOLEM_API Application
	{
		// Members ********************************************************************************
	private:
		std::unique_ptr<Window> m_window;
		// Methods ********************************************************************************
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in client
	Application* CreateApplication();
}


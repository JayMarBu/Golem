#pragma once
#include "Core.h"

namespace golem
{
	class GOLEM_API Application
	{
		// Members ********************************************************************************

		// Methods ********************************************************************************
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in client
	Application* CreateApplication();
}


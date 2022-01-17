#pragma once
#include "Core.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "Golem/LayerStack.h"

namespace golem
{
	class GOLEM_API Application
	{
		// Members ********************************************************************************
	private:
		std::unique_ptr<Window> m_window;
		bool m_isRunning = true;
		LayerStack m_layerStack;
		// Methods ********************************************************************************
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	};

	// To be defined in client
	Application* CreateApplication();
}


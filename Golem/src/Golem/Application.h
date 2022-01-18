#pragma once
#include "Core.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "Golem/LayerStack.h"
#include "Render/Device.h"
#include "Render/Renderer.h"
#include "ImGui/ImGuiLayer.h"

namespace golem
{
	class Application
	{
		// Members ********************************************************************************
	protected:
		std::unique_ptr<Window> m_window;
		std::unique_ptr<Device> m_device;
		std::unique_ptr<golem::Renderer> m_renderer;

		ImGuiLayer* m_guiLayer;

		bool m_isRunning = true;
		LayerStack m_layerStack;
	private:
		static Application* s_instance;
		// Methods ********************************************************************************
	public:
		Application();
		virtual ~Application();

		static Application& Get() {return *s_instance;}

		inline Window& GetWindow() { return *m_window;}
		inline Device& GetDevice() { return *m_device;}
		inline Renderer& GetRenderer() { return *m_renderer;}

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


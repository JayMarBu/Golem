#pragma once
#include "Core.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "Golem/LayerStack.h"
#include "Render/Device.h"
#include "Render/Renderer.h"
#include "ImGui/ImGuiLayer.h"

#include "Temp/TextureManager.h"
#include "ThreadPool.h"

#include "Render/RenderTexture.h"


namespace golem
{
	class Application
	{
		// Members ********************************************************************************
	protected:
		Scope<Window> m_window;
		Scope<Device> m_device;
		Scope<golem::Renderer> m_renderer;
		ThreadPool m_threadPool{8};

		// Temp stuff ---------------------------------------------------------
		Scope<TextureManager> m_textureManager;

		double m_deltaTime = 0;

		// --------------------------------------------------------------------

		ImGuiLayer* m_guiLayer;

		bool m_isRunning = true;
		LayerStack m_layerStack;

		std::mutex m_eventMutex;
		std::queue<Event*> m_events;
	private:
		static Application* s_instance;
		// Methods ********************************************************************************
	public:
		Application(const std::string& name = "Golem App");
		virtual ~Application();

		static Application& Get() {return *s_instance;}

		inline ThreadPool& GetThreadPool() {return m_threadPool;}
		inline Window& GetWindow() { return *m_window;}
		inline Window* GetWindowPtr() { return m_window.get();}
		inline Device& GetDevice() { return *m_device;}
		inline Renderer& GetRenderer() { return *m_renderer;}
		inline TextureManager& GetTextureManager() { return *m_textureManager;}

		static void FireEvent(Event* e);
		static double DeltaTime() {return s_instance->m_deltaTime;}

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void PollEvents();

	};

	// To be defined in client
	Application* CreateApplication();
}


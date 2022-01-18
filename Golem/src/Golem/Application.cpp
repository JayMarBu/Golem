#include "golpch.h"
#include "Application.h"
#include "Events/Event.h"
#include "imgui.h"

namespace golem
{
#define BIND_EVENT_FUNC(x) std::bind(&Application::x, this, std::placeholders::_1) 

	golem::Application* Application::s_instance = nullptr;

	Application::Application()
	{
		GOL_CORE_ASSERT(s_instance == nullptr, "Application already exists!");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FUNC(OnEvent));

		m_device = std::make_unique<Device>(*m_window);

		m_renderer = std::make_unique<Renderer>(*m_window, *m_device);
	}

	Application::~Application()
	{
		
	}

	void Application::Run()
	{
		while(m_isRunning)
		{
			m_window->OnUpdate();

			auto commandBuffer = m_renderer->BeginFrame();
			if (!commandBuffer)
				continue;

			m_renderer->BeginSwapChainRenderPass(commandBuffer);

			for(auto layer : m_layerStack)
				layer->OnUpdate(commandBuffer);

			m_renderer->EndSwapChainRenderPass(commandBuffer);
			m_renderer->EndFrame();
		}
		vkDeviceWaitIdle(*m_device);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));

		//if(e.GetEventType() != EventType::MouseMoved)
		//	GOL_CORE_TRACE("{0}",e.ToString());

		for(auto it = m_layerStack.end(); it != m_layerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if(e.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_layerStack.PushOverlay(overlay);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_isRunning = false;

		return true;
	}

}
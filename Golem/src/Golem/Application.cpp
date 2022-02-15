#include "golpch.h"
#include "Application.h"
#include "Events/Event.h"
#include "imgui.h"
#include "Render/FrameInfo.h"
#include "Input/Input.h"

#include "backends/imgui_impl_vulkan.h"
#include "Time.h"

namespace golem
{
	golem::Application* Application::s_instance = nullptr;

	Application::Application(const std::string& name)
	{
		GOL_CORE_ASSERT(s_instance == nullptr, "Application already exists!");
		s_instance = this;

		m_window = Scope<Window>(Window::Create({name}));
		m_window->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvent));

		m_device = CreateScope<Device>(*m_window);

		m_renderer = CreateScope<Renderer>(*m_window, *m_device);

		m_guiLayer = new ImGuiLayer();
		PushOverlay(m_guiLayer);

		// Temp stuff ---------------------------------------------------------
		m_textureManager = CreateScope<TextureManager>(*m_device);
		// --------------------------------------------------------------------
	}

	Application::~Application()
	{
	}

	void Application::FireEvent(Event* e)
	{
		Application& app = Application::Get();

		std::unique_lock<std::mutex> lock{ app.m_eventMutex };
		app.m_events.emplace(e);
	}

	void Application::Run()
	{
		while(m_isRunning)
		{
			GOL_PROFILE_FUNCTION();

			PollEvents();

			Time::Frame();

			auto commandBuffer = m_renderer->BeginFrame();
			if (!commandBuffer)
				continue;

			for (auto layer : m_layerStack)
				layer->OnRender(commandBuffer);

			{
				GOL_PROFILE_SCOPE("Application::Run -> final renderpass");
			
				m_renderer->BeginSwapChainRenderPass(commandBuffer);
				m_guiLayer->Begin();

				for (auto layer : m_layerStack)
					layer->OnImGuiRender();
				m_guiLayer->End(commandBuffer);

				m_renderer->EndSwapChainRenderPass(commandBuffer);
				m_renderer->EndFrame();
			}

			{
				GOL_PROFILE_SCOPE("Application::Run -> IMGUI viewport update");
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}

		vkDeviceWaitIdle(*m_device);
	}

	void Application::OnEvent(Event& e)
	{
		//std::unique_lock<std::mutex> lock{m_eventMutex};

		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(Application::OnWindowResize));

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

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		//vkDeviceWaitIdle(*m_device);
		//offscreenPass.renderTexture->Resize(e.GetWidth(), e.GetHeight(), offscreenPass.renderPass);

		return true;
	}

	void Application::PollEvents()
	{
		GOL_PROFILE_FUNCTION();

		m_window->OnUpdate();
		m_threadPool.PollEvents();

		std::unique_lock<std::mutex> lock{ m_eventMutex };

		while (!m_events.empty())
		{
			auto e = std::move(m_events.front());
			m_events.pop();
			OnEvent(*e);
			CLEANUP(e);
		}
	}

}
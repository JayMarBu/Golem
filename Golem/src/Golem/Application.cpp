#include "golpch.h"
#include "Application.h"
#include "Events/Event.h"
#include "imgui.h"
#include "Render/FrameInfo.h"
#include "Input/Input.h"

namespace golem
{
	golem::Application* Application::s_instance = nullptr;

	struct GlobalUBO
	{
		glm::mat4 projectionView{ 1.0f };
		glm::vec3 lightDir = glm::normalize(glm::vec3(1, -3, -1));
	};

	Application::Application()
	{
		GOL_CORE_ASSERT(s_instance == nullptr, "Application already exists!");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvent));

		m_device = std::make_unique<Device>(*m_window);

		m_renderer = std::make_unique<Renderer>(*m_window, *m_device);

		m_guiLayer = new ImGuiLayer();
		PushOverlay(m_guiLayer);

		// Temp stuff ---------------------------------------------------------
		
		m_textureManager = std::make_unique<TextureManager>(*m_device);

		m_sampler = std::make_unique<Sampler>();

		m_camera.gObject.transform.translation = { 0.0f,0.0f,-5 };
		m_camera.controller.moveSpeed = 5;

		std::shared_ptr<Model> model = Model::CreateModelFromPrimative(*m_device, Primitives::Cube, false);
		auto cube = TempGameObject::Create();
		cube.model = model;
		cube.transform.translation = { 0.f,0.f,-.01f };
		m_gameObjects.push_back(std::move(cube));

		m_textureManager->LoadTexture("bunny", "media/textures/bunny.png");

		// init ubo
		m_UBObuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < m_UBObuffers.size(); i++)
		{
			m_UBObuffers[i] = std::make_unique<Buffer>(
				*m_device,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				std::lcm(
					m_device->properties.limits.minUniformBufferOffsetAlignment,
					m_device->properties.limits.nonCoherentAtomSize)
				);

			m_UBObuffers[i]->Map();
		}

		// init descriptor pools
		m_globalPool = DescriptorPool::Builder(*m_device)
			.SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		m_globalSetLayout = DescriptorSetLayout::Builder(*m_device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();

		m_globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < m_globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = m_UBObuffers[i]->DescriptorInfo();
			auto imageInfo = m_textureManager->ImageInfo(*m_sampler, "default");

			DescriptorWriter(*m_globalSetLayout, *m_globalPool)
				.WriteBuffer(0, &bufferInfo)
				.WriteImage(1, &imageInfo)
				.Build(m_globalDescriptorSets[i]);
		}

		// init render systems
		m_simpleRenderSystem = std::make_unique<SimpleRenderSystem>(
			*m_device,
			m_renderer->GetSwapChainRenderPass(),
			m_globalSetLayout->GetDescriptorSetLayout(),
			"shaders/simple_shader/simple_shader.vert.spv",
			"shaders/simple_shader/simple_shader.frag.spv");

		// Temp stuff ---------------------------------------------------------
	}

	Application::~Application()
	{
		
	}

	void Application::Run()
	{
		while(m_isRunning)
		{
			m_window->OnUpdate();

			// Temp stuff ---------------------------------------------------------
			m_timer.frame();

			//if (m_camera.controller.two_D)
			//	m_camera.controller.MoveInPlaneXY(m_timer.getTime(), m_camera.gObject);
			//else
			m_camera.controller.MoveInPlaneXZ(m_timer.getTime(), m_camera.gObject);
			m_camera.camera.SetViewXYZ(m_camera.gObject.transform.translation, m_camera.gObject.transform.rotation);

			float aspect = m_renderer->GetAspectRatio();
			m_camera.camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.0f);
			// Temp stuff ---------------------------------------------------------

			auto commandBuffer = m_renderer->BeginFrame();
			if (!commandBuffer)
				continue;

			// Temp stuff ---------------------------------------------------------
			// ******fill frame info struct * *****
				FrameInfo fInfo{
					m_renderer->GetFrameIndex(),
					m_timer.getTime(),
					commandBuffer,
					m_camera.camera,
					m_globalDescriptorSets[m_renderer->GetFrameIndex()]
				};

			// ****** update uniform buffers ******
			GlobalUBO ubo{};
			ubo.projectionView = m_camera.camera.GetProjection() * m_camera.camera.GetView();

			m_UBObuffers[fInfo.frameIndex]->WriteToBuffer(&ubo);
			m_UBObuffers[fInfo.frameIndex]->Flush();
			// Temp stuff ---------------------------------------------------------

			m_renderer->BeginSwapChainRenderPass(commandBuffer);

			m_simpleRenderSystem->RenderGameObjects(fInfo, m_gameObjects);

			for(auto layer : m_layerStack)
				layer->OnUpdate();

			m_guiLayer->Begin();
			for (auto layer : m_layerStack)
				layer->OnImGuiRender();
			m_guiLayer->End(commandBuffer);

			m_renderer->EndSwapChainRenderPass(commandBuffer);
			m_renderer->EndFrame();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		vkDeviceWaitIdle(*m_device);
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClose));

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
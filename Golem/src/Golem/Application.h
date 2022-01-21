#pragma once
#include "Core.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"
#include "Golem/LayerStack.h"
#include "Render/Device.h"
#include "Render/Renderer.h"
#include "ImGui/ImGuiLayer.h"

#include "Temp/TextureManager.h"
#include "Render/Buffer.h"
#include "Render/RenderSystem/SimpleRenderSystem.h"
#include "Temp/TempGameObject.h"
#include "Render/Objects/Sampler.h"
#include "Temp/KeyboardMovementController.h"
#include "Render/Camera.h"
#include "FrameTimer.h"

namespace golem
{
	struct CameraWrapper
	{
		CameraWrapper() : gObject(TempGameObject::Create()) {}

		Camera camera{};
		TempGameObject gObject;
		KeyboardMovementController controller{};
	};

	class Application
	{
		// Members ********************************************************************************
	protected:
		std::unique_ptr<Window> m_window;
		std::unique_ptr<Device> m_device;
		std::unique_ptr<golem::Renderer> m_renderer;

		// Temp stuff ---------------------------------------------------------
		std::unique_ptr<TextureManager> m_textureManager;

		std::unique_ptr<DescriptorPool> m_globalPool;
		std::unique_ptr<DescriptorSetLayout> m_globalSetLayout;
		std::vector<VkDescriptorSet> m_globalDescriptorSets;

		std::vector<std::unique_ptr<Buffer>> m_UBObuffers;

		std::unique_ptr<SimpleRenderSystem> m_simpleRenderSystem;

		std::vector<TempGameObject> m_gameObjects;
		CameraWrapper m_camera{};

		std::unique_ptr<Sampler> m_sampler;

		FrameTimer m_timer;
		// Temp stuff ---------------------------------------------------------

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


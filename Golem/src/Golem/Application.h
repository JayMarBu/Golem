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

// Temp -----------------------------------------
#include "Temp/TempGameObject.h"
#include "Render/RenderSystem/SimpleRenderSystem.h"
#include "Render/Objects/Sampler.h"

// ----------------------------------------------

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

		struct FrameBufferAttachment 
		{
			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
		};

		struct OffscreenPass 
		{
			int32_t width, height;
			VkFramebuffer frameBuffer;
			FrameBufferAttachment depth;
			FrameBufferAttachment colour;
			VkRenderPass renderPass;
			VkSampler depthSampler;
			VkDescriptorImageInfo descriptor;
			VkFormat colourFormat;
			VkFormat depthFormat;
		} offscreenPass;

		
		std::vector<TempGameObject> temp_gameobjects;

		golem::Scope<golem::DescriptorPool> m_globalPool;
		golem::Scope<golem::DescriptorSetLayout> m_globalSetLayout;
		std::vector<VkDescriptorSet> m_globalDescriptorSets;

		std::vector<golem::Scope<golem::Buffer>> m_UBObuffers;

		golem::Scope<golem::SimpleRenderSystem> m_simpleRenderSystem;

		golem::Camera camera{};
		TempGameObject m_camGobj;

		golem::Scope<golem::Sampler> m_sampler;

		Scope<Pipeline> testPipeline; 
		VkPipelineLayout m_pipelineLayout;

		// --------------------------------------------------------------------

		ImGuiLayer* m_guiLayer;

		bool m_isRunning = true;
		LayerStack m_layerStack;

		std::mutex m_eventMutex;
	private:
		static Application* s_instance;
		// Methods ********************************************************************************
	public:
		Application();
		virtual ~Application();

		static Application& Get() {return *s_instance;}

		inline ThreadPool& GetThreadPool() {return m_threadPool;}
		inline Window& GetWindow() { return *m_window;}
		inline Device& GetDevice() { return *m_device;}
		inline Renderer& GetRenderer() { return *m_renderer;}
		inline TextureManager& GetTextureManager() { return *m_textureManager;}

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		void prepareOffscreenRenderpass();
		void prepareOffscreenFramebuffer();
		void buildPipeline();
	};

	// To be defined in client
	Application* CreateApplication();
}


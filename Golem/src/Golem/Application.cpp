#include "golpch.h"
#include "Application.h"
#include "Events/Event.h"
#include "imgui.h"
#include "Render/FrameInfo.h"
#include "Input/Input.h"

#include "backends/imgui_impl_vulkan.h"

namespace golem
{
	golem::Application* Application::s_instance = nullptr;

	Application::Application()
	{
		GOL_CORE_ASSERT(s_instance == nullptr, "Application already exists!");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FUNC(Application::OnEvent));

		m_device = std::make_unique<Device>(*m_window);

		m_renderer = std::make_unique<Renderer>(*m_window, *m_device);

		PrepareOffscreenRenderpass();

		offscreenPass.renderTexture = Ref<RenderTexture>(new RenderTexture(
			m_renderer->GetSwapChain().extent().width,
			m_renderer->GetSwapChain().extent().height,
			offscreenPass.renderPass));

		m_guiLayer = new ImGuiLayer();
		PushOverlay(m_guiLayer);

		// Temp stuff ---------------------------------------------------------
		m_textureManager = std::make_unique<TextureManager>(*m_device);
		// --------------------------------------------------------------------
	}

	Application::~Application()
	{
		vkDestroyRenderPass(*m_device, offscreenPass.renderPass, nullptr);
	}

	void Application::Run()
	{
		while(m_isRunning)
		{
			m_window->OnUpdate();
			m_threadPool.PollEvents();

			auto commandBuffer = m_renderer->BeginFrame();
			if (!commandBuffer)
				continue;

			BeginRenderPass(commandBuffer);

			for (auto layer : m_layerStack)
				layer->OnRender(commandBuffer);

			vkCmdEndRenderPass(commandBuffer);


			m_renderer->BeginSwapChainRenderPass(commandBuffer);

			m_guiLayer->Begin();
			m_guiLayer->RenderGameWindow(offscreenPass.renderTexture);

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

	void Application::PrepareOffscreenRenderpass()
	{
		auto colourFormat = m_renderer->GetSwapChain().imageFormat();
		auto depthFormat = m_renderer->GetSwapChain().FindDepthFormat();

		VkAttachmentDescription attachmentDescription{};
		attachmentDescription.format = depthFormat;
		attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = colourFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthReference;

		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, attachmentDescription };
		VkRenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassCreateInfo.pAttachments = attachments.data();
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassCreateInfo.pDependencies = dependencies.data();

		SAFE_RUN_VULKAN_FUNC(vkCreateRenderPass(*m_device, &renderPassCreateInfo, nullptr, &offscreenPass.renderPass), "");
	}

	void Application::BeginRenderPass(VkCommandBuffer commandBuffer)
	{
		VkClearColorValue defaultClearColor = { { 0.1f, 0.1f, 0.1f, 1.0f } };
		VkClearValue clearValues[2];
		VkViewport viewport{};
		VkRect2D scissor{};

		clearValues[0].color = defaultClearColor;
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = offscreenPass.renderPass;
		renderPassBeginInfo.framebuffer = offscreenPass.renderTexture->GetFrameBuffer();
		renderPassBeginInfo.renderArea.extent.width = offscreenPass.renderTexture->GetWidth();
		renderPassBeginInfo.renderArea.extent.height = offscreenPass.renderTexture->GetHeight();
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		viewport.width = (float)offscreenPass.renderTexture->GetWidth();
		viewport.height = (float)offscreenPass.renderTexture->GetHeight();
		viewport.minDepth = 0;
		viewport.maxDepth = 1;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		scissor.extent.width = offscreenPass.renderTexture->GetWidth();
		scissor.extent.height = offscreenPass.renderTexture->GetHeight();
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void Application::OnEvent(Event& e)
	{
		std::unique_lock<std::mutex> lock{m_eventMutex};

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
		vkDeviceWaitIdle(*m_device);
		// TODO: should implement explicit reset function in RenderTexture to preserve descriptor pools and only rebuild
		// internal framebuffer instead of complete recreating the entire object
		offscreenPass.renderTexture.reset(new RenderTexture(e.GetWidth(), e.GetHeight(), offscreenPass.renderPass));

		//for (int i = 0; i < m_globalDescriptorSets.size(); i++)
		//{
		//	VkDescriptorImageInfo descriptorImageInfo{};
		//	descriptorImageInfo.sampler = offscreenPass.renderTexture->GetSampler()->getSampler();
		//	descriptorImageInfo.imageView = offscreenPass.renderTexture->GetColourAttachment()->view;
		//	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		//	golem::DescriptorWriter(*m_globalSetLayout, *m_globalPool)
		//		.WriteImage(0, &descriptorImageInfo)
		//		.Overwrite(m_globalDescriptorSets[i]);
		//}

		return true;
	}

}
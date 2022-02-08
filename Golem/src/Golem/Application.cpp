#include "golpch.h"
#include "Application.h"
#include "Events/Event.h"
#include "imgui.h"
#include "Render/FrameInfo.h"
#include "Input/Input.h"

namespace golem
{
	golem::Application* Application::s_instance = nullptr;

	Application::Application()
		: m_camGobj(golem::TempGameObject::Create()) // Temp 
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
		prepareOffscreenFramebuffer();

		m_sampler = std::make_unique<golem::Sampler>();

		m_camGobj.transform.translation = { 0.0f,0.0f,-5 };

		auto& device = golem::Application::Get().GetDevice();

		Ref<golem::Model> cubeModel = golem::Model::CreateModelFromPrimative(device, golem::Primitives::Cube, false);
		auto cube = golem::TempGameObject::Create();
		cube.model = cubeModel;
		cube.transform.translation = { 1.5f,0.f,0 };
		temp_gameobjects.push_back(std::move(cube));

		// init ubo
		m_UBObuffers.resize(golem::SwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < m_UBObuffers.size(); i++)
		{
			m_UBObuffers[i] = std::make_unique<golem::Buffer>(
				device,
				sizeof(golem::GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				std::lcm(
					device.properties.limits.minUniformBufferOffsetAlignment,
					device.properties.limits.nonCoherentAtomSize)
				);

			m_UBObuffers[i]->Map();
		}

		// init descriptor pools
		m_globalPool = golem::DescriptorPool::Builder(device)
			.SetMaxSets(golem::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, golem::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		m_globalSetLayout = golem::DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();

		m_globalDescriptorSets.resize(golem::SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < m_globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = m_UBObuffers[i]->DescriptorInfo();

			VkDescriptorImageInfo descriptorImageInfo{};
			descriptorImageInfo.sampler = offscreenPass.depthSampler;
			descriptorImageInfo.imageView = offscreenPass.colour.view;
			descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			golem::DescriptorWriter(*m_globalSetLayout, *m_globalPool)
				.WriteImage(0, &descriptorImageInfo)
				.Build(m_globalDescriptorSets[i]);
		}

		buildPipeline();

		// init render systems
		/*m_simpleRenderSystem = std::make_unique<golem::SimpleRenderSystem>(
			device,
			m_globalSetLayout->GetDescriptorSetLayout());*/

		// Temp stuff ---------------------------------------------------------
	}

	Application::~Application()
	{
		vkDestroyPipelineLayout(*m_device, m_pipelineLayout, nullptr);

		vkDestroySampler(*m_device, offscreenPass.depthSampler, nullptr);

		// colour attachment
		vkDestroyImageView(*m_device, offscreenPass.colour.view, nullptr);
		vkDestroyImage(*m_device, offscreenPass.colour.image, nullptr);
		vkFreeMemory(*m_device, offscreenPass.colour.mem, nullptr);

		// Depth attachment
		vkDestroyImageView(*m_device, offscreenPass.depth.view, nullptr);
		vkDestroyImage(*m_device, offscreenPass.depth.image, nullptr);
		vkFreeMemory(*m_device, offscreenPass.depth.mem, nullptr);

		vkDestroyFramebuffer(*m_device, offscreenPass.frameBuffer, nullptr);

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

			// Temp -----------------------------
			
			VkClearColorValue defaultClearColor = { { 0.025f, 0.025f, 0.025f, 1.0f } };
			VkClearValue clearValues[2];
			VkViewport viewport{};
			VkRect2D scissor{};

			clearValues[0].color = defaultClearColor;
			clearValues[1].depthStencil = { 1.0f, 0 };

			VkRenderPassBeginInfo renderPassBeginInfo{};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = offscreenPass.renderPass;
			renderPassBeginInfo.framebuffer = offscreenPass.frameBuffer;
			renderPassBeginInfo.renderArea.extent.width = offscreenPass.width;
			renderPassBeginInfo.renderArea.extent.height = offscreenPass.height;
			renderPassBeginInfo.clearValueCount = 2;
			renderPassBeginInfo.pClearValues = clearValues;
			
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			viewport.width = (float)offscreenPass.width;
			viewport.height = (float)offscreenPass.height;
			viewport.minDepth = 0;
			viewport.maxDepth = 1;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			scissor.extent.width = offscreenPass.width;
			scissor.extent.height = offscreenPass.height;
			scissor.offset.x = 0;
			scissor.offset.y = 0;
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Set depth bias (aka "Polygon offset")
			// Required to avoid shadow mapping artifacts
			vkCmdSetDepthBias(
				commandBuffer,
				1.25f,
				0.0f,
				1.75f);

			for (auto layer : m_layerStack)
				layer->OnRender(commandBuffer);
			//vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.offscreen);
			//vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets.offscreen, 0, nullptr);
			//scenes[sceneIndex].draw(drawCmdBuffers[i]);

			vkCmdEndRenderPass(commandBuffer);

			// ----------------------------------

			m_renderer->BeginSwapChainRenderPass(commandBuffer);

			// Temp -----------------------------
			testPipeline->Bind(commandBuffer);
			vkCmdBindDescriptorSets(
				commandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				m_pipelineLayout,
				0, 1,
				&m_globalDescriptorSets[m_renderer->GetFrameIndex()],
				0, nullptr
			);
			vkCmdDraw(commandBuffer, 6, 1, 0, 0);
			// ----------------------------------

			for(auto layer : m_layerStack)
				layer->OnRender(commandBuffer);

			m_guiLayer->Begin();
			for (auto layer : m_layerStack)
				layer->OnImGuiRender();
			m_guiLayer->End(commandBuffer);

			m_renderer->EndSwapChainRenderPass(commandBuffer);
			m_renderer->EndFrame();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			for (auto layer : m_layerStack)
				layer->OnPostRender();
		}

		vkDeviceWaitIdle(*m_device);
	}

	void Application::prepareOffscreenRenderpass()
	{
		VkAttachmentDescription attachmentDescription{};
		attachmentDescription.format = offscreenPass.depthFormat;
		attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							// Clear depth at beginning of the render pass
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;						// We will read from depth, so it's important to store the depth attachment results
		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;					// We don't care about initial layout of the attachment
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;// Attachment will be transitioned to shader read at render pass end

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			// Attachment will be used as depth/stencil during render pass

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = offscreenPass.colourFormat;
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
		subpass.pDepthStencilAttachment = &depthReference;									// Reference to our depth attachment

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

	void Application::prepareOffscreenFramebuffer()
	{
		offscreenPass.colourFormat = m_renderer->GetSwapChain().imageFormat();
		offscreenPass.depthFormat = m_renderer->GetSwapChain().FindDepthFormat();

		offscreenPass.width = 2048;
		offscreenPass.height = 2048;

		// Depth **********************************************************************************
		VkImageCreateInfo depthImage{};
		depthImage.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depthImage.imageType = VK_IMAGE_TYPE_2D;
		depthImage.extent.width = offscreenPass.width;
		depthImage.extent.height = offscreenPass.height;
		depthImage.extent.depth = 1;
		depthImage.mipLevels = 1;
		depthImage.arrayLayers = 1;
		depthImage.samples = VK_SAMPLE_COUNT_1_BIT;
		depthImage.tiling = VK_IMAGE_TILING_OPTIMAL;
		depthImage.format = offscreenPass.depthFormat;																// Depth stencil attachment
		depthImage.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;		// We will sample directly from the depth attachment for the shadow mapping
		SAFE_RUN_VULKAN_FUNC(vkCreateImage(*m_device, &depthImage, nullptr, &offscreenPass.depth.image), "");

		VkMemoryAllocateInfo depthMemAlloc{};
		depthMemAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		VkMemoryRequirements depthMemReqs;
		vkGetImageMemoryRequirements(*m_device, offscreenPass.depth.image, &depthMemReqs);
		depthMemAlloc.allocationSize = depthMemReqs.size;
		depthMemAlloc.memoryTypeIndex = m_device->FindMemoryType(depthMemReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		SAFE_RUN_VULKAN_FUNC(vkAllocateMemory(*m_device, &depthMemAlloc, nullptr, &offscreenPass.depth.mem), "");
		SAFE_RUN_VULKAN_FUNC(vkBindImageMemory(*m_device, offscreenPass.depth.image, offscreenPass.depth.mem, 0), "");

		VkImageViewCreateInfo depthStencilView{};
		depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthStencilView.format = offscreenPass.depthFormat;
		depthStencilView.subresourceRange = {};
		depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		depthStencilView.subresourceRange.baseMipLevel = 0;
		depthStencilView.subresourceRange.levelCount = 1;
		depthStencilView.subresourceRange.baseArrayLayer = 0;
		depthStencilView.subresourceRange.layerCount = 1;
		depthStencilView.image = offscreenPass.depth.image;
		SAFE_RUN_VULKAN_FUNC(vkCreateImageView(*m_device, &depthStencilView, nullptr, &offscreenPass.depth.view), "");

		// Colour *********************************************************************************
		VkImageCreateInfo colourImage{};
		colourImage.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		colourImage.imageType = VK_IMAGE_TYPE_2D;
		colourImage.extent.width = offscreenPass.width;
		colourImage.extent.height = offscreenPass.height;
		colourImage.extent.depth = 1;
		colourImage.mipLevels = 1;
		colourImage.arrayLayers = 1;
		colourImage.samples = VK_SAMPLE_COUNT_1_BIT;
		colourImage.tiling = VK_IMAGE_TILING_OPTIMAL;
		colourImage.format = offscreenPass.colourFormat;																// Depth stencil attachment
		colourImage.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;		// We will sample directly from the depth attachment for the shadow mapping
		SAFE_RUN_VULKAN_FUNC(vkCreateImage(*m_device, &colourImage, nullptr, &offscreenPass.colour.image),"");

		VkMemoryAllocateInfo colourMemAlloc{};
		colourMemAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		VkMemoryRequirements colourMemReqs;
		vkGetImageMemoryRequirements(*m_device, offscreenPass.colour.image, &colourMemReqs);
		colourMemAlloc.allocationSize = colourMemReqs.size;
		colourMemAlloc.memoryTypeIndex = m_device->FindMemoryType(colourMemReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		SAFE_RUN_VULKAN_FUNC(vkAllocateMemory(*m_device, &colourMemAlloc, nullptr, &offscreenPass.colour.mem),"");
		SAFE_RUN_VULKAN_FUNC(vkBindImageMemory(*m_device, offscreenPass.colour.image, offscreenPass.colour.mem, 0),"");

		VkImageViewCreateInfo colourView{};
		colourView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colourView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colourView.format = offscreenPass.colourFormat;
		colourView.subresourceRange = {};
		colourView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colourView.subresourceRange.baseMipLevel = 0;
		colourView.subresourceRange.levelCount = 1;
		colourView.subresourceRange.baseArrayLayer = 0;
		colourView.subresourceRange.layerCount = 1;
		colourView.image = offscreenPass.colour.image;
		SAFE_RUN_VULKAN_FUNC(vkCreateImageView(*m_device, &colourView, nullptr, &offscreenPass.colour.view),"");

		// Create sampler to sample from to depth attachment
		// Used to sample in the fragment shader for shadowed rendering
		VkSamplerCreateInfo sampler{};
		sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler.magFilter = VK_FILTER_LINEAR;
		sampler.minFilter = VK_FILTER_LINEAR;

		sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler.addressModeV = sampler.addressModeU;
		sampler.addressModeW = sampler.addressModeU;

		sampler.anisotropyEnable = VK_TRUE;
		sampler.maxAnisotropy = m_device->properties.limits.maxSamplerAnisotropy;

		sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		sampler.unnormalizedCoordinates = VK_FALSE;

		sampler.compareEnable = VK_FALSE;
		sampler.compareOp = VK_COMPARE_OP_ALWAYS;

		sampler.mipLodBias = 0.0f;
		sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler.minLod = 0.0f;
		sampler.maxLod = 1.0f;

		SAFE_RUN_VULKAN_FUNC(vkCreateSampler(*m_device, &sampler, nullptr, &offscreenPass.depthSampler), "");

		prepareOffscreenRenderpass();

		std::array<VkImageView, 2> views { offscreenPass.colour.view, offscreenPass.depth.view };

		// Create frame buffer
		VkFramebufferCreateInfo fbufCreateInfo{};
		fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbufCreateInfo.renderPass = offscreenPass.renderPass;
		fbufCreateInfo.attachmentCount = static_cast<uint32_t>(views.size());
		fbufCreateInfo.pAttachments = views.data();
		fbufCreateInfo.width = offscreenPass.width;
		fbufCreateInfo.height = offscreenPass.height;
		fbufCreateInfo.layers = 1;

		SAFE_RUN_VULKAN_FUNC(vkCreateFramebuffer(*m_device, &fbufCreateInfo, nullptr, &offscreenPass.frameBuffer),"");
	}

	void Application::buildPipeline()
	{
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ m_globalSetLayout->GetDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineCreateInfo.pushConstantRangeCount = 0;
		pipelineCreateInfo.pPushConstantRanges = nullptr;

		SAFE_RUN_VULKAN_FUNC(vkCreatePipelineLayout(*m_device, &pipelineCreateInfo, nullptr, &m_pipelineLayout), "failed to create pipelineLayout");

		GOL_CORE_ASSERT(m_pipelineLayout != nullptr, "cannot create pipeline before pipeline layout");

		PipelineConfigInfo configInfo;
		Pipeline::DefaultPipelineConfigInfo(configInfo);
		configInfo.renderPass = Application::Get().GetRenderer().GetSwapChainRenderPass();
		configInfo.pipelineLayout = m_pipelineLayout;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;

		ShaderPaths sPaths{};

		sPaths.vert_filepath = "shaders/frameBufferTest/frameBufferTest.vert.spv";
		sPaths.frag_filepath = "shaders/frameBufferTest/frameBufferTest.frag.spv";

		testPipeline = std::make_unique<Pipeline>(
			*m_device,
			sPaths,
			configInfo,
			true
			);
	}

	void Application::OnEvent(Event& e)
	{
		std::unique_lock<std::mutex> lock{m_eventMutex};

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
#include "golpch.h"
#include "Renderer.h"
#include <GLFW/glfw3.h>

namespace golem
{

	Renderer::Renderer(Window& _window, Device& _device)
		: m_window(_window), m_device(_device)
	{
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	Renderer::~Renderer()
	{
		FreeCommandBuffers();
	}

	VkCommandBuffer Renderer::BeginFrame()
	{
		GOL_CORE_ASSERT(!m_isFrameStarted, "Another frame is already in progress, EndFrame() has not yet been called for the previous frame");

		auto result = m_swapChain->AcquireNextImage(&m_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("failed to acquire next swapchain image");

		m_isFrameStarted = true;

		auto commandBuffer = GetCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		SAFE_RUN_VULKAN_FUNC(vkBeginCommandBuffer(commandBuffer, &beginInfo), "failed to being recording command buffer");

		return commandBuffer;
	}

	void Renderer::EndFrame()
	{
		GOL_PROFILE_FUNCTION();

		assert(m_isFrameStarted && "cant end frame while frame is not in progress. use BeginFrame() to start new frame");

		auto commandBuffer = GetCurrentCommandBuffer();

		SAFE_RUN_VULKAN_FUNC(vkEndCommandBuffer(commandBuffer), "failed to end command buffer");

		auto result = m_swapChain->SubmitCommandBuffers(&commandBuffer, &m_currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.WasResized())
		{
			m_window.ResetWindowResizedFlag();
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("failed to submit command buffer");

		m_isFrameStarted = false;

		m_currentFrameIndex = (m_currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		GOL_CORE_ASSERT(m_isFrameStarted, "can't begin render pass while frame is not in progress. use BeginFrame() to start new frame");
		GOL_CORE_ASSERT(commandBuffer == GetCurrentCommandBuffer(), "can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_swapChain->renderPass();
		renderPassInfo.framebuffer = m_swapChain->frameBuffer(m_currentImageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = m_swapChain->extent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f,0.1f,0.1f,1.0f };
		clearValues[1].depthStencil = { 1, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_swapChain->extent().width);
		viewport.height = static_cast<float>(m_swapChain->extent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_swapChain->extent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		GOL_CORE_ASSERT(m_isFrameStarted, "can't end render pass while frame is not in progress. use BeginFrame() to start new frame");
		GOL_CORE_ASSERT(commandBuffer == GetCurrentCommandBuffer(), "can't end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}

	void Renderer::CreateCommandBuffers()
	{
		m_commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_device.commandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

		SAFE_RUN_VULKAN_FUNC(vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()), "failed to allocate command buffers");
	}

	void Renderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_device, m_device.commandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
		m_commandBuffers.clear();
	}

	void Renderer::RecreateSwapChain()
	{
		auto extent = m_window.GetExtent();

		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_window.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_device);

		if (m_swapChain == nullptr)
			m_swapChain = std::make_unique<SwapChain>(m_device, extent);
		else
		{
			std::shared_ptr<SwapChain> oldSwapChain = std::move(m_swapChain);

			m_swapChain = std::make_unique<SwapChain>(m_device, extent, oldSwapChain);
			
			GOL_CORE_ASSERT(oldSwapChain->IsSwapChainFormatCompatible(*m_swapChain.get()), "Swap chain image format has changed");
		}

		// TODO: should really check if the pipelines needs re-created
	}

}
#pragma once

#include "Golem/Window.h"
#include "Golem/Render/Device.h"
#include "Golem/Render/SwapChain.h"

namespace golem
{

	class GOLEM_API Renderer
	{
		// Members ********************************************************************************
	public:

	private:
		Window& m_window;
		Device& m_device;
		std::unique_ptr<SwapChain> m_swapChain;

		std::vector<VkCommandBuffer> m_commandBuffers;

		uint32_t m_currentImageIndex = 0;
		int m_currentFrameIndex = 0;
		bool m_isFrameStarted = false;

		// Methods ********************************************************************************
	public:
		Renderer(Window& _window, Device& _device);
		~Renderer();

		REMOVE_COPY_CONSTRUCTOR(Renderer);

		VkCommandBuffer BeginFrame();
		void EndFrame();

		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

		inline VkRenderPass GetSwapChainRenderPass() const { return m_swapChain->renderPass(); }
		inline float GetAspectRatio() const { return m_swapChain->extentAspectRatio(); }
		inline bool IsFrameInProgress() const { return m_isFrameStarted; }
		inline uint32_t GetImageCount() const { return m_swapChain->imageCount(); }


		inline int GetFrameIndex() const
		{
			assert(m_isFrameStarted && "Cannot get frame index when frame not in progress. have you begun this frame using BeginFrame()?");
			return m_currentFrameIndex;
		}

		inline VkCommandBuffer GetCurrentCommandBuffer() const
		{
			assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress. have you begun this frame using BeginFrame()?");
			return m_commandBuffers[m_currentFrameIndex];
		}

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();

		void RecreateSwapChain();
	};
}
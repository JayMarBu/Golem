#include "golpch.h"
#include "RenderTexture.h"
#include "../Application.h"

namespace golem
{

	RenderTexture::RenderTexture(uint32_t width, uint32_t height, VkRenderPass renderPass)
		: m_width{width}, m_height{height}
	{
		Init(renderPass);
	}

	RenderTexture::~RenderTexture()
	{
		auto& device = Application::Get().GetDevice();

		// colour attachment
		vkDestroyImageView(device, m_colour.view, nullptr);
		vkDestroyImage(device, m_colour.image, nullptr);
		vkFreeMemory(device, m_colour.mem, nullptr);

		// Depth attachment
		vkDestroyImageView(device, m_depth.view, nullptr);
		vkDestroyImage(device, m_depth.image, nullptr);
		vkFreeMemory(device, m_depth.mem, nullptr);

		vkDestroyFramebuffer(device, m_frameBuffer, nullptr);
	}

	void RenderTexture::Init(VkRenderPass renderPass)
	{
		auto& device = Application::Get().GetDevice(); 

		CreateColourAttachment(device);
		CreateDepthStencilAttachment(device);
		CreateSampler(device);
		CreateFrameBuffer(device, renderPass);
		CreateDescriptorSets(device);
	}

	void RenderTexture::CreateColourAttachment(Device& device)
	{
		auto colourFormat = Application::Get().GetRenderer().GetSwapChain().imageFormat();

		// create colour attachment image
		VkImageCreateInfo colourImage{};
		colourImage.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		colourImage.imageType = VK_IMAGE_TYPE_2D;
		colourImage.extent.width = m_width;
		colourImage.extent.height = m_height;
		colourImage.extent.depth = 1;
		colourImage.mipLevels = 1;
		colourImage.arrayLayers = 1;
		colourImage.samples = VK_SAMPLE_COUNT_1_BIT;
		colourImage.tiling = VK_IMAGE_TILING_OPTIMAL;
		colourImage.format = colourFormat;
		colourImage.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		SAFE_RUN_VULKAN_FUNC(vkCreateImage(device, &colourImage, nullptr, &m_colour.image), "failed to create colour attachment image");

		// allocate image memory
		VkMemoryAllocateInfo colourMemAlloc{};
		colourMemAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		VkMemoryRequirements colourMemReqs;
		vkGetImageMemoryRequirements(device, m_colour.image, &colourMemReqs);
		colourMemAlloc.allocationSize = colourMemReqs.size;
		colourMemAlloc.memoryTypeIndex = device.FindMemoryType(colourMemReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		SAFE_RUN_VULKAN_FUNC(vkAllocateMemory(device, &colourMemAlloc, nullptr, &m_colour.mem), "Failed to allocate image memory");
		SAFE_RUN_VULKAN_FUNC(vkBindImageMemory(device, m_colour.image, m_colour.mem, 0), "Failed to bind image memory");

		// create image view
		VkImageViewCreateInfo colourView{};
		colourView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		colourView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colourView.format = colourFormat;
		colourView.subresourceRange = {};
		colourView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colourView.subresourceRange.baseMipLevel = 0;
		colourView.subresourceRange.levelCount = 1;
		colourView.subresourceRange.baseArrayLayer = 0;
		colourView.subresourceRange.layerCount = 1;
		colourView.image = m_colour.image;
		SAFE_RUN_VULKAN_FUNC(vkCreateImageView(device, &colourView, nullptr, &m_colour.view), "Failed to create image view");
	}

	void RenderTexture::CreateDepthStencilAttachment(Device& device)
	{
		auto depthFormat = Application::Get().GetRenderer().GetSwapChain().FindDepthFormat();

		VkImageCreateInfo depthImage{};
		depthImage.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depthImage.imageType = VK_IMAGE_TYPE_2D;
		depthImage.extent.width = m_width;
		depthImage.extent.height = m_height;
		depthImage.extent.depth = 1;
		depthImage.mipLevels = 1;
		depthImage.arrayLayers = 1;
		depthImage.samples = VK_SAMPLE_COUNT_1_BIT;
		depthImage.tiling = VK_IMAGE_TILING_OPTIMAL;
		depthImage.format =	depthFormat;
		depthImage.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		SAFE_RUN_VULKAN_FUNC(vkCreateImage(device, &depthImage, nullptr, &m_depth.image), "");

		VkMemoryAllocateInfo depthMemAlloc{};
		depthMemAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		VkMemoryRequirements depthMemReqs;
		vkGetImageMemoryRequirements(device, m_depth.image, &depthMemReqs);
		depthMemAlloc.allocationSize = depthMemReqs.size;
		depthMemAlloc.memoryTypeIndex = device.FindMemoryType(depthMemReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		SAFE_RUN_VULKAN_FUNC(vkAllocateMemory(device, &depthMemAlloc, nullptr, &m_depth.mem), "");
		SAFE_RUN_VULKAN_FUNC(vkBindImageMemory(device, m_depth.image, m_depth.mem, 0), "");

		VkImageViewCreateInfo depthStencilView{};
		depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depthStencilView.format = depthFormat;
		depthStencilView.subresourceRange = {};
		depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		depthStencilView.subresourceRange.baseMipLevel = 0;
		depthStencilView.subresourceRange.levelCount = 1;
		depthStencilView.subresourceRange.baseArrayLayer = 0;
		depthStencilView.subresourceRange.layerCount = 1;
		depthStencilView.image = m_depth.image;
		SAFE_RUN_VULKAN_FUNC(vkCreateImageView(device, &depthStencilView, nullptr, &m_depth.view), "");
	}

	void RenderTexture::CreateSampler(Device& device)
	{
		/*VkSamplerCreateInfo sampler{};
		sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler.magFilter = VK_FILTER_LINEAR;
		sampler.minFilter = VK_FILTER_LINEAR;

		sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		sampler.addressModeV = sampler.addressModeU;
		sampler.addressModeW = sampler.addressModeU;

		sampler.anisotropyEnable = VK_TRUE;
		sampler.maxAnisotropy = device.properties.limits.maxSamplerAnisotropy;

		sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		sampler.unnormalizedCoordinates = VK_FALSE;

		sampler.compareEnable = VK_FALSE;
		sampler.compareOp = VK_COMPARE_OP_ALWAYS;

		sampler.mipLodBias = 0.0f;
		sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler.minLod = 0.0f;
		sampler.maxLod = 1.0f;

		SAFE_RUN_VULKAN_FUNC(vkCreateSampler(device, &sampler, nullptr, &m_sampler), "");*/

		m_sampler = Ref<Sampler>(new Sampler(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE));
	}

	void RenderTexture::CreateFrameBuffer(Device& device, VkRenderPass renderPass)
	{
		std::array<VkImageView, 2> views{ m_colour.view, m_depth.view };

		// Create frame buffer
		VkFramebufferCreateInfo fbufCreateInfo{};
		fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbufCreateInfo.renderPass = renderPass;
		fbufCreateInfo.attachmentCount = static_cast<uint32_t>(views.size());
		fbufCreateInfo.pAttachments = views.data();
		fbufCreateInfo.width = m_width;
		fbufCreateInfo.height = m_height;
		fbufCreateInfo.layers = 1;

		SAFE_RUN_VULKAN_FUNC(vkCreateFramebuffer(device, &fbufCreateInfo, nullptr, &m_frameBuffer), "");
	}

	void RenderTexture::CreateDescriptorSets(Device& device)
	{
		m_descriptorPool = golem::DescriptorPool::Builder(device)
			.SetMaxSets(golem::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, golem::SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		m_descriptorSetLayout = golem::DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();

		m_descriptorSets.resize(golem::SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < m_descriptorSets.size(); i++)
		{
			VkDescriptorImageInfo descriptorImageInfo{};
			descriptorImageInfo.sampler = m_sampler->getSampler();
			descriptorImageInfo.imageView = GetColourAttachment()->view;
			descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			golem::DescriptorWriter(*m_descriptorSetLayout, *m_descriptorPool)
				.WriteImage(0, &descriptorImageInfo)
				.Build(m_descriptorSets[i]);
		}
	}

}
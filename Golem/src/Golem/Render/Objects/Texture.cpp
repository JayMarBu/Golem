#include "golpch.h"
#include "Texture.h"
#include "Golem/Application.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Golem/Render/Buffer.h"

namespace golem
{
	Texture::Texture(std::string filePath, VkFormat imageFormat, VkImageTiling tiling, VkBufferUsageFlags usage)
		: m_imageFormat(imageFormat), m_tiling(tiling), m_usageFlags(usage)
	{
		CreateTextureImage(filePath);
		CreateTextureImageView();
	}

	Texture::~Texture()
	{
		auto& device = Application::Get().GetDevice();

		vkDestroyImageView(device, m_imageView, nullptr);

		vkDestroyImage(device, m_textureImage, nullptr);
		vkFreeMemory(device, m_textureImageMemory, nullptr);
	}

	void Texture::CreateTextureImage(std::string filePath)
	{
		auto& device = Application::Get().GetDevice();

		int texChannels;
		stbi_uc* pixels = stbi_load(filePath.c_str(), (int*)&m_width, (int*)&m_height, &texChannels, STBI_rgb_alpha);

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		Buffer stagingBuffer(
			device,
			4,
			m_width * m_height,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer(pixels);

		stbi_image_free(pixels);

		CreateTexture(
			m_width, m_height,
			m_imageFormat,//VK_FORMAT_R8G8B8A8_SRGB,
			m_tiling, // VK_IMAGE_TILING_OPTIMAL,
			m_usageFlags,//VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_textureImage, m_textureImageMemory
		);

		TransitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		CopyBufferToImage(stagingBuffer.GetBuffer(), m_textureImage, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
		TransitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void Texture::CreateTexture(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		auto& device = Application::Get().GetDevice();

		// create image info struct
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// create image
		SAFE_RUN_VULKAN_FUNC(vkCreateImage(device, &imageInfo, nullptr, &image), "failed to create image!");

		// get image memory requirements
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		// create memory allocation info struct
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, properties);

		// allocate image memory
		SAFE_RUN_VULKAN_FUNC(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory), "failed to allocate image memory!");

		// bind image to memory location
		vkBindImageMemory(device, image, imageMemory, 0);
	}

	void Texture::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		auto& device = Application::Get().GetDevice();

		VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		device.EndSingleTimeCommands(commandBuffer);
	}

	void Texture::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		auto& device = Application::Get().GetDevice();

		VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		device.EndSingleTimeCommands(commandBuffer);
	}

	void Texture::CreateTextureImageView()
	{
		m_imageView = CreateImageView(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB);
	}

	VkImageView Texture::CreateImageView(VkImage image, VkFormat format)
	{
		auto& device = Application::Get().GetDevice();

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;

		SAFE_RUN_VULKAN_FUNC(vkCreateImageView(device, &viewInfo, nullptr, &imageView), "failed to create texture image view!");

		return imageView;
	}

}
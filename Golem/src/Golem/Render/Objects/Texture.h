#pragma once

namespace golem
{
	class Texture
	{
		// Members ********************************************************************************
	private:
		VkImage m_textureImage;
		VkDeviceMemory m_textureImageMemory;
		VkImageView m_imageView;
		uint32_t m_width;
		uint32_t m_height;

		VkDeviceSize m_texelSize;
		uint32_t m_texelCount;

		VkFormat m_imageFormat;
		VkImageTiling m_tiling;
		VkBufferUsageFlags m_usageFlags;

		// Methods ********************************************************************************
	public:
		Texture(
			std::string filePath,
			VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
			VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
			VkBufferUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
		);

		~Texture();

		REMOVE_COPY_CONSTRUCTOR(Texture);

		inline VkImageView imageView() const { return m_imageView; }
	private:
		void CreateTextureImage(std::string filePath);

		void CreateTexture(
			uint32_t width,
			uint32_t height,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& imageMemory
		);

		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

		void CreateTextureImageView();

		VkImageView CreateImageView(VkImage image, VkFormat format);
	};
}
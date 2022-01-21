#pragma once
#include "../Render/Device.h"
#include "../Render/Objects/Texture.h"

namespace golem
{
	class TextureManager
	{
		// Members ********************************************************************************
	private:
		Device& m_device;

		std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

		// Methods ********************************************************************************
	public:
		TextureManager(Device& device);
		~TextureManager();

		inline VkDescriptorImageInfo ImageInfo(VkSampler sampler, const std::string texture_id) { return VkDescriptorImageInfo{ sampler, GetTexture(texture_id), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }; }

		void LoadTexture(const std::string& id, const std::string& filepath);
		VkImageView GetTexture(const std::string& id);

	private:

		void CreateDefaultTexture();
		bool DoesFileExist(const std::string& filepath);
	};
}
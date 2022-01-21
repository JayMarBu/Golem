#include "golpch.h"
#include "TextureManager.h"

namespace golem
{
	TextureManager::TextureManager(Device& device)
		: m_device(device)
	{
		CreateDefaultTexture();
	}

	TextureManager::~TextureManager()
	{
		//vkDestroySampler(m_device, m_sampler, nullptr);
		//vkDestroyImageView(m_device, m_textureImageView, nullptr);

		//vkDestroyImage(m_device, m_textureImage, nullptr);
		//vkFreeMemory(m_device, m_textureImageMemory, nullptr);
	}

	void TextureManager::LoadTexture(const std::string& id, const std::string& filepath)
	{
		if (MAP_CONTAINS(m_textures, id))
			return;

		if (!DoesFileExist(filepath))
			throw std::runtime_error("failed to load texture! [" + filepath + "] does not exist!");

		m_textures.insert(std::make_pair(id, std::make_unique<Texture>(filepath)));
	}

	VkImageView TextureManager::GetTexture(const std::string& id)
	{
		if (MAP_CONTAINS(m_textures, id))
		{
			return m_textures[id]->imageView();
		}
		else
		{
			m_textures["default"]->imageView();
		}
	}

	void TextureManager::CreateDefaultTexture()
	{
		m_textures.insert(std::make_pair("default", std::make_unique<Texture>("media/textures/DefaultDiffuse.png")));
	}

	bool TextureManager::DoesFileExist(const std::string& filepath)
	{
		std::ifstream infile(filepath);
		return infile.good();
	}
}
#include "golpch.h"
#include "Sampler.h"
#include "Golem/Application.h"

namespace golem
{

	Sampler::Sampler(AddressMode addressMode /*= { VK_SAMPLER_ADDRESS_MODE_REPEAT }*/, VkBorderColor borderColour /*= VK_BORDER_COLOR_INT_OPAQUE_BLACK*/, VkBool32 anisotropyEnable /*= VK_TRUE*/, MipMapInfo mipmapInfo /*= MipMapInfo() */)
	{
		auto& device = Application::Get().GetDevice();

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		samplerInfo.addressModeU = addressMode.u;
		samplerInfo.addressModeV = addressMode.v;
		samplerInfo.addressModeW = addressMode.w;

		samplerInfo.anisotropyEnable = anisotropyEnable;
		samplerInfo.maxAnisotropy = device.properties.limits.maxSamplerAnisotropy;

		samplerInfo.borderColor = borderColour;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = mipmapInfo.mipmapMode;
		samplerInfo.mipLodBias = mipmapInfo.mipLodBias;
		samplerInfo.minLod = mipmapInfo.minLod;
		samplerInfo.maxLod = mipmapInfo.maxLod;

		SAFE_RUN_VULKAN_FUNC(vkCreateSampler(device, &samplerInfo, nullptr, &m_sampler), "failed to create texture sampler!");
	}

	Sampler::~Sampler()
	{
		auto& device = Application::Get().GetDevice();
		vkDestroySampler(device, m_sampler, nullptr);
	}

}
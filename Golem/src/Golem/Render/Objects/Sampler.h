#pragma once

namespace golem
{
	class Sampler
	{
		// Members ********************************************************************************

	public:
		struct MipMapInfo
		{
			VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			float mipLodBias = 0.0f;
			float minLod = 0.0f;
			float maxLod = 0.0f;
		};

		struct AddressMode
		{
			VkSamplerAddressMode u;
			VkSamplerAddressMode v;
			VkSamplerAddressMode w;

			AddressMode() = default;
			AddressMode(VkSamplerAddressMode addressMode)
				: u(addressMode), v(addressMode), w(addressMode) {}
		};
	private:
		VkSampler m_sampler;
		// Methods ********************************************************************************
	public:

		Sampler(
			AddressMode addressMode = { VK_SAMPLER_ADDRESS_MODE_REPEAT },
			VkBorderColor borderColour = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			VkBool32 anisotropyEnable = VK_TRUE,
			MipMapInfo mipmapInfo = MipMapInfo()
		);
		~Sampler();

		REMOVE_COPY_CONSTRUCTOR(Sampler);

		inline VkSampler getSampler() const { return m_sampler; }

		operator VkSampler() const { return getSampler(); }
	};
}
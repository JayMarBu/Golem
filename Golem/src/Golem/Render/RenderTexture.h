#pragma once
#include "Golem/Render/Objects/Sampler.h"
#include "Device.h"

#include "Golem/Render/Buffer.h"
#include "Golem/Render/Descriptors.h"

namespace golem
{
	class RenderTexture
	{
		// Members ********************************************************************************
	public:
		
		struct FrameBufferAttachment
		{
			VkImage image;
			VkDeviceMemory mem;
			VkImageView view;
		};

	private:

		uint32_t m_width, m_height;
		VkFramebuffer m_frameBuffer;
		FrameBufferAttachment m_depth;
		FrameBufferAttachment m_colour;
		Ref<Sampler> m_sampler;

		Scope<DescriptorPool> m_descriptorPool;
		Scope<DescriptorSetLayout> m_descriptorSetLayout;
		std::vector<VkDescriptorSet> m_descriptorSets;

		// Methods ********************************************************************************
	public:
		RenderTexture(uint32_t width, uint32_t height, VkRenderPass renderPass);
		virtual ~RenderTexture();

		Ref<Sampler> GetSampler() const {return m_sampler;}
		uint32_t GetWidth() const {return m_width;}
		uint32_t GetHeight() const {return m_height;}

		FrameBufferAttachment* GetColourAttachment() {return &m_colour;} 
		FrameBufferAttachment* GetDepthStencilAttachment() {return &m_depth;} 

		VkFramebuffer GetFrameBuffer() const { return m_frameBuffer; }

		VkDescriptorSet GetDescriptorSet(uint32_t i) const { return m_descriptorSets[i];}
		VkDescriptorSetLayout GetDescriptorSetLayout() const {return m_descriptorSetLayout->GetDescriptorSetLayout();}

		void Resize(uint32_t width, uint32_t height, VkRenderPass renderPass);

	private:
		void Init(VkRenderPass renderPass);

		void CreateColourAttachment(Device& device);
		void CreateDepthStencilAttachment(Device& device);
		void CreateSampler(Device& device);
		void CreateFrameBuffer(Device& device, VkRenderPass renderPass);
		void CreateDescriptorSets(Device& device);

		void Cleanup();
	};
}
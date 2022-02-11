#pragma once
#include "RenderSystemBase.h"
#include "../Descriptors.h"
#include "../Buffer.h"
#include "../FrameInfo.h"

namespace golem
{
	class InfiniGridRenderSystem : public RenderSystemBase
	{
		// Members ********************************************************************************
	public:
		bool render = false;
	private:
		std::unique_ptr<DescriptorPool> m_pool;
		std::unique_ptr<DescriptorSetLayout> m_setLayout;
		std::vector<VkDescriptorSet> m_descriptorSets;

		std::vector<std::unique_ptr<Buffer>> m_UBObuffers;
		// Methods ********************************************************************************
	public:
		InfiniGridRenderSystem(Device& device);
		virtual ~InfiniGridRenderSystem() {}

		REMOVE_COPY_CONSTRUCTOR(InfiniGridRenderSystem);
		void Render(FrameInfo& fInfo);
	private:
		virtual void CreatePipelineLayout(
			VkDescriptorSetLayout descriptorSet,
			uint32_t pushConstantSize,
			VkShaderStageFlags pushConstantShaderStages);
	};
}
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
		Scope<DescriptorPool> m_pool;
		Scope<DescriptorSetLayout> m_setLayout;
		std::vector<VkDescriptorSet> m_descriptorSets;

		std::vector<Scope<Buffer>> m_UBObuffers;
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

		void CreateDescriptors();

		void CreateMainPipeline();
	};
}
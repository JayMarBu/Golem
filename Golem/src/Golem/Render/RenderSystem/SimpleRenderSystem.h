#pragma once
#include "../Device.h"
#include "../FrameInfo.h"
#include "Golem/Temp/TempGameObject.h"
#include "RenderSystemBase.h"

namespace golem
{
	class SimpleRenderSystem : public RenderSystemBase
	{
		// Members ********************************************************************************
	public:
		
	private:
		// Methods ********************************************************************************
	public:
		SimpleRenderSystem(
			Device& _device,
			VkDescriptorSetLayout descriptorSet
		);
		virtual ~SimpleRenderSystem(){}

		REMOVE_COPY_CONSTRUCTOR(SimpleRenderSystem);

		void RenderGameObjects(FrameInfo& fInfo, std::vector<TempGameObject>& gameObjects);

		virtual void CreatePipelineLayout(
			VkDescriptorSetLayout descriptorSet,
			uint32_t pushConstantSize,
			VkShaderStageFlags pushConstantShaderStages) override;
	};
}
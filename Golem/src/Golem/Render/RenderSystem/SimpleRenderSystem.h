#pragma once
#include "../Device.h"
#include "../FrameInfo.h"
#include "RenderSystemBase.h"
#include "Golem/ECS/Scene.h"

namespace golem
{
	struct FrameInfo;

	class SimpleRenderSystem : public RenderSystemBase
	{
		// Members ********************************************************************************
	public:
		
	private:
		System m_renderSystem;

		// Methods ********************************************************************************
	public:
		SimpleRenderSystem(
			Device& _device,
			VkDescriptorSetLayout descriptorSet
		);
		virtual ~SimpleRenderSystem(){}

		REMOVE_COPY_CONSTRUCTOR(SimpleRenderSystem);

		void RenderGameObjects(FrameInfo& fInfo);

		virtual void CreatePipelineLayout(
			VkDescriptorSetLayout descriptorSet,
			uint32_t pushConstantSize,
			VkShaderStageFlags pushConstantShaderStages) override;

		void RenderGameObject(GameObject gObj, FrameInfo& fInfo);
	};
}
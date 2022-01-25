#pragma once
#include "RenderSystemBase.h"
#include "../FrameInfo.h"
#include "Golem/Temp/TempGameObject.h"

namespace golem
{
	class PointLightRenderSystem : public RenderSystemBase
	{
		// Members ********************************************************************************
	public:
		
	private:

		// Methods ********************************************************************************
	public:
		PointLightRenderSystem(
			Device& _device,
			VkRenderPass _renderPass,
			VkDescriptorSetLayout descriptorSet
		);
		virtual ~PointLightRenderSystem();


		REMOVE_COPY_CONSTRUCTOR(PointLightRenderSystem);

		void Update(FrameInfo& fInfo, GlobalUBO& ubo, std::vector<TempGameObject>& gameObjects);
		void Render(FrameInfo& fInfo, std::vector<TempGameObject>& gameObjects);
	protected:
		
		virtual void CreatePipelineLayout(
			VkDescriptorSetLayout descriptorSet,
			uint32_t pushConstantSize,
			VkShaderStageFlags pushConstantShaderStages = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
			);
	};
}
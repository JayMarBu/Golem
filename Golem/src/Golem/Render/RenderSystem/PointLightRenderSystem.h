#pragma once
#include "RenderSystemBase.h"
#include "../FrameInfo.h"
#include "Golem/ECS/Scene.h"
#include "Golem/ECS/GameObject.h"

namespace golem
{
	class PointLightRenderSystem : public RenderSystemBase
	{
		// Members ********************************************************************************
	public:
		
	private:
		System m_renderSystem;
		System m_updateSystem;

		// Methods ********************************************************************************
	public:
		PointLightRenderSystem(
			Device& _device,
			VkDescriptorSetLayout descriptorSet
		);
		virtual ~PointLightRenderSystem(){}


		REMOVE_COPY_CONSTRUCTOR(PointLightRenderSystem);

		void Update(FrameInfo& fInfo, GlobalUBO& ubo);
		void Render(FrameInfo& fInfo);
	protected:
		
		virtual void CreatePipelineLayout(
			VkDescriptorSetLayout descriptorSet,
			uint32_t pushConstantSize,
			VkShaderStageFlags pushConstantShaderStages = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
			);

		void RenderGameObject(GameObject gObj, FrameInfo& fInfo);
		void UpdateGameObject(GameObject gObj, FrameInfo& fInfo);
	};
}
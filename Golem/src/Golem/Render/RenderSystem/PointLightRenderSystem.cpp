#include "golpch.h"
#include "PointLightRenderSystem.h"

namespace golem
{
	struct PointLightPushConstant
	{
		glm::vec4 pos{};
		glm::vec4 colour{};
		float radius;
	};

	PointLightRenderSystem::PointLightRenderSystem(Device& _device, VkDescriptorSetLayout descriptorSet)
		: RenderSystemBase(_device)
	{
		CreatePipelineLayout(descriptorSet, sizeof(PointLightPushConstant));

		m_shaderPaths.vert_filepath = "shaders/point_light_shader/point_light_shader.vert";
		m_shaderPaths.frag_filepath = "shaders/point_light_shader/point_light_shader.frag";

		Pipeline::DefaultPipelineConfigInfo(m_configInfo);

		m_configInfo.vertexAttribDesc.clear();
		m_configInfo.vertexBindingDesc.clear();

		CreatePipeline();
	}

	void PointLightRenderSystem::Update(FrameInfo& fInfo, GlobalUBO& ubo, std::vector<TempGameObject>& gameObjects)
	{
		int lightIndex = 0;
		for (int i = 0; i < gameObjects.size(); i++)
		{
			if(gameObjects[i].pointLight == nullptr)
				continue;

			ubo.pointLights[lightIndex].pos = glm::vec4(gameObjects[i].transform.translation, 1.0f);
			ubo.pointLights[lightIndex].lightColour = glm::vec4(gameObjects[i].colour, gameObjects[i].pointLight->lightIntensity);

			lightIndex++;
		}
		ubo.numLights = lightIndex;
	}

	void PointLightRenderSystem::Render(FrameInfo& fInfo, std::vector<TempGameObject>& gameObjects)
	{
		m_pipeline->Bind(fInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			fInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0, 1,
			&fInfo.globalDescriptorSet,
			0, nullptr
		);

		int lightIndex = 0;
		for (int i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i].pointLight == nullptr)
				continue;
			
			PointLightPushConstant pushData{};

			pushData.pos = glm::vec4(gameObjects[i].transform.translation,1.0f);
			pushData.colour = glm::vec4(gameObjects[i].colour, 1.0f);
			pushData.radius = gameObjects[i].transform.scale.x;

			vkCmdPushConstants(
				fInfo.commandBuffer,
				m_pipelineLayout,
				VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(PointLightPushConstant),
				&pushData
			);

			vkCmdDraw(fInfo.commandBuffer, 6, 1, 0, 0);
			lightIndex++;
		}
	}

	void PointLightRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout descriptorSet, uint32_t pushConstantSize, VkShaderStageFlags pushConstantShaderStages /*= VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT */)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = pushConstantShaderStages;
		pushConstantRange.offset = 0;
		pushConstantRange.size = pushConstantSize;

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ descriptorSet };

		VkPipelineLayoutCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineCreateInfo.pushConstantRangeCount = 1;
		pipelineCreateInfo.pPushConstantRanges = &pushConstantRange;

		SAFE_RUN_VULKAN_FUNC(vkCreatePipelineLayout(m_device, &pipelineCreateInfo, nullptr, &m_pipelineLayout), "failed to create pipelineLayout");
	}

}
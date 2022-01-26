#include "golpch.h"
#include "SimpleRenderSystem.h"

namespace golem
{
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	SimpleRenderSystem::SimpleRenderSystem(Device& _device, VkDescriptorSetLayout descriptorSet)
		: RenderSystemBase(_device)
	{
		CreatePipelineLayout(descriptorSet, sizeof(SimplePushConstantData), VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT);

		m_shaderPaths.vert_filepath = "shaders/simple_shader/simple_shader.vert";
		m_shaderPaths.frag_filepath = "shaders/simple_shader/simple_shader.frag";

		Pipeline::DefaultPipelineConfigInfo(m_configInfo);

		CreatePipeline();
	}

	void SimpleRenderSystem::RenderGameObjects(FrameInfo& fInfo, std::vector<TempGameObject>& gameObjects)
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

		for (auto& obj : gameObjects)
		{
			
			if(obj.model == nullptr)
				continue;
			
			//obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + (1.f * fInfo.frameTime), glm::two_pi<float>());
			//obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + (0.1f * fInfo.frameTime), glm::two_pi<float>());

			SimplePushConstantData pushData{};

			pushData.modelMatrix = obj.transform.positionRotMat() * obj.transform.scaleMat();
			pushData.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(
				fInfo.commandBuffer,
				m_pipelineLayout,
				VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(SimplePushConstantData),
				&pushData
			);

			obj.model->Bind(fInfo.commandBuffer);
			obj.model->Draw(fInfo.commandBuffer);
		}
	}

	void SimpleRenderSystem::CreatePipelineLayout(
		VkDescriptorSetLayout descriptorSet,
		uint32_t pushConstantSize,
		VkShaderStageFlags pushConstantShaderStages /* = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT */)
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
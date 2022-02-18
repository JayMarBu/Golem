#include "golpch.h"
#include "SimpleRenderSystem.h"
#include "Golem/Application.h"
#include "Golem/ECS/Components/RenderComponents.h"
#include "Golem/ECS/GameObject.h"
#include "Golem/ECS/Components/Transform.h"

namespace golem
{
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	SimpleRenderSystem::SimpleRenderSystem(Device& _device, VkDescriptorSetLayout descriptorSet)
		: RenderSystemBase(_device), m_renderSystem([=](GameObject go, FrameInfo& info) {RenderGameObject(go, info);})
	{
		CreatePipelineLayout(descriptorSet, sizeof(SimplePushConstantData), VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT);

		m_shaderPaths.vert_filepath = "shaders/simple_shader/simple_shader.vert";
		m_shaderPaths.frag_filepath = "shaders/simple_shader/simple_shader.frag";

		Pipeline::DefaultPipelineConfigInfo(m_configInfo);

		CreatePipeline();


	}

	void SimpleRenderSystem::RenderGameObjects(FrameInfo& fInfo)
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

		m_renderSystem.Run<MeshRendererComponent>(Application::Get().GetScene(), fInfo);
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

	void SimpleRenderSystem::RenderGameObject(GameObject gObj, FrameInfo& fInfo)
	{
		SimplePushConstantData pushData{};

		auto& transform = gObj.GetComponent<Transform>();
		auto& meshRenderer = gObj.GetComponent<MeshRendererComponent>();

		//pushData.modelMatrix = obj.transform.positionRotMat() * obj.transform.scaleMat();
		//pushData.normalMatrix = obj.transform.normalMatrix();

		pushData.modelMatrix = transform;
		pushData.normalMatrix = transform.NormalMatrix();

		vkCmdPushConstants(
			fInfo.commandBuffer,
			m_pipelineLayout,
			VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(SimplePushConstantData),
			&pushData
		);

		meshRenderer.model->Bind(fInfo.commandBuffer);
		meshRenderer.model->Draw(fInfo.commandBuffer);
	}

}
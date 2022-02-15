#include "golpch.h"
#include "PointLightRenderSystem.h"
#include "Golem/ECS/Components/RenderComponents.h"

namespace golem
{
	struct PointLightPushConstant
	{
		glm::vec4 pos{};
		glm::vec4 colour{};
		float radius;
	};

	PointLightRenderSystem::PointLightRenderSystem(Device& _device, VkDescriptorSetLayout descriptorSet)
		: RenderSystemBase(_device),
		m_renderSystem([=](GameObject go, FrameInfo& info) {RenderGameObject(go,info);}),
		m_updateSystem([=](GameObject go, FrameInfo& info) {RenderGameObject(go,info);})
	{
		CreatePipelineLayout(descriptorSet, sizeof(PointLightPushConstant));

		m_shaderPaths.vert_filepath = "shaders/point_light_shader/point_light_shader.vert";
		m_shaderPaths.frag_filepath = "shaders/point_light_shader/point_light_shader.frag";

		Pipeline::DefaultPipelineConfigInfo(m_configInfo);

		m_configInfo.vertexAttribDesc.clear();
		m_configInfo.vertexBindingDesc.clear();

		CreatePipeline();
	}

	void PointLightRenderSystem::Update(FrameInfo& fInfo, GlobalUBO& ubo)
	{
		int lightIndex = 0;
		
		System system([&ubo, &lightIndex](GameObject gObj, FrameInfo& fInfo)
		{
			auto& transform = gObj.GetComponent<Transform>();
			auto& pointLight = gObj.GetComponent<PointLightComponent>();

			ubo.pointLights[lightIndex].pos = glm::vec4(transform.translation, 1.0f);
			ubo.pointLights[lightIndex].lightColour = glm::vec4(pointLight.colour, pointLight.intensity);

			lightIndex++;	
		});

		system.Run<PointLightComponent>(Application::Get().GetScene(), fInfo);

		ubo.numLights = lightIndex;
	}

	void PointLightRenderSystem::Render(FrameInfo& fInfo)
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

		m_renderSystem.Run<PointLightComponent>(Application::Get().GetScene(),fInfo);
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

	void PointLightRenderSystem::RenderGameObject(GameObject gObj, FrameInfo& fInfo)
	{
		PointLightPushConstant pushData{};

		auto& transform = gObj.GetComponent<Transform>();
		auto& pointLight = gObj.GetComponent<PointLightComponent>();

		pushData.pos = glm::vec4(transform.translation, 1.0f);
		pushData.colour = glm::vec4(pointLight.colour, 1.0f);
		pushData.radius = transform.scale.x;

		vkCmdPushConstants(
			fInfo.commandBuffer,
			m_pipelineLayout,
			VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(PointLightPushConstant),
			&pushData
		);

		vkCmdDraw(fInfo.commandBuffer, 6, 1, 0, 0);
	}

	void PointLightRenderSystem::UpdateGameObject(GameObject gObj, FrameInfo& fInfo)
	{

	}

}
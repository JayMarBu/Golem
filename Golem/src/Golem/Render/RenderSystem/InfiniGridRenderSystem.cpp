#include "golpch.h"
#include "InfiniGridRenderSystem.h"
#include "../SwapChain.h"

namespace golem
{
	struct GridUBO
	{
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec3 pos;
	};

	InfiniGridRenderSystem::InfiniGridRenderSystem(Device& device)
		: RenderSystemBase(device)
	{
		CreateMainPipeline();
	}

	void InfiniGridRenderSystem::Render(FrameInfo& fInfo)
	{
		if(!render)
			return;

		m_pipeline->Bind(fInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			fInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0, 1,
			&m_descriptorSets[fInfo.frameIndex],
			0, nullptr
		);

		GridUBO ubo{};
		ubo.pos = {0,0,0};
		ubo.proj = fInfo.camera.GetProjection();
		ubo.view = fInfo.camera.GetView();

		m_UBObuffers[fInfo.frameIndex]->WriteToBuffer(&ubo);
		m_UBObuffers[fInfo.frameIndex]->Flush();

		vkCmdDraw(fInfo.commandBuffer, 6, 1, 0, 0);
	}

	void InfiniGridRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout descriptorSet, uint32_t pushConstantSize, VkShaderStageFlags pushConstantShaderStages)
	{
		VkPipelineLayoutCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineCreateInfo.setLayoutCount = 1;
		pipelineCreateInfo.pSetLayouts = &descriptorSet;
		pipelineCreateInfo.pushConstantRangeCount = 0;
		pipelineCreateInfo.pPushConstantRanges = nullptr;

		SAFE_RUN_VULKAN_FUNC(vkCreatePipelineLayout(m_device, &pipelineCreateInfo, nullptr, &m_pipelineLayout), "failed to create pipelineLayout");
	}

	void InfiniGridRenderSystem::CreateDescriptors()
	{
		m_UBObuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < m_UBObuffers.size(); i++)
		{
			m_UBObuffers[i] = std::make_unique<Buffer>(
				m_device,
				sizeof(GridUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				std::lcm(
					m_device.properties.limits.minUniformBufferOffsetAlignment,
					m_device.properties.limits.nonCoherentAtomSize)
				);

			m_UBObuffers[i]->Map();
		}

		m_pool = DescriptorPool::Builder(m_device)
			.SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();

		m_setLayout = golem::DescriptorSetLayout::Builder(m_device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		m_descriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < m_descriptorSets.size(); i++)
		{
			auto bufferInfo = m_UBObuffers[i]->DescriptorInfo();

			golem::DescriptorWriter(*m_setLayout, *m_pool)
				.WriteBuffer(0, &bufferInfo)
				.Build(m_descriptorSets[i]);
		}
	}

	void InfiniGridRenderSystem::CreateMainPipeline()
	{
		CreateDescriptors();

		CreatePipelineLayout(m_setLayout->GetDescriptorSetLayout(), 0, VK_SHADER_STAGE_VERTEX_BIT);

		m_shaderPaths.vert_filepath = "shaders/infinigrid/infinigrid.vert";
		m_shaderPaths.frag_filepath = "shaders/infinigrid/infinigrid.frag";

		Pipeline::DefaultPipelineConfigInfo(m_configInfo);

		m_configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
		m_configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		m_configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		m_configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;

		m_configInfo.vertexAttribDesc.clear();
		m_configInfo.vertexBindingDesc.clear();

		CreatePipeline();
	}

}
#pragma once
#include "../Pipeline.h"
#include "../Objects/Vertex.h"

namespace golem
{
	class RenderSystemBase
	{
		// Members ********************************************************************************
	protected:
		Device& m_device;

		std::unique_ptr<Pipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;

		VkShaderStageFlags m_pushConstantShaderStages;

		// Methods ********************************************************************************
	public:
		RenderSystemBase() = delete;
		~RenderSystemBase();

		REMOVE_COPY_CONSTRUCTOR(RenderSystemBase);

	protected:
		RenderSystemBase(Device& device) : m_device(device) {}

		virtual void CreatePipelineLayout(
			VkDescriptorSetLayout descriptorSet,
			uint32_t pushConstantSize,
			VkShaderStageFlags pushConstantShaderStages = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
		);

		virtual void CreatePipeline(
			VkRenderPass renderPass,
			ShaderPaths shaderPaths,
			PipelineConfigInfo& pipelineConfig,
			const std::vector<VkVertexInputBindingDescription>& vertexBindingDesc = Vertex::GetBindingDescriptions(),
			const std::vector<VkVertexInputAttributeDescription>& vertexAttribDesc = Vertex::GetAttributeDescriptions()
		);
	};
}
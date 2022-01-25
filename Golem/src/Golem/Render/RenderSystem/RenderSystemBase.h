#pragma once
#include "../Pipeline.h"
#include "../Objects/Vertex.h"
#include <mutex>

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
		PipelineConfigInfo m_configInfo;

		Pipeline* m_newPipeline = nullptr;
		bool hasRegenerated = false;
		bool isRegenerating = false;

		mutable std::mutex m_regenerationMutex;

		// Methods ********************************************************************************
	public:
		RenderSystemBase() = delete;
		virtual ~RenderSystemBase();

		REMOVE_COPY_CONSTRUCTOR(RenderSystemBase);

		void RuntimeCreatePipeline(
			VkRenderPass renderPass,
			ShaderPaths shaderPaths
		);

		bool HasRegenerated() const 
		{
			std::unique_lock<std::mutex> lock{m_regenerationMutex};
			return hasRegenerated;
		}

		bool IsRegenerating() const
		{
			std::unique_lock<std::mutex> lock{ m_regenerationMutex };
			return isRegenerating;
		}

		void CompleteRegeneration();

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
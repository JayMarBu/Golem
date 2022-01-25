#include "golpch.h"
#include "RenderSystemBase.h"
#include "../ShaderCompile.h"
#include "Golem/Application.h"
//#include "../ShaderCompile.h"

namespace golem
{

	RenderSystemBase::~RenderSystemBase()
	{
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	}

	void RenderSystemBase::CreatePipelineLayout(
		VkDescriptorSetLayout descriptorSet,
		uint32_t pushConstantSize,
		VkShaderStageFlags pushConstantShaderStages)
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
		if (pushConstantSize > 0)
		{
			pipelineCreateInfo.pushConstantRangeCount = 1;
			pipelineCreateInfo.pPushConstantRanges = &pushConstantRange;
		}
		else
		{
			pipelineCreateInfo.pushConstantRangeCount = 0;
			pipelineCreateInfo.pPushConstantRanges = nullptr;
		}

		SAFE_RUN_VULKAN_FUNC(vkCreatePipelineLayout(m_device, &pipelineCreateInfo, nullptr, &m_pipelineLayout), "failed to create pipelineLayout");
	}

	void RenderSystemBase::CreatePipeline(
		VkRenderPass renderPass,
		ShaderPaths shaderPaths,
		PipelineConfigInfo& pipelineConfig,
		const std::vector<VkVertexInputBindingDescription>& vertexBindingDesc,
		const std::vector<VkVertexInputAttributeDescription>& vertexAttribDesc)
	{
		GOL_CORE_ASSERT(m_pipelineLayout != nullptr, "cannot create pipeline before pipeline layout");

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_pipelineLayout;

		m_pipeline = std::make_unique<Pipeline>(
			m_device,
			shaderPaths,
			pipelineConfig
		);
	}

	void RenderSystemBase::RuntimeCreatePipeline(
		VkRenderPass renderPass,
		ShaderPaths shaderPaths)
	{
		if(isRegenerating)
			return;
		
		GOL_CORE_ASSERT(m_pipelineLayout != nullptr, "cannot create pipeline before pipeline layout");

		{
			std::unique_lock<std::mutex> lock{ m_regenerationMutex };
			isRegenerating = true;
		}

		std::vector<uint32_t> vertCode{};
		if(!CompileShader(shaderPaths.vert_filepath, shaderc_shader_kind::shaderc_glsl_vertex_shader, vertCode)) 
		{
			{
				std::unique_lock<std::mutex> lock{ m_regenerationMutex };
				isRegenerating = false;
			}
			return;
		}

		std::vector<uint32_t> fragCode{};
		if (!CompileShader(shaderPaths.frag_filepath, shaderc_shader_kind::shaderc_glsl_fragment_shader, fragCode))
		{
			{
				std::unique_lock<std::mutex> lock{ m_regenerationMutex };
				isRegenerating = false;
			}
			return;
		}

		m_configInfo.renderPass = renderPass;
		m_configInfo.pipelineLayout = m_pipelineLayout;

		bool success = false;

		m_newPipeline = new Pipeline(
			m_device,
			std::move(vertCode),
			std::move(fragCode),
			m_configInfo,
			success
		);

		if(success)
		{
			{
				std::unique_lock<std::mutex> lock{ m_regenerationMutex };
				hasRegenerated = true;
				isRegenerating = false;
			}

			//CompleteRegeneration();

			GOL_CORE_INFO("Pipeline reconstruction has succeeded");

		}
		else
		{
			CLEANUP(m_newPipeline);
			m_newPipeline = nullptr;
			{
				std::unique_lock<std::mutex> lock{ m_regenerationMutex };
				isRegenerating = false;
				hasRegenerated = false;
			}
			GOL_CORE_ERROR("Pipeline reconstruction has failed");
		}
	}

	void RenderSystemBase::CompleteRegeneration()
	{
		vkDeviceWaitIdle(Application::Get().GetDevice());
		{
			std::unique_lock<std::mutex> lock{ m_regenerationMutex };
			hasRegenerated = false;
		}

		m_pipeline.reset(m_newPipeline);
		m_newPipeline = nullptr;
	}
}
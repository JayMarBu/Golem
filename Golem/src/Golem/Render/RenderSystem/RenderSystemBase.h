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
		// Rendering data
		Device& m_device;

		Scope<Pipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;

		ShaderPaths m_shaderPaths;
		VkShaderStageFlags m_pushConstantShaderStages;
		PipelineConfigInfo m_configInfo;

		// Async recompile data
		Pipeline* m_newPipeline = nullptr;
		bool m_hasRegenerated = false;
		bool m_isRegenerating = false;

		mutable std::mutex m_regenerationMutex;

		// Methods ********************************************************************************
	public:
		RenderSystemBase() = delete;
		virtual ~RenderSystemBase();

		REMOVE_COPY_CONSTRUCTOR(RenderSystemBase);

		virtual void RuntimeCreatePipeline();

		bool HasRegenerated() const 
		{
			std::unique_lock<std::mutex> lock{m_regenerationMutex};
			return m_hasRegenerated;
		}

		bool IsRegenerating() const
		{
			std::unique_lock<std::mutex> lock{ m_regenerationMutex };
			return m_isRegenerating;
		}

		void CompleteRegeneration();

		static std::string SPVShaderPath(std::string& path);

	protected:
		RenderSystemBase(Device& device) : m_device(device) {}

		virtual void CreatePipelineLayout(
			VkDescriptorSetLayout descriptorSet,
			uint32_t pushConstantSize,
			VkShaderStageFlags pushConstantShaderStages) = 0;

		virtual void CreatePipeline();

		void EndRecompilation(bool success);

		void SetIsRegenerating(bool isRegenerating) 
		{
			std::unique_lock<std::mutex> lock{ m_regenerationMutex };
			m_isRegenerating = isRegenerating;
		}

		void SetHasRegenerated(bool hasRegenerated)
		{
			std::unique_lock<std::mutex> lock{ m_regenerationMutex };
			m_hasRegenerated = hasRegenerated;
		}

		void SetBothRegenerationFlags(bool isRegenerating,bool hasRegenerated)
		{
			std::unique_lock<std::mutex> lock{ m_regenerationMutex };
			m_isRegenerating = isRegenerating;
			m_hasRegenerated = hasRegenerated;
		}

		
	};
}
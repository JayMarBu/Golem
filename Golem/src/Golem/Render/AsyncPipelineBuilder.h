#pragma once

#include "Golem/Render/Pipeline.h"
#include <thread>

namespace golem
{
	struct PipelineBuildInfo
	{
		Device& device;
		ShaderPaths shaderPaths;
		PipelineConfigInfo& configInfo;
		std::vector<VkVertexInputBindingDescription>& vertexBindingDesc;
		std::vector<VkVertexInputAttributeDescription>& vertexAttribDesc;
	};

	class AsyncPipelineBuilder
	{
		// Members ********************************************************************************
	private:
		std::queue<PipelineBuildInfo> m_inputQueue;
		std::queue<Pipeline*> m_outputQueue;

		// Methods ********************************************************************************
	public:
		AsyncPipelineBuilder();

	private:
		void Run();

		void PushToQueue(PipelineBuildInfo& buildInfo);

		Pipeline* PopQueue();

	};
}
#include "golpch.h"
#include "Pipeline.h"
#include "../Application.h"
#include "Objects/Vertex.h"
//#include "Golem/Render/ShaderCompile.h"

namespace golem
{
	Pipeline::Pipeline(
		Device& device,
		ShaderPaths shaderPaths,
		const PipelineConfigInfo& configInfo,
		bool runtimeCompile)
		: m_device(device)
	{
		if(!runtimeCompile)
		{
			CreateGraphicsPipeline(
				shaderPaths.vert_filepath,
				shaderPaths.frag_filepath,
				configInfo);
			return;
		}
		CreateGraphicsPipelineNoVerts(
			shaderPaths.vert_filepath,
			shaderPaths.frag_filepath,
			configInfo);
	}

	Pipeline::Pipeline(
		Device& device,
		std::vector<uint32_t>&& vertShaderCode,
		std::vector<uint32_t>&& fragShaderCode,
		const PipelineConfigInfo& configInfo,
		bool& success)
		: m_device(device)
	{
		ReCreateGraphicsPipeline(
			std::move(vertShaderCode),
			std::move(fragShaderCode),
			configInfo,
			success);
	}

	void Pipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo)
	{
		configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

		configInfo.viewportInfo.viewportCount = 1;
		configInfo.viewportInfo.pViewports = nullptr;
		configInfo.viewportInfo.scissorCount = 1;
		configInfo.viewportInfo.pScissors = nullptr;

		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssemblyInfo.primitiveRestartEnable = false;

		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

		configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};  // Optional
		configInfo.depthStencilInfo.back = {};   // Optional

		configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
		configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
		configInfo.dynamicStateInfo.flags = 0;

		configInfo.vertexAttribDesc = Vertex::GetAttributeDescriptions();
		configInfo.vertexBindingDesc = Vertex::GetBindingDescriptions();
	}

	void Pipeline::Bind(VkCommandBuffer commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
	}

	Pipeline::~Pipeline()
	{
		// clean up shader modules
		vkDestroyShaderModule(m_device, m_vertShaderModule, nullptr);
		vkDestroyShaderModule(m_device, m_fragShaderModule, nullptr);

		vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
	}

	std::vector<char> Pipeline::ReadFile(const std::string& filepath)
	{
		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

		GOL_CORE_ASSERT(file.is_open(), "failed to open file: {0}", filepath);

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	void Pipeline::CreateGraphicsPipeline(
		const std::string& vert_filepath,
		const std::string& frag_filepath,
		const PipelineConfigInfo& configInfo)
	{
		GOL_CORE_ASSERT(configInfo.pipelineLayout != VK_NULL_HANDLE, "cannot create graphics pipeline:: no pipelineLayout provided in configInfo");
		GOL_CORE_ASSERT(configInfo.renderPass != VK_NULL_HANDLE, "cannot create graphics pipeline:: no renderPass provided in configInfo");

		auto vertCode = ReadFile(vert_filepath);
		auto fragCode = ReadFile(frag_filepath);

		CreateShaderModule(vertCode, &m_vertShaderModule);
		CreateShaderModule(fragCode, &m_fragShaderModule);

		BuildPipeline(configInfo);
	}

	void Pipeline::CreateGraphicsPipelineNoVerts(const std::string& vert_filepath, const std::string& frag_filepath, const PipelineConfigInfo& configInfo)
	{
		GOL_CORE_ASSERT(configInfo.pipelineLayout != VK_NULL_HANDLE, "cannot create graphics pipeline:: no pipelineLayout provided in configInfo");
		GOL_CORE_ASSERT(configInfo.renderPass != VK_NULL_HANDLE, "cannot create graphics pipeline:: no renderPass provided in configInfo");

		auto vertCode = ReadFile(vert_filepath);
		auto fragCode = ReadFile(frag_filepath);

		CreateShaderModule(vertCode, &m_vertShaderModule);
		CreateShaderModule(fragCode, &m_fragShaderModule);

		BuildPipelineNoVerts(configInfo);
	}

	void Pipeline::ReCreateGraphicsPipeline(
		std::vector<uint32_t>&& vertShaderCode,
		std::vector<uint32_t>&& fragShaderCode,
		const PipelineConfigInfo& configInfo,
		bool& success)
	{
		GOL_CORE_ASSERT(configInfo.pipelineLayout != VK_NULL_HANDLE, "cannot create graphics pipeline:: no pipelineLayout provided in configInfo");
		GOL_CORE_ASSERT(configInfo.renderPass != VK_NULL_HANDLE, "cannot create graphics pipeline:: no renderPass provided in configInfo");

		if(!CreateShaderModule(vertShaderCode, &m_vertShaderModule))
		{
			success = false;
			return;
		}

		if (!CreateShaderModule(fragShaderCode, &m_fragShaderModule))
		{
			success = false;
			return;
		}

		BuildPipeline(configInfo, &success);
	}

	void Pipeline::BuildPipeline(const PipelineConfigInfo& configInfo, bool* success)
	{
		// create array storing all pipeline stages
		VkPipelineShaderStageCreateInfo shaderStages[2];

		// assign shader pipeline stage to ShaderStage struct
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;

		// assign the shader module to the pipeline stage
		shaderStages[0].module = m_vertShaderModule;
		shaderStages[0].pName = "main";

		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

		// repeat for the fragment shader
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = m_fragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		// create struct to describe the format of 
		// vertex data being sent to the GPU
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		//auto bindingDescription = Model::Vertex::getBindingDescriptions();
		//auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(configInfo.vertexBindingDesc.size());
		vertexInputInfo.pVertexBindingDescriptions = configInfo.vertexBindingDesc.data();
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(configInfo.vertexAttribDesc.size());
		vertexInputInfo.pVertexAttributeDescriptions = configInfo.vertexAttribDesc.data();

		// create pipeline info struct
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		// reference all pipeline create structs created above
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &configInfo.viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;
		// attach handle to pipeline layout
		pipelineInfo.layout = configInfo.pipelineLayout;
		// attach render pass
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		// create graphics pipeline
		if(success == nullptr)
			SAFE_RUN_VULKAN_FUNC(vkCreateGraphicsPipelines(m_device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline), "failed to create graphics pipeline!")
		else
		{
			
			auto result = vkCreateGraphicsPipelines(m_device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);
			if (result != VK_SUCCESS)
			{
				GOL_CORE_ERROR("failed to create graphics pipeline!");
				*success = false;
				return;
			}
			*success = true;
		}
	}

	void Pipeline::BuildPipelineNoVerts(const PipelineConfigInfo& configInfo, bool* success /*= nullptr*/)
	{
		// create array storing all pipeline stages
		VkPipelineShaderStageCreateInfo shaderStages[2];

		// assign shader pipeline stage to ShaderStage struct
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;

		// assign the shader module to the pipeline stage
		shaderStages[0].module = m_vertShaderModule;
		shaderStages[0].pName = "main";

		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

		// repeat for the fragment shader
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = m_fragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		// create struct to describe the format of 
		// vertex data being sent to the GPU
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		//auto bindingDescription = Model::Vertex::getBindingDescriptions();
		//auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		// create pipeline info struct
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		// reference all pipeline create structs created above
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &configInfo.viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;
		// attach handle to pipeline layout
		pipelineInfo.layout = configInfo.pipelineLayout;
		// attach render pass
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		// create graphics pipeline
		if (success == nullptr)
			SAFE_RUN_VULKAN_FUNC(vkCreateGraphicsPipelines(m_device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline), "failed to create graphics pipeline!")
		else
		{

			auto result = vkCreateGraphicsPipelines(m_device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);
			if (result != VK_SUCCESS)
			{
				GOL_CORE_ERROR("failed to create graphics pipeline!");
				*success = false;
				return;
			}
			*success = true;
		}
	}

	bool Pipeline::CreateShaderModule(const std::vector<uint32_t>& code, VkShaderModule* shaderModule)
	{
		if(*shaderModule != NULL)
		{
			vkDestroyShaderModule(m_device, *shaderModule, nullptr);
			*shaderModule = NULL;
		}

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size()*sizeof(uint32_t);
		createInfo.pCode = code.data();

		//SAFE_RUN_VULKAN_FUNC(vkCreateShaderModule(m_device.device(), &createInfo, nullptr, shaderModule), "failed to create shader module");

		{
			auto result = vkCreateShaderModule(m_device.device(), &createInfo, nullptr, shaderModule); 
			if(result != VK_SUCCESS)
			{
				GOL_CORE_ERROR("failed to create shader module");
				return false;
			}
		}

		return true;
	}

	void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		if (*shaderModule != NULL)
		{
			vkDestroyShaderModule(m_device, *shaderModule, nullptr);
			*shaderModule = NULL;
		}

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		SAFE_RUN_VULKAN_FUNC(vkCreateShaderModule(m_device.device(), &createInfo, nullptr, shaderModule), "failed to create shader module");
	}

}
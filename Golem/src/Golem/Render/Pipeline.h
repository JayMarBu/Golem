#pragma once

#include "Golem/Render/Device.h"

namespace golem
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo() = default;
		REMOVE_COPY_CONSTRUCTOR(PipelineConfigInfo);

		// STRUCT INFO:
		/*
			* viewport info:
			*	information regarding both the viewport and the scissor rectangle
			*	viewport:
			*		information regarding how to translate from view space (-1,-1 -> 1,1),
			*		to pixel space (0,0 -> image_width, image_height)
			*	scissor rectangle:
			*		the scissor rectangle acts like a clipping mask
			*		for any pixels being rendered to the viewport
			*
			* input assembler info:
			*	input assembler stage -> the first shader stage to be run in the pipeline. comes before vertex shader.
			*	information regarding how the render pipeline should read incoming vertices.
			*	whether geometry should be read as triangles, triangle fans, lines ect.
			*
			* rasterization info:
			*	rasterization stage -> the shader stage inbetween vertex shader* and fragment shader
			*	* if tessellation or geometry shader are present they also run before rasterization
			*	information regarding how the GPU should rasterize vertex information.
			*		polygon mode determines whether geometry is filled (VK_POLYGON_MODE_FILL) or
			*		wireframe (VK_POLYGON_MODE_LINE)
			*		cull mode & font face describe whether the GPU should cull faces and the winding order in
			*		which to do so.
			*		depth clamp determines whether geometry outside the bounds of the near & far planes
			*		are to be discarded or clamped to the position of the respective plane.
			*
			* multi sampling info:
			*	information regarding the pipelines ability to multi sample.
			*	multi sampling is responsible for aliasing pixels during the rasterization stage
			*
			* colour blending attachment info:
			*	describes how colours should be blended in the blend stage of the pipeline.
			*	this information can be configured per render stage. mostly used for
			*	transparency.
			*
			* global colour blend information
			*	describes how colours should be blended globally if no attachment exists
			*	logic op flag allows the user to instead use
			*	bit wise operations to blend colours. this effectively
			*	disables all colour blend attachments.
			*
			* depth and stencil info
			*	information regarding the depth buffer configuration and
			*	stencil buffer configuration.
			*
			* layout info
			*	information regarding how the pipeline as a whole will be laid out,
			*	what shader stages are where and which are in/not in use.
			*	also contains info regarding the uniform and push values
			*	referenced by the shader that can be updated at draw time.
			*/
		
		// viewport info
		VkPipelineViewportStateCreateInfo viewportInfo{};

		// input assembler info
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};

		// rasterization info
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};

		// multi sampling info
		VkPipelineMultisampleStateCreateInfo multisampleInfo{};

		// colour blending info
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};

		// global colour blend information
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};

		// depth and stencil info
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};

		std::vector<VkDynamicState> dynamicStateEnables{};
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};

		// layout info
		VkPipelineLayout pipelineLayout = nullptr;

		std::vector<VkVertexInputAttributeDescription> vertexAttribDesc{};
		std::vector<VkVertexInputBindingDescription> vertexBindingDesc{};

		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;

		void SetData(PipelineConfigInfo& other)
		{
			viewportInfo = other.viewportInfo;
			inputAssemblyInfo = other.inputAssemblyInfo;
			rasterizationInfo = other.rasterizationInfo;
			multisampleInfo = other.multisampleInfo;
			colorBlendAttachment = other.colorBlendAttachment;
			colorBlendInfo = other.colorBlendInfo;
			depthStencilInfo = other.depthStencilInfo;
			dynamicStateEnables = other.dynamicStateEnables;
			dynamicStateInfo = other.dynamicStateInfo;
		}
	};

	struct ShaderPaths
	{
		std::string vert_filepath{};
		std::string frag_filepath{};
		std::string hull_filepath{};
		std::string geometry_filepath{};
		std::string domain_filepath{};
	};

	class Pipeline
	{
		// Members ********************************************************************************
	private:
		Device& m_device;
		VkPipeline m_graphicsPipeline;
		VkShaderModule m_vertShaderModule = NULL;
		VkShaderModule m_fragShaderModule = NULL;

		// Methods ********************************************************************************
	public:
		Pipeline(
			Device& device,
			ShaderPaths shaderPaths,
			const PipelineConfigInfo& configInfo,
			bool runtimeCompile = false);

		Pipeline(
			Device& device,
			std::vector<uint32_t>&& vertShaderCode,
			std::vector<uint32_t>&& fragShaderCode,
			const PipelineConfigInfo& configInfo,
			bool& success
			);

		~Pipeline();

		REMOVE_COPY_CONSTRUCTOR(Pipeline)

		static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

		void Bind(VkCommandBuffer commandBuffer);



	private:
		static std::vector<char> ReadFile(const std::string& filepath);

		void BuildPipeline(
			const PipelineConfigInfo& configInfo,
			bool* success = nullptr);

		void CreateGraphicsPipeline(
			const std::string& vert_filepath,
			const std::string& frag_filepath,
			const PipelineConfigInfo& configInfo
		);

		void ReCreateGraphicsPipeline(
			std::vector<uint32_t>&& vertShaderCode,
			std::vector<uint32_t>&& fragShaderCode,
			const PipelineConfigInfo& configInfo,
			bool& success
		);

		bool CreateShaderModule(const std::vector<uint32_t>& code, VkShaderModule* shaderModule);
		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	};
}
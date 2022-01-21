#include "golpch.h"
#include "SimpleRenderSystem.h"

namespace golem
{
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	SimpleRenderSystem::SimpleRenderSystem(
		Device& _device,
		VkRenderPass _renderPass,
		VkDescriptorSetLayout descriptorSet,
		const std::string& vert_filepath,
		const std::string& frag_filepath
		)
		: RenderSystemBase(_device)
	{
		CreatePipelineLayout(descriptorSet, sizeof(SimplePushConstantData));

		ShaderPaths shaderPaths{};

		shaderPaths.vert_filepath = vert_filepath;
		//shaderPaths.vert_filepath = "shaders/simple_shader/simple_shader.vert.spv";
		shaderPaths.frag_filepath = frag_filepath;
		//shaderPaths.frag_filepath = "shaders/simple_shader/simple_shader.frag.spv";


		PipelineConfigInfo pipelineConfig{};
		Pipeline::DefaultPipelineConfigInfo(pipelineConfig);

		CreatePipeline(_renderPass, shaderPaths, pipelineConfig);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{

	}

	void SimpleRenderSystem::RenderGameObjects(FrameInfo& fInfo, std::vector<TempGameObject>& gameObjects)
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

		for (auto& obj : gameObjects)
		{
			//obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + (1.f*fInfo.frameTime), glm::two_pi<float>());
			//obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + (0.1f* fInfo.frameTime), glm::two_pi<float>());

			SimplePushConstantData pushData{};

			pushData.modelMatrix = obj.transform.positionRotMat() * obj.transform.scaleMat();
			pushData.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(
				fInfo.commandBuffer,
				m_pipelineLayout,
				VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT,
				0,
				sizeof(SimplePushConstantData),
				&pushData
			);

			obj.model->Bind(fInfo.commandBuffer);
			obj.model->Draw(fInfo.commandBuffer);
		}
	}

}
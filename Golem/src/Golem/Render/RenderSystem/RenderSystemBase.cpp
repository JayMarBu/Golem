#include "golpch.h"
#include "RenderSystemBase.h"
#include "../ShaderCompile.h"
#include "Golem/Application.h"
#include "Golem/Events/RenderSystemEvents.h"
//#include "../ShaderCompile.h"

namespace golem
{

	RenderSystemBase::~RenderSystemBase()
	{
		vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	}

	void RenderSystemBase::CreatePipeline()
	{
		GOL_CORE_ASSERT(m_pipelineLayout != nullptr, "cannot create pipeline before pipeline layout");

		m_configInfo.renderPass = Application::Get().GetRenderer().GetSwapChainRenderPass();
		m_configInfo.pipelineLayout = m_pipelineLayout;

		ShaderPaths sPaths{};

		sPaths.vert_filepath = SPVShaderPath(m_shaderPaths.vert_filepath);
		sPaths.frag_filepath = SPVShaderPath(m_shaderPaths.frag_filepath);

		m_pipeline = std::make_unique<Pipeline>(
			m_device,
			sPaths,
			m_configInfo
		);
	}

	void RenderSystemBase::EndRecompilation(bool success)
	{
		if (!success)
		{
			CLEANUP(m_newPipeline);
			m_newPipeline = nullptr;
			SetBothRegenerationFlags(false, false);
			GOL_CORE_ERROR("Pipeline reconstruction has failed");
			return;
		}

		SetBothRegenerationFlags(false, true);
		GOL_CORE_INFO("Pipeline reconstruction has succeeded");

		golem::ShaderReCompileEvent* e = new golem::ShaderReCompileEvent(this);
		golem::Application::Get().GetThreadPool().FireEvent(e);
	}

	std::string RenderSystemBase::SPVShaderPath(std::string& path)
	{
		std::stringstream ss;
		ss << path << ".spv";
		return ss.str();
	}

	void RenderSystemBase::RuntimeCreatePipeline()
	{
		if(IsRegenerating())
			return;
		
		GOL_CORE_ASSERT(m_pipelineLayout != nullptr, "cannot create pipeline before pipeline layout");

		SetIsRegenerating(true);

		std::vector<uint32_t> vertCode{};
		if(!CompileShader(m_shaderPaths.vert_filepath, shaderc_shader_kind::shaderc_glsl_vertex_shader, vertCode)) 
		{
			SetIsRegenerating(false);
			return;
		}

		std::vector<uint32_t> fragCode{};
		if (!CompileShader(m_shaderPaths.frag_filepath, shaderc_shader_kind::shaderc_glsl_fragment_shader, fragCode))
		{
			SetIsRegenerating(false);
			return;
		}

		m_configInfo.renderPass = Application::Get().GetRenderer().GetSwapChainRenderPass();
		m_configInfo.pipelineLayout = m_pipelineLayout;

		bool success = false;

		m_newPipeline = new Pipeline(
			m_device,
			std::move(vertCode),
			std::move(fragCode),
			m_configInfo,
			success
		);

		EndRecompilation(success);
	}

	void RenderSystemBase::CompleteRegeneration()
	{
		vkDeviceWaitIdle(Application::Get().GetDevice());
		SetHasRegenerated(false);

		m_pipeline.reset(m_newPipeline);
		m_newPipeline = nullptr;
	}
}
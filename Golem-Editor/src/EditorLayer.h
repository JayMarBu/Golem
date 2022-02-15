#pragma once
#include "Golem/Layer.h"

#include "Golem.h"

#include "ImGui/imgui.h"
#include "Golem/Temp/KeyboardMovementController.h"
#include "Golem/FrameTimer.h"

#include "Golem/Render/RenderSystem/SimpleRenderSystem.h"
#include "Golem/Render/RenderSystem/PointLightRenderSystem.h"
#include "Golem/Render/RenderSystem/InfiniGridRenderSystem.h"

#include "Golem/Events/RenderSystemEvents.h"

namespace golem
{
	class GameObject;

	class EditorLayer : public Layer
	{
		// Members ********************************************************************************
	private:
		Scope<DescriptorPool> m_globalPool;
		Scope<DescriptorSetLayout> m_globalSetLayout;
		std::vector<VkDescriptorSet> m_globalDescriptorSets;

		std::vector<Scope<Buffer>> m_UBObuffers;

		Scope<SimpleRenderSystem> m_simpleRenderSystem;
		Scope<PointLightRenderSystem> m_pointLightRenderSystem;

		GameObject m_camera;

		Scope<Sampler> m_sampler;

		FrameTimer m_timer;

		glm::vec2 m_viewportSize;
		glm::vec2 m_viewportCentre;
		bool m_viewportFocused = false;

		VkRenderPass m_renderPass;
		Ref<RenderTexture> m_renderTexture;

		// Methods ********************************************************************************
	public:
		EditorLayer();
		virtual ~EditorLayer();

		void OnRender(VkCommandBuffer commandBuffer) override;

		void OnImGuiRender() override;

		void OnEvent(Event& e) override;

	private:
		void CreateRenderPass();
		void CreateScene();
		void CreateUBO();
		void CreateDescriptors();
		void CreateRenderSystems();

		void BeginRenderPass(VkCommandBuffer commandBuffer);
		inline void EndRenderPass(VkCommandBuffer commandBuffer) { vkCmdEndRenderPass(commandBuffer); }

		bool OnShaderRecompile(ShaderReCompileEvent& e);
		bool OnViewportResize(ViewportResizeEvent& e);

		void BeginDocking();
		inline void EndDocking() { ImGui::End(); }

		void DrawSceneViewport();
	};
}


#pragma once
#include "Golem/Layer.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/ApplicationEvent.h"
#include "../Render/Descriptors.h"
#include "../Render/RenderTexture.h"



namespace golem
{
	static void check_vk_result(VkResult err) {
		if (err == 0) return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0) abort();
	}

	class ImGuiLayer : public Layer
	{
		// Members ********************************************************************************
	private:
		
		std::unique_ptr<DescriptorPool> m_descriptorPool;

		//ImGui_ImplVulkanH_Window m_ImGuiWindowHandle;

		//uint32_t m_QueueFamily = (uint32_t)-1;

		//VkDescriptorPool m_descriptorPool;
		// Methods ********************************************************************************
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& e) override;
		void OnImGuiRender() override;

		void Begin();
		void End(VkCommandBuffer commandBuffer);

		void RenderGameWindow(Ref<RenderTexture> renderTexture);
	private: 
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrollEvent(MouseScrolledEvent& e);

		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);

		bool OnKeyTypedEvent(KeyTypedEvent& e);
	};
}

#pragma once
#include "Golem/Layer.h"
#include "../Events/MouseEvent.h"
#include "../Events/KeyEvent.h"
#include "../Events/ApplicationEvent.h"

namespace golem
{
	static void check_vk_result(VkResult err) {
		if (err == 0) return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0) abort();
	}

	class GOLEM_API ImGuiLayer : public Layer
	{
		// Members ********************************************************************************
	private:

		VkDescriptorPool m_descriptorPool;
		// Methods ********************************************************************************
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(VkCommandBuffer commandBuffer) override;
		void OnEvent(Event& e) override;

		void NewFrame();
		void Render(VkCommandBuffer commandBuffer);
	private: 
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrollEvent(MouseScrolledEvent& e);

		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);

		bool OnKeyTypedEvent(KeyTypedEvent& e);

		bool OnWindowResizedEvent(WindowResizeEvent& e);

	};
}

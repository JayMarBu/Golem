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

		// Methods ********************************************************************************
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& e) override;

		void Begin();
		void End(VkCommandBuffer commandBuffer);
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

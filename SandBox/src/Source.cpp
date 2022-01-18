#include "Golem.h"

class ExampleLayer : public golem::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate(VkCommandBuffer commandBuffer) override
	{

	}

	void OnEvent(golem::Event& e) override
	{
		if(e.GetEventType() != golem::EventType::MouseMoved)
			GOL_TRACE("{0}", e.ToString());
	}
};

class Sandbox : public golem::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new golem::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

golem::Application* golem::CreateApplication()
{
	return new Sandbox();
}
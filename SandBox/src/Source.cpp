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
		//GOL_INFO("ExampleLayer::OnUpdate()");
	}

	void OnEvent(golem::Event& event) override
	{
		//GOL_TRACE("{0}", event.ToString());
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
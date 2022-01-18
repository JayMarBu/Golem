#include "Golem.h"
#include "ImGui/imgui.h"

class ExampleLayer : public golem::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{

	}

	void OnImGuiRender() override
	{
		//ImGui::Begin("test");
		//ImGui::End();
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
		//PushOverlay(new golem::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

golem::Application* golem::CreateApplication()
{
	return new Sandbox();
}
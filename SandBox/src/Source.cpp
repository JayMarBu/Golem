#include "Golem.h"

class Sandbox : public golem::Application
{
public:
	Sandbox()
	{
	}

	~Sandbox()
	{

	}
};

golem::Application* golem::CreateApplication()
{
	return new Sandbox();
}
#pragma once

#ifdef GOL_PLATFORM_WINDOWS

extern golem::Application* golem::CreateApplication();

int main(int argc, char** argv)
{
	golem::Log::Init();

	auto app = golem::CreateApplication();
	app->Run();
	delete app;
}
#endif
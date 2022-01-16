#pragma once

#ifdef GOL_PLATFORM_WINDOWS

extern golem::Application* golem::CreateApplication();

int main(int argc, char** argv)
{
	golem::Log::Init();

	GOL_CORE_WARN("test");
	GOL_INFO("info");

	auto app = golem::CreateApplication();
	app->Run();
	delete app;
}
#endif
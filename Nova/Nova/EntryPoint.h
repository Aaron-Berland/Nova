#pragma once
#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

extern Nova::Application* Nova::CreateApplication();
int main(int argc, char ** argv)
{
	Nova::Log::Init();
	NOVA_CORE_WARN("Initialized Log");
	NOVA_INFO("Initialized Log");
	auto app = Nova::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif
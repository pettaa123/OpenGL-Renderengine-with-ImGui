#pragma once

#include "application.h"
#include "log.h"


extern Engine::Application* Engine::createApplication(Engine::ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	auto app = Engine::createApplication({ argc, argv });
	app->Run();
	delete app;
}


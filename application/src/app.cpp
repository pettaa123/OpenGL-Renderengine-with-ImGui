#include <engine/core/entryPoint.h>
#include "exampleLayer.h"
#include "targetLayer.h"

class App : public Engine::Application
{
public:
	App(const Engine::ApplicationSpecification& specification)
		: Engine::Application(specification)
	{
		//pushLayer(new ExampleLayer());
		//pushLayer(new WebcamLayer());
		pushLayer(new TargetLayer());
	}

	~App()
	{
	}
};

Engine::Application* Engine::createApplication(Engine::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.name = "Rendering";
	spec.workingDirectory = "../application";
	spec.commandLineArgs = args;

	return new App(spec);
}

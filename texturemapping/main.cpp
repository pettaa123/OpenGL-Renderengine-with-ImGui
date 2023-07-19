#include <engine.h>
#include <core/EntryPoint.h>

#include "src/texturemapping.h"
#include "src/exampleLayer.h"

class TexturemappingApp : public Engine::Application
{
public:
	TexturemappingApp(const Engine::ApplicationSpecification& specification)
		: Engine::Application(specification)
	{
		// PushLayer(new ExampleLayer());
		pushLayer(new Texturemapping());
	}

	~TexturemappingApp()
	{
	}
};

Engine::Application* Engine::createApplication(Engine::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.name = "Texturemapping";
	spec.workingDirectory = "../";
	spec.commandLineArgs = args;

	return new TexturemappingApp(spec);
}

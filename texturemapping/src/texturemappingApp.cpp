#include <engine.h>
#include <core/EntryPoint.h>

#include "texturemapping2d.h"
#include "exampleLayer.h"

class TexturemappingApp : public Engine::Application
{
public:
	TexturemappingApp(const Engine::ApplicationSpecification& specification)
		: Engine::Application(specification)
	{
		pushLayer(new ExampleLayer());
		//pushLayer(new Texturemapping2D());
	}

	~TexturemappingApp()
	{
	}
};

Engine::Application* Engine::createApplication(Engine::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.name = "Texturemapping";
	spec.workingDirectory = "../texturemapping";
	spec.commandLineArgs = args;

	return new TexturemappingApp(spec);
}

#include <core/EntryPoint.h>

#include "exampleLayer.h"
#include "webcamLayer.h"
#include "calibratedCamera.h"

class App : public Engine::Application
{
public:
	App(const Engine::ApplicationSpecification& specification)
		: Engine::Application(specification)
	{
		//pushLayer(new ExampleLayer());
		pushLayer(new WebcamLayer());
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

	TextureMapping::CalibratedCamera calibratedCamera;
	calibratedCamera.setName("Harry");

	return new App(spec);
}

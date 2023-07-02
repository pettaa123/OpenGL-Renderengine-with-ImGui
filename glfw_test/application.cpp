#include "application.h"
#include <memory>
#include "renderer/renderer.h"


Application::Application() {

}

Application::~Application() {

}

int Application::run() {

    std::unique_ptr<Engine::Renderer> renderer = std::make_unique<Engine::Renderer>();

    if (renderer->init("mainy", false))
        exit(EXIT_FAILURE);

    renderer->runRenderLoop();

    exit(EXIT_SUCCESS);
}
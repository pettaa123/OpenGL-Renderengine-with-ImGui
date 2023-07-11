#include "renderer.h"
#include "log.h"


namespace Engine {

    std::unique_ptr<Renderer::SceneData> Renderer::s_sceneData = std::make_unique<Renderer::SceneData>();

    void Renderer::init() {
        //RenderCommand::Init();
        //Renderer2D::Init();
    }

    void Renderer::shutdown()
    {
        //Renderer2D::Shutdown();
    }

    void Renderer::onWindowResize(uint32_t width, uint32_t height)
    {
        //RenderCommand::SetViewport(0, 0, width, height);
    }

    //void Renderer::beginScene(OrthographicCamera& camera)
    //{
    //    s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    //}
    //
    //void Renderer::endScene()
    //{
    //}


}
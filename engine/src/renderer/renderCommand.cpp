#include "renderer/renderCommand.h"

namespace Engine {

	std::unique_ptr<OpenGLRendererAPI> RenderCommand::s_rendererAPI = std::make_unique<OpenGLRendererAPI>();

}
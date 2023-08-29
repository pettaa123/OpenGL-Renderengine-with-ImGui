#include "engine/renderer/renderCommand.h"

namespace Engine {

	std::unique_ptr<OpenGLRenderer> RenderCommand::s_renderer = std::make_unique<OpenGLRenderer>();

}
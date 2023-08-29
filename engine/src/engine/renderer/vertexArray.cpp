
#include "engine/renderer/vertexArray.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/openGLVertexArray.h"
#include <memory>

namespace Engine {

	std::shared_ptr<VertexArray> VertexArray::create()
	{
		return std::make_shared<OpenGLVertexArray>();
	}

}
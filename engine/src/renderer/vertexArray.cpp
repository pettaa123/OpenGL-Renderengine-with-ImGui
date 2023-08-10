
#include "renderer/vertexArray.h"

#include "renderer/renderer.h"
#include "renderer/openGLVertexArray.h"
#include <memory>

namespace Engine {

	std::shared_ptr<VertexArray> VertexArray::create()
	{
		return std::make_shared<OpenGLVertexArray>();
	}

}
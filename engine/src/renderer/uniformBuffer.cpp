#include "openGLUniformBuffer.h"
#include <glad/glad.h>

namespace Engine {

	std::shared_ptr<UniformBuffer> UniformBuffer::create(uint32_t size, uint32_t binding)
	{
		return std::make_shared<OpenGLUniformBuffer>(size, binding);
	}
}


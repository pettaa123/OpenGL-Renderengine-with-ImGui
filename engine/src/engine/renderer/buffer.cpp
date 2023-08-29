#include "engine/renderer/buffer.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/openGLBuffer.h"

namespace Engine {

	VertexBuffer* VertexBuffer::create(uint32_t size)
	{
		return new OpenGLVertexBuffer(size);
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size)
	{
		return std::make_shared<OpenGLVertexBuffer>(vertices, size);
	}

	IndexBuffer* IndexBuffer::create(uint32_t size)
	{
		return new OpenGLIndexBuffer(size);
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t size)
	{
		return std::make_shared<OpenGLIndexBuffer>(indices, size);
	}

}
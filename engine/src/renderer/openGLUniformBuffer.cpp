#include "renderer/openGLUniformBuffer.h"

#include <glad/glad.h>

namespace Engine {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &m_rendererID);
		glNamedBufferData(m_rendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_rendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_rendererID);
	}


	void OpenGLUniformBuffer::setData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_rendererID, offset, size, data);
	}

}

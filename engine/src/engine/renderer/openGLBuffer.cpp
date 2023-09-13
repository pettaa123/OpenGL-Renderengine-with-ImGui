#include "engine/renderer/openGLBuffer.h"

#include <glad/glad.h>

namespace Engine {

	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t count, uint32_t elemSize)
		:m_count(count)
	{
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const float* vertices,uint32_t count, uint32_t elemSize)
		:m_count(count)
	{
		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, (uint32_t)(count*elemSize), vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_rendererID);
	}

	void OpenGLVertexBuffer::bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}

	void OpenGLVertexBuffer::unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::setData(const void* data,uint32_t count, uint32_t elemSize)
	{
		m_count = count;
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, (uint32_t)(count*elemSize), data, GL_STATIC_DRAW);
	}

	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_count(count)
	{
		glCreateBuffers(1, &m_rendererID);
		
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
		: m_rendererID(0), m_count(count)
	{
		glCreateBuffers(1, &m_rendererID);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_rendererID);
	}

	void OpenGLIndexBuffer::setData(const void* data, uint32_t count)
	{
		m_count = count;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW);
	}

	void OpenGLIndexBuffer::bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	}

	void OpenGLIndexBuffer::unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}

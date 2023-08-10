#pragma once

#include "renderer/vertexArray.h"

#include <vector>

namespace Engine {

	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void bind() const;
		virtual void unbind() const;

		virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const { return m_vertexBuffers; }
		virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const { return m_indexBuffer; }

	private:
		uint32_t m_rendererID;
		uint32_t m_vertexBufferIndex = 0;
		std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
	};

}

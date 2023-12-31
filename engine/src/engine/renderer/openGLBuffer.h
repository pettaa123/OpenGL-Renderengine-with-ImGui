#pragma once

#include "engine/renderer/buffer.h"

namespace Engine {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t count,uint32_t size);
		OpenGLVertexBuffer(const float* vertices,uint32_t count, uint32_t elemSize);
		virtual ~OpenGLVertexBuffer();

		virtual void bind() const override;
		virtual void unbind() const override;
		
		virtual void setData(const void* data,uint32_t count, uint32_t elemSize) override;

		virtual const BufferLayout& getLayout() const override { return m_layout; }
		virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }

		virtual uint32_t getCount() const { return m_count; }
	private:
		uint32_t m_rendererID;
		BufferLayout m_layout;
		uint32_t m_count=0;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		OpenGLIndexBuffer(uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void setData(const void* data, uint32_t size) override;

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual uint32_t getCount() const { return m_count; }
	private:
		uint32_t m_rendererID;
		uint32_t m_count;
	};

}
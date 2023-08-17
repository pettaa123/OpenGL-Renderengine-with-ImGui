#pragma once


#include "renderer/openGLRenderer.h"

namespace Engine {

	//hides opengl away from application

	class RenderCommand
	{
	public:
		inline static void init()
		{
			s_renderer->init();
		}

		inline static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_renderer->setViewport(x, y, width, height);
		}

		inline static void setClearColor(const glm::vec4& color)
		{
			s_renderer->setClearColor(color);
		}

		inline static void clear()
		{
			s_renderer->clear();
		}

		inline static void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_renderer->drawIndexed(vertexArray, indexCount);
		}

		inline static void drawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_renderer->drawLines(vertexArray, vertexCount);
		}

		inline static void setLineWidth(float width)
		{
			s_renderer->setLineWidth(width);
		}
	private:
		static std::unique_ptr<OpenGLRenderer> s_renderer;
	};

}

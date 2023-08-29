#pragma once

#include "engine/renderer/openGLVertexArray.h"
#include "glm/glm.hpp"

namespace Engine {

	class OpenGLRenderer
	{
	public:
		virtual void init();
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		virtual void setClearColor(const glm::vec4& color);
		virtual void clear();

		virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0);
		virtual void drawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount);
		
		virtual void setLineWidth(float width);
	};


}

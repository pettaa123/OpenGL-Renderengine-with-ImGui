#include <glad/glad.h>
#include "engine/renderer/openGLRenderer.h"
#include "engine/core/log.h"

namespace Engine {
	
	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         Log::fatal(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       Log::error(message); return;
			case GL_DEBUG_SEVERITY_LOW:          Log::warn(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: Log::info(message); return;
		}
		
		assert(false && "Unknown severity level!");
	}

	void OpenGLRenderer::init()
	{
	#ifdef _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRenderer::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRenderer::setClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRenderer::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRenderer::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->bind();
		uint32_t count = indexCount ? indexCount : vertexArray->getIndexBuffer()->getCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::drawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRenderer::setLineWidth(float width)
	{
		glLineWidth(width);
	}


}

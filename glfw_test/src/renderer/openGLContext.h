#pragma once

#include <memory>

struct GLFWwindow;

namespace Engine {

	class OpenGLContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void init();
		virtual void swapBuffers();

		static std::unique_ptr<OpenGLContext> create(GLFWwindow* window);
	private:
		GLFWwindow* m_windowHandle;
	};

}
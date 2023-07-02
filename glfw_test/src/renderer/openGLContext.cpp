#include "openGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "log.h"

namespace Engine {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_windowHandle(windowHandle)
	{
		if (windowHandle == NULL)
			Log::error("Window handle is null!");
	}

	void OpenGLContext::init()
	{
		glfwMakeContextCurrent(m_windowHandle);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			Log::error("Failed to initialize Glad!");

		Log::info("OpenGL Info:");
		Log::info("  Vendor: {0}", glGetString(GL_VENDOR));
		Log::info("  Renderer: {0}", glGetString(GL_RENDERER));
		Log::info("  Version: {0}", glGetString(GL_VERSION));

		if (GLVersion.major < 4 || (GLVersion.major == 4 && GLVersion.minor < 5))
			Log::error("Engine requires at least OpenGL version 4.5!");

	}

	void OpenGLContext::swapBuffers()
	{
		glfwSwapBuffers(m_windowHandle);
	}

	std::unique_ptr<OpenGLContext> OpenGLContext::create(GLFWwindow* window){
		return std::make_unique<OpenGLContext>(window);
	}

}
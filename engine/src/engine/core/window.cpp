#include <iostream>
#include <sstream>

#include "engine/core/window.h"
#include "engine/event/mouseEvent.h"
#include "engine/event/keyEvent.h"
#include "engine/event/applicationEvent.h"
#include "engine/core/log.h"

namespace Engine {

	Window* Window::create(const WindowSpecification& specification)
	{
		return new Window(specification);
	}

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		Log::error(std::format("Error: {0}", description));
	}


	/* Create window & initilialize its OpenGL context */
	Window::Window(const WindowSpecification& specification) :
		m_specification(specification)
	{
		init();
	}


	Window::~Window() {
		shutdown();
	}

	void Window::init() {
		m_data.title = m_specification.title;
		m_data.width = m_specification.width;
		m_data.height = m_specification.height;

		Log::info(std::format("Creating window {0} ({1}, {2})", m_specification.title, m_specification.width, m_specification.height));


		// Setup Our GLFW error callback, we do this before Init so we know what goes wrong with init if it fails:
		glfwSetErrorCallback(GLFWErrorCallback);


		// initialize glfw library
		if (!s_GLFWInitialized)
		{
			if (!glfwInit()) {
				m_window = NULL;
				Log::error("Failed to initialize GLFW");
				return;
			}
			s_GLFWInitialized = true;
		}

		if (!m_specification.decorated)
		{
			// This removes titlebar on all platforms
			// and all of the native window effects on non-Windows platforms
			glfwWindowHint(GLFW_DECORATED, false);
		}

		if (m_specification.fullscreen)
		{
			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

			glfwWindowHint(GLFW_DECORATED, false);
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			m_window = glfwCreateWindow(mode->width, mode->height, m_data.title.c_str(), primaryMonitor, nullptr);
		}
		else
		{
			m_window = glfwCreateWindow((int)m_specification.width, (int)m_specification.height, m_data.title.c_str(), nullptr, nullptr);
		}

#if defined(_DEBUG)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		m_context = OpenGLContext::create(m_window);
		m_context->init();

		glfwSetWindowUserPointer(m_window, &m_data);
		setVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.eventCallback(event);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.eventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.eventCallback(event);
					break;
				}
				}
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.eventCallback(event);
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods){
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.eventCallback(event);
					break;
				}
				}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.eventCallback(event);
			});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.eventCallback(event);
			});
	}

	void Window::shutdown()
	{

		glfwDestroyWindow(m_window);
		s_GLFWInitialized = false;
		glfwTerminate();
	}

	void Window::onUpdate()
	{
		glfwPollEvents();
		m_context->swapBuffers();
	}

	void Window::setVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_data.vsync = enabled;
	}

	void Window::setWindowSize(int width,int height)
	{
		glfwSetWindowSize(m_window, width,height);
	}

	void Window::getWindowSize(int* width, int* height)
	{
		glfwGetWindowSize(m_window, width, height);
	}

	bool Window::isVSync() const
	{
		return m_data.vsync;
	}

	/**
	 * Calculate fps & show it on title bar
	 * called every frame in main loop
	 */
	void Window::showFps() {
		m_fps++;
		double current_time = glfwGetTime();

		// more than 1 sec elapsed
		if (current_time - m_last_time >= 1.0) {
			std::ostringstream stream;
			stream << m_data.title << " (FPS: " << m_fps << ")";

			glfwSetWindowTitle(m_window, stream.str().c_str());
			m_last_time = current_time;
			m_fps = 0;
		}
	}
}
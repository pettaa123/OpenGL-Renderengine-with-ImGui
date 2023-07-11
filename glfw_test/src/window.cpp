#include <iostream>
#include <sstream>

#include "window.h"
#include "event/mouseEvent.h"
#include "event/keyEvent.h"
#include "event/applicationEvent.h"
#include "log.h"

namespace Engine {

	std::unique_ptr<Window> Window::create(const WindowProps& props)
	{
		return std::make_unique<Window>(props);
	}

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		fprintf(stderr, "Error: %s\n", description);
	}


	/* Create window & initilialize its OpenGL context */
	Window::Window(const WindowProps& props) :
		m_fps(0)
	{
		init(props);
	}


	Window::~Window() {
		shutdown();
	}

	void Window::init(const WindowProps& props) {
		m_data.title = props.title;
		m_data.width = props.width;
		m_data.height = props.height;

		Log::info(std::format("Creating window {0} ({1}, {2})", props.title, props.width, props.height));


		// Setup Our GLFW error callback, we do this before Init so we know what goes wrong with init if it fails:
		glfwSetErrorCallback(GLFWErrorCallback);


		// initialize glfw library
		if (s_GLFWWindowCount == 0)
		{
			if (!glfwInit()) {
				m_window = NULL;
				Log::error("Failed to initialize GLFW");
				return;
			}
		}

#if defined(_DEBUG)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;

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
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
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
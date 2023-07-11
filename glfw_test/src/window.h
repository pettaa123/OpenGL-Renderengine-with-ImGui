#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include "event/event.h"
#include "renderer/openGLContext.h"

namespace Engine {

	struct WindowProps
	{
		std::string title;
		uint32_t width;
		uint32_t height;

		WindowProps(const std::string& title = "Peters Engine",
			uint32_t width = 1280,
			uint32_t height = 1024)
			: title(title), width(width), height(height)
		{
		}
	};

	class Window {

	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowProps& props);
		~Window();

		void onUpdate();

		inline int getHeight() { return m_data.height; }
		inline int getWidth() { return m_data.width; }

		// Window attributes
		void setEventCallback(const EventCallbackFn& callback) { m_data.eventCallback = callback; }
		void setVSync(bool enabled);
		bool isVSync() const;

		void* getNativeWindow() const { return m_window; }

		static std::unique_ptr<Window> create(const WindowProps& props = WindowProps());

	private:

		void init(const WindowProps& props);
		void shutdown();

		void showFps();



	private:

		GLFWwindow* m_window;
		std::unique_ptr<OpenGLContext> m_context;

		struct WindowData
		{
			std::string title;
			int width;
			int height;
			bool vsync;

			EventCallbackFn eventCallback;
		};

		WindowData m_data;

		double m_last_time;
		unsigned int m_fps;

	};

}

#endif // WINDOW_HPP

#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include "event/event.h"
#include "renderer/openGLContext.h"

namespace Engine {

	struct WindowSpecification
	{
		std::string title = "Engine";
		uint32_t width = 1600;
		uint32_t height = 900;
		bool decorated = true;
		bool fullscreen = false;
		bool vSync = true;
	};

	class Window {

	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowSpecification& specification = WindowSpecification());
		~Window();

		void onUpdate();

		inline int getHeight() { return m_data.height; }
		inline int getWidth() { return m_data.width; }

		// Window attributes
		void setEventCallback(const EventCallbackFn& callback) { m_data.eventCallback = callback; }
		void setVSync(bool enabled);
		bool isVSync() const;

		void* getNativeWindow() const { return m_window; }

		static Window* create(const WindowSpecification& specification = WindowSpecification());

	private:

		void init();
		void shutdown();

		void showFps();



	private:

		GLFWwindow* m_window;
		std::unique_ptr<OpenGLContext> m_context;
		WindowSpecification m_specification;

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

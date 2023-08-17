#pragma once

#include "window.h"
#include "layerStack.h"
#include "event/event.h"
#include "event/applicationEvent.h"
#include "imgui/imGuiLayer.h"
#include <cassert>
#include <mutex>

int main(int argc, char** argv);

namespace Engine {

	struct ApplicationCommandLineArgs
	{
		int count = 0;
		char** args = nullptr;

		const char* operator[](int index) const
		{
			assert(index < count);
			return args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string name = "Engine Application";
		std::string workingDirectory;
		ApplicationCommandLineArgs commandLineArgs;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void close();

		void onEvent(Engine::Event& e);

		void pushLayer(Engine::Layer* layer);
		void pushOverlay(Engine::Layer* layer);

		inline static Application& s_get() { return *s_instance; };

		inline Engine::Window& getWindow() { return *m_window; };

		const ApplicationSpecification& GetSpecification() const { return m_specification; }


	private:

		static Application* s_instance;

		void run();
		bool onWindowClose(Engine::WindowCloseEvent& e);
		bool onWindowResize(Engine::WindowResizeEvent& e);

		ApplicationSpecification m_specification;
		std::unique_ptr<Engine::Window> m_window;
		//layer stack decides when imguilayer gets destroyed and deleted
		Engine::ImGuiLayer* m_imGuiLayer;

		bool m_running = true;
		bool m_minimized = false;
		Engine::LayerStack m_layerStack;
		float m_lastFrameTime = 0.0f;

		std::vector<std::function<void()>> m_mainThreadQueue;
		std::mutex m_mainThreadQueueMutex;

		friend int ::main(int argc, char** argv);
	};
	// To be defined in CLIENT
	Application* createApplication(ApplicationCommandLineArgs args);
}


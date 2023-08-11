#include "application.h"
#include <memory>
#include <cassert>
#include "renderer/renderer.h"
#include "log.h"
#include "imgui/imGuiLayer.h"
#include <filesystem>

namespace Engine {


	Application* Application::s_instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_specification(specification)
	{

		assert(s_instance == nullptr);
		s_instance = this;

		// Set working directory here
		if (!m_specification.workingDirectory.empty())
			std::filesystem::current_path(m_specification.workingDirectory);

		m_window = std::unique_ptr<Engine::Window>(Engine::Window::create());
		m_window->setEventCallback(std::bind_front(&Application::onEvent, this)); //std::bind(&Application::onEvent

		Renderer::init();

		m_imGuiLayer = new Engine::ImGuiLayer();
		pushOverlay(m_imGuiLayer);
	}

	void Application::pushLayer(Engine::Layer* layer) {
		m_layerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Engine::Layer* layer) {
		m_layerStack.pushOverlay(layer);
		layer->onAttach();
	}

	void Application::close() {
		m_running = false;
	}

	void Application::onEvent(Engine::Event& e) {
		Engine::EventDispatcher dispatcher(e);
		//bind_front c++20 instead of bind+placeholders
		dispatcher.dispatch<Engine::WindowCloseEvent>(std::bind_front(&Application::onWindowClose, this));
		dispatcher.dispatch<Engine::WindowResizeEvent>(std::bind_front(&Application::onWindowResize, this));

		for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
		{
			if (e.handled)
				break;
			(*it)->onEvent(e);
		}
	}

	bool Application::onWindowClose(Engine::WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

	bool Application::onWindowResize(Engine::WindowResizeEvent& e)
	{
		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		Renderer::onWindowResize(e.getWidth(), e.getHeight());

		return false;
	}

	Application::~Application()
	{
		Renderer::shutdown();
	}

	void Application::run()
	{
		while (m_running)
		{
			float time = (float)glfwGetTime();
			Engine::Timestep timestep = time - m_lastFrameTime;
			m_lastFrameTime = time;

			executeMainThreadQueue();

			if (!m_minimized)
			{
				{
					for (Engine::Layer* layer : m_layerStack)
						layer->onUpdate(timestep);
				}

				m_imGuiLayer->begin();
				{

					for (Engine::Layer* layer : m_layerStack)
						layer->onImGuiRender();
				}
				m_imGuiLayer->end();
			}

			m_window->onUpdate();
		}
	}

	void Application::submitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_mainThreadQueueMutex);
		
		m_mainThreadQueue.emplace_back(function);
	}

	void Application::executeMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_mainThreadQueueMutex);
		
		for (auto& func : m_mainThreadQueue)
			func();
		
		m_mainThreadQueue.clear();
	}
}
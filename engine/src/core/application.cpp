#include "application.h"
#include <memory>
#include <cassert>
#include "renderer/renderer.h"
#include "log.h"
#include "imgui/imGuiLayer.h"

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{
	}
	void onUpdate(Engine::Timestep ts) override { Log::info("ExampleLayer::update"); };
	void onEvent(Engine::Event& event) override { Log::debug(std::format("{0}", event.toString())); }

};

Application* Application::s_instance = nullptr;

Application::Application() {
	assert(s_instance == nullptr);
	s_instance = this;
    m_window = std::unique_ptr<Engine::Window>(Engine::Window::create());
	m_window->setEventCallback(std::bind_front(&Application::onEvent,this)); //std::bind(&Application::onEvent
	
	pushLayer(new ExampleLayer());
	pushOverlay(new Engine::ImGuiLayer());

	run();
}

void Application::pushLayer(Engine::Layer* layer){
	m_layerStack.pushLayer(layer);
	layer->onAttach();
}

void Application::pushOverlay(Engine::Layer* layer) {
	m_layerStack.pushOverlay(layer);
	layer->onAttach();
}

void Application::close(){
	m_running = false;
}

void Application::onEvent(Engine::Event& e) {
    Engine::EventDispatcher dispatcher(e);
	//bind_front c++20 instead of bind+placeholders
	dispatcher.Dispatch<Engine::WindowCloseEvent>(std::bind_front(&Application::onWindowClose, this));
	dispatcher.Dispatch<Engine::WindowResizeEvent>(std::bind_front(&Application::onWindowResize, this));

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
	//Renderer::onWindowResize(e.GetWidth(), e.GetHeight());

	return false;
}

Application::~Application() {

}

void Application::run()
{
	while (m_running)
	{
		float time = glfwGetTime();
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

void Application::executeMainThreadQueue()
{
	//std::scoped_lock<std::mutex> lock(m_mainThreadQueueMutex);
	//
	//for (auto& func : m_MainThreadQueue)
	//	func();
	//
	//m_mainThreadQueue.clear();
}
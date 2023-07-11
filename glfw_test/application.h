#pragma once

#include "window.h"
#include "layerStack.h"
#include "event/event.h"
#include "event/applicationEvent.h"

class Application
{
public:
	Application();
	~Application();

	void onEvent(Engine::Event& e);

	void pushLayer(Engine::Layer* layer);
	void pushOverlay(Engine::Layer* layer);

private:

	void run();
	bool onWindowClose(Engine::WindowCloseEvent& e);
	bool onWindowResize(Engine::WindowResizeEvent& e);

	void executeMainThreadQueue();
	std::unique_ptr<Engine::Window> m_window;

	bool m_running = true;
	bool m_minimized = false;
	Engine::LayerStack m_layerStack;
	float m_lastFrameTime = 0.0f;
};


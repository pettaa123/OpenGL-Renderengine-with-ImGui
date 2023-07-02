#pragma once

#include "glad/glad.h"
#include "window.h"
#include <memory>

namespace Engine {

	class Renderer {

	public:
		Renderer();
		int init(const std::string& title, bool fullscreen);
		int runRenderLoop();

		void onEvent(Event& e);

	private:
		std::unique_ptr<Engine::Window> m_window;
		GLuint m_vertex_buffer, m_vertex_shader, m_fragment_shader, m_program;
		GLint m_mvp_location, m_vpos_location, m_vcol_location;
		GLuint VAO;
	};

}
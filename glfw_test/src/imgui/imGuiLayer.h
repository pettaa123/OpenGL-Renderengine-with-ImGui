#pragma once

#include "layer.h"

namespace Engine {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		void onAttach();
		void onDetach();
		void onEvent(Event& e);
	private:
		float m_time = 0.0f;
	};
}
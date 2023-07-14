#pragma once

#include "core/layer.h"

namespace Engine {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void onAttach() override;
		void onDetach() override;
		void onEvent(Event& e) override;
		void onImGuiRender() override;

		void begin();
		void end();

		void blockEvents(bool block) { m_blockEvents = block; }

		void setDarkThemeColors();

		uint32_t getActiveWidgetID() const;
	private:

		bool m_blockEvents = true;
	};
}
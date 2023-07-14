#pragma once

#include "core/keyCodes.h"
#include "core/mouseCodes.h"

#include <glm/glm.hpp>

namespace Engine {

	class Input
	{
	public:
		static bool s_isKeyPressed(KeyCode key);

		static bool s_isMouseButtonPressed(MouseCode button);
		static glm::vec2 s_getMousePosition();
		static float s_getMouseX();
		static float s_getMouseY();
	};
}

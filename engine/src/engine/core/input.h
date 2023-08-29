#pragma once

#include "engine/core/keyCodes.h"
#include "engine/core/mouseCodes.h"

#include <glm/glm.hpp>

namespace Engine {

	class Input
	{
	public:
		static bool isKeyPressed(KeyCode key);

		static bool isMouseButtonPressed(MouseCode button);
		static glm::vec2 getMousePosition();
		static float getMouseX();
		static float getMouseY();
	};
}

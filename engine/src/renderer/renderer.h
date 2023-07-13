#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace Engine {

	class Renderer {

	public:

		static void init();
		static void shutdown();

		static void onWindowResize(uint32_t width, uint32_t height);


	private:

		struct SceneData {
			glm::mat4 viewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_sceneData;
	};

}
#pragma once

#include "engine/renderer/renderCommand.h"
#include "engine/renderer/shader.h"
#include "engine/renderer/model.h"
#include "engine/renderer/camera.h"
#include "engine/renderer/orthographicCamera.h"


namespace Engine {

	class Renderer {

	public:

		static void init();

		static void onWindowResize(uint32_t width, uint32_t height);

		static void beginScene(OrthographicCamera& camera);
		static void beginScene(Camera& camera);
		static void endScene();

		static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void submit(const std::shared_ptr<Shader>& shader, Model* model, const glm::mat4& transform = glm::mat4(1.0f));

	private:

		struct SceneData {
			glm::mat4 viewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_sceneData;
	};

}
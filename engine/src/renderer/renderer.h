#pragma once

#include "renderer/renderCommand.h"
#include "renderer/shader.h"
#include "renderer/model.h"
#include "camera.h"
#include "orthographicCamera.h"


namespace Engine {

	class Renderer {

	public:

		static void init();

		static void onWindowResize(uint32_t width, uint32_t height);

		static void beginScene(OrthographicCamera& camera);
		static void beginScene(Camera& camera);
		static void endScene();

		static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void submit(const std::shared_ptr<Shader>& shader, const std::unique_ptr<Model>& model, const glm::mat4& transform = glm::mat4(1.0f));

	private:

		struct SceneData {
			glm::mat4 viewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_sceneData;
	};

}
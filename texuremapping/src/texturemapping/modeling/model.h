#pragma once

#include "engine/renderer/model.h"
#include "engine/renderer/texture.h"
#include "texturemapping/modeling/textureDescription.h"
#include <memory>

//wrapper class for the engine model, mostly because texturemapping expects that the model consists of only 1 mesh

namespace TextureMapping {

	class Model : public Engine::Model {
	public:

		Model(const std::string& path);

		void loadVertices();
		void loadNormals();
		inline const std::vector<glm::vec3>& getNormals() const { return m_normals; };
		inline const std::vector<glm::vec3> getVertices() const { return m_vertices; };
		inline size_t getNormalsCount() const { return m_normals.size(); };
		inline size_t getVertexCount() const { return m_vertices.size(); };

		inline size_t getVerticesCount() const { return m_vertices.size(); };

		TextureDescription textureDescription;

		void setTexture(std::shared_ptr<Texture> texture) {
			textureDescription.texture = texture;
		}

	private:

		std::vector<glm::vec3> m_vertices; //combined vertex positions
		std::vector<glm::vec3> m_normals; //combined normals

		

	};
}
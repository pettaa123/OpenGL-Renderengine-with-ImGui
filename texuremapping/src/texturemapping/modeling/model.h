#pragma once

#include "engine/renderer/model.h"
#include "engine/renderer/texture.h"
#include "texturemapping/modeling/textureDescription.h"
#include <memory>
#include <algorithm>

//wrapper class for the engine model, mostly because texturemapping expects that the model consists of only 1 mesh

namespace TextureMapping {


	class Model : public Engine::Model {
	public:

		Model(const std::string& path);

		void init();
		std::vector<Engine::Mesh::Vertex>& vertices() { return m_vertices; };
		inline size_t getVertexCount() const { return m_vertices.size(); };

		inline const std::vector<Engine::Mesh::Index>& getIndices() const { return m_indices; };
		inline size_t getIndexCount() const { return m_indices.size(); };

		void recreateMesh();

		TextureDescription textureDescription;

		void setTexture(std::shared_ptr<Texture>& texture) {
			textureDescription.texture = texture;		
		}

		void draw();

		const std::vector<float> getVertexCache() const;

	private:
		std::vector<Engine::Mesh::Vertex> m_vertices;
		std::vector<Engine::Mesh::Index> m_indices;

		

	};
}
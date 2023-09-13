#include "texturemapping/modeling/model.h"

namespace TextureMapping {

	Model::Model(const std::string& path)
		:Engine::Model(path)
	{
		init();
	}

	const std::vector<float> Model::getVertexCache() const {
		std::vector<float> verticesAsFloats(m_vertices.size() * 3);
		uint32_t idx = 0;

		for (const auto& vertex : m_vertices) {
			verticesAsFloats[idx++] = vertex.position.x;
			verticesAsFloats[idx++] = vertex.position.y;
			verticesAsFloats[idx++] = vertex.position.z;
		}
		return verticesAsFloats;
	}

	void Model::init() {

		auto acc_lambda = [&](uint32_t count, const Engine::Mesh& m) {return count + m.getVerticesCount(); };
		uint32_t count = std::accumulate(m_meshes.begin(), m_meshes.end(), 0, acc_lambda);
		m_vertices.reserve(count);


		for (auto& m : m_meshes) {
			m_indices.insert(m_indices.end(), m.getIndices().begin(), m.getIndices().end());
			auto vertexVec = m.getVertices();
			for (const auto& vertex : vertexVec)
				m_vertices.push_back(vertex);
		}

	}

	void Model::recreateMesh() {

		Engine::Model::init(m_vertices);

	}
}
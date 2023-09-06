#include "texturemapping/modeling/model.h"

namespace TextureMapping {

	Model::Model(const std::string& path)
		:Engine::Model(path)
	{
		loadVertices();
		loadNormals();
	}

	void Model::loadVertices() {

		auto acc_lambda = [&](uint32_t count, const Engine::Mesh& m) {return count + m.getVerticesCount(); };
		uint32_t count = std::accumulate(m_meshes.begin(), m_meshes.end(), 0, acc_lambda);
		m_vertices.reserve(count);

		for (auto& m : m_meshes) {
			for (auto& v : m.getVertices()) {
				m_vertices.push_back(v.position);
			}
		}
	}

	void Model::loadNormals() {
		auto acc_lambda = [&](uint32_t count, const Engine::Mesh& m) {return count + m.getVerticesCount(); };
		uint32_t count = std::accumulate(m_meshes.begin(), m_meshes.end(), 0, acc_lambda);
		m_normals.reserve(count);

		for (auto& m : m_meshes) {
			for (auto& v : m.getVertices()) {
				m_normals.push_back(v.normal);
			}
		}
	}
}
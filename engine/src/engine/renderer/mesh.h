#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "engine/renderer/buffer.h"
#include "engine/renderer/shader.h"
#include "engine/renderer/vertexArray.h"
#include "engine/renderer/buffer.h"
#include "engine/renderer/texture.h"

namespace Engine {

	class Mesh
	{
	public:

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec3 tangent;
			glm::vec3 binormal;
			glm::vec2 texCoords;
		};

		//struct Texture {
		//	uint32_t id;
		//	std::string type;
		//	std::string path;  // we store the path of the texture to compare with other textures
		//};

		struct Index
		{
			uint32_t v1, v2, v3;
		};

		std::vector<std::shared_ptr<Texture2D>> textures;

		static_assert(sizeof(Vertex) == 14 * sizeof(float));
		static const int numAttributes = 7;

		static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

		Mesh(std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<std::vector<Index>> indices,  std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer, std::vector<std::shared_ptr<Texture2D>> textures);
		Mesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<Texture2D> texture);
		Mesh(const std::string& filename);
		~Mesh();

		void setupMesh();

		uint32_t getIndicesCount() { return (uint32_t)m_indexBuffer->getCount();}

		uint32_t getVerticesCount() const { return (uint32_t)m_vertices->size(); }

		const std::vector<Index>& getIndices() const { return *m_indices; }

		const std::vector<Vertex>& getVertices() const { return *m_vertices; }

		std::shared_ptr<VertexArray> getVertexArray() { return m_VAO;}

		const std::string& getFilePath() const { return m_filePath; }

		void draw(Shader& shader);

	private:
		std::shared_ptr<std::vector<Vertex>> m_vertices;
		std::shared_ptr<std::vector<Index>> m_indices;
		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
		std::shared_ptr<Texture2D> m_texture;

		std::string m_filePath;

		std::shared_ptr<VertexArray> m_VAO;
	};
}

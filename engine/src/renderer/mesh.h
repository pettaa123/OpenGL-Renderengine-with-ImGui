#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "renderer/buffer.h"
#include "renderer/shader.h"
#include "renderer/vertexArray.h"

#include "renderer/buffer.h"
#include "renderer/Texture.h"

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

		Mesh(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer, std::vector<std::shared_ptr<Texture2D>> textures);
		Mesh(const std::string& filename);
		~Mesh();

		void setupMesh();

		inline void getCount() {
			m_indexBuffer->getCount();
		}
		inline std::shared_ptr<VertexArray> getVertexArray() {
			return m_VAO;
		}

		inline const std::string& getFilePath() const { return m_filePath; }

		void draw(Shader& shader);


	private:
		std::vector<Vertex> m_vertices;
		std::vector<Index> m_indices;
		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;

		std::string m_filePath;

		std::shared_ptr<VertexArray> m_VAO;
	};
}

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace TextureMapping {

	/// Data container for vertex information.
	class VertexInformation {
	public:

		VertexInformation(int vertexId, const glm::vec3& vertex, const glm::vec2& texCoord, const std::vector<glm::vec2>& underlayingTexCoords)
			:vertexIdx(vertexId),
			vertex(vertex),
			texCoord(texCoord),
			underlayingTexCoords(underlayingTexCoords)
		{}


		VertexInformation(int vertexId)
			:vertexIdx(vertexId),
			vertex(glm::vec3(0.0f)),
			texCoord(glm::vec2(0.0f)),
			underlayingTexCoords(std::vector<glm::vec2>())
		{}

		VertexInformation()
			:vertexIdx(-1),
			vertex(glm::vec3(0.0f)),
			texCoord(glm::vec2(0.0f)),
			underlayingTexCoords(std::vector<glm::vec2>())
		{}


		int vertexIdx;
		glm::vec3 vertex;
		glm::vec2 texCoord;
		std::vector<glm::vec2> underlayingTexCoords;

		bool operator==(const VertexInformation& rhs) const
		{
			return (vertexIdx == rhs.vertexIdx) &&
				glm::all(glm::equal(vertex, rhs.vertex)) &&
				glm::all(glm::equal(texCoord, rhs.texCoord)) &&
				(underlayingTexCoords == rhs.underlayingTexCoords);
				//std::equal(underlayingTexCoords.begin(), underlayingTexCoords.end(), [](auto& a, auto& b) {return glm::all(glm::equal(a, b)); });
		}

		bool operator<(const VertexInformation& other) const {
			// Define your comparison logic here
			// For example, compare based on vertexId
			return vertexIdx < other.vertexIdx;
		}
	};
}

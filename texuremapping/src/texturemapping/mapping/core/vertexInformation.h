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
		//this class gets used to split triangles, so actually important for comparison are the vertex coordinates
		bool operator==(const VertexInformation& rhs) const
		{
			return glm::all(glm::equal(vertex, rhs.vertex));
		}

		bool operator<(const VertexInformation& rhs) const {
			return glm::any(glm::lessThan(vertex, rhs.vertex));
		}
	};
}

#pragma once

#include <glm/glm.hpp>
#include "textureMapping/mapping/core/vertexInformation.h"

namespace TextureMapping {


	/// Data container for intersection results.
	class IntersectionResult {
	public:
		int sideIdx;
		VertexInformation vertexInfo;

		// Initializes a new instance of the <see cref="IntersectionResult"/> class.

		IntersectionResult(int sideIdx, glm::vec3 intersectionVertex, glm::vec2 intersectionTexCoords, std::vector<glm::vec2> underlayingIntersectionTexCoords)
			:sideIdx(sideIdx) {
			vertexInfo = VertexInformation(-1, intersectionVertex, intersectionTexCoords, underlayingIntersectionTexCoords);
		}

		IntersectionResult()
			:sideIdx(-1) {
		}

		bool operator==(const IntersectionResult& rhs) const {
			return (sideIdx == rhs.sideIdx) && (vertexInfo == rhs.vertexInfo);
		}

	};
}


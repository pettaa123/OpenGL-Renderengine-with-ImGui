#pragma once

#include <unordered_set>
#include <glm/glm.hpp>

namespace TextureMapping {

	/// Data container for projection results.
	struct ProjectionResult {
		std::unordered_set<int> triangleHash;
		std::pair<std::vector<glm::vec2>, std::vector<int>> trianglePair;
	};
}

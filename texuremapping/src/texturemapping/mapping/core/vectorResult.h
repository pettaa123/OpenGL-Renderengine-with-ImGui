#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace TextureMapping{

    /// Data container for vector results.
    struct VectorResult {

        std::vector<int> validIndices;
        std::vector<int> invalidIndices;
        std::vector<glm::vec2> validTextureCoordinates;
        std::vector<glm::vec2> invalidTextureCoordinates;
        std::vector<glm::vec3> validVertices;
        std::vector<glm::vec3> invalidVertices;
    };
}

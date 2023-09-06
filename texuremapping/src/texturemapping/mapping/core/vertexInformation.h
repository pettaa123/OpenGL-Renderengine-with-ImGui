#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace TextureMapping{

    /// Data container for vertex information.
    struct VertexInformation {
        int vertexIdx=0;
        glm::vec3 vertex{ 0.0f };
        glm::vec2 texCoord{ 0.0f };
        std::vector<glm::vec2> underlayingTexCoords;
    };
}

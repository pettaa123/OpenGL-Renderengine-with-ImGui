#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace TextureMapping{

    /// Data container for vertex information.
    struct VertexInformation {
        int vertexIdx;
        glm::vec3 vertex;
        glm::vec2 texCoord;
        std::vector<glm::vec2> underlayingTexCoords;
    };
}

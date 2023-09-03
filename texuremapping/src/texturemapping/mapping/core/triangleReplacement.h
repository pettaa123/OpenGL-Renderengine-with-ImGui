#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "texturemapping/core/mappingDataSet.h"

namespace TextureMapping{

    /// <summary>
    /// Data container for triangle replacements.
    /// </summary>
    struct TriangleReplacement {

        int triangleStartVertexId;
        std::vector<int> triangleArrayIndices;
        std::vector<glm::vec3> newVertices;
        std::vector<MappingDataSet> dataSets;
        std::vector<std::vector<glm::vec2>> newTextureCoordinates;
        std::vector<MappingDataSet> affectedDataSets;

    };
}

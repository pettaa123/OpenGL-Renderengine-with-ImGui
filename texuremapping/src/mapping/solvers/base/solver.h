
#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace TextureMapping{

    /// Interface for 2D/3D PnP solvers.

    class Solver {

    public:

        virtual ~Solver() = default;
        // pure virtual function
        virtual glm::mat3x4 calculateProjectionMatrix(glm::mat3 intrinsicsMat, std::vector<glm::vec3> modelPoints, std::vector<glm::vec2> imagePoints) = 0;
    };
}
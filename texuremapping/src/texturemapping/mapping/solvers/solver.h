#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "texturemapping/core/log.h"

namespace TextureMapping{

    enum class SolvingMethod {
        EPnPGN,
        EPnP,
        DLT,
        Robot,
        LHM
    };

    /// Interface for 2D/3D PnP solvers.

    class Solver {

    public:

        virtual ~Solver() = default;
        // pure virtual function
        virtual glm::mat3x4 calculateProjectionMatrix(const glm::mat3& intrinsicsMat, const std::vector<glm::vec3>& modelPoints, const std::vector<glm::vec2>& imagePoints) const = 0;
        static std::shared_ptr<Solver> create(const SolvingMethod solvingMethod, const bool normalizeData);
    };
}
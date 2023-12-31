#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "renderer/model.h"
#include "texturemapping/core/mappingDataSet.h"
#include "texturemapping/mapping/solvers/base/solver.h"
#include "texturemapping/optimization/optimizationResult.h"
#include "concurrentqueue/concurrentqueue.h"

namespace TextureMapping{

    /// <summary>
    /// Data container for optimization parameters.
    /// </summary>
    struct OptimizationParameters {

        glm::mat3 intrinsicsOpti;
        Engine::Model model;
        std::shared_ptr<Solver> solver;
        int minNumberOfPoints;
        ConcurrentQueue<OptimizationResult> resultQueue;
        std::vector<MappingDataSet> correspondences;

    };
}
#pragma once

#include <glm/glm.hpp>
#include <numeric>
#include <vector>

#include "texturemapping/optimization/errorDescription.h"
#include "texturemapping/optimization/optimizationResult.h"

namespace TextureMapping {

    /// Data container for optimization results.
    class OptimizationResult {
    public:

        OptimizationResult(glm::mat3x4 projectionMatrix, std::vector<ErrorDescription> errorDescriptions)
            : m_projectionMatrix(projectionMatrix),
            m_errorDescriptions(errorDescriptions)
        {
            auto avg_lambda = [&](float a, ErrorDescription b) {return a + b.error / errorDescriptions.size(); };
            m_averageErrorInPixels = std::accumulate(errorDescriptions.begin(), errorDescriptions.end(), 0.0f, avg_lambda);
        }

        const glm::mat3x4 getProjectionMatrix() { return m_projectionMatrix; }
        const float getAverageErrorInPixels() { return m_averageErrorInPixels; }
        std::vector<ErrorDescription> getErrorDescriptions() { return m_errorDescriptions; }

    private:
        std::vector<ErrorDescription> m_errorDescriptions;
        glm::mat3x4 m_projectionMatrix;
        float m_averageErrorInPixels=0;
    };
}

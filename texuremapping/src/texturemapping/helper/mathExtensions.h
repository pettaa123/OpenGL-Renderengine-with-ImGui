#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <cmath>
#include <limits>

namespace TextureMapping {
    class MathExtension {
    public:
        /// Calculates the centroid from an array of 3D vectors.
        static glm::vec3 calculateCentroid(const std::vector<glm::vec3>& vectors);

        /// Calculates the centroid from an array of 3D vectors.
        static glm::vec2 calculateCentroid(const std::vector<glm::vec2>& vectors);

        /// Multiplies a 3x3 matrix by a 3x4 matrix.
        static glm::mat3x4 mult(const glm::mat3& matrixA, const glm::mat3x4& matrixB);

        /// Multiplies a 3x4 matrix by a 4x4 matrix.
        static glm::mat3x4 mult(const glm::mat3x4& matrixA, const glm::mat4& matrixB);

        /// Multiplies a 3x4 matrix by a 3D vector.
        static glm::vec2 mult(const glm::mat3x4& matrix, const glm::vec3& vector);

        static std::vector<float> toVector(const glm::mat3x4& matrix);
    };
}
#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <cmath>
#include <limits>

namespace TextureMapping {
    class MathExtension {
    public:
        // initialize glm::mat3x4 from rotation matrix and translation vector;
        static glm::mat3x4 createProjectionMatrix(const glm::mat3& rotation_matrix, const glm::vec3& translation_vector);

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

        /// Determines whether a 2D point is within a triangle consisting of three vertices.
        static bool isPointInsideTriangle(const glm::vec2& point, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3);

        /// Returns a boolean according to the side Point 3 lies realtive to the line made up by Point 1 and Point 2.
        static bool pointRelativeToSide(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3);
    };
}
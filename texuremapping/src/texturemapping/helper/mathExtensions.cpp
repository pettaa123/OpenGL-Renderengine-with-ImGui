#include "texturemapping/helper/mathExtensions.h"



namespace TextureMapping {

    glm::vec3 MathExtension::calculateCentroid(const std::vector<glm::vec3>& vectors) {
        // Use double instead of float, the numbers could get quite large
        double centroidX = 0;
        double centroidY = 0;
        double centroidZ = 0;

        for (auto& p : vectors) {
            centroidX += p.x;
            centroidY += p.y;
            centroidZ += p.z;
        }

        // Don't do centroid = new ... before centroidXYZ /= VertexCount.
        // The division has to be performed before creating the vector, to make sure that the values are small enough for floats.
        size_t sze = vectors.size();
        centroidX /= sze;
        centroidY /= sze;
        centroidZ /= sze;

        return glm::vec3((float)centroidX, (float)centroidY, (float)centroidZ);
    };

    glm::vec2 MathExtension::calculateCentroid(const std::vector<glm::vec2>& vectors) {
        // Use double instead of float, the numbers could get quite large
        double centroidX = 0;
        double centroidY = 0;

        for (auto& p : vectors) {
            centroidX += p.x;
            centroidY += p.y;
        }

        // Don't do centroid = new ... before centroidXYZ /= VertexCount.
        // The division has to be performed before creating the vector, to make sure that the values are small enough for floats.
        size_t sze = vectors.size();
        centroidX /= sze;
        centroidY /= sze;

        return glm::vec2((float)centroidX, (float)centroidY);
    };

    /// Multiplies a 3x3 matrix by a 3x4 matrix.
    glm::mat3x4 MathExtension::mult(const glm::mat3& matrixA, const glm::mat3x4& matrixB) {
        glm::mat3x4 result;
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 4; col++) {
                for (int i = 0; i < 3; i++) {
                    result[row][col] += matrixA[row][i] * matrixB[i][col];
                }
            }
        }
        return result;
    };

    /// Multiplies a 3x4 matrix by a 4x4 matrix.
    glm::mat3x4 MathExtension::mult(const glm::mat3x4& matrixA, const glm::mat4& matrixB) {
        glm::mat3x4 result;
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 4; col++) {
                for (int i = 0; i < 4; i++) {
                    result[row][col] += matrixA[row][i] * matrixB[i][col];
                }
            }
        }
        return result;
    }

}
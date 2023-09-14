#include "texturemapping/helper/mathExtensions.h"


namespace TextureMapping {

    glm::mat3x4 MathExtension::createProjectionMatrix(const glm::mat3& rotation_matrix, const glm::vec3& translation_vector) {
        glm::mat3x4 projection_matrix;


        projection_matrix[0] = glm::vec4(rotation_matrix[0], translation_vector.x);
        projection_matrix[1] = glm::vec4(rotation_matrix[1], translation_vector.y);
        projection_matrix[2] = glm::vec4(rotation_matrix[2], translation_vector.z);


        return projection_matrix;
    }

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
        glm::mat3x4 result(0.0f);
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
        glm::mat3x4 result(0.0f);
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 4; col++) {
                for (int i = 0; i < 4; i++) {
                    result[row][col] += matrixA[row][i] * matrixB[i][col];
                }
            }
        }
        return result;
    }

    glm::vec2 MathExtension::mult(const glm::mat3x4& matrix, const glm::vec3& vector) {
        float x = matrix[0][0] * vector.x + matrix[0][1] * vector.y + matrix[0][2] * vector.z + matrix[0][3];
        float y = matrix[1][0] * vector.x + matrix[1][1] * vector.y + matrix[1][2] * vector.z + matrix[1][3];
        float w = matrix[2][0] * vector.x + matrix[2][1] * vector.y + matrix[2][2] * vector.z + matrix[2][3];
        x /= w;
        y /= w;

        glm::vec2 result(x, y);
        return result;
    }


    std::vector<float> MathExtension::toVector(const glm::mat3x4 & matrix) {
        std::vector<float> values(12);
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 4; x++) {
                values[x + 4 * y] = matrix[y][x];
            }
        }
        return values;
    }

    bool MathExtension::isPointInsideTriangle(const glm::vec2& point, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3) {
        bool b1 = pointRelativeToSide(point, v1, v2);
        bool b2 = pointRelativeToSide(point, v2, v3);
        bool b3 = pointRelativeToSide(point, v3, v1);
        bool isInside = b1 == b2 && b2 == b3;
        return isInside;
    }

    bool MathExtension::pointRelativeToSide(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) {
        float value = (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
        bool sideValue = value < 0;
        return sideValue;
    }

}
#pragma once

#include <glm/glm.hpp>

namespace TextureMapping {

	/// Barycentric coordinate.
	class BarycentricCoordinate {
	private:
		float m_u;
		float m_v;
		float m_w;

	public:
		// Initializes a new instance of the <see cref="BarycentricCoordinate"/> class.
		BarycentricCoordinate(float u, float v, float w) :
			m_u(u),
			m_v(v),
			m_w(w) {}

		/// <summary>
		/// Generates a barycentric coordinate from a triangle point.
		/// </summary>
		/// <param name="p">The p.</param>
		/// <param name="v0">First triangle vertex.</param>
		/// <param name="v1">Second triangle vertex.</param>
		/// <param name="v2">Third triangle vertex.</param>
		/// <returns></returns>
		static BarycentricCoordinate fromTrianglePoint(const glm::vec2& p, const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2) {
			glm::vec2 a = v1 - v0;
			glm::vec2 b = v2 - v0;
			glm::vec2 c = p - v0;

			float d00 = glm::dot(a, a);
			float d01 = glm::dot(a, b);
			float d11 = glm::dot(b, b);
			float d20 = glm::dot(c, a);
			float d21 = glm::dot(c, b);
			float denom = d00 * d11 - d01 * d01;

			float v = (d11 * d20 - d01 * d21) / denom;
			float w = (d00 * d21 - d01 * d20) / denom;
			float u = 1.0f - v - w;

			BarycentricCoordinate coordinate(u, v, w);
			return coordinate;
		}

		/// <summary>
		/// Converts the barycentric coordinate to a 2D vector.
		/// </summary>
		/// <param name="v0">First triangle vertex.</param>
		/// <param name="v1">Second triangle vertex.</param>
		/// <param name="v2">Third triangle vertex.</param>
		/// <returns></returns>
		glm::vec2 toVector2(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2) {
			// According to wikipedia, the values should be divided by the sum of u,v and w.
			// But the sum should be always 1
			float x = m_u * v0.x + m_v * v1.x + m_w * v2.x;
			float y = m_u * v0.y + m_v * v1.y + m_w * v2.y;
			glm::vec2 vector(x, y);
			return vector;
		}

		/// <summary>
		/// Converts the barycentric coordinate to a 3D vector.
		/// </summary>
		/// <param name="v0">First triangle vertex.</param>
		/// <param name="v1">Second triangle vertex.</param>
		/// <param name="v2">Third triangle vertex.</param>
		/// <returns></returns>
		glm::vec3 toVector3(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
			// According to wikipedia, the values should be divided by the sum of u,v and w.
			// But the sum should be always 1
			float x = m_u * v0.x + m_v * v1.x + m_w * v2.x;
			float y = m_u * v0.y + m_v * v1.y + m_w * v2.y;
			float z = m_u * v0.z + m_v * v1.z + m_w * v2.z;
			glm::vec3 vector(x, y, z);
			return vector;
		}
	};
}

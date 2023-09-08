#pragma once

#include "glm/glm.hpp"
#include <optional>

namespace TextureMapping {

	/// <summary>
	/// 2D ray respresentation
	/// </summary>
	class Ray2 {
	private:

		glm::vec2 m_origin;
		glm::vec2 m_direction;

	public:

		/// <summary>
		/// Initializes a new instance of the <see cref="Ray2"/> class.
		/// </summary>
		/// <param name="position">The position.</param>
		/// <param name="direction">The direction.</param>
		Ray2(glm::vec2 origin = glm::vec2(0.0f), glm::vec2 direction = glm::vec2(0.0f));

		inline glm::vec2 getOrigin() const { return m_origin; };
		inline glm::vec2 getDirection() const { return m_direction; };

		/// <summary>
		/// Gets the intersection of two rays.
		/// </summary>
		/// <param name="ray">The ray.</param>
		/// <returns></returns>
		std::optional<glm::vec2> getIntersectionWithRay(const Ray2& ray);

		float cross(const Ray2& other) const;

		float cross(const glm::vec2& v1, const glm::vec2& v2) const;

	};
}

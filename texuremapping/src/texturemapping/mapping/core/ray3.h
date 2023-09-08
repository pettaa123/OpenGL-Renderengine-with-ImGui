#pragma once

#include "glm/glm.hpp"
#include <optional>

namespace TextureMapping {

	/// 3D ray respresentation
	class Ray3 {
	private:

		glm::vec3 m_origin;
		glm::vec3 m_direction;

	public:

		/// <summary>
		/// Initializes a new instance of the <see cref="Ray2"/> class.
		/// </summary>
		/// <param name="position">The position.</param>
		/// <param name="direction">The direction.</param>
		Ray3(glm::vec3 origin=glm::vec3(0.0f), glm::vec3 direction = glm::vec3(0.0f));

		inline glm::vec3 getOrigin() const { return m_origin; };
		inline glm::vec3 getDirection() const { return m_direction; };

	};
}

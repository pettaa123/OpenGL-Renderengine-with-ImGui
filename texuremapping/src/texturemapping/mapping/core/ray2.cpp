#include "texturemapping/mapping/core/ray2.h"

namespace TextureMapping {

	Ray2::Ray2(glm::vec2 origin, glm::vec2 direction)
		:m_origin(origin),
		m_direction(direction) {
	}

	float Ray2::cross(const Ray2& other) const {
		return m_direction.x * other.getDirection().y - other.getDirection().x * m_direction.y;
	}

	float Ray2::cross(const glm::vec2& v1, const glm::vec2& v2) const {
		return v1.x * v2.y - v2.x * v1.y;
	}

	std::optional<glm::vec2> Ray2::getIntersectionWithRay(const Ray2& other) {

		float crossRS = this->cross(other);
		// The rays are parallel, colinear or coincident
		if (fabsf(crossRS) < FLT_EPSILON)
			return {};

		glm::vec2 p = m_origin;
		glm::vec2 q = other.getOrigin();

		float crossSR = other.cross(*this);

		float t = cross((q - p),other.getDirection()) / crossRS;
		float u = cross((p - q),m_direction) / crossSR;

		if (t < 0 || t > 1 || u < 0 || u > 1) {
			return {};
		}

		glm::vec2 intersection = p + t * m_direction;
		return intersection;
	}


}
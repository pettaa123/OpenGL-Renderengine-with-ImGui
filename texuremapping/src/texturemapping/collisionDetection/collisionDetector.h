#pragma once

#include <glm/vec3.hpp>
#include <optional>
#include "texturemapping/modeling/model.h"

namespace TextureMapping {

	struct Ray3;

	class CollisionDetector {

	public:

		static bool hitBackFaces;

		static std::optional<glm::vec3> getModelIntersection(TextureMapping::Model* model, glm::vec3 rayStart, glm::vec3 rayDirection, bool transformRay = true, bool transformHitPoint = true);

	private:
		/// <summary>
		/// Gets the model intersection from bounding boxes.
		/// </summary>
		/// <param name="model">The model.</param>
		/// <param name="ray">The ray.</param>
		/// <returns></returns>
		static std::optional<glm::vec3> getModelIntersectionFromBoundingBox(const Engine::Model* model, const Ray3& ray);

		/// <summary>
		/// Gets the model intersection from vertex iteration.
		/// </summary>
		/// <param name="model">The model.</param>
		/// <param name="ray">The ray.</param>
		/// <returns></returns>
		static std::optional<glm::vec3> getModelIntersectionFromVertexIteration(TextureMapping::Model* model,const Ray3& ray);

		/// <summary>
		/// Gets the intersection point between a triangle and a point.
		/// http://hugi.scene.org/online/hugi25/hugi%2025%20-%20coding%20corner%20graphics,%20sound%20&%20synchronization%20ken%20ray-triangle%20intersection%20tests%20for%20dummies.htm
		/// </summary>
		/// <param name="v0">First triangle vertex.</param>
		/// <param name="v1">Second triangle vertex.</param>
		/// <param name="v2">Third triangle vertex.</param>
		/// <param name="ray">The ray.</param>
		/// <returns></returns>
		static std::optional<glm::vec3> getHitPointOnTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const Ray3& ray);

	};
}
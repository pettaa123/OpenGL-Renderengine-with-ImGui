#include "texturemapping/collisionDetection/collisionDetector.h"
#include <glm/vec3.hpp>

namespace TextureMapping {

	/// 3D ray respresentation
	struct Ray3 {
		glm::vec3 origin;
		glm::vec3 direction;
	};

	bool CollisionDetector::hitBackFaces = true;

	std::optional<glm::vec3> CollisionDetector::getModelIntersection(TextureMapping::Model* model, glm::vec3 rayStart, glm::vec3 rayDirection, bool transformRay, bool transformHitPoint) {
		//if (model->primitiveType != PrimitiveType.Triangles) {
		//    return null;
		//    //throw new Exception("The intersection method only works with triangles.");
		//}

		// The model actually already created a rotation matrix, but it is a Matrix4, we need Matrix3. It is easier to calculate a new one here.
		//Matrix3 rotMatrix = Matrix3.CreateFromQuaternion(model.RotationQuaternion);
		float size = model->size;

		glm::quat modelRot = model->rotationQuaternion;

		if (transformRay) {
			glm::quat inversedModelRot = glm::inverse(model->rotationQuaternion);
			//Matrix3 inverseRotMatrix = rotMatrix.Inverted();
			//inverseRotMatrix.Transpose();

			rayStart = rayStart - model->position;
			//rayStart = inverseRotMatrix.Mult(rayStart);
			rayStart = inversedModelRot * rayStart;
			//rayStart /= size;
			rayDirection = inversedModelRot * rayDirection;
		}

		TextureMapping::Ray3 ray(rayStart, rayDirection);


		std::optional<glm::vec3> hitPoint;
		if (model->boundingBoxTree == nullptr) {
			hitPoint = getModelIntersectionFromVertexIteration(model, ray);
		}
		else {
			hitPoint = getModelIntersectionFromBoundingBox(model, ray);
		}

		//watch.Stop();
		//Console.WriteLine($"Casting took {watch.ElapsedMilliseconds} ms");

		if (hitPoint.has_value()) {
			// The size is not affected by the parent model matrix (if the intersection model is a child of it)
			if (transformRay) {
				hitPoint = size * hitPoint.value();
			}

			if (transformHitPoint) {
				// TODO: Why is the inverted rotation matrix needed? Maybe use Vector.Transform?
				hitPoint = glm::inverse(modelRot) * hitPoint.value();
				hitPoint = hitPoint.value() + model->position;
			}
		}

		return hitPoint;

	};


	std::optional<glm::vec3> CollisionDetector::getHitPointOnTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const Ray3& ray) {
		glm::vec3 e2 = v2 - v0;
		glm::vec3 e1 = v1 - v0;
		glm::vec3 r = glm::cross(ray.direction, e2);
		glm::vec3 s = ray.origin - v0;
		glm::vec3 q = glm::cross(s, e1);

		float u = glm::dot(s, r);
		float a = glm::dot(e1, r);
		float f = 1 / a;

		// Front facing triangle
		if (a > FLT_EPSILON) {
			if (u < 0 || u > a) {
				return {};
			}
			float v = glm::dot(ray.direction, q);
			if (v < 0 || u + v > a) {
				return {};
			}
		}
		// Back facing triangle
		else if (CollisionDetector::hitBackFaces && a < -FLT_EPSILON) {
			if (u > 0 || u < a) {
				return {};
			}
			float v = glm::dot(ray.direction, q);
			if (v > 0 || u + v < a) {
				return {};
			}
		}
		else {
			return {};
		}

		float t = f * glm::dot(e2, q);
		std::optional<glm::vec3> hitPoint = ray.origin + t * ray.direction;

		return hitPoint;
	}


	std::optional<glm::vec3> CollisionDetector::getModelIntersectionFromVertexIteration(TextureMapping::Model* model, const Ray3& ray) {
		std::optional<glm::vec3> hitPoint;
		std::vector<Engine::Mesh::Vertex>& vertices = model->vertices();
		for (size_t i = 0; i < vertices.size(); i += 3) {
			hitPoint = getHitPointOnTriangle(vertices[i].position, vertices[i + 1].position, vertices[i + 2].position, ray);
			if (hitPoint.has_value())
				break;
		}
		return  hitPoint;
	}

	std::optional<glm::vec3> CollisionDetector::getModelIntersectionFromBoundingBox(const Engine::Model* model, const TextureMapping::Ray3& ray) {
		// TODO: Do this recursively
		/*
		std::vector<BoundingBoxOctree> hitBoxes = model->boundingBoxTree->getHitBoxes(ray);
		hitBoxes = SortHitBoxes(hitBoxes, ray);

		foreach(BoundingBoxOctree box in hitBoxes) {
			List<Vector3> hitPoints = new List<Vector3>();

			for (int i = 0; i < box.IncludedTriangleIds.Count; i++) {
				int idx = box.IncludedTriangleIds[i] * 3;
				Vector3 v0 = model.Vertices[idx];
				Vector3 v1 = model.Vertices[idx + 1];
				Vector3 v2 = model.Vertices[idx + 2];

				Vector3 ? hitPoint = GetHitPointOnTriangle(v0, v1, v2, ray);
				if (hitPoint != null) {
					hitPoints.Add(hitPoint.Value);
				}
			}

			if (hitPoints.Count > 0) {
				Vector3 closesHitPoint = GetClosestHitPoint(hitPoints, ray);
				return closesHitPoint;
			}
		}

		return null;
		*/
		return {};
	}
}
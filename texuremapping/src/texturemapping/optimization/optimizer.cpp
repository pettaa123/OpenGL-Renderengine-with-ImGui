#include "texturemapping/optimization/optimizer.h"

namespace TextureMapping {

	std::shared_ptr<OptimizationResult> Optimizer::analyseProjectionMatrix(Model model, MappingDataSet correspondence, glm::mat3x4 projectionMatrix) {
		size_t numberOfPoints = correspondence.modelPoints.size();
		std::vector<glm::vec3> modelPoints = correspondence.modelPoints;
		std::vector<glm::vec2> imagePoints = correspondence.imagePoints;
		std::vector<ErrorDescription> errorsDescriptions(numberOfPoints);

		for (size_t i = 0; i < numberOfPoints; i++) {
			/*
			Vector3? hitPoint = LineProjector.GetIntersectionWithModel(model, projectionMatrix, imagePoints[i], false);
			if (hitPoint == null) {
				return null;
			}
			*/

			glm::vec2 pixel = MathExtension::mult(projectionMatrix, modelPoints[i]);
			float error = glm::length(pixel - imagePoints[i]);
			errorsDescriptions[i] = ErrorDescription(error);
		}
		return std::make_shared<OptimizationResult>(projectionMatrix, errorsDescriptions);
	}

}
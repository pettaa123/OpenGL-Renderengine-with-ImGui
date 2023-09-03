#include "texturemapping/mapping/core/undistorter.h"

namespace TextureMapping {

	std::vector<glm::vec2> Undistorter::undistortPoints(const MappingDataSet& dataSet, const Intrinsics& intrinsics) {
		float k1 = intrinsics.k1;
		float k2 = intrinsics.k2;
		float p1 = intrinsics.p1;
		float p2 = intrinsics.p2;
		float k3 = intrinsics.k3;
		glm::mat3 invIntrinsics = intrinsics.toMat3inv();
		std::vector<glm::vec2> undistortedPoints;
		undistortedPoints.reserve(dataSet.imagePoints.size());

		for (size_t i = 0; i < dataSet.imagePoints.size(); i++) {
			glm::vec3 point(dataSet.imagePoints[i].x, dataSet.imagePoints[i].y, 1.0f);
			glm::vec3 zPlanePoint = invIntrinsics * point;
			float x = zPlanePoint.x;
			float y = zPlanePoint.y;

			float r2 = powf(x, 2) + powf(y, 2);
			float r4 = powf(r2, 2);
			float r6 = powf(r4, 2);
			float distortedX = x * (1 + k1 * r2 + k2 * r4 + k3 * r6) + 2 * p1 * x * y + p2 * (r2 + 2 * x * x);
			float distortedY = y * (1 + k1 * r2 + k2 * r4 + k3 * r6) + 2 * p2 * x * y + p1 * (r2 + 2 * y * y);

			glm::vec3 distorted(distortedX, distortedY, 1.0f);
			glm::vec3 imagePlanePoint = intrinsics.toMat3() * distorted;
			glm::vec3 undistortedPoint = (point + (imagePlanePoint - point) * 2.0f);
			undistortedPoints.push_back(glm::vec2(undistortedPoint.x, undistortedPoint.y));

			bool pointStillInImage = (
				imagePlanePoint.x > 0 && imagePlanePoint.x <= dataSet.loadedImage.width &&
				imagePlanePoint.y > 0 && imagePlanePoint.y <= dataSet.loadedImage.height);

			assert(pointStillInImage && "After undistorting, one or more image points are now located outside the image.");
		}

		return undistortedPoints;
	};
}

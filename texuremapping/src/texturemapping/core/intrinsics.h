#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace TextureMapping {
	class Intrinsics {
	public:
		float fx, fy;
		float cx, cy;
		float k1, k2;
		float p1, p2;
		float k3;
		uint32_t width, height;
		uint32_t focusPos;
		inline glm::mat3 toMat3() const {
			return glm::mat3{
				fx,0,cx,
				0,fy,cy,
				0,0,1
			};
		}
		inline glm::mat3 toMat3inv() const {
			return glm::inverse(toMat3());
		};

		inline std::vector<float> distortionCoeffs() const {
			return std::vector<float>{k1, k2, p1, p2, k3};
		}
	};
}
#pragma once

namespace TextureMapping {
	struct Intrinsics {
		float fx, fy;
		float cx, cy;
		int width, height;
		float k1, k2, k3, k4, k5, k6;
		float p1, p2;
	};
}
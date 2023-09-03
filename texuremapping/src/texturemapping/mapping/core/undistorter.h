#pragma once

#include "texturemapping/core/mappingDataSet.h"
#include "texturemapping/core/intrinsics.h"

namespace TextureMapping {

	class Undistorter {
	public:
		static std::vector<glm::vec2> undistortPoints(const MappingDataSet& dataSet,const Intrinsics& intrinsics);
	};
}
#pragma once

#include <cstdint>
#include <memory>
#include "baseLib/stbImage.h"


namespace MarkerLib {
	// interface
	class MarkerFinder
	{
		class MarkerFinderImpl;
		std::unique_ptr<MarkerFinderImpl> m_markerFinder;
	public:
		MarkerFinder();
		~MarkerFinder();
		MarkerFinder& operator=(const MarkerFinder&) = delete;
		MarkerFinder(const MarkerFinder&) = delete;
		//pointBuf form [n][2]
		int selectAndRefineCorners(const BaseLib::STBimage& stbImage, float* pointBuf, uint32_t pointBufLen);
		int refineCorner(const BaseLib::STBimage& stbImage, float* pointX, float* pointY, int32_t winSize=5, int32_t zeroZone=-1, int32_t maxIterations=100, double epsilon=0.01);
		BaseLib::STBimage clampImage(const BaseLib::STBimage& stbImage, uint16_t lowerBound, uint16_t upperBound);
	};

}

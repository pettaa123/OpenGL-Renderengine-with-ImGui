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
		BaseLib::STBimage clampImage(const BaseLib::STBimage& stbImage, uint16_t lowerBound, uint16_t upperBound);

	};

}

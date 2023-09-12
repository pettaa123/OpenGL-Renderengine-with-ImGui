#pragma once

#include <memory>

namespace MarkerLib {

	enum struct SolvePnPMethod:int {
		SOLVEPNP_ITERATIVE = 0,
		SOLVEPNP_EPNP = 1,
		SOLVEPNP_P3P = 2,
		SOLVEPNP_DLS = 3,
		SOLVEPNP_UPNP = 4,
		SOLVEPNP_AP3P = 5,
		SOLVEPNP_IPPE = 6,
		SOLVEPNP_IPPE_SQUARE = 7,
		SOLVEPNP_SQPNP = 8
	};

	class PoseEstimator {
	public:

		PoseEstimator();
		~PoseEstimator();
		PoseEstimator(const PoseEstimator&) = delete;

		//objPts [n][3], imgPts [n][2], camMatrix float[9], distCoeffs [5], rvec[3], tvec[3], repErrs[n] 
		int estimate(const float* objPts,const uint32_t objPtsLen,const float* imgPts,const uint32_t imgPtsLen,const float* camMatrix,const float* distCoeffs, float* rvec, float* tvec,
			bool useExtrinsicGuess = false,const int flags = 0, float* repErrs=nullptr,const uint32_t repErrsLer=0);
	private:
	// pimpl - private code is in .cpp

	private:
		class PoseEstimatorImpl;
		std::unique_ptr<PoseEstimatorImpl> m_poseEstimator;
	};

}
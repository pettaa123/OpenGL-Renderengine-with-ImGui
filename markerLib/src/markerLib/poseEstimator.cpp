#include "markerLib/poseEstimator.h"
#include <opencv2/opencv.hpp>

namespace MarkerLib {

	class PoseEstimator::PoseEstimatorImpl {
	public:
		~PoseEstimatorImpl() = default;
		PoseEstimatorImpl() {};

		//int estimate(const std::vector<cv::Point3f>& objectPoints, const std::vector<cv::Point2f>& imagePoints, const std::vector<double>& cameraMatrix, const std::vector<float>& distCoeffs, cv::Vec<float, 3>& rvec, cv::Vec<float, 3>& tvec, bool useExtrinsicGuess, int flags) {
		int estimate(const std::vector<cv::Point3f>&objectPoints, const std::vector<cv::Point2f>&imagePoints, const cv::Mat& cameraMatrix, const std::vector<float>& distCoeffs, std::vector<float>& rvec, std::vector<float>& tvec, bool useExtrinsicGuess, int flags) {

			if (!cv::solvePnP(objectPoints,imagePoints,cameraMatrix, distCoeffs,rvec,tvec,false,flags))
				return EXIT_FAILURE;

			return EXIT_SUCCESS;
		}
	};

	//*********************************************
	//definitions only for PoseEstimator

	PoseEstimator::~PoseEstimator() = default;

	PoseEstimator::PoseEstimator()
		:m_poseEstimator(std::make_unique<PoseEstimatorImpl>())
	{}

	int PoseEstimator::estimate(const float* objPts, uint32_t objPtsLen, const float* imgPts, uint32_t imgPtsLen, const float* camMatrix, const float* distCoeffs, float* rvec, float* tvec,bool useExtrinsicGuess, int flags) {

		uint32_t objLen = objPtsLen / 3 / sizeof(float);
		std::vector<cv::Point3f> objectPoints(objLen);
		for (uint32_t i = 0; i < objLen; i++) {
			objectPoints[i].x = objPts[i * 3];
			objectPoints[i].y = objPts[i * 3 + 1];
			objectPoints[i].z = objPts[i * 3 + 2];
		}


		uint32_t imgLen = imgPtsLen / 2 / sizeof(float);
		assert(imgLen == objLen);
		std::vector<cv::Point2f> imagePoints(imgLen);
		for (uint32_t i = 0; i < imgLen; i++) {
			imagePoints[i].x = imgPts[i * 2];
			imagePoints[i].y = imgPts[i * 2 + 1];
		}

		cv::Mat K(3,3,CV_32F,const_cast<float*>(camMatrix));

		std::vector<float> distortionCoeffs(distCoeffs, distCoeffs + 5);		

		std::vector<float> rVec(3);
		std::vector<float> tVec(3);

		if (m_poseEstimator->estimate(objectPoints, imagePoints, K, distortionCoeffs, rVec, tVec, false, flags))
			return EXIT_FAILURE;

		std::memcpy(tVec.data(), tvec, sizeof(float) * 3);

		cv::Mat rotation_matrix(3, 3, CV_32F);
		cv::Rodrigues(rVec, rotation_matrix);

		std::memcpy(rotation_matrix.data, rvec, sizeof(float) * 9);


		return EXIT_SUCCESS;
	}
}



#include "markerLib/poseEstimator.h"
#include <opencv2/opencv.hpp>

namespace MarkerLib {

	class PoseEstimator::PoseEstimatorImpl {
	public:
		~PoseEstimatorImpl() = default;
		PoseEstimatorImpl() {};

		//int estimate(const std::vector<cv::Point3f>& objectPoints, const std::vector<cv::Point2f>& imagePoints, const std::vector<double>& cameraMatrix, const std::vector<float>& distCoeffs, cv::Vec<float, 3>& rvec, cv::Vec<float, 3>& tvec, bool useExtrinsicGuess, int flags) {
		int estimate(const std::vector<cv::Point3f>&objectPoints, const std::vector<cv::Point2f>&imagePoints, const cv::Mat& cameraMatrix, const std::vector<float>& distCoeffs, std::vector<float>& rvec, std::vector<float>& tvec, bool useExtrinsicGuess, int flags) {

			if (!cv::solvePnP(objectPoints,imagePoints,cameraMatrix, distCoeffs,rvec,tvec, useExtrinsicGuess,flags))
				return EXIT_FAILURE;

			return EXIT_SUCCESS;
		}

		float calcReprojectionError(const std::vector<cv::Point3f>& objectPoints, const std::vector<cv::Point2f>& imagePoints,  std::vector<float>& rvec, std::vector<float>& tvec, const cv::Mat& cameraMatrix, const std::vector<float>& distCoeffs) {
			std::vector<cv::Point2f> repPts(imagePoints.size());
			cv::projectPoints(objectPoints, rvec, tvec, cameraMatrix, distCoeffs, repPts);
			assert(imagePoints.size() == objectPoints.size());
			std::cout << "reprojection error x/y (px)" << std::endl;
			for (int i = 0; i < imagePoints.size(); i++) {
				std::cout << "pt" << i << ": " << repPts[i].x - imagePoints[i].x << " / " << repPts[i].y - imagePoints[i].y << std::endl;
			}
			//rmse
			return (float)cv::norm(repPts, imagePoints, cv::NORM_L2) / sqrt(2 * repPts.size());
		}
	};

	//*********************************************
	//definitions only for PoseEstimator

	PoseEstimator::~PoseEstimator() = default;

	PoseEstimator::PoseEstimator()
		:m_poseEstimator(std::make_unique<PoseEstimatorImpl>())
	{}

	int PoseEstimator::estimate(const float* objPts,const uint32_t objPtsLen, const float* imgPts,const uint32_t imgPtsLen, const float* camMatrix, const float* distCoeffs, float* rmat, float* tvec,bool useExtrinsicGuess, int flags,float* repErrs,const uint32_t repErrsLen) {

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

		if (m_poseEstimator->estimate(objectPoints, imagePoints, K, distortionCoeffs, rVec, tVec, useExtrinsicGuess, flags))
			return EXIT_FAILURE;

		std::memcpy(tvec, tVec.data(), sizeof(float) * 3);

		cv::Mat rotation_matrix(3, 3, CV_32F);
		cv::Rodrigues(rVec, rotation_matrix);

		std::memcpy(rmat, rotation_matrix.data, sizeof(float) * 9);

		//************************************************************
		//calculate reprojection errror(s) depending of the number of projection matrix solutions
		std::vector<float> repErrVec(1);
		repErrVec[0]= (m_poseEstimator->calcReprojectionError(objectPoints, imagePoints, rVec, tVec, K, distortionCoeffs));

		std::memcpy(repErrs, repErrVec.data() , sizeof(float));



		return EXIT_SUCCESS;
	}
}



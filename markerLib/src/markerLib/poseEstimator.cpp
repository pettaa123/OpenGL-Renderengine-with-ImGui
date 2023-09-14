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

		std::cout << K;

		std::vector<float> distortionCoeffs(distCoeffs, distCoeffs + 5);		

		std::vector<float> rVec_(3);
		std::vector<float> tVec_(3);

		if (m_poseEstimator->estimate(objectPoints, imagePoints, K, distortionCoeffs, rVec_, tVec_, useExtrinsicGuess, flags))
			return EXIT_FAILURE;

		std::memcpy(tvec, tVec_.data(), sizeof(float) * 3);

		std::cout << tvec[0];
		std::cout << tvec[1];
		std::cout << tvec[2];

		cv::Mat rotation_matrix(3, 3, CV_32F);
		cv::Rodrigues(rVec_, rotation_matrix);

		std::cout << "r: " << rotation_matrix;

		std::memcpy(rmat, (float*)rotation_matrix.data, sizeof(float) * 9);

		std::cout << "0: " << ((float*)rotation_matrix.data)[0] << std::endl;
		std::cout << "1: " << ((float*)rotation_matrix.data)[1] << std::endl;
		std::cout << "2: " << ((float*)rotation_matrix.data)[2] << std::endl;
		std::cout << "3: " << ((float*)rotation_matrix.data)[3] << std::endl;
		std::cout << "4: " << ((float*)rotation_matrix.data)[4] << std::endl;
		std::cout << "5: " << ((float*)rotation_matrix.data)[5] << std::endl;
		std::cout << "6: " << ((float*)rotation_matrix.data)[6] << std::endl;
		std::cout << "7: " << ((float*)rotation_matrix.data)[7] << std::endl;
		std::cout << "8: " << ((float*)rotation_matrix.data)[8] << std::endl;

		//************************************************************
		//calculate reprojection errror(s) depending of the number of projection matrix solutions
		std::vector<float> repErrVec(1);
		repErrVec[0]= (m_poseEstimator->calcReprojectionError(objectPoints, imagePoints, rVec_, tVec_, K, distortionCoeffs));

		std::memcpy(repErrs, repErrVec.data() , sizeof(float));



		return EXIT_SUCCESS;
	}
}



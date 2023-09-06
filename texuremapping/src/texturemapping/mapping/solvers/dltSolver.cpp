#pragma once

#include "texturemapping/mapping/solvers/dltSolver.h"
#include "texturemapping/helper/mathExtensions.h"
#include <cmath>
#include <Eigen/Dense>
#include <iostream>
#include "texturemapping/core/log.h"
//----------------------------------
#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace TextureMapping {

	DLTSolver::DLTSolver(bool normalizeData)
		:m_normalizeData(normalizeData){}

	glm::mat3x4 DLTSolver::calculateProjectionMatrix(const glm::mat3& intrinsics, const std::vector<glm::vec3>& modelPoints_in, const std::vector<glm::vec2>& imagePoints_in) const {

		glm::mat4 modelNormalizationMatrix;
		glm::mat3 imageNormalizationMatrix;

		std::vector<glm::vec3> modelPoints;
		std::vector<glm::vec2> imagePoints;

		if (m_normalizeData) {
			modelNormalizationMatrix = getNormalizationMatrix(modelPoints_in);
			//std::cout << glm::to_string(modelNormalizationMatrix) << std::endl;
			imageNormalizationMatrix = getNormalizationMatrix(imagePoints_in);
			//std::cout << glm::to_string(imageNormalizationMatrix) << std::endl;
			modelPoints = applyNormalization(modelNormalizationMatrix, modelPoints_in);
			//std::cout << glm::to_string(modelPoints[0]) << std::endl;
			imagePoints = applyNormalization(imageNormalizationMatrix, imagePoints_in);
			//std::cout << glm::to_string(imagePoints[0]) << std::endl;
		}
		else
		{
			modelPoints = modelPoints_in;
			imagePoints = imagePoints_in;
		}
		
		//////////////////////////////////////CREATE MATRIX


		size_t rows = modelPoints.size()*2;
		Eigen::Matrix<double, Eigen::Dynamic, 12, Eigen::RowMajor> mat;
		mat.resize(rows, 12);
		mat.setZero();
		int ii = 0;
		for (size_t i = 0; i < rows; i += 2) {			
			glm::vec3 m = modelPoints[ii];
			glm::vec2 p = imagePoints[ii];
			Eigen::Matrix<double, 1, 12,Eigen::RowMajor> r;
			r << (double)m.x, (double)m.y, (double)m.z, 1, 0, 0, 0, 0, -(double)p.x * (double)m.x, -(double)p.x * (double)m.y, -(double)p.x * (double)m.z, -(double)p.x;
			mat.block<1, 12>(i, 0) = r;
			Eigen::Matrix<double, 1, 12, Eigen::RowMajor> r2;
			r2 << 0, 0, 0, 0, (double)m.x, (double)m.y, (double)m.z, 1, -(double)p.y * (double)m.x, -(double)p.y * (double)m.y, -(double)p.y * (double)m.z, -(double)p.y;
			mat.block<1, 12>(i+1,0) = r2;
			ii++;
		};
		//std::cout << mat << std::endl;
		///////////////////////////////////////////////////

		Eigen::MatrixXd lastColumn;

		try {
			Eigen::JacobiSVD<Eigen::Matrix<double,Eigen::Dynamic,12,Eigen::RowMajor>> svd = mat.jacobiSvd(Eigen::ComputeFullU | Eigen::ComputeFullV);
			Eigen::MatrixXd v = svd.matrixV();
			//std::cout << svd.singularValues() << std::endl;
			//std::cout << svd.matrixU() << std::endl;
			//std::cout << v << std::endl;
			lastColumn = v.col(v.cols() - 1);
		}
		catch (std::exception& e){
			Log::error(e.what());
		}
		//std::cout << lastColumn << std::endl;
		//Svd<float> svd = matrix.Svd();
		//Matrix<float> v = svd.VT.Transpose();
		
		//Vector<float> lastColumn = v.Column(v.ColumnCount - 1);
	    
		//Matrix3x4 projectionMatrix = ReshapeVectorToMatrix(lastColumn);

		glm::mat3x4 projectionMatrix;
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 4; col++) {
				projectionMatrix[row][col] = (float)lastColumn.coeff(row*4+col,0);
			}
		}

		//std::cout << glm::to_string(projectionMatrix) << std::endl;

		if (m_normalizeData) {
			//projectionMatrix = imageNormalizationMatrix.Inverted().Mult(projectionMatrix).Mult(modelNormalizationMatrix);			
			projectionMatrix = MathExtension::mult(MathExtension::mult(glm::inverse(imageNormalizationMatrix), projectionMatrix), modelNormalizationMatrix);
		}
		//std::cout << glm::to_string(projectionMatrix) << std::endl;
		projectionMatrix = projectionMatrix / (projectionMatrix[2][3]);

		//Tuple<Matrix3x4, Matrix<float>, Matrix<float>, Vector<float>> decomp = internalParameters(projectionMatrix);


		//std::cout << glm::to_string(projectionMatrix) << std::endl;
		return projectionMatrix;

	}
	
	//Tuple<Matrix3x4, Matrix<float>, Matrix<float>, Vector<float>> InternalParameters(Matrix3x4 matrix) {
	//	/*Matrix<float> projectionMatrix = MatrixToDenseMatrix(matrix);
	//	Matrix<float> threeByThree = SliceMatrix(projectionMatrix, 3, 3);
	//	Tuple<Matrix<float>, Matrix<float>> rq = RQ(threeByThree);
	//
	//	Matrix<float> K = rq.Item1;
	//	Matrix<float> R = rq.Item2;
	//
	//	K = K / K[2, 2];*/
	//	//PETER Herbert Süße S. 356
	//
	//	Matrix<float> projectionMatrix = MatrixToDenseMatrix(matrix);
	//	Matrix<float> A = SliceMatrix(projectionMatrix, 3, 3);
	//	Matrix<float> M = A.Inverse();
	//	M = M.Multiply(-1);
	//	Vector<float> C = M.Multiply(projectionMatrix.Column(3)); //Camera center
	//	A = A.Inverse();
	//	QR<float> B = A.QR(QRMethod.Full);
	//	Matrix<float> K = B.R.Inverse();
	//	Matrix<float> R = B.Q.Transpose();
	//	Vector<float> t = K.Inverse().Multiply(projectionMatrix.Column(3));  //t=K^-1*a
	//
	//	return new Tuple<Matrix3x4, Matrix<float>, Matrix<float>, Vector<float>>(matrix, K, R, t);
	//}


	glm::mat3 DLTSolver::getNormalizationMatrix(const std::vector<glm::vec2>& data) const{
		glm::vec2 centroid = MathExtension::calculateCentroid(data);
		float scaling = getNormalizationScaling(data, centroid);
		glm::mat3 normalizationMatrix(
			scaling, 0, -scaling * centroid.x,
			0, scaling, -scaling * centroid.y,
			0, 0, 1);
		return normalizationMatrix;
	}

	glm::mat4 DLTSolver::getNormalizationMatrix(const std::vector<glm::vec3>& data) const {
		glm::vec3 centroid = MathExtension::calculateCentroid(data);
		float scaling = getNormalizationScaling(data, centroid);
		glm::mat4 normalizationMatrix(
			scaling, 0, 0, -scaling * centroid.x,
			0, scaling, 0, -scaling * centroid.y,
			0, 0, scaling, -scaling * centroid.z,
			0, 0, 0, 1);
		return normalizationMatrix;
	}


	float DLTSolver::getNormalizationScaling(const std::vector<glm::vec2>& data, const glm::vec2& centroid) const {
		double normSum = 0;
		for (auto& d : data) {
			normSum += glm::length(d - centroid);
		}
		double normAverage = normSum / data.size();
		double scaling = sqrt(2) / normAverage;
		return (float)scaling;
	}

	float DLTSolver::getNormalizationScaling(const std::vector<glm::vec3>& data,const glm::vec3& centroid) const {
		double normSum = 0;
		for (auto& d : data) {
			normSum += glm::length(d - centroid);
		}
		double normAverage = normSum / data.size();
		double scaling = sqrt(3) / normAverage;
		return (float)scaling;
	}

	std::vector<glm::vec3> DLTSolver::applyNormalization(const glm::mat4& normalizationMatrix, const std::vector<glm::vec3>& data) const {
		std::vector<glm::vec3> normalizedData(data.size());
		for (int i = 0; i < data.size(); i++) {
			// TODO: Check if this is still right after the vector4 in this method is being normalized
			auto t = glm::vec4(data[i], 1.0f)* normalizationMatrix;
			normalizedData[i] = glm::vec3(glm::vec4(data[i], 1.0f) * normalizationMatrix);
		}
		return normalizedData;
	}

	std::vector<glm::vec2> DLTSolver::applyNormalization(const glm::mat3& normalizationMatrix, const std::vector<glm::vec2>& data) const {
		std::vector<glm::vec2> normalizedData(data.size());
		for (int i = 0; i < data.size(); i++) {
			// TODO: Check if this is still right after the vector4 in this method is being normalized
			normalizedData[i] = glm::vec2(glm::vec3(data[i], 1) * normalizationMatrix);
		}
		return normalizedData;
	}
}
#pragma once

#include "texturemapping/mapping/solvers/solver.h"

namespace TextureMapping {

	class DLTSolver : public Solver {

	private:
		bool m_normalizeData;

	public:
		/// <summary>
		/// Solves 2D/3D correspondences by Direct Linear Transformation.
		/// </summary>
		DLTSolver(bool normalizeData=true);
		~DLTSolver() override {};

		glm::mat3x4 calculateProjectionMatrix(const glm::mat3& intrinsics, const std::vector<glm::vec3>& modelPoints, const std::vector<glm::vec2>& imagePoints) const override;



	private:

		std::vector<glm::vec3> applyNormalization(const glm::mat4& normalizationMatrix, const std::vector<glm::vec3>& data) const;
		std::vector<glm::vec2> applyNormalization(const glm::mat3& normalizationMatrix, const std::vector<glm::vec2>& data) const;
		float getNormalizationScaling(const std::vector<glm::vec2>& data, const glm::vec2& centroid) const;
		float getNormalizationScaling(const std::vector<glm::vec3>& data, const glm::vec3& centroid) const;
		glm::mat3 getNormalizationMatrix(const std::vector<glm::vec2>& data) const;
		glm::mat4 getNormalizationMatrix(const std::vector<glm::vec3>& data) const;
		/// Calculates the intrinsic camera parameters from DLT projeciton Matrix.
		//Tuple<Matrix3x4, Matrix<float>, Matrix<float>, Vector<float>> internalParameters(const glm::mat3x4& matrix)
	};

}
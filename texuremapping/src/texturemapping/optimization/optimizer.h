#pragma once

#include "texturemapping/modeling/model.h"
#include "texturemapping/mapping/solvers/solver.h"
#include "texturemapping/core/mappingDataSet.h"
#include "texturemapping/helper/mathExtensions.h"

namespace TextureMapping {

	/// <summary>
	/// Optimizes the 2D/3D correspondences.
	/// </summary>
	class Optimizer {

	private:

		Model& m_model;
		Solver& m_solver;
		int m_minNumberOfPoints;
		static const int MIN_NUMBER_OF_POINTS = 4;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="Optimizer"/> class.
		/// </summary>
		/// <param name="model">The model.</param>
		/// <param name="minNumberOfPoints">The minimum number of points.</param>
		/// <param name="solver">The solver.</param>
		/// <exception cref="System.Exception">At least 6 points are required for solving.</exception>
		Optimizer(Model& model, int minNumberOfPoints, Solver& solver)
			:m_model(model),
			m_solver(solver),
			m_minNumberOfPoints(minNumberOfPoints)
		{
			// TODO: How many points does PnP need?
			assert(minNumberOfPoints <= MIN_NUMBER_OF_POINTS && "At least 6 points are required for solving.");
		}

		/// Analyses a dataset's correspondences.
		//void analyseCorrespondence(MappingDataSet& dataSet) {
		//	std::vector<std::vector<int>> combinationIndices = getCombinationIndices(dataSet.modelPoints.size());
		//	//ConcurrentBag<OptimizationResult> results = new ConcurrentBag<OptimizationResult>();
		//	//
		//	//
		//	//Parallel.ForEach(combinationIndices, (indices) = > {
		//	//    Vector3[] modelPoints = GetPointSubset(indices, dataSet.ModelPoints);
		//	//    Vector2[] imagePoints = GetPointSubset(indices, dataSet.ImagePoints);
		//	//
		//	//    Matrix3x4 projectionMatrix = _solver.CalculateProjectionMatrix(modelPoints, imagePoints);
		//	//    OptimizationResult result = AnalyseProjectionMatrix(_model, dataSet, projectionMatrix);
		//	//    results.Add(result);
		//	//});
		//	//
		//	//OptimizationResult minErrorResult = results.Where(r = > r != null).MinBy(r = > r.AverageErrorInPixels);
		//	//dataSet.OptimizationResult = minErrorResult;
		//}

		/*
		public static Matrix3 GetIntrinsicParametersFromProjectionMatrix(Matrix3x4 projectionMatrix) {
			Matrix<float> matrix = Get3x3FromProjectionMatrix(projectionMatrix);
			QR<float> qr = matrix.Inverse().QR(QRMethod.Thin);
			Matrix<float> R = qr.R.Inverse();
			Matrix<float> Q = qr.Q.Transpose();
			//Matrix3 intrinsic = ConvertMathNetToTKMatrix(R);
			intrinsic = intrinsic.Divide(intrinsic.M33);
			return intrinsic;
		}

		private static Matrix<float> Get3x3FromProjectionMatrix(Matrix3x4 matrix) {
			float[][] rows = new float[3][];
			rows[0] = new float[] { matrix.M11, matrix.M12, matrix.M13 };
			rows[1] = new float[] { matrix.M21, matrix.M22, matrix.M23 };
			rows[2] = new float[] { matrix.M31, matrix.M32, matrix.M33 };
			Matrix<float> result = DenseMatrix.OfRowArrays(rows);
			return result;
		}

		private static Matrix3 ConvertMathNetToTKMatrix(Matrix<float> matrix) {
			Matrix3 result = new Matrix3(matrix[0, 0], matrix[0, 1], matrix[0, 2],
										 matrix[1, 0], matrix[1, 1], matrix[1, 2],
										 matrix[2, 0], matrix[2, 1], matrix[2, 2]);
			return result;
		}
		*/

		/// Analyses a projection matrix.
		static std::shared_ptr<OptimizationResult> analyseProjectionMatrix(Model model, MappingDataSet correspondence, glm::mat3x4 projectionMatrix);

	private:
		/// <summary>
		/// Gets a subset of points.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="indices">The indices.</param>
		/// <param name="data">The data.</param>
		/// <returns></returns>
		//T[] getPointSubset<T>(int[] indices, T[] data) {
		//	T[] subset = new T[indices.Length];
		//	for (int i = 0; i < subset.Length; i++) {
		//		// The index list actually contains the number values (i.e. 1-9) instead of the indices
		//		subset[i] = data[indices[i] - 1];
		//	}
		//	return subset;
		//}

		/// <summary>
		/// Gets the combination indices.
		/// </summary>
		/// <param name="numberOfPoints">The number of points.</param>
		/// <returns></returns>
		//int[][] GetCombinationIndices(int numberOfPoints) {
		//	List<List<int>> combinationIndicesList = new List<List<int>>();
		//	for (int i = _minNumberOfPoints; i <= numberOfPoints; i++) {
		//		combinationIndicesList.AddRange(GetSubsets(numberOfPoints, i));
		//	}
		//
		//	int[][] combinationIndicesArray = new int[combinationIndicesList.Count][];
		//	for (int i = 0; i < combinationIndicesArray.Length; i++) {
		//		combinationIndicesArray[i] = combinationIndicesList[i].ToArray();
		//	}
		//	return combinationIndicesArray;
		//}

		/// <summary>
		/// Gets the point subsets.
		/// http://stackoverflow.com/questions/23974569/how-to-generate-all-subsets-of-a-given-size
		/// </summary>
		/// <param name="n">The n.</param>
		/// <param name="subsetSize">Size of the subset.</param>
		/// <returns></returns>
		//std::vector<std::vector<int>> getSubsets(int n, int subsetSize) {
		//	IEnumerable<int> sequence = Enumerable.Range(1, n);
		//
		//	// generate list of sequences containing only 1 element e.g. {1}, {2}, ...
		//	var oneElemSequences = sequence.Select(x = > new[] { x }).ToList();
		//
		//	// generate List of int sequences
		//	var result = std::vector<std::vector<int>>();
		//	// add initial empty set
		//	result.Add(new List<int>());
		//
		//	// generate powerset, but skip sequences that are too long
		//	foreach(var oneElemSequence in oneElemSequences) {
		//		int length = result.Count;
		//
		//		for (int i = 0; i < length; i++) {
		//			if (result[i].Count >= subsetSize)
		//				continue;
		//
		//			result.Add(result[i].Concat(oneElemSequence).ToList());
		//		}
		//	}
		//
		//	return result.Where(x = > x.Count == subsetSize).ToList();
		//}
		//
	};
}

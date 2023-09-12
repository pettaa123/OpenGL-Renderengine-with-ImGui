#pragma once

#include "texturemapping/mapping/core/mergingResult.h"
#include "texturemapping/core/mappingDataSet.h"
#include "texturemapping/modeling/model.h"
#include "texturemapping/hardwareacceleration/openCL/openCLAccelerator.h"
#include "texturemapping/mapping/solvers/solver.h"
#include "texturemapping/mapping/imageToModelProjector.h"
#include "texturemapping/optimization/optimizer.h"

namespace TextureMapping {
	/// Maps images to a model.
	class TextureMapper {


	private:
		bool m_isCancelled;
		//glm::mat4 robRootToTCP;
		//glm::mat4 objToTCP;
		//glm::mat4 robRootToObject;
		//glm::mat4 flanschToTCP;

	public:

		/// Initializes a new instance of the <see cref="TextureMapper"/> class.
		TextureMapper():
		m_isCancelled(false){}


		/// <summary>
		/// Calculates the projection matrix and projects the image.
		/// </summary>
		/// <param name="dataSets">The data sets.</param>
		/// <param name="model">The model.</param>
		/// <param name="parameters">The parameters.</param>
		/// <param name="intrinsic">The intrinsic.</param>
		/// <param name="loadingObserver">The loading observer.</param>
		/// <returns></returns>
		std::optional<MergingResult> project(const glm::mat3& intrinsics, std::vector<MappingDataSet>& dataSets, Model& model);


		/// <summary>
		/// Cancels the current process.
		/// </summary>
		void cancel()
		{
			m_isCancelled = true;
		}

private:
	/*
		/// Separate thread for optimization.
		void optimizationThread(object parametersObj)
		{
			StartWatch(1);
			OptimizationParameters parameters = (OptimizationParameters)parametersObj;
			Optimizer optimizer = new Optimizer(parameters.Model, parameters.MinNumberOfPoints, parameters.Solver);

			foreach(MappingDataSet correspondence in parameters.Correspondences)
			{
				optimizer.AnalyseCorrespondence(correspondence);
				parameters.ResultQueue.Enqueue(correspondence.OptimizationResult);
			}
			StopWatch(1);
			Profiler.ProjectionMatrixTimeMs = TrackedTimes[1];
		}

		glm::mat4 ComputeM(float c, float b, float a, float x, float y, float z)
		{
			Matrix4 rotMZ = Matrix4.CreateRotationZ(a * ((float)Math.PI / 180f));
			Matrix4 rotMY = Matrix4.CreateRotationY(b * ((float)Math.PI / 180f));
			Matrix4 rotMX = Matrix4.CreateRotationX(c * ((float)Math.PI / 180f));
			Matrix4 transXYZ = Matrix4.CreateTranslation(x, y, z);

			Matrix4 M = rotMX * rotMY * rotMZ * transXYZ;
			//Transponieren um üblichen Aufbau herzustellen
			M = Matrix4.Transpose(M);
			return M;
		}



		private DenseMatrix Matrix3x4ToDenseMatrix(Matrix3x4 matrix)
		{
			DenseMatrix result = new DenseMatrix(3, 4);
			for (int row = 0; row < 3; row++)
			{
				for (int col = 0; col < 4; col++)
				{
					result[row, col] = matrix[row, col];
				}
			}
			return result;
		}
		private Matrix3x4 DenseMatrixToMatrix3x4(Matrix<float> matrix)
		{
			Matrix3x4 result = new Matrix3x4();
			for (int row = 0; row < 3; row++)
			{
				for (int col = 0; col < 4; col++)
				{
					result[row, col] = matrix[row, col];
				}
			}
			return result;
		}
		private DenseMatrix Matrix4ToDenseMatrix(Matrix4 matrix)
		{
			DenseMatrix result = new DenseMatrix(4, 4);
			for (int row = 0; row < 4; row++)
			{
				for (int col = 0; col < 4; col++)
				{
					result[row, col] = matrix[row, col];
				}
			}
			return result;
		}
		private DenseMatrix Matrix3ToDenseMatrix(Matrix3 matrix)
		{
			DenseMatrix result = new DenseMatrix(3, 3);
			for (int row = 0; row < 3; row++)
			{
				for (int col = 0; col < 3; col++)
				{
					result[row, col] = matrix[row, col];
				}
			}
			return result;
		}
		private Tuple<Matrix<float>, Matrix<float>, Vector<float>> InternalParameters(Matrix3x4 matrix)
		{
			/*Matrix<float> projectionMatrix = MatrixToDenseMatrix(matrix);
			Matrix<float> threeByThree = SliceMatrix(projectionMatrix, 3, 3);
			Tuple<Matrix<float>, Matrix<float>> rq = RQ(threeByThree);

			Matrix<float> K = rq.Item1;
			Matrix<float> R = rq.Item2;

			K = K / K[2, 2];*/
			//PETER Herbert Süße S. 356
	/*
			Matrix<float> projectionMatrix = Matrix3x4ToDenseMatrix(matrix);
			Matrix<float> M = SliceMatrix(projectionMatrix, 3, 3);

			Matrix<float> Z = M.Inverse();
			Z = Z.Multiply(-1);
			Vector<float> C = Z.Multiply(projectionMatrix.Column(3));//Camera center


			M = flipud(M);
			M = M.Transpose();
			QR<float> QR = M.QR(QRMethod.Thin);
			Matrix<float> K = flipud(QR.R.Transpose());
			K = fliplr(K);
			//K=K.Divide(K[2, 2]);
			Matrix<float> R = QR.Q.Transpose();
			R = flipud(R);
			//Vector<float> t = K.Inverse().Multiply(projectionMatrix.Column(3));  //t=K^-1*a

			return new Tuple<Matrix<float>, Matrix<float>, Vector<float>>(K, R, C);


		}
		private static Matrix<float> SliceMatrix(Matrix<float> matrix, int rows, int cols)
		{
			Matrix<float> result = new DenseMatrix(rows, cols);
			for (int row = 0; row < rows; row++)
			{
				for (int col = 0; col < cols; col++)
				{
					result[row, col] = matrix[row, col];
				}
			}
			return result;
		}

		private static Matrix<float> flipud(Matrix<float> matrix)
		{
			Matrix<float> flippedMatrix = Matrix<float>.Build.Dense(matrix.RowCount, matrix.ColumnCount);

			for (int row = 0; row < matrix.ColumnCount; row++)
			{
				for (int column = 0; column < matrix.RowCount; column++)
				{
					flippedMatrix[row, column] = matrix[matrix.RowCount - row - 1, column];
				}
			}
			return flippedMatrix;
		}
		private static Matrix<float> fliplr(Matrix<float> matrix)
		{
			Matrix<float> flippedMatrix = Matrix<float>.Build.Dense(matrix.RowCount, matrix.ColumnCount);

			for (int row = 0; row < matrix.ColumnCount; row++)
			{
				for (int column = 0; column < matrix.RowCount; column++)
				{
					flippedMatrix[row, column] = matrix[row, matrix.ColumnCount - column - 1];
				}
			}
			return flippedMatrix;
		}
		*/
	};

}
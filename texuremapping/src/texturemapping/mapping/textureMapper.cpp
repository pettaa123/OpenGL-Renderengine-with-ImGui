#include "texturemapping/mapping/textureMapper.h"

namespace TextureMapping
{

	std::optional<MergingResult> TextureMapper::project(const glm::mat3& intrinsics, std::vector<MappingDataSet>& dataSets, Model& model) {

		//Berechnung camToObj


		/*Matrix4 flanschToCam = ComputeM(-35.7881f, 0.0774f, 150.4886f, 18.9372f, 38.8273f, 705.3491f);

		//Matrix4 flanschToCam = ComputeM(-36.240f, 0.301f, 150.534f, -8.312f, 12.310f, 774.834f);

		robRootToTCP = ComputeM(95.0031891f, -2.78482485f, 12.7106524f, 2693.27881f, -2047.74744f, 1709.01147f);



		robRootToObject = ComputeM(-0.2677f, 1.0485f, -168.1434f, 5619.8482f, 806.3089f, 1094.4304f);

		flanschToTCP = ComputeM(31.685f, 0.03f, -27.824f, -257.36f, -479.61f, 1706.6f);*/




		//if (parameters.Device.IsCUDA)
		//{
		//	executor = new CUDAAccelerator(parameters.IsDLT, parameters.Device.CUDADevice, model);
		//}
		//else
		//{

		//ToDo: create a accelerator factory
		std::shared_ptr<Accelerator> executor = Accelerator::create(model);
		//}

		// PNP: ---- Changes begin ----
		// PNP: Create the PnPSolver from the "OpenCV" project instead of using the SolverFactory

		std::shared_ptr<Solver> solver = Solver::create(SolvingMethod::DLT, true); // intrinsicMatrix, intrinisc.DistortionCoefficients, normalized);

		ImageToModelProjector projector(model, *executor.get());
		/*
		ConcurrentQueue<OptimizationResult> resultQueue = null;

		if (parameters.UseOptimization)
		{
			resultQueue = new ConcurrentQueue<OptimizationResult>();
			OptimizationParameters optimizationParameters = new OptimizationParameters()
			{
				Correspondences = dataSets,
				MinNumberOfPoints = parameters.MinNumberOfPoints,
				Model = model,
				Solver = solver,
				ResultQueue = resultQueue,
			};

			Thread optimizationThread = new Thread(OptimizationThread);
			optimizationThread.IsBackground = true;
			optimizationThread.Start(optimizationParameters);
		}
		*/

		for (size_t i = 0; i < dataSets.size(); i++)
		{
			glm::mat3x4 projectionMatrix;

			//if (parameters.UseOptimization)
			//{
			//	while (!resultQueue.TryDequeue(out result))
			//	{
			//		Thread.Sleep(10);
			//	}
			//	projectionMatrix = result.ProjectionMatrix;
			//}
			//else
			{
				// PNP: Calculate the projection matrix and the camera pose using the PnP solver
				// PNP: From camera pose and image sensor size, determine the sensor normal (pointing towards the model) for the top left pixel in real world coordinates

				//if (parameters.IsDLT)
				//{

					//projectionMatrix = solver.CalculateProjectionMatrix(dataSet.ModelPoints, dataSet.ImagePoints);

				projectionMatrix = solver->calculateProjectionMatrix(intrinsics, dataSets[i].modelPoints, dataSets[i].imagePoints);
				//if (i != 0)
				//{
				//	//Tuple<Matrix<float>, Matrix<float>, Vector<float>> internalPams = InternalParameters(projectionMatrix);
				//	float[, ] intrinsicDoubleArray = { { 1973.79552f, 0f, 306.5967f }, { 0f, 1968.77287f, 251.25286f }, { 0f, 0f, 1f } };
				//	Matrix<float> intrinsics = Matrix<float>.Build.DenseOfArray(intrinsicDoubleArray);
				//	Matrix<float> projectionMatrixFloat = Matrix3x4ToDenseMatrix(projectionMatrix);
				//
				//
				//	//internalPams.Item1[1, 1] =internalPams.Item1[1, 1] * (-1);
				//	Matrix<float> res = intrinsics.Inverse().Multiply(projectionMatrixFloat);
				//
				//
				//	Matrix4 projjB0 = ComputeM(dataSets[0].TCPRot.X, dataSets[0].TCPRot.Y, dataSets[0].TCPRot.Z, dataSets[0].TCPTrans.X, dataSets[0].TCPTrans.Y, dataSets[0].TCPTrans.Z);
				//	Matrix4 projjB1 = ComputeM(dataSet.TCPRot.X, dataSet.TCPRot.Y, dataSet.TCPRot.Z, dataSet.TCPTrans.X, dataSet.TCPTrans.Y, dataSet.TCPTrans.Z);
				//	Matrix4 Tx = Matrix4.Mult(Matrix4.Invert(projjB1), projjB0);
				//
				//
				//	Matrix<float> TxFloat = Matrix4ToDenseMatrix(Tx);
				//
				//	float[] x = { 0.0f, 0.0f, 0.0f, 1.0f };
				//	Vector<float> v = Vector<float>.Build.DenseOfArray(x);
				//	res = res.InsertRow(3, v);
				//
				//	res = TxFloat.Multiply(res);
				//	res = res.RemoveRow(3);
				//
				//	Matrix<float> M = SliceMatrix(res, 3, 3);
				//
				//	Matrix<float> Z = M.Inverse();
				//	Z = Z.Multiply(-1);
				//	Vector<float> C = Z.Multiply(res.Column(3));//Camera center
				//
				//	projectionMatrixFloat = intrinsics.Multiply(res);
				//	projectionMatrixFloat = projectionMatrixFloat.Divide(projectionMatrixFloat[2, 3]);
				//
				//	projectionMatrix = DenseMatrixToMatrix3x4(projectionMatrixFloat);
				//
				//}


				//else if (false)
				//{
				//
				//	//Aenderung
				//	/*
				//	robRootToTCP = ComputeM(dataSet.TCPRot.X, dataSet.TCPRot.Y, dataSet.TCPRot.Z, dataSet.TCPTrans.X, dataSet.TCPTrans.Y, dataSet.TCPTrans.Z);
				//	objToTCP = Matrix4.Mult(Matrix4.Invert(robRootToObject), robRootToTCP);
				//
				//
				//	Matrix4 projj = Matrix4.Invert(Matrix4.Mult(robRootToTCP, Matrix4.Invert(flanschToTCP)));
				//	projj = Matrix4.Mult(Matrix4.Invert(flanschToCam), projj);
				//	projj = Matrix4.Mult(projj, robRootToObject);*/
				//
				//	//CamToObject
				//	Matrix4 projj = ComputeM(dataSet.TCPRot.X, dataSet.TCPRot.Y, dataSet.TCPRot.Z, dataSet.TCPTrans.X, dataSet.TCPTrans.Y, dataSet.TCPTrans.Z);
				//	projj = Matrix4.Invert(projj);
				//
				//
				//
				//
				//
				//
				//	IntrinsicParameters intrinsic = null;
				//	intrinsic = XmlHelper.Load<IntrinsicParameters>(parameters.IntriniscFile);
				//
				//	Matrix3 intrinsicMatrix = new Matrix3(intrinsic.FocalLength.X, 0, intrinsic.PrincipalPoint.X,
				//		0, intrinsic.FocalLength.Y, intrinsic.PrincipalPoint.Y,
				//		0, 0, 1);
				//
				//	//Matrix 
				//	//https://docs.opencv.org/2.4/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html
				//	Matrix3x4 projjKR = new Matrix3x4(projj.Row0, projj.Row1, projj.Row2);
				//	DenseMatrix projjKRFloat;
				//	projjKRFloat = Matrix3x4ToDenseMatrix(projjKR);
				//	DenseMatrix intrinsicFloat;
				//	intrinsicFloat = Matrix3ToDenseMatrix(intrinsicMatrix);
				//
				//	intrinsicFloat.Multiply(projjKRFloat, projjKRFloat);
				//
				//
				//	projectionMatrix = DenseMatrixToMatrix3x4(projjKRFloat);
				//
				//
				//	//Berechnet Fehler in Abstand[Pixeln] von auf textur gemappte Modelpunkte via projektionsmatrix 
				//}
				//ToDo:
				dataSets[i].optimizationResult = Optimizer::analyseProjectionMatrix(model, dataSets[i], projectionMatrix);

			}

			if (m_isCancelled)
			{
				return {};
			}

			// PNP: Hand over the calculated sensor normal to the projector
			//Errechnet die Korrespondenz zwischen Textur und STL und speichert das gefilterte Ergebnis in einem Tupel aus Texturkoordinaten und dazugehörigen TriangleID
			projector.projectImage(dataSets[i], (int)i, projectionMatrix);

		}

		std::optional<MergingResult> mergingResult;
		if (!m_isCancelled)
		{
			//Schneidet die Bildüberlappungen,sticht die Bilder und passt die texturKoordinaten dem gestitchten Bild an.
			mergingResult = projector.finish();
		}

		return mergingResult;
	}
}

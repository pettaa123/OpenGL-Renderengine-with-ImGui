#include "texturemapping/hardwareacceleration/openCL/openCLAccelerator.h"
#include "texturemapping/core/intrinsics.h"
#include <boost/compute/utility/dim.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace TextureMapping {


	OpenCLAccelerator::OpenCLAccelerator(boost::compute::device device, const TextureMapping::Model& model) :
		Accelerator{ model },
		m_device(device)
	{
		//m_flagsReadOnly = ComputeMemoryFlags::UseHostPointer | ComputeMemoryFlags::ReadOnly;
		//m_flagsWriteOnly = ComputeMemoryFlags::UseHostPointer | ComputeMemoryFlags::WriteOnly;
		createKernels();

	}

	/// Gets the OpenCL devices.
	std::vector<boost::compute::device> OpenCLAccelerator::getDevices(bool onlyGraphicsCards)
	{
		// get the default device
		std::vector<boost::compute::device> devices = boost::compute::system::devices();

		if (onlyGraphicsCards)
		{
			std::vector<boost::compute::device> gpus;
			//CL_DEVICE_TYPE_GPU
			std::copy_if(devices.begin(), devices.end(), std::back_inserter(gpus),
				[](boost::compute::device& gpu) { return(gpu.type() == boost::compute::device::type::gpu); });
			devices = gpus;
		}

		return devices;
	}

	/// Projects the image.
	ProjectionResult OpenCLAccelerator::projectImage(const MappingDataSet& dataSet, std::vector<float>& projectionMatrix, int dataSetID)
	{
		// PNP: Use the ray casting kernel. Remove the projection matrix below and
		//      send the sensor normal and the sensor size in real world to the kernel.
		//      The kernel will move the normal to each pixel of the sensor and perform a ray casting towards the model.

		cl_int imageWidth = dataSet.projectionImage.width;
		cl_int imageHeight = dataSet.projectionImage.height;

		// TODO: Evaluate if this is really necessary. If you experience problems try uncommenting this
		// Don't forget to uncomment it also in line 90/91 and in the kernel
		/*
		float criterionFactor = 0.1f;
		Vector2 imageSize = new Vector2(imageWidth, imageHeight);
		Vector2 pixelMinCriterion = -criterionFactor * imageSize;
		Vector2 pixelMaxCriterion = (1 + criterionFactor) * imageSize;
		*/
		//TEMPORÄRER MERKER




		//NIMMT TCP4 anstatt modelpoints als Übergabe für die Auswahl der STL vertices in der Projection.cl
		//float[] TCP;
		int numberOfModelPoints = (int)dataSet.modelPoints.size();
		const float* data = glm::value_ptr(dataSet.modelPoints[0]);
		//to floats
		std::vector<float> modelPoints(data, data + numberOfModelPoints * 3);



		int numberOfPoints = (int)dataSet.imagePolygonPointsInPixels.size();
		data = glm::value_ptr(dataSet.imagePolygonPointsInPixels[0]);
		//to floats
		std::vector<float> polygonPoints(data, data + numberOfModelPoints * 2);


		std::vector<float> textureCoordinates(m_defaultTexCoords);

		try {
			// create memory buffers for the input and output
			std::vector< boost::compute::buffer> buffers;

			buffers.push_back(boost::compute::buffer(m_context, m_vertices.size() * sizeof(float), CL_MEM_READ_ONLY));
			buffers.push_back(boost::compute::buffer(m_context, projectionMatrix.size() * sizeof(float), CL_MEM_READ_ONLY));
			buffers.push_back(boost::compute::buffer(m_context, polygonPoints.size() * sizeof(float), CL_MEM_READ_ONLY));
			buffers.push_back(boost::compute::buffer(m_context, modelPoints.size() * sizeof(float), CL_MEM_READ_ONLY));
			buffers.push_back(boost::compute::buffer(m_context, textureCoordinates.size() * sizeof(float), CL_MEM_READ_WRITE));

			// set the kernel arguments
			uint32_t a = 0;
			for (auto& b : buffers) {
				m_rayCastingKernel.set_arg(a++, b);
			}
			m_rayCastingKernel.set_arg(a++, numberOfPoints);
			m_rayCastingKernel.set_arg(a++, numberOfModelPoints);
			m_rayCastingKernel.set_arg(a++, imageWidth);
			m_rayCastingKernel.set_arg(a++, imageHeight);

			boost::compute::command_queue queue(m_context, m_device);

			// create a command queue

			// write the data from to the device
			queue.enqueue_write_buffer(buffers[0], 0, buffers[0].size(), m_vertices.data());
			queue.enqueue_write_buffer(buffers[1], 0, buffers[1].size(), projectionMatrix.data());
			queue.enqueue_write_buffer(buffers[2], 0, buffers[2].size(), polygonPoints.data());
			queue.enqueue_write_buffer(buffers[3], 0, buffers[3].size(), modelPoints.data());
			queue.enqueue_write_buffer(buffers[4], 0, buffers[4].size(), textureCoordinates.data());

			size_t global_size = m_numberOfTriangles;
			// run the add kernel
			queue.enqueue_1d_range_kernel(m_rayCastingKernel, 0, global_size, 0);
			queue.finish();

			// transfer results back to the host array 'c'
			queue.enqueue_read_buffer(buffers[4], 0, buffers[4].size(), textureCoordinates.data());

			//for (size_t i = 0; i < textureCoordinates.size(); i++)
			//{
			//	if (textureCoordinates[i] != -1000)
			//		Log::info(std::format(" {} = {} ", i, textureCoordinates[i]));
			//}
		}

		catch (boost::compute::opencl_error e) {
			Log::error(e.error_string());
		}

		ProjectionResult result = filterResult(textureCoordinates);


		return result;

	}

	/// <summary>
	/// Undistorts the image.
	/// </summary>
	/// <param name="image">The image.</param>
	/// <param name="intrinsicParameters">The intrinsic parameters.</param>
	/// <param name="distortionCoefficients">The distortion coefficients.</param>
	void OpenCLAccelerator::undistortImage(STBimage& image, const Intrinsics& intrinsics)
	{
		uint32_t width = image.width;
		uint32_t height = image.height;

		//Variable to pass to the memory
		float* intrinsicValues = &intrinsics.toMat3()[0][0];

		float* invIntrinsicValues = &intrinsics.toMat3inv()[0][0];


		//Create OpenCL buffers (copy data to device)
		// The type of your buffer is declared (e.g. <float>)

		// create memory buffers for the input and output


		boost::compute::image_format imageformat(boost::compute::image_format::channel_order::rgba, boost::compute::image_format::channel_data_type::unsigned_int8);
		boost::compute::image2d imageIn(m_context, width, height, imageformat, CL_MEM_READ_ONLY); //new ComputeImage2D(_context, ComputeMemoryFlags.WriteOnly, imageFormat, width, height, 0, IntPtr.Zero),
		boost::compute::image2d imageOut(m_context, width, height, imageformat, CL_MEM_WRITE_ONLY);

		std::vector< boost::compute::buffer> buffers;
		try {
			buffers.push_back(boost::compute::buffer(m_context, 9 * sizeof(float), CL_MEM_READ_ONLY)); //intrinsicValues
			buffers.push_back(boost::compute::buffer(m_context, 9 * sizeof(float), CL_MEM_READ_ONLY)); //invIntrinsicValues
			buffers.push_back(boost::compute::buffer(m_context, 5 * sizeof(float), CL_MEM_READ_ONLY)); //distortionCoefficients
		}
		catch (boost::compute::opencl_error e) {
			Log::error(e.error_string());
		}

		// set the kernel arguments
		uint32_t a = 0;
		try {
			m_undistortKernel.set_arg(a++, imageIn);
			m_undistortKernel.set_arg(a++, imageOut);
			for (auto& b : buffers) {
				m_undistortKernel.set_arg(a++, b);
			}
		}
		catch (boost::compute::opencl_error e) {
			Log::error(e.error_string());
		}
		// create a command queue
		boost::compute::command_queue queue(m_context, m_device);
		try {
			queue.enqueue_write_image(imageIn, imageIn.origin(), imageIn.size(), image.data.get());
			// write the data from to the device
			queue.enqueue_write_buffer(buffers[0], 0, buffers[0].size(), intrinsicValues);
			queue.enqueue_write_buffer(buffers[1], 0, buffers[1].size(), invIntrinsicValues);
			queue.enqueue_write_buffer(buffers[2], 0, buffers[2].size(), intrinsics.distortionCoeffs().data());
		}
		catch (boost::compute::opencl_error e) {
			Log::error(e.error_string());
		}
		size_t global_offset[] = { 0,0 };// { width,height};
		size_t global_size[] = { width,height };// { width,height};
		try {
			queue.enqueue_nd_range_kernel(m_undistortKernel, 2, global_offset, global_size, NULL);
			queue.finish();
		}
		catch (boost::compute::opencl_error e) {
			Log::error(e.error_string());
		}

		try {
			queue.enqueue_read_image(imageOut, imageOut.origin(), imageOut.size(), image.data.get());
		}
		catch (boost::compute::opencl_error e) {
			Log::error(e.error_string());
		}
	}

	void OpenCLAccelerator::cropImage(STBimage& image, std::vector<glm::vec2> polygon, Color4& defaultColor)
	{
		//int width = image.Width;
		//int height = image.Height;
		//// 24bppRgb does not work, OpenCL expects rgba. Maybe this could be changed?
		//// (Even changing the ComputeImageFormat does not help)
		//PixelFormat pixelFormat = PixelFormat.Format32bppArgb;
		//
		//ComputeImageFormat imageFormat = new ComputeImageFormat(ComputeImageChannelOrder.Rgba, ComputeImageChannelType.UnsignedInt8);
		//BitmapData bitmapData = image.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.ReadOnly, pixelFormat);
		//
		//List<ComputeMemory> buffers = new List<ComputeMemory>() {
		//	new ComputeImage2D(_context, ComputeMemoryFlags.ReadOnly, imageFormat, width, height, 0, IntPtr.Zero),
		//		new ComputeImage2D(_context, ComputeMemoryFlags.WriteOnly, imageFormat, width, height, 0, IntPtr.Zero),
		//		new ComputeBuffer<float>(_context, _flagsReadOnly, polygon.ToFloats()),
		//		new ComputeBuffer<int>(_context, _flagsReadOnly, defaultColor.ToInts())
		//};
		//int a = 0;
		//for (int i = 0; i < buffers.Count; i++)
		//{
		//	_cropKernel.SetMemoryArgument(a++, buffers[i]);
		//}
		//_cropKernel.SetValueArgument(a++, polygon.Length);
		//
		//ComputeCommandQueue queue = new ComputeCommandQueue(_context, _device, ComputeCommandQueueFlags.None);
		//queue.WriteToImage(bitmapData.Scan0, (ComputeImage2D)buffers[0], true, null);
		//queue.Execute(_cropKernel, null, new long[] { width, height }, null, null);
		//queue.ReadFromImage((ComputeImage2D)buffers[1], bitmapData.Scan0, true, null);
		//
		//queue.Dispose();
		//buffers.ForEach(b = > b.Dispose());
		//image.UnlockBits(bitmapData);
	}

	/// Creates the kernels.
	void OpenCLAccelerator::createKernels()
	{
		//PNP: Create and load a kernel for ray casting
		std::filesystem::path resourcePrefix("assets/kernels");

		auto s = resourcePrefix / "Projection.cl";
		assert(std::filesystem::exists(s) && "Kernel doesnt exist");
		boost::compute::program program;
		try {
			m_context = boost::compute::context(m_device);
			program = boost::compute::program::create_with_source_file(s.string(), m_context);
			program.build();
			m_rayCastingKernel = program.create_kernel("project");
		}
		catch (boost::compute::opencl_error e) {
			Log::error(e.error_string());
		}

		s = resourcePrefix / "ImageProcessing.cl";
		try {
			program = boost::compute::program::create_with_source_file(s.string(), m_context);
			program.build();
			m_undistortKernel = program.create_kernel("undistortImage");
			m_cropKernel = program.create_kernel("cropImage");
		}
		catch (boost::compute::opencl_error e) {
			Log::error(e.error_string());
		}

	}
}


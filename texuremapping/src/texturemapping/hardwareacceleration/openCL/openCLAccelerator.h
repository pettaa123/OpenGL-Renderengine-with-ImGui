#pragma once

#define CL_TARGET_OPENCL_VERSION 120

#include <algorithm>
#include <fstream>
#include <filesystem>
#include "texturemapping/hardwareacceleration/base/accelerator.h"
#include "compute/include/boost/compute/core.hpp"
#include "texturemapping/core/color4.h"




namespace TextureMapping {


	/// Uses hardware acceleration by OpenCL.
	class OpenCLAccelerator : public Accelerator {

	private:

		boost::compute::device m_device;
		boost::compute::context m_context;
		boost::compute::kernel m_undistortKernel;
		boost::compute::kernel m_rayCastingKernel;
		boost::compute::kernel m_cropKernel;

		// TODO: If Error comes on Build program try:
		// CL_CONFIG_CPU_VECTORIZER_MODE=1
		// https://software.intel.com/de-de/forums/opencl/topic/596328
		/// Creates the kernels.
		void createKernels();



	public:

		OpenCLAccelerator(boost::compute::device device,const TextureMapping::Model& model);


		/// Gets the OpenCL devices.
		static std::vector<boost::compute::device> getDevices(bool onlyGraphicsCards = true);


		/// Projects the image.
		ProjectionResult projectImage(const MappingDataSet& dataSet, std::vector<float>& projectionMatrix, int dataSetID) override;


		/// <summary>
		/// Undistorts the image.
		/// </summary>
		/// <param name="image">The image.</param>
		/// <param name="intrinsicParameters">The intrinsic parameters.</param>
		/// <param name="distortionCoefficients">The distortion coefficients.</param>
		void undistortImage(BaseLib::STBimage& image, const Intrinsics& intrinsicParameters) override;


		void cropImage(BaseLib::STBimage& image, std::vector<glm::vec2> polygon, Color4& defaultColor);


	};
}

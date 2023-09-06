#include "texturemapping/hardwareacceleration/openCL/openCLAccelerator.h"


namespace TextureMapping {

	std::shared_ptr<Accelerator> Accelerator::create(const Model& model) {
		std::vector<boost::compute::device> d = OpenCLAccelerator::getDevices();
		return std::make_shared<OpenCLAccelerator>(d[0],model);
	}

}
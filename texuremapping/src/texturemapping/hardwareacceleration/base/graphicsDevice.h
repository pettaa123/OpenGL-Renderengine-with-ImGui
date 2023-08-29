#pragma once

#include <string>
#include "boost/compute/core.hpp"

namespace TextureMapping{

    /// Graphics card representation.
    struct GraphicsDevice {
        std::string name;
        std::string driverVersion;
        bool isCUDA;
        //CUDADevice CUDADevice;
        boost::compute::device openCLDevice;
    };
}

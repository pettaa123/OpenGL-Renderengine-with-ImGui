#pragma once

#include <filesystem>
#include "texturemapping/core/intrinsics.h"

namespace TextureMapping {

    class IntrinsicsImporter {

    public:

        static Intrinsics loadFromJSON(std::filesystem::path& file);
    };
}


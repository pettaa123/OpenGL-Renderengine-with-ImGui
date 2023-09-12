#pragma once

#include <filesystem>
#include "texturemapping/core/mappingDataSet.h"

namespace TextureMapping {

    class MappingDataSetImporter {

    public:

        static std::vector<MappingDataSet> loadFromJSON(const std::filesystem::path& folder);
    };
}


#pragma once

#include <filesystem>
#include "texturemapping/core/mappingDataSet.h"

namespace TextureMapping {

    class MappingDataSetImporter {

    public:

        static std::vector<MappingDataSet> loadFromJSON(std::filesystem::path& folder);
    };
}


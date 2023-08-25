#pragma once

#include <filesystem>
#include "core/mappingDataSet.h"

namespace TextureMapping {

    // Imports correspondences from Excel or XML.
    static class MappingDataSetImporter {
    public:

        // Loads the correspondences from JSON files.
        static std::vector<MappingDataSet> loadFromJSON(std::filesystem::path folder) {
            folder = Path.GetFullPath(folder);
            std::vector<MappingDataSet> dataSets;
            OrdinalStringComparer comparer = new OrdinalStringComparer();

            string[] files = Directory.GetFiles(folder, "*.xml", SearchOption.TopDirectoryOnly);
            Array.Sort(files, comparer);
            for (int i = 0; i < files.Length; i++) {
                MappingDataSet dataSet = XmlHelper.Load<MappingDataSet>(files[i]);
                dataSets.Add(dataSet);
            }

            return dataSets;
        }
    };
}


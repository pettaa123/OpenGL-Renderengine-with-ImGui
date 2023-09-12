#include "texturemapping/import/mappingDataSetImporter.h"
#include <format>
#include <fstream>
#include "json/single_include/nlohmann/json.hpp"
#include "engine/core/log.h"

namespace TextureMapping {

    std::vector<MappingDataSet> MappingDataSetImporter::loadFromJSON(const std::filesystem::path& folder) {

        std::string ext(".json");

        std::vector<MappingDataSet> dataSets;

        for (auto& p : std::filesystem::directory_iterator(folder))
        {
            if (p.path().extension() == ext) {
                Log::info(std::format("Importing: {}", p.path().string()));
                std::ifstream f(p.path());
                nlohmann::json data;
                try {
                    data = nlohmann::json::parse(f);
                }
                catch (nlohmann::json::parse_error& ex) {
                    Log::error(std::format("parse error at byte {}", ex.byte));
                }
                assert(!data.is_discarded() && std::format("Parse Error reading JSON File: {}", p.path().string()).c_str());

                MappingDataSet mds;

                //json j;
                //std::vector<glm::vec2> points{ glm::vec2(70, -140), glm::vec2(30, -30), glm::vec2(20, 125) };
                //j["points"] = points;

                try {
                    mds.imageFile = data["imageFile"];
                    mds.imagePoints = data["imagePoints"];
                    mds.modelPoints = data["modelPoints"];
                    //mds.tcpRot = data["tcpRot"];
                    //mds.tcpTrans = data["tcpTrans"];

                }
                catch (nlohmann::json::exception& ex) {
                    Log::error(std::format("Excpetion creating MappingDataSet's {} from JSON - {}", p.path().string(), ex.what()));
                }


                BaseLib::STBimage image;

                assert(image.load(folder / mds.imageFile) && "Couldn't load dataSet image");

                mds.loadedImage = image;

                dataSets.push_back(mds);
            }
        }

        return dataSets;

    }
}


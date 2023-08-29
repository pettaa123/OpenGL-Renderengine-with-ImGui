#include "texturemapping/import/intrinsicsImporter.h"
#include <format>
#include <fstream>
#include "json/single_include/nlohmann/json.hpp"
#include "engine/core/log.h"

namespace TextureMapping {

	Intrinsics IntrinsicsImporter::loadFromJSON(std::filesystem::path& file) {

		assert(std::filesystem::exists(file) && "File dont exist");
		std::ifstream f(file);
		if (!f)
			Log::error(std::format("Error reading instrinsics from {}", file.string()));


		nlohmann::json data;
		try {
			data = nlohmann::json::parse(f);
		}
		catch (nlohmann::json::parse_error& ex) {
			Log::error(std::format("parse error at byte {}", ex.byte));
		}
		assert(!data.is_discarded() && "Parse Error reading JSON File");

		Intrinsics intrinsics;


		try {
			intrinsics.fx = data["fx"];
			intrinsics.fy = data["fy"];
			intrinsics.cx = data["cx"];
			intrinsics.cy = data["cy"];
			intrinsics.k1 = data["k1"];
			intrinsics.k2 = data["k2"];
			intrinsics.p1 = data["p1"];
			intrinsics.p2 = data["p2"];
			intrinsics.k3 = data["k3"];
			intrinsics.width = data["width"];
			intrinsics.height = data["height"];
			intrinsics.focusPos = data["focusPos"];
		}
		catch (nlohmann::json::exception& ex) {
			Log::error(std::format("Excpetion creating Intrinsics from JSON - {}", ex.what()));
		}

		return intrinsics;

	};
}


#pragma once

#include <glm/glm.hpp>
#include <unordered_set>
#include "texturemapping/core/polygonMarker.h"
#include "engine/renderer/model.h"
#include "texturemapping/core/stbImage.h"
#include "texturemapping/optimization/optimizationResult.h"

//////////////////////////////////////////////////////////
#include "json/single_include/nlohmann/json.hpp"
namespace glm {
	inline void to_json(nlohmann::json& j, const glm::vec2& P) {
		j = { { "x", P.x }, { "y", P.y } };
	};

	inline void from_json(const nlohmann::json& j, glm::vec2& P) {
		P.x = j.at("x").get<float>();
		P.y = j.at("y").get<float>();
	}

	inline void to_json(nlohmann::json& j, const glm::vec3& P) {
		j = { { "x", P.x }, { "y", P.y } };
	};

	inline void from_json(const nlohmann::json& j, glm::vec3& P) {
		P.x = j.at("x").get<float>();
		P.y = j.at("y").get<float>();
		P.z = j.at("z").get<float>();
	}
}
///////////////////////////////////////////////////////////


namespace TextureMapping {


	// Data container for 2D/3D correspondences and all sort of related information (i.e. images).

	class MappingDataSet {

	public:

		std::vector<glm::vec2> imagePoints;
		std::vector<glm::vec3> modelPoints;
		std::string imageFile;
		glm::vec3 tcpTrans;
		glm::vec3 tcpRot;
		std::string name;
		std::shared_ptr<Engine::Model> model;
		STBimage loadedImage;
		std::optional<STBimage> alteredImage;
		STBimage projectionImage;
		std::vector<glm::vec2> imagePolygonPointsInUV;
		std::vector<glm::vec2> imagePolygonPointsInPixels;
		std::shared_ptr<OptimizationResult> optimizationResult;
		std::unordered_set<int> triangleHash; //HashSet<int>
		std::pair< std::vector<glm::vec2>, std::vector<int>> texCoordsAndTriangleIds;
		bool drawImagePoints;
		bool visualizeModelPoints;
		bool cropImage;
		bool convexHull;
		glm::vec2 texCoordsOffset;
		int imageRow;
		int imageColumn;
		std::vector<PolygonMarker> markers;

		/// Initializes a new instance of the <see cref="MappingDataSet"/> class.
		MappingDataSet();

			/// Crops the image.
		void performCropping();


		/// Prepares the image polygon.
		void prepareImagePolygon();

	private:
		/// Crops the image in a simple way.
		void cropImageSimple();
	};

}
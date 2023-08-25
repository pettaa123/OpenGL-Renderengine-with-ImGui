#pragma once

#include <glm/glm.hpp>
#include <unordered_set>
#include <ranges>
#include <cmath>
#include "polygonMarker.h"
#include "renderer/model.h"
#include "stbImage.h"
#include "polygonHelper.h"




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
		std::shared_ptr<STBimage> loadedImage;
		std::shared_ptr<STBimage> alteredImage;
		std::shared_ptr<STBimage> projectionImage;
		std::vector<glm::vec2> imagePolygonPointsInUV;
		std::vector<glm::vec2> imagePolygonPointsInPixels;
		std::shared_ptr<OptimizationResult> optimizationResult;
		std::unordered_set<int> triangleHash; //HashSet<int>
		std::pair<std::vector<glm::vec2>, std::vector<int>> texCoordsAndTriangleIds;
		bool drawImagePoints;
		bool visualizeModelPoints;
		bool cropImage;
		bool convexHull;
		glm::vec2 texCoordsOffset;
		int imageRow;
		int imageColumn;
		std::vector<PolygonMarker> markers;

		/// Initializes a new instance of the <see cref="MappingDataSet"/> class.
		MappingDataSet(){
		}

			/// Crops the image.
		void performCropping() {
			prepareImagePolygon();

			// Don't do this anymore. Now that an image can be cut at arbitrary points, image points can be outside the defined polygon.
			// Cropping the image to the minimum size the polygon requires will lead to projection errors.
			// One could check if all image points are inside the polygon and then perform simple cropping. But this is sth for the future
			// and not neccessary right now.
			/*
			if (CropImage) {
				CropImageSimple();
			}
			*/

			imagePolygonPointsInUV.reserve(imagePolygonPointsInPixels.size());
			for (int i = 0; i < imagePolygonPointsInPixels.size(); i++) {
				imagePolygonPointsInUV[i] = glm::vec2(imagePolygonPointsInPixels[i].x / projectionImage->width, imagePolygonPointsInPixels[i].y / projectionImage->height);
			}
			imagePolygonPointsInUV = imagePolygonPointsInUV;
		}


		/// Prepares the image polygon.
		void prepareImagePolygon() {
			std::vector<glm::vec2> polygonPoints;

			if (cropImage && markers.size() > 2) {
				for (size_t i = 0; i < markers.size(); i++)
				{
					polygonPoints.push_back(markers[i].position);
				}
			}
			else {
				int endX = loadedImage->width - 1;
				int endY = loadedImage->height - 1;
				polygonPoints = {
					glm::vec2(0, 0),
					glm::vec2(endX, 0),
					glm::vec2(endX, endY),
					glm::vec2(0, endY)
				};
			}

			if (convexHull) {
				imagePolygonPointsInPixels = HelperLib::PolygonHelper::getConvexHull(polygonPoints);
			}
			else {
				imagePolygonPointsInPixels = polygonPoints;
			}

			/*
			if (CropImage) {
				if (PolygonOffsetInPixels != 0) {
					ImagePolygonPointsInPixels = PolygonHelper.PerformPolygonOffsetting(ImagePolygonPointsInPixels, PolygonOffsetInPixels);
				}
			}
			*/

			// Insert the first point at the end again, the IsPointInsidePolygon algorithm needs it
			std::vector<glm::vec2> polyList(imagePolygonPointsInPixels);
			polyList.push_back(imagePolygonPointsInPixels[0]);
			imagePolygonPointsInPixels=std::vector<glm::vec2>(polyList);
		}

	private:
		/// Crops the image in a simple way.
		void cropImageSimple() {
			std::vector<glm::vec2> polygon = imagePolygonPointsInPixels;

			auto min_lambda_x = [](glm::vec2& a, glm::vec2& b) {return (a.y < b.y); };
			int minX = std::lround( std::min_element(polygon.begin(),polygon.end(), min_lambda_x)->x );

			auto max_lambda_x = [](glm::vec2& a, glm::vec2& b) {return (a.x > b.x); };
			int maxX = std::lround( std::min_element(polygon.begin(), polygon.end(), max_lambda_x)->x );

			auto min_lambda_y = [](glm::vec2& a, glm::vec2& b) {return (a.y < b.y); };
			int minY = std::lround( std::min_element(polygon.begin(), polygon.end(), min_lambda_y)->y );

			auto max_lambda_y = [](glm::vec2& a, glm::vec2& b) {return (a.y > b.y); };
			int maxY = std::lround( std::min_element(polygon.begin(), polygon.end(), max_lambda_y)->y );

			//Rectangle cropArea = new Rectangle(minX, minY, maxX - minX, maxY - minY);
			//Bitmap croppedImage = new Bitmap(cropArea.Width, cropArea.Height);

			//using (Graphics gfx = Graphics.FromImage(croppedImage)) {
			//	gfx.DrawImage(ProjectionImage, new Rectangle(0, 0, croppedImage.Width, croppedImage.Height), cropArea, GraphicsUnit.Pixel);
			//}
			//
			//ProjectionImage.Dispose();
			//ProjectionImage = croppedImage;
			//
			//for (int i = 0; i < polygon.Length; i++) {
			//	polygon[i].X -= minX;
			//	polygon[i].Y -= minY;
			//}
			//
			//for (int i = 0; i < ImagePoints.Length; i++) {
			//	ImagePoints[i].X -= minX;
			//	ImagePoints[i].Y -= minY;
			//}

			//ProjectionImage.Save(@"C:\Users\Paschl\Downloads\test.jpg");
		}
	};

}
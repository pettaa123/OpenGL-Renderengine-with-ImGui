#include "texturemapping/core/mappingDataSet.h"
#include "texturemapping/helper/polygonHelper.h"

namespace TextureMapping {

	MappingDataSet::MappingDataSet() :
		model(nullptr),
		optimizationResult(nullptr),
		drawImagePoints(false),
		visualizeModelPoints(false),
		cropImage(false),
		convexHull(false),
		imageRow(0),
		imageColumn(0)
	{}

		/// Crops the image.
		void MappingDataSet::performCropping() {
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
			imagePolygonPointsInUV[i] = glm::vec2(imagePolygonPointsInPixels[i].x / projectionImage.width, imagePolygonPointsInPixels[i].y / projectionImage.height);
		}
		imagePolygonPointsInUV = imagePolygonPointsInUV;
	}

	/// Prepares the image polygon.
	void MappingDataSet::prepareImagePolygon() {
		std::vector<glm::vec2> polygonPoints;

		if (cropImage && markers.size() > 2) {
			for (size_t i = 0; i < markers.size(); i++)
			{
				polygonPoints.push_back(markers[i].position);
			}
		}
		else {
			int endX = loadedImage.width - 1;
			int endY = loadedImage.height - 1;
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
		imagePolygonPointsInPixels = std::vector<glm::vec2>(polyList);
	}

	/// Crops the image in a simple way.
	void MappingDataSet::cropImageSimple() {
		std::vector<glm::vec2> polygon = imagePolygonPointsInPixels;

		auto min_lambda_x = [](glm::vec2& a, glm::vec2& b) {return (a.y < b.y); };
		int minX = std::lround(std::min_element(polygon.begin(), polygon.end(), min_lambda_x)->x);

		auto max_lambda_x = [](glm::vec2& a, glm::vec2& b) {return (a.x > b.x); };
		int maxX = std::lround(std::min_element(polygon.begin(), polygon.end(), max_lambda_x)->x);

		auto min_lambda_y = [](glm::vec2& a, glm::vec2& b) {return (a.y < b.y); };
		int minY = std::lround(std::min_element(polygon.begin(), polygon.end(), min_lambda_y)->y);

		auto max_lambda_y = [](glm::vec2& a, glm::vec2& b) {return (a.y > b.y); };
		int maxY = std::lround(std::min_element(polygon.begin(), polygon.end(), max_lambda_y)->y);

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

}
#pragma once
#include <optional>
#include "texturemapping/hardwareacceleration/base/accelerator.h"
#include "texturemapping/modeling/model.h"
#include "texturemapping/modeling/texture.h"
#include "texturemapping/helper/mathExtensions.h"
#include "texturemapping/mapping/core/mergingResult.h"
#include "texturemapping/mapping/meshCutter.h"
#include "texturemapping/mapping/core/mergingResult.h"


namespace TextureMapping {


	/// Projects an image to a model.
	class ImageToModelProjector {
	private:
		TextureMapping::Model& m_model;
		Accelerator& m_accelerator;
		std::vector<MappingDataSet> m_projectedDataSets;
		bool m_isCancelled;
		//ApplicationParameters m_parameters;
	public:

		ImageToModelProjector(TextureMapping::Model& model, Accelerator& accelerator);

		/// Projects the image to the model.
		void projectImage(MappingDataSet& dataSet, int originId, glm::mat3x4& projectionMatrix);

		/// Solves overlaying conflicts and generates the merged texture.
		std::optional<MergingResult> finish();

		/// Cancels the current process.
		void cancel() {
			m_isCancelled = true;
		}

		/// Determines whether a coordinate is a valid texture coordinate.
		bool isTextureCoordinate(const glm::vec2& coordinate) {
			bool isSet = coordinate.x != -1000 && coordinate.y != -1000;
			return isSet;
		}

	private:
		/// <summary>
		/// Merges the projected images.
		/// This code will stretch all pictures to the same size.
		/// </summary>
		/// <param name="borderSize">Size of the border.</param>
		/// <returns></returns>
		/// <exception cref="Exception">The merged texture size ({mergedWidth}x{mergedHeight})</exception>
		MergingResult mergeProjectedImages(int borderSize) {
			int twoBorderSize = 2 * borderSize;
			std::vector<BaseLib::STBimage> images;

			for (auto& p : m_projectedDataSets) {
				images.push_back(p.projectionImage);
			}

			// Stretch all images to the size of the biggest image
			// Otherwise, a bigger image will get compressed/shrinkened and will loose data
			// Using stretching, no information will get lost
			int maxImageWidth = 0;
			int maxImageHeight = 0;

			for (auto& p : m_projectedDataSets) {
				maxImageWidth = (p.projectionImage.width > maxImageWidth) ? p.projectionImage.width : maxImageWidth;
				maxImageHeight = (p.projectionImage.height > maxImageHeight) ? p.projectionImage.height : maxImageHeight;
			}

			int imagesPerRow = (int)ceil(sqrt(images.size()));
			int imagesPerColumn = (int)ceil(images.size() / imagesPerRow);

			int mergedWidth = imagesPerRow * (maxImageWidth + 2 * borderSize);
			int mergedHeight = imagesPerColumn * (maxImageHeight + 2 * borderSize);

			//SystemSpecifications specifications = OpenGLEngine.GetSystemSpecifications();
			assert((mergedWidth < 16384) & (mergedHeight < 16384) && "The merged texture size ({mergedWidth}x{mergedHeight}) exceeds the graphics card's capabilities.");

			BaseLib::STBimage mergedImage;
			mergedImage.width = mergedWidth;
			mergedImage.height = mergedHeight;
			mergedImage.channels = m_projectedDataSets[0].projectionImage.channels;
			mergedImage.data = std::make_shared<uint8_t[]>(mergedWidth * mergedHeight * mergedImage.channels);


			// For testing
			//gfx.FillRectangle(Brushes.Red, 0, 0, mergedImage.Width, mergedImage.Height);

			for (int i = 0; i < mergedWidth * mergedHeight * mergedImage.channels;) {
				mergedImage.data[i++] = 0xFF;
				mergedImage.data[i++] = 0x00;
				mergedImage.data[i++] = 0x00;
				mergedImage.data[i++] = 0xFF;
			}

			int xOffset = borderSize;
			int yOffset = borderSize;

			size_t offset = (borderSize + mergedWidth) * images[0].channels;
			size_t writeIdx = -1;
			for (size_t j = 0; j < mergedHeight - twoBorderSize; j++) {
				for (size_t i = 0; i < images.size(); i++) {
					for (size_t k = 0; k < images[i].width * images[i].channels; k++)
					{
						writeIdx = offset + k;
						size_t readIdx = k + j * images[i].width * images[i].channels;
						mergedImage.data[writeIdx] = images[i].data.get()[readIdx];
					}
					offset = writeIdx+ 2 * images[0].channels +1;
				}
				//gfx.DrawImage(image, xOffset - borderSize, yOffset - borderSize, maxImageWidth + twoBorderSize, maxImageHeight + twoBorderSize);
				//gfx.DrawImage(image, xOffset, yOffset, maxImageWidth, maxImageHeight);
				
			}



			mergedImage.write("red2.png");

			MergingResult mergingResult;
			mergingResult.columns = imagesPerRow;
			mergingResult.rows = imagesPerColumn;
			mergingResult.image = mergedImage;
			mergingResult.heightFactor = 1.0f / imagesPerColumn - (float)twoBorderSize / mergedHeight;
			mergingResult.widthFactor = 1.0f / imagesPerRow - (float)twoBorderSize / mergedWidth;


			//mergedImage.Save(@"C:\Users\Paschl\Downloads\merged.jpg", ImageFormat.Jpeg);
			return mergingResult;
		}

		void transformTextureCoordinates(MergingResult& mergingResult, std::vector<int>& triangleOriginIds, int borderSize) {
			//Wie viele Bilder pro Spalte
			int columns = mergingResult.columns;
			//Faktor mit denen die alten Texturkoordinaten pro Bild zu verrechnen sind, damits für das mergedImage passt.
			float widthFactor = mergingResult.widthFactor;
			float heightFactor = mergingResult.heightFactor;
			std::vector<glm::vec2>& modelTexCoords = m_model.textureDescription.coordinates;
			
			for (size_t i = 0; i < m_projectedDataSets.size(); i++) {
				MappingDataSet& dataSet = m_projectedDataSets[i];

				int row = (int)i / columns;
				int column = (int)i % columns;
				float xOffset = (1.0f + 2 * borderSize * column) / mergingResult.image.width;
				float yOffset = (1.0f + 2 * borderSize * row) / mergingResult.image.height;
				dataSet.imageRow = row;
				dataSet.imageColumn = column;
				dataSet.texCoordsOffset = glm::vec2(xOffset, yOffset);
			}

			for (size_t i = 0; i < triangleOriginIds.size(); i++) {
				int texIdx = (int)i * 3;
				MappingDataSet& dataSet = m_projectedDataSets[triangleOriginIds[i]];

				for (int a = 0; a < 3; a++) {
					glm::vec2& coordinate = modelTexCoords[texIdx + a];

					if (isTextureCoordinate(coordinate)) {
						// Don't use "coordinate =" here (it's a struct)
						float transformedX = (dataSet.imageColumn + coordinate.x) * widthFactor + dataSet.texCoordsOffset.x;
						float transformedY = (dataSet.imageRow + coordinate.y) * heightFactor + dataSet.texCoordsOffset.y;
						modelTexCoords[texIdx + a] = glm::vec2(transformedX, transformedY);
					}
				}
			}
		}
	};
}

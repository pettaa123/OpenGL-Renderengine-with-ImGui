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
		std::optional<MergingResult> finish() {
			assert(m_projectedDataSets.size() != 0 && "projectedDataSets empty");

			// Sort the data sets according to their (re?)projection errors
			// The image with lowest error will be on top of others
			//_projectedDataSets = _projectedDataSets.OrderBy(p => p.OptimizationResult.AverageErrorInPixels).ToList();

			MeshCutter cutter(m_model);
			//Texturekoordinaten / Liste aus welchem dataSet diese Stammen
			std::optional<std::pair<std::vector<glm::vec2>, std::vector<int>>> textureCoordinates = cutter.solveOverlayingTexturesConflicts(m_projectedDataSets);

			if (m_isCancelled) {
				return {};
			}

			m_model.textureDescription.coordinates = textureCoordinates->first;

			int borderSize = 1;
			MergingResult mergingResult = mergeProjectedImages(borderSize);
			//mergingResult.Image.Save(@"C:\Users\tupp_je\Desktop\mergingResult.jpg");

			if (m_isCancelled) {
				return {};
			}
			//Rechnet die Texturkoordinaten von Einzelbildern auf das gestitchte Bild um
			//und legt diese in _model.Texturedescription.Coordinates ab
			transformTextureCoordinates(mergingResult, textureCoordinates->second, borderSize);
			//_projectedDataSets.ForEach(p => p.Image.Dispose());

			// TODO: Use MipMaps here?
			//Die Texturkoordinaten sind geflippt        


			std::shared_ptr<Texture> imageTexture = std::make_shared<Texture>(mergingResult.image, true);
			TextureParameters parameters{
				0x2601,//TextureMagFilter.Linear,
				0x2601,//TextureMinFilter.Linear,
				0x8370,//TextureWrapMode.MirroredRepeat,
				0x8370//TextureWrapMode.MirroredRepeat
			};

			m_model.setTexture(imageTexture);
			m_model.textureDescription.parameters = parameters;

			return mergingResult;
		}

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

			// For testing
			//gfx.FillRectangle(Brushes.Red, 0, 0, mergedImage.Width, mergedImage.Height);

			int xOffset = borderSize;
			int yOffset = borderSize;

			
			std::unique_ptr<uint8_t[]> mergedData = std::make_unique<uint8_t[]>(mergedImage.width * mergedImage.height * mergedImage.channels);

			size_t offset=borderSize;

			for (size_t i = 0; i < images.size(); i++) {
				// For some reason, some images need to have the width and height parameter to be set
				for (int j = 0; j < images[i].width * images[i].height * images[i].channels;j++) {
					mergedData[offset - borderSize + j] = images[i].data.get()[j];
				}
				//gfx.DrawImage(image, xOffset - borderSize, yOffset - borderSize, maxImageWidth + twoBorderSize, maxImageHeight + twoBorderSize);
				//gfx.DrawImage(image, xOffset, yOffset, maxImageWidth, maxImageHeight);
				offset += (maxImageWidth + maxImageHeight + twoBorderSize);

				if (xOffset == mergedWidth + borderSize) {
					xOffset = borderSize;
					yOffset += maxImageHeight + twoBorderSize;
				}
			}
			

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
			std::vector<glm::vec2> modelTexCoords = m_model.textureDescription.coordinates;

			for (size_t i = 0; i < m_projectedDataSets.size(); i++) {
				MappingDataSet dataSet = m_projectedDataSets[i];

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
				MappingDataSet dataSet = m_projectedDataSets[triangleOriginIds[i]];

				for (int a = 0; a < 3; a++) {
					glm::vec2 coordinate = modelTexCoords[texIdx + a];

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

#include "texturemapping/mapping/imageToModelProjector.h"

namespace TextureMapping {

	ImageToModelProjector::ImageToModelProjector(TextureMapping::Model& model, Accelerator& accelerator)
		:m_model(model),
		m_accelerator(accelerator),
		m_isCancelled(false)
	{}

    void ImageToModelProjector::projectImage(MappingDataSet& dataSet, int originId, glm::mat3x4& projectionMatrix) {
        std::vector<float> projectionMatrixVector = MathExtension::toVector(projectionMatrix);
        // PNP: Hand over the additional params from the TextureMapper to the accelerator
        //openclaccelerator projectimage
        ProjectionResult projectionResult = m_accelerator.projectImage(dataSet, projectionMatrixVector, originId);
        dataSet.triangleHash = projectionResult.triangleHash;
        dataSet.texCoordsAndTriangleIds = projectionResult.trianglePair;
        MappingDataSet projectedDataSet(dataSet);
        m_projectedDataSets.push_back(projectedDataSet);
    }

	/// Solves overlaying conflicts and generates the merged texture.
	std::optional<MergingResult> ImageToModelProjector::finish() {
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

		m_model.recreateMesh();

		m_model.setTexture(imageTexture);
		m_model.textureDescription.parameters = parameters;

		return mergingResult;
	}
}

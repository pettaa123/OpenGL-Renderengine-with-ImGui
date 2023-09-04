#include "texturemapping/mapping/imageToModelProjector.h"

namespace TextureMapping {

	ImageToModelProjector::ImageToModelProjector(TextureMapping::Model& model, Accelerator& accelerator)
		:m_model(model),
		m_accelerator(accelerator)
	{}

    void ImageToModelProjector::projectImage(MappingDataSet& dataSet, int originId, glm::mat3x4& projectionMatrix) {
        std::vector<glm::vec3> modelPoints = dataSet.modelPoints;
        std::vector<glm::vec2> imagePoints = dataSet.imagePoints;

        std::vector<float> projectionMatrixVector = MathExtension::toVector(projectionMatrix);
        // PNP: Hand over the additional params from the TextureMapper to the accelerator
        //openclaccelerator projectimage
        ProjectionResult projectionResult = m_accelerator.projectImage(dataSet, projectionMatrixVector, originId);
        dataSet.triangleHash = projectionResult.triangleHash;
        dataSet.texCoordsAndTriangleIds = projectionResult.trianglePair;
        m_projectedDataSets.push_back(dataSet);
    }
}

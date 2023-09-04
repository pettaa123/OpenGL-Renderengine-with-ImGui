#pragma once

#include "texturemapping/hardwareacceleration/base/accelerator.h"
#include "engine/renderer/model.h"
#include "texturemapping/helper/mathExtensions.h"
#include "texturemapping/mapping/core/mergingResult.h"

namespace TextureMapping{


    /// Projects an image to a model.
    class ImageToModelProjector {
    private:
        Engine::Model &m_model;
        Accelerator &m_accelerator;
        std::vector<MappingDataSet> m_projectedDataSets;
        bool m_isCancelled;
        //ApplicationParameters m_parameters;
    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="ImageToModelProjector"/> class.
        /// </summary>
        /// <param name="model">The model.</param>
        /// <param name="accelerator">The accelerator.</param>
        /// <param name="cancelObserver">The cancel observer.</param>
        ImageToModelProjector(Engine::Model& model, Accelerator& accelerator);

        /// <summary>
        /// Projects the image to the model.
        /// </summary>
        /// <param name="dataSet">The data set.</param>
        /// <param name="originId">The origin identifier.</param>
        /// <param name="projectionMatrix">The projection matrix.</param>
        void projectImage(MappingDataSet& dataSet, int originId, glm::mat3x4& projectionMatrix) {
            std::vector<glm::vec3> modelPoints = dataSet.modelPoints;
            std::vector<glm::vec2> imagePoints = dataSet.imagePoints;

            std::vector<float> projectionMatrixVector = MathExtension::toVector(projectionMatrix);
            // PNP: Hand over the additional params from the TextureMapper to the accelerator
            //openclaccelerator projectimage
            ProjectionResult projectionResult = m_accelerator.projectImage(dataSet,projectionMatrixVector ,originId);
            dataSet.triangleHash = projectionResult.triangleHash;
            dataSet.texCoordsAndTriangleIds = projectionResult.trianglePair;
            m_projectedDataSets.push_back(dataSet);
        }

        /// <summary>
        /// Solves overlaying conflicts and generates the merged texture.
        /// </summary>
        /// <param name="loadingObserver">The loading observer.</param>
        /// <returns></returns>
        MergingResult finish() {
            assert(m_projectedDataSets.size() != 0 && "projectedDataSets empty")

            // Sort the data sets according to their (re?)projection errors
            // The image with lowest error will be on top of others
            //_projectedDataSets = _projectedDataSets.OrderBy(p => p.OptimizationResult.AverageErrorInPixels).ToList();

            MeshCutter cutter = new MeshCutter(_model, _cancelObserver);
            //Texturekoordinaten / Liste aus welchem dataSet diese Stammen
            Tuple<List<Vector2>, List<int>> textureCoordinates = cutter.SolveOverlayingTexturesConflicts(_projectedDataSets, loadingObserver);

            if (_isCancelled) {
                return null;
            }

            _model.TextureDescription.Coordinates = textureCoordinates.Item1;

            StartWatch();
            int borderSize = 1;
            MergingResult mergingResult = MergeProjectedImages(borderSize);
            //mergingResult.Image.Save(@"C:\Users\tupp_je\Desktop\mergingResult.jpg");

            if (_isCancelled) {
                return null;
            }
            //Rechnet die Texturkoordinaten von Einzelbildern auf das gestitchte Bild um
            //und legt diese in _model.Texturedescription.Coordinates ab
            TransformTextureCoordinates(mergingResult, textureCoordinates.Item2, borderSize);
            //_projectedDataSets.ForEach(p => p.Image.Dispose());

            // TODO: Use MipMaps here?
            //Die Texturkoordinaten sind geflippt
            Texture imageTexture = new Texture(mergingResult.Image, true, true);
            TextureParameters parameters = new TextureParameters() {
                MagFilter = TextureMagFilter.Linear,
                MinFilter = TextureMinFilter.Linear,
                WrappingModeS = TextureWrapMode.MirroredRepeat,
                WrappingModeT = TextureWrapMode.MirroredRepeat
            };
            StopWatch();
            Profiler.GenerateTextureTimeMs = TrackedTime;

            _model.SetTexture(imageTexture);
            _model.TextureDescription.Parameters = parameters;

            return mergingResult;
        }

        /// <summary>
        /// Cancels the current process.
        /// </summary>
        public void Cancel() {
            _isCancelled = true;
        }

        /// <summary>
        /// Merges the projected images.
        /// This code will stretch all pictures to the same size.
        /// </summary>
        /// <param name="borderSize">Size of the border.</param>
        /// <returns></returns>
        /// <exception cref="Exception">The merged texture size ({mergedWidth}x{mergedHeight})</exception>
        private MergingResult MergeProjectedImages(int borderSize) {
            int twoBorderSize = 2 * borderSize;
            List<Bitmap> images = _projectedDataSets.Select(p = > p.ProjectionImage).ToList();

            // Stretch all images to the size of the biggest image
            // Otherwise, a bigger image will get compressed/shrinkened and will loose data
            // Using stretching, no information will get lost
            int maxImageWidth = images.Max(i = > i.Width);
            int maxImageHeight = images.Max(i = > i.Height);

            int imagesPerRow = MathExt.Ceiling(MathExt.Sqrtf(images.Count));
            int imagesPerColumn = MathExt.Ceiling((float)images.Count / imagesPerRow);

            int mergedWidth = imagesPerRow * (maxImageWidth + 2 * borderSize);
            int mergedHeight = imagesPerColumn * (maxImageHeight + 2 * borderSize);

            SystemSpecifications specifications = OpenGLEngine.GetSystemSpecifications();
            if (mergedWidth > specifications.MaxTextureSize || mergedHeight > specifications.MaxTextureSize) {
                throw new Exception($"The merged texture size ({mergedWidth}x{mergedHeight}) exceeds the graphics card's capabilities.");
            }

            Bitmap mergedImage = new Bitmap(mergedWidth, mergedHeight);
            Graphics gfx = Graphics.FromImage(mergedImage);
            // For testing
            //gfx.FillRectangle(Brushes.Red, 0, 0, mergedImage.Width, mergedImage.Height);

            int xOffset = borderSize;
            int yOffset = borderSize;
            foreach(Bitmap image in images) {
                // For some reason, some images need to have the width and height parameter to be set
                gfx.DrawImage(image, xOffset - borderSize, yOffset - borderSize, maxImageWidth + twoBorderSize, maxImageHeight + twoBorderSize);
                gfx.DrawImage(image, xOffset, yOffset, maxImageWidth, maxImageHeight);
                xOffset += maxImageWidth + twoBorderSize;

                if (xOffset == mergedWidth + borderSize) {
                    xOffset = borderSize;
                    yOffset += maxImageHeight + twoBorderSize;
                }
            }

            MergingResult mergingResult = new MergingResult() {
                Columns = imagesPerRow,
                Rows = imagesPerColumn,
                Image = mergedImage,
                HeightFactor = 1f / imagesPerColumn - (float)twoBorderSize / mergedHeight,
                WidthFactor = 1f / imagesPerRow - (float)twoBorderSize / mergedWidth
            };

            //mergedImage.Save(@"C:\Users\Paschl\Downloads\merged.jpg", ImageFormat.Jpeg);
            return mergingResult;
        }

        /// <summary>
        /// Transforms the texture coordinates.
        /// </summary>
        /// <param name="mergingResult">The merging result.</param>
        /// <param name="triangleOriginIds">The triangle origin ids.</param>
        /// <param name="borderSize">Size of the border.</param>
        private void TransformTextureCoordinates(MergingResult mergingResult, List<int> triangleOriginIds, int borderSize) {
            //Wie viele Bilder pro Spalte
            int columns = mergingResult.Columns;
            //Faktor mit denen die alten Texturkoordinaten pro Bild zu verrechnen sind, damits für das mergedImage passt.
            float widthFactor = mergingResult.WidthFactor;
            float heightFactor = mergingResult.HeightFactor;
            List<Vector2> modelTexCoords = _model.TextureDescription.Coordinates;

            for (int i = 0; i < _projectedDataSets.Count; i++) {
                MappingDataSet dataSet = _projectedDataSets[i];

                int row = i / columns;
                int column = i % columns;
                float xOffset = (1f + 2 * borderSize * column) / mergingResult.Image.Width;
                float yOffset = (1f + 2 * borderSize * row) / mergingResult.Image.Height;
                dataSet.ImageRow = row;
                dataSet.ImageColumn = column;
                dataSet.TexCoordsOffset = new Vector2(xOffset, yOffset);
            }

            for (int i = 0; i < triangleOriginIds.Count; i++) {
                int texIdx = i * 3;
                MappingDataSet dataSet = _projectedDataSets[triangleOriginIds[i]];

                for (int a = 0; a < 3; a++) {
                    Vector2 coordinate = modelTexCoords[texIdx + a];

                    if (coordinate.IsTextureCoordinate()) {
                        // Don't use "coordinate =" here (it's a struct)
                        float transformedX = (dataSet.ImageColumn + coordinate.X) * widthFactor + dataSet.TexCoordsOffset.X;
                        float transformedY = (dataSet.ImageRow + coordinate.Y) * heightFactor + dataSet.TexCoordsOffset.Y;
                        modelTexCoords[texIdx + a] = new Vector2(transformedX, transformedY);
                    }
                }
            }
        }
    }
}

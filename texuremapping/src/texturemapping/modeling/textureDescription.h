#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "texturemapping/modeling/texture.h"

namespace TextureMapping {

    struct TextureParameters {
        float mipMapLevelFactor;
        int minFilter;
        int minMipMapFilter;
        int magFilter;
        int wrappingModeS;
        int wrappingModeT;
    };

    struct TextureLayout {
        float stretchX;
        float stretchY;
        bool useTextureDimensions;
    };



    /// <summary>
    /// Data container for texure parameters.
    /// </summary>
    class TextureDescription {

    public:
        std::shared_ptr<Texture> texture;
        TextureLayout layout;
        std::vector<glm::vec2> coordinates;
        TextureParameters parameters;
        bool allowMipMapFactorCalculation;

        /// <summary>
        /// Initializes a new instance of the <see cref="TextureDescription"/> class.
        /// </summary>
        TextureDescription();


    private:
        /// <summary>
        /// Generates default parameters.
        /// </summary>
         /// <returns></returns>
        TextureParameters generateDefaultParameters();

    };
}




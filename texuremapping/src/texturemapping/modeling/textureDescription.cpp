#include "texturemapping/modeling/textureDescription.h"

namespace TextureMapping {

    TextureDescription::TextureDescription() {
        allowMipMapFactorCalculation = true;
        parameters = generateDefaultParameters();
    }

    TextureParameters TextureDescription::generateDefaultParameters() {
        TextureParameters parameters{
            1,
            0x2601,//TextureMagFilter.Linear
            0x2601,//TextureMinFilter.Linear,
            0x2703,//TextureMinFilter.LinearMipmapLinear,
            0x2901,//TextureWrapMode.Repeat,
            0x2901//TextureWrapMode.Repeat
        };
        return parameters;
    }
}

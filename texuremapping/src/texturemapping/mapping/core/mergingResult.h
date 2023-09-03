#pragma once

#include "texturemapping/core/stbImage.h"

namespace TextureMapping{

    /// Data container for merging results.
    struct MergingResult {

        int rows;
        int columns;
        STBimage image;
        float widthFactor;
        float heightFactor;
    };
}

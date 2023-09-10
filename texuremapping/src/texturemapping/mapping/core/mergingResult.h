#pragma once

#include "baseLib/stbImage.h"

namespace TextureMapping{

    /// Data container for merging results.
    struct MergingResult {

        int rows=0;
        int columns=0;
        BaseLib::STBimage image;
        float widthFactor=0;
        float heightFactor=0;
    };
}

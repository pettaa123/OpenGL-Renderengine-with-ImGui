#pragma once
#include <stdint.h>
#include <glm/glm.hpp>

namespace TextureMapping{

    /// Data container for defining crop areas on the images.

    struct PointF
    {
        uint32_t x;
        uint32_t y;
    };

    class PolygonMarker {
    public:
        // The relative values are needed in case the SetupForm2 will be made resizable
            //Control control;
        glm::vec2 position;
        //public SizeF RelativeSize
        PointF relativePosition;

        //void CreateControlRegion() {
        //     using (GraphicsPath path = new GraphicsPath()) {
        //         path.AddEllipse(new Rectangle(0, 0, Control.Width, Control.Height));
        //         Control.Region = new Region(path);
        //     }
        // }
    };
}

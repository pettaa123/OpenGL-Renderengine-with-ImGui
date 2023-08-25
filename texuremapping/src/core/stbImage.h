#pragma once


#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

namespace TextureMapping {
	//container for stb_image data
	struct STBimage {
		std::string name;
		int width;
		int height;
		int channels;
		stbi_uc* data;

	};
}
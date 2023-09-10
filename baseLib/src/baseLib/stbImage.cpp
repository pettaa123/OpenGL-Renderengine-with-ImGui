#include "baseLib/stbImage.h"
#include "stb_image/stb_image.h"

namespace BaseLib {
	bool STBimage::load(const std::filesystem::path& imagePath) {
		name = imagePath.filename().string();
		uint8_t* img = stbi_load(imagePath.string().c_str(), &width, &height, &channels, 0);
		if (img == NULL)
			return false;
		data.reset(img);
		return (data != nullptr) ? true : false;
	};
}


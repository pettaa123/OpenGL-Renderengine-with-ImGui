#include "texturemapping/core/stbImage.h"
#include "stb_image/stb_image.h"

namespace TextureMapping {
	bool STBimage::load(const std::filesystem::path& imagePath) {
		name = imagePath.filename().string();
		data.reset(stbi_load(imagePath.string().c_str(), &width, &height, &channels, 0));
		return (data != nullptr) ? true : false;
	};
}


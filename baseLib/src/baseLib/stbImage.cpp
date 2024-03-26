#include "baseLib/stbImage.h"
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

namespace BaseLib {
	bool STBimage::load_U8(const std::filesystem::path& imagePath) {
		name = imagePath.filename().string();
		uint8_t* img = stbi_load(imagePath.string().c_str(), &width, &height, &channels, 0);
		if (img == NULL)
			return false;
		dataU8.reset(img);
		return (dataU8.get() != nullptr) ? true : false;
	}

	bool STBimage::load_U16(const std::filesystem::path& imagePath) {
		name = imagePath.filename().string();
		uint16_t* img = stbi_load_16(imagePath.string().c_str(), &width, &height, &channels, 0);
		if (img == NULL)
			return false;
		dataU16.reset(img);
		return (dataU16.get() != nullptr) ? true : false;
	}
	//stbi can only write 8bpp
	bool STBimage::write_U8(const std::filesystem::path& savePath) {
		if (stbi_write_png(savePath.string().c_str(), width, height, channels, dataU8.get(), width * channels))
			return EXIT_SUCCESS;
		return EXIT_FAILURE;
	}
}


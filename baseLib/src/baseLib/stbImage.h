#pragma once

#include <string>
#include <memory>
#include <filesystem>

namespace BaseLib {
	//container for stb_image data
	struct STBimage {
		std::string name;
		int width=0;
		int height=0;
		int channels=0;
		std::shared_ptr<uint8_t[]> data = nullptr;
		std::shared_ptr<uint16_t[]> data16 = nullptr;
		bool load(const std::filesystem::path& imagePath);
		bool load_16(const std::filesystem::path& imagePath);
		bool write(const std::filesystem::path& imagePath);
	};
}
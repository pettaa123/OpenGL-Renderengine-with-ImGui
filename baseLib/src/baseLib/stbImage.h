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
		std::unique_ptr<uint8_t[]> dataU8 = nullptr;
		std::unique_ptr<uint16_t[]> dataU16 = nullptr;
		std::unique_ptr<float_t[]> dataSGL = nullptr;
		std::unique_ptr<double_t[]> dataDBL = nullptr;

		bool load_U8(const std::filesystem::path& imagePath);
		bool load_U16(const std::filesystem::path& imagePath);
		bool write_U8(const std::filesystem::path& imagePath);
	};
}
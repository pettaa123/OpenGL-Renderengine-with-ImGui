#pragma once

#include <string>
#include <memory>
#include <filesystem>

namespace TextureMapping {
	//container for stb_image data
	struct STBimage {
		std::string name;
		int width=0;
		int height=0;
		int channels=0;
		std::shared_ptr<uint8_t> data;

		bool load(const std::filesystem::path& imagePath);
	};
}
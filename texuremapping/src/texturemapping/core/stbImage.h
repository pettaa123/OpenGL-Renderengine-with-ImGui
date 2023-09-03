#pragma once

#include <string>
#include <memory>
#include <filesystem>

namespace TextureMapping {
	//container for stb_image data
	struct STBimage {
		std::string name;
		int width;
		int height;
		int channels;
		std::shared_ptr<unsigned char> data;

		bool load(const std::filesystem::path& imagePath);
	};
}
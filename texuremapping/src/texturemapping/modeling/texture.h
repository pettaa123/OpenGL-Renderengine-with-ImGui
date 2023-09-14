#pragma once

#include "engine/renderer/texture.h"
#include "engine/renderer/texture.h"
#include "baseLib/stbImage.h"

namespace TextureMapping {

    //wrapper class for engine texture
	class Texture {

	public:

		Texture(const BaseLib::STBimage& image, bool generateMipMaps = true);

		bool getUseMipMaps() { return m_useMipMaps; };
		int getId() { return m_id; };
		int getWidth() { return m_width; }
		int getheight() { return m_height; }
		BaseLib::STBimage& getImage() { return m_image; }

	private:
		std::string m_file;
		BaseLib::STBimage m_image;
		bool m_isFlipped;
		bool m_useMipMaps;
		int m_id;
		int m_width;
		int m_height;

	};
}

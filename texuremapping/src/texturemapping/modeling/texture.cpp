#include "texturemapping/modeling/texture.h"

namespace TextureMapping {


	Texture::Texture(const BaseLib::STBimage& image, bool generateMipMaps)
		:m_image(image),
		m_useMipMaps(generateMipMaps)
	{
		Engine::TextureSpecification textureSpecs;
		if (m_image.channels == 4)
		{
			textureSpecs.format = Engine::ImageFormat::RGBA8;
		}
		else if (m_image.channels == 3)
		{
			textureSpecs.format = Engine::ImageFormat::RGB8;
		}
		textureSpecs.width = m_image.width;
		textureSpecs.height = m_image.height;
		textureSpecs.generateMips = m_useMipMaps;


		std::shared_ptr<Engine::Texture2D> tex = Engine::Texture2D::create(textureSpecs);
		tex->setData(m_image.data.get(), m_image.width * m_image.height * m_image.channels);
	}
}

#include "renderer/texture.h"

#include "renderer/renderer.h"
#include "renderer/openGLTexture.h"

namespace Engine {

	std::shared_ptr<Texture2D> Texture2D::create(const TextureSpecification& specification)
	{
		return std::make_shared<OpenGLTexture2D>(specification);
	}

	std::shared_ptr<Texture2D> Texture2D::create(const std::string& path)
	{
		return std::make_shared<OpenGLTexture2D>(path);
	}

}

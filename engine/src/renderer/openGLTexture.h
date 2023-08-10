#pragma once

#include "renderer/Texture.h"

#include <glad/glad.h>

namespace Engine {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();

		virtual const TextureSpecification& getSpecification() const override { return m_specification; }

		virtual uint32_t getWidth() const override { return m_width;  }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual uint32_t getRendererID() const override { return m_rendererID; }

		virtual const std::string& getPath() const override { return m_path; }
		
		virtual void setData(void* data, uint32_t size) override;

		virtual void bind(uint32_t slot = 0) const override;

		virtual bool isLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_rendererID == other.getRendererID();
		}
	private:
		TextureSpecification m_specification;

		std::string m_path;
		bool m_IsLoaded = false;
		uint32_t m_width, m_height;
		uint32_t m_rendererID;
		GLenum m_internalFormat, m_dataFormat;
	};

}

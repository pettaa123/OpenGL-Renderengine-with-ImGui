
#include "material.h"
#include "renderer/openGLMaterial.h"

#include "renderer/openGLRenderer.h"

namespace Engine {

	std::shared_ptr<Material> Material::create(const std::shared_ptr<Shader>& shader, const std::string& name)
	{
		return std::shared_ptr<OpenGLMaterial>::create(shader, name);
	}

}
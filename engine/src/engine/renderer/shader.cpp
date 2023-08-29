
#include "engine/renderer/shader.h"
#include "engine/renderer/Renderer.h"
#include "engine/renderer/openGLShader.h"
#include <cassert>

namespace Engine {

	std::shared_ptr<Shader> Shader::create(const std::string& filepath)
	{
		 return std::make_shared<OpenGLShader>(filepath);
	}

	std::shared_ptr<Shader> Shader::create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
	}

	void ShaderLibrary::add(const std::string& name, const std::shared_ptr<Shader>& shader)
	{
		assert(!exists(name) && "Shader already exists!");
		m_shaders[name] = shader;
	}

	void ShaderLibrary::add(const std::shared_ptr<Shader>& shader)
	{
		auto& name = shader->getName();
		add(name, shader);
	}

	std::shared_ptr<Shader> ShaderLibrary::load(const std::string& filepath)
	{
		auto shader = Shader::create(filepath);
		add(shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::create(filepath);
		add(name, shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::get(const std::string& name)
	{
		assert(exists(name) && "Shader not found!");
		return m_shaders[name];
	}

	bool ShaderLibrary::exists(const std::string& name) const
	{
		return m_shaders.find(name) != m_shaders.end();
	}

}
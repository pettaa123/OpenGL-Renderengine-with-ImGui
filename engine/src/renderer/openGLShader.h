#pragma once

#include "renderer/shader.h"
#include <glm/glm.hpp>


// TODO: REMOVE! 
typedef unsigned int GLenum;

namespace Engine {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void bind() const;
		virtual void unbind() const;

		virtual void setInt(const std::string& name, int value);
		virtual void setIntArray(const std::string& name, int* values, uint32_t count);
		virtual void setFloat(const std::string& name, float value);
		virtual void setFloat2(const std::string& name, const glm::vec2& value);
		virtual void setFloat3(const std::string& name, const glm::vec3& value);
		virtual void setFloat4(const std::string& name, const glm::vec4& value);
		virtual void setMat4(const std::string& name, const glm::mat4& value);

		virtual const std::string& getName() const { return m_name; }

		void uploadUniformInt(const std::string& name, int value);
		void uploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void uploadUniformFloat(const std::string& name, float value);
		void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void uploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void uploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		std::string readFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> preProcess(const std::string& source);
		//void compileOrGetOpenGLBinaries();

		//void reflect(GLenum stage, const std::vector<uint32_t>& shaderData);

		void compile(const std::unordered_map<GLenum, std::string>& shaderSources);

	private:
		uint32_t m_rendererID;
		std::string m_filePath;
		std::string m_name;

		std::unordered_map<GLenum, std::string> m_openGLSourceCode;
	};

}

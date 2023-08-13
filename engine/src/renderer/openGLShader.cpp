
#include "openGLShader.h"
#include "core/timer.h"
#include "core/log.h"

#include <fstream>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <array>

//#include <shaderc/shaderc.hpp>
//#include <spirv_cross/spirv_cross.hpp>
//#include <spirv_cross/spirv_glsl.hpp>

namespace Engine {

	namespace Utils {

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			assert(false && "Unknown shader type!");
			return 0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
				case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
				case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			assert(false);
			return nullptr;
		}

		static const char* getCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "assets/cache/shader/opengl";
		}

		static void createCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = getCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		//static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		//{
		//	switch (stage)
		//	{
		//		case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
		//		case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
		//	}
		//	assert(false);
		//	return "";
		//}


	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_filePath(filepath)
	{

		Utils::createCacheDirectoryIfNeeded();

		std::string source = readFile(filepath);
		auto shaderSources = preProcess(source);

		compile(shaderSources);

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		compile(sources);

	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_rendererID);
	}


	void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources) {
		
		GLuint program = glCreateProgram();
		assert(shaderSources.size() <= 2 && "Change me if you want to use more than 2 shaders");
		std::array<GLenum,2> shaderIDs;
		int shaderIDIndex = 0;

		for (auto& kv : shaderSources) {
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);


			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				//We don't need the shader anymore.
				glDeleteShader(shader);

				Log::error(std::format("{0}", infoLog.data()));
				assert(false && "Shader compilation failure!");
				break;
			}
			glAttachShader(program, shader);
			shaderIDs[shaderIDIndex++] = shader;
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			//Log::error("Shader linking failed ({0}):\n{1}", m_filePath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);

			Log::error(std::format("{0}",infoLog.data()));
			assert(false && "Shader link failure!");
			return;
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_rendererID = program;
	}


	std::string OpenGLShader::readFile(const std::string& filepath)
	{

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
			}
			else
			{
				Log::error(std::format("Could not read from file '{0}'", filepath));
			}
		}
		else
		{
			Log::error(std::format("Could not open file '{0}'", filepath));
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string& source)
	{

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			assert(eol != std::string::npos && "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			assert(Utils::ShaderTypeFromString(type) && "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			assert(nextLinePos != std::string::npos && "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	//void OpenGLShader::reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	//{
	//	spirv_cross::Compiler compiler(shaderData);
	//	spirv_cross::ShaderResources resources = compiler.get_shader_resources();
	//
	//	HZ_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
	//	HZ_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
	//	HZ_CORE_TRACE("    {0} resources", resources.sampled_images.size());
	//
	//	HZ_CORE_TRACE("Uniform buffers:");
	//	for (const auto& resource : resources.uniform_buffers)
	//	{
	//		const auto& bufferType = compiler.get_type(resource.base_type_id);
	//		uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
	//		uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
	//		int memberCount = bufferType.member_types.size();
	//
	//		HZ_CORE_TRACE("  {0}", resource.name);
	//		HZ_CORE_TRACE("    Size = {0}", bufferSize);
	//		HZ_CORE_TRACE("    Binding = {0}", binding);
	//		HZ_CORE_TRACE("    Members = {0}", memberCount);
	//	}
	//}

	void OpenGLShader::bind() const
	{
		glUseProgram(m_rendererID);
	}

	void OpenGLShader::unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::setInt(const std::string& name, int value)
	{
		uploadUniformInt(name, value);
	}

	void OpenGLShader::setIntArray(const std::string& name, int* values, uint32_t count)
	{
		uploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::setFloat(const std::string& name, float value)
	{
		uploadUniformFloat(name, value);
	}

	void OpenGLShader::setFloat2(const std::string& name, const glm::vec2& value)
	{
		uploadUniformFloat2(name, value);
	}

	void OpenGLShader::setFloat3(const std::string& name, const glm::vec3& value)
	{
		uploadUniformFloat3(name, value);
	}

	void OpenGLShader::setFloat4(const std::string& name, const glm::vec4& value)
	{
		uploadUniformFloat4(name, value);
	}

	void OpenGLShader::setMat4(const std::string& name, const glm::mat4& value)
	{
		uploadUniformMat4(name, value);
	}

	void OpenGLShader::uploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::uploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::uploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::uploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::uploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::uploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}

#include "rypch.h"
#include "OpenGLShader.h"
#define GL_ARB_separate_shader_objects
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Rynex {

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;
		if (type == "Geomtry") return GL_GEOMETRY_SHADER;
		if (type == "Geomtry") return GL_TESS_CONTROL_SHADER;

		RY_CORE_ASSERT(false, "Unkowne Shader Type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& source, const std::string& name)
	{
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLShader::OpenGLShader(const std::string& filePath)");
#endif
		//std::string source = ReadFile(filePath);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
		m_Name = name;
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)");
#endif
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLShader::Bind() const");
#endif
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::UnBind() const
	{
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLShader::UnBind() const");
#endif
		glUseProgram(0);
	}

	void OpenGLShader::AddShader(const std::string& shader, Shader::Type shaderType)
	{
		RY_CORE_ASSERT(false, "Not Rady!");
	}


	void OpenGLShader::SetInt(const std::string& name, int value)
	{
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLShader::SetInt(const std::string& name, int value)");
#endif
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* value, uint32_t count)
	{
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLShader::SetIntArray(const std::string& name, int* value, uint32_t count)");
#endif
		UploadUniformIntArray(name, value, count);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)");
#endif
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)");
#endif
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
#if CONSOLE_LOG_FUNKTION_OPENGL
		RY_CORE_INFO("void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)");
#endif
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::SetAlgorithm(Shader::Algorithm)
	{
		RY_CORE_ASSERT(false, "Not Rady!");
		
	}

	Shader::Algorithm OpenGLShader::GetAlgorithm()
	{
		RY_CORE_ASSERT(false, "Not Rady!");
		return Algorithm();
	}

	void OpenGLShader::SetLayouteData(BufferLayout& bufferLayout, Type shaderType, void* value, uint32_t layoute)
	{
		RY_CORE_ASSERT(false,"Not Rady!");
	}

	const BufferLayout& OpenGLShader::GetLayout(Type shaderType, uint32_t layoute)
	{
		RY_CORE_ASSERT(false, "Not Rady!");
		return BufferLayout();
	}

	


	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSource;

		const char* typeToken = "#type";
		size_t typeTokenLeangth = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			RY_CORE_ASSERT(eol != std::string::npos, "Sytex error");
			size_t begin = pos + typeTokenLeangth + 1;
			std::string type = source.substr(begin, eol - begin);
			RY_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invadlid shader type specification");

			size_t nextLinePos = source.find_first_of("\r\n",eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSource[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSource;
	}

	void OpenGLShader::Compile(std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLint program = glCreateProgram();
		RY_CORE_ASSERT(shaderSources.size() <= 2, "only 2 Shaders for now!");
		std::array<GLenum, 2> glShaderIDs;
		
		int glShaderIndex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLint shader = glCreateShader(type);

			const GLchar* sourceCstr = source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);
				RY_CORE_ERROR("{0}", infoLog.data());
				RY_CORE_ASSERT(false, "Shader Compilation failure!");
				break;
			}
			glAttachShader(program, shader);
			glShaderIDs[glShaderIndex++]=shader;
		}

		m_RendererID = program;
		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);



			glDeleteProgram(program);
			for (auto& id : glShaderIDs)
				glDeleteShader(id);

			RY_CORE_ERROR("{0}", infoLog.data());
			RY_CORE_ASSERT(false, "Linked Program Compilation failure!");
			return;
		}

		for (auto& id : glShaderIDs)
			glDetachShader(program, id);

	}


	void OpenGLShader::UploadUniformFloat(const std::string& name, float values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, values);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, values.x, values.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, values.x, values.y, values.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}


	void OpenGLShader::UploadUniformInt(const std::string& name, const int values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, values);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}
}
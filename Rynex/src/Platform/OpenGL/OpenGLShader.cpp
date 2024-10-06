#include "rypch.h"
#include "OpenGLShader.h"

#define GL_ARB_separate_shader_objects
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Rynex {

	namespace Utils {

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "Vertex")						return GL_VERTEX_SHADER;
			if (type == "Fragment" || type == "Pixel")	return GL_FRAGMENT_SHADER;
			if (type == "Geomtry")						return GL_GEOMETRY_SHADER;
			if (type == "TessControl")					return GL_TESS_CONTROL_SHADER;
			if (type == "TessEvalution")				return GL_TESS_EVALUATION_SHADER;
			if (type == "Compute")						return GL_COMPUTE_SHADER;

			RY_CORE_ASSERT(false, "Unkowne Shader Type!");
			return 0;
		}

		static Shader::Type ShaderTypeEnumFromString(const std::string& type)
		{
			if (type == "Vertex")						return Shader::Type::Vertex;
			if (type == "Fragment" || type == "Pixel")	return Shader::Type::Fragment;
			if (type == "Geomtry")						return Shader::Type::Geometry;
			if (type == "TessControl")					return Shader::Type::TeselationControl;
			if (type == "TessEvalution")				return Shader::Type::TeselationEvelution;
			if (type == "Compute")						return Shader::Type::Compute;

			RY_CORE_ASSERT(false, "Unkowne Shader Type!");
			return Shader::Type::None;
		}

		static std::string StringFromShaderType(GLenum type)
		{
			switch (type)
			{
				case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
				case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
				case GL_GEOMETRY_SHADER: return "GL_GEOMETRY_SHADER";
				case GL_TESS_CONTROL_SHADER: return "GL_TESS_CONTROL_SHADER";
				case GL_TESS_EVALUATION_SHADER: return "GL_TESS_EVALUATION_SHADER";
				case GL_COMPUTE_SHADER: return "GL_COMPUTE_SHADER";
				default:
					break;
			}

			RY_CORE_ASSERT(false, "Unkowne Shader Type!");
			return 0;
		}

		static bool CheckeShader(GLint shader)
		{
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
				return false;
			}
			return true;
		}

		static GLint CreateShader(const GLchar* shaderSource, GLenum type)
		{
			GLint shader = glCreateShader(type);
			glShaderSource(shader, 1, &shaderSource, 0);
			glCompileShader(shader);
			RY_CORE_INFO("CheckeShader -> {0}", StringFromShaderType(type).c_str());
			if (CheckeShader(shader))
				return shader;
			return -1;
		}

		static bool CheckProgrammLinking(GLint program, std::array<GLenum, 4>& glShaderIDs)
		{
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
				return false;
			}
			return true;
		}

		static void GetUniformList(const std::string& shaderCode,std::map<std::string, std::string>& uniformMap)
		{
			std::istringstream stream(shaderCode);
			std::string word;

			while (stream >> word) {
				if (word == "uniform") {
					std::string type, name;
					if (stream >> type >> name) {
						if (!name.empty() && name.back() == ';') {
							name.pop_back();
						}
						uniformMap[name] = type;
					}
				}
			}
		}

		static void GetUniformList(const std::string& shaderCode, std::map<std::string, UniformElement>& uniformMap)
		{
			std::istringstream stream(shaderCode);
			std::string word;

			while (stream >> word) {
				if (word == "uniform") {
					std::string type, name;
					if (stream >> type >> name) {
						if (!name.empty() && name.back() == ';') {
							name.pop_back();
						}
						UniformElement& uniformElement = uniformMap[name];
						uniformElement.Name = name;
						uniformElement.Type = BufferAPI::GetShaderDataTypeFromString(type);

					}
				}
			}
		}

		

		
	}

	OpenGLShader::OpenGLShader(const std::string& source, const std::string& name)
	{
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
		m_Name = name;
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::ReganrateShader(const std::string& source)
	{
		glDeleteProgram(m_RendererID);
		auto shaderSources = PreProcess(source);
		Compile(shaderSources);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::AddShader(const std::string& shader, Shader::Type shaderType)
	{
		RY_CORE_ASSERT(false, "Not Rady!");
	}

	void OpenGLShader::SetPatcheVertecies(uint32_t count)
	{
		RY_CORE_ASSERT(count != 0 , "Patch Verticies need more then 0 Verticies!");
		RY_CORE_ASSERT(m_ShaderType & (int)Shader::Type::TeselationControl && m_ShaderType & (int)Shader::Type::TeselationEvelution, "Shader need to be a TeselationControl and TeselationEvelution Shader!");
		glPatchParameteri(GL_PATCH_VERTICES , count);
	}

	void OpenGLShader::SetUniformValue(const std::string& name, void* value, ShaderDataType type)
	{

		switch (type)
		{
			case ShaderDataType::Float:
			{
				UploadUniformFloat(name, value);
				break;
			}
			case ShaderDataType::Float2:
			{
				UploadUniformFloat2(name, value);
				break;
			}
			case ShaderDataType::Float3:
			{
				UploadUniformFloat3(name, value);
				break;
			}
			case ShaderDataType::Float4:
			{
				UploadUniformFloat4(name,value);
				break;
			}
			case ShaderDataType::Float3x3:
			{
				UploadUniformMat3(name, value);
				break;
			}
			case ShaderDataType::Float4x4:
			{
				UploadUniformMat4(name, value);
				break;
			}
			case ShaderDataType::Int:
			{
				UploadUniformInt(name, value);
				break;
			}
			default:
				break;
		}

	}

	
	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		UploadUniformInt(name, value);
	}


	void OpenGLShader::SetIntArray(const std::string& name, int* value, uint32_t count)
	{
		UploadUniformIntArray(name, value, count);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
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

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, void* values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, static_cast<float*>(values));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, void* values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, static_cast<float*>(values));
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSource;
		std::map<std::string, std::string> sUniformBuffer;
		std::map<std::string, UniformElement> uniformBuffer;

		const char* typeToken = "#type";
		size_t typeTokenLeangth = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			RY_CORE_ASSERT(eol != std::string::npos, "Sytex error");
			size_t begin = pos + typeTokenLeangth + 1;
			std::string type = source.substr(begin, eol - begin);
			RY_CORE_ASSERT(type == "Vertex" || type == "Fragment" || type == "Pixel" || type == "TessControl" || type == "TessEvalution" || type == "Geomtry" || type == "Compute", "Invadlid shader type specification");
			
			size_t nextLinePos = source.find_first_of("\r\n",eol);
			pos = source.find(typeToken, nextLinePos);

			Utils::GetUniformList(source, uniformBuffer);
			Utils::GetUniformList(source, sUniformBuffer);
			shaderSource[Utils::ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
			m_ShaderType |= (int)Utils::ShaderTypeEnumFromString(type);
		}
		m_sUniformLayoute = sUniformBuffer;
		return shaderSource;
	}

	void OpenGLShader::Compile(std::unordered_map<GLenum, std::string>& shadersSources)
	{

		GLint program = glCreateProgram();
		RY_CORE_ASSERT(shadersSources.size() <= 4, "only 4 Shaders for now!");
		std::array<GLenum, 4> glShaderIDs;
		
		int glShaderIndex = 0;
		for (auto& shaderSrc : shadersSources)
		{
			GLenum type = shaderSrc.first;
			const std::string& source = shaderSrc.second;
			const GLchar* sourceCstr = source.c_str();
			
			GLint shader = Utils::CreateShader(sourceCstr, type);
			glAttachShader(program, shader);
			glShaderIDs[glShaderIndex++]=shader;
		}

		m_RendererID = program;
		glLinkProgram(program);

		
		Utils::CheckProgrammLinking(program, glShaderIDs);

		for (auto& id : glShaderIDs)
			glDetachShader(program, id);

	}


	void OpenGLShader::UploadUniformFloat(const std::string& name, float values)
	{

		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, void* values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1fv(location, 1, static_cast<float*>(values));
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, values.x, values.y);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, void* values)
	{

		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2fv(location, 1, static_cast<float*>(values));
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, values.x, values.y, values.z);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, void* values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3fv(location, 1, static_cast<float*>(values));
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, void* values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4fv(location, 1, static_cast<float*>(values));
	}


	void OpenGLShader::UploadUniformInt(const std::string& name, const int values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, values);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, void* values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, 1, static_cast<int*>(values));
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, void* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, static_cast<int*>(values));
	}
}
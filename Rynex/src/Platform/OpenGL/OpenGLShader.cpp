#include "rypch.h"
#include "OpenGLShader.h"

#define GL_ARB_separate_shader_objects
#include <fstream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Rynex/Core/Application.h"

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

		static GLenum ShaderTypeFrom(Shader::Type type)
		{
			switch (type)
			{
			case Shader::Type::Vertex:					return GL_VERTEX_SHADER;
			case Shader::Type::Fragment:				return GL_FRAGMENT_SHADER;
			case Shader::Type::TeselationControl:		return GL_TESS_CONTROL_SHADER;
			case Shader::Type::TeselationEvelution:		return GL_TESS_EVALUATION_SHADER;
			case Shader::Type::Compute:					return GL_COMPUTE_SHADER;
			case Shader::Type::Geometry:				return GL_GEOMETRY_SHADER;
			case Shader::Type::MeshShader:		
				RY_CORE_ASSERT(false, "Not Impl Shader Type MeshShader");		
				return 0;
			case Shader::Type::None:					
				RY_CORE_ASSERT(false, "Unkowne Shader Type! None"); 
				return 0;
			default:
				break;
			}
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
				case GL_VERTEX_SHADER:			return "GL_VERTEX_SHADER";
				case GL_FRAGMENT_SHADER:		return "GL_FRAGMENT_SHADER";
				case GL_GEOMETRY_SHADER:		return "GL_GEOMETRY_SHADER";
				case GL_TESS_CONTROL_SHADER:	return "GL_TESS_CONTROL_SHADER";
				case GL_TESS_EVALUATION_SHADER: return "GL_TESS_EVALUATION_SHADER";
				case GL_COMPUTE_SHADER:			return "GL_COMPUTE_SHADER";
				default:
					break;
			}

			RY_CORE_ASSERT(false, "Unkowne Shader Type!");
			return 0;
		}


		static uint32_t GetErrorMassgaeLine(const std::vector<GLchar>& infoLog, uint32_t* places, uint32_t *startIndex)
		{
			uint8_t state = 0;
			uint32_t number = 0;
			uint32_t size = infoLog.size();
			for (uint32_t i = 0; i < size; i++)
			{
				auto& charekter = infoLog[i];
				switch (state)
				{
				case 0:
				{
					if (charekter == '(')
					{
						state = 1;
						*startIndex = i + 1;
					}
					break;
				}
				case 1:
				{
					if (charekter == ')')
					{
						state = 2;
						return number;
					}
					number = number * 10 + (charekter - '0');
					*places++;
					break;

				}
				
				}
			}
		}

		static void ChangeLocaleLineInGlobleLine(std::vector<GLchar>& infoLog, uint32_t shaderLineOffset)
		{
			if (shaderLineOffset == 0)
				return;
			uint32_t places = 0, startIndex = 0;
			uint32_t number = GetErrorMassgaeLine(infoLog, &places, &startIndex);
			number += shaderLineOffset;
			RY_CORE_FATAL("Mayby Line {} is mean!", number);
			return;
#if 0
			uint32_t count = 1;
			while (1.0f < (number * std::pow(0.1, count)))
				count++;
			
			if (places < count)
			{
				uint32_t space = count - places;
				uint32_t end = startIndex + space;
				for (uint32_t i = startIndex; i < end; i++)
				{
					auto it = infoLog.begin() + i;
					infoLog.insert(it, ' ');
				}
				
			}

			{
				uint32_t end = startIndex + count;
				for (uint32_t i = startIndex; i < end; i++)
				{
					uint32_t num = (number * std::pow(0.1, count - i));
					RY_CORE_ASSERT(num < 10, "We need values between 0-9!");

					if (num < 10)
						infoLog[i] = num + '0';
					else
					{
						RY_CORE_FATAL("Mayby Line {} is mean!", number);
						return;
					}
				}
			}
#endif // TODO: Config the opengl Error Masge

		}

		static bool CheckeShader(GLint shader, uint32_t shaderLineOffset)
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
				ChangeLocaleLineInGlobleLine(infoLog, shaderLineOffset);

				RY_CORE_ERROR("{0}", infoLog.data());
				RY_CORE_ASSERT(false, "Shader Compilation failure!");
				return false;
			}
			return true;
		}

		static GLint CreateShader(const GLchar* shaderSource, GLenum type, uint32_t shaderLineOffset)
		{
			GLint shader = glCreateShader(type);
			glShaderSource(shader, 1, &shaderSource, 0);
			glCompileShader(shader);
			RY_CORE_INFO("CheckeShader -> {0}", StringFromShaderType(type).c_str());
			if (CheckeShader(shader, shaderLineOffset))
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
#if 0
		static void GetUniformList(const std::string& shaderCode, std::map<std::string, UniformElement>& uniformMap)
		{
			std::istringstream stream(shaderCode);
			std::string word;
			std::string type, name;
			while (stream >> word) 
			{
				if (word == "uniform") 
				{
					
					if (stream >> type >> name) {
						if (!name.empty() && name.back() == ';') 
						{
							name.pop_back();
						}
						UniformElement& uniformElement = uniformMap[name];
						uniformElement.Name = name;
						uniformElement.Type = BufferAPI::GetShaderDataTypeFromString(type);
					}
				}
			}
			stream.clear();
		}
#endif

		static uint32_t GetLineCount(const std::string& shader)
		{
			uint32_t count = 0;
			std::string sub = "\n";
			size_t pos = shader.find(sub);

			while (pos != std::string::npos)
			{
				++count;
				pos = shader.find(sub, pos + sub.length());
			}
			return count;
		}

		
	}


	OpenGLShader::OpenGLShader(std::string&& source)
		: m_Source(std::move(source))
	{
		Application::Get().SubmiteToMainThreedQueue([this]() {
			InitAsync();
		});
	}

	OpenGLShader::OpenGLShader(const std::string& source, const std::string& name)
		: m_Name(name), m_Source(source)
	{
		std::unordered_map<uint32_t, std::string> shaderSources = PreProcess(m_Source);
		Compile(shaderSources);
		
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

	void OpenGLShader::InitAsync()
	{
		auto shaderSources = PreProcess(m_Source);
		Compile(shaderSources);
		m_Source.clear();
		m_Source.shrink_to_fit();
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

	void OpenGLShader::SetUniformValueArray(const std::string& name, void* value, ShaderDataType type, uint32_t count)
	{
		switch (type)
		{
		case ShaderDataType::Float:
		{
			UploadUniformFloatArray(name, value, count);
			break;
		}
		case ShaderDataType::Float2:
		{
			UploadUniformFloat2Array(name, value, count);
			break;
		}
		case ShaderDataType::Float3:
		{
			UploadUniformFloat3Array(name, value, count);
			break;
		}
		case ShaderDataType::Float4:
		{
			UploadUniformFloat4Array(name, value, count);
			break;
		}
		case ShaderDataType::Float3x3:
		{
			UploadUniformMat3Array(name, value, count);
			break;
		}
		case ShaderDataType::Float4x4:
		{
			UploadUniformMat4Array(name, value, count);
			break;
		}
		case ShaderDataType::Int:
		{
			//UploadUniformIntArray(name, value, count);
			break;
		}
		default:
			break;
		}
	}


	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSource;
		
#if 0
		std::map<std::string, std::string> sUniformBuffer;
		std::map<std::string, Buu> uniformBuffer;
#endif
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
#if 0
			Utils::GetUniformList(source, uniformBuffer);
			Utils::GetUniformList(source, sUniformBuffer);
#endif
			std::string& shader = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
			m_ShaderMap[Utils::ShaderTypeEnumFromString(type)] = shader;
			shaderSource[Utils::ShaderTypeFromString(type)] = shader;
			m_ShaderType |= (int)Utils::ShaderTypeEnumFromString(type);
		}
#if 0
		m_sUniformLayoute = sUniformBuffer;
#endif
		return shaderSource;
	}

	void OpenGLShader::Compile(std::unordered_map<GLenum, std::string>& shadersSources)
	{

		GLint program = glCreateProgram();
		RY_CORE_ASSERT(shadersSources.size() <= 4, "only 4 Shaders for now!");
		std::array<GLenum, 4> glShaderIDs;
		
		int glShaderIndex = 0;
		uint32_t shaderLineOffset = 0;
		for (auto& shaderSrc : shadersSources)
		{
			GLenum type = shaderSrc.first;
			const std::string& source = shaderSrc.second;
			const GLchar* sourceCstr = source.c_str();
			
			GLint shader = Utils::CreateShader(sourceCstr, type, shaderLineOffset);
			shaderLineOffset += Utils::GetLineCount(sourceCstr);
			glAttachShader(program, shader);
			glShaderIDs[glShaderIndex++]=shader;
			
		}

		m_RendererID = program;
		glLinkProgram(program);

		
		Utils::CheckProgrammLinking(program, glShaderIDs);

		for (auto& id : glShaderIDs)
			glDetachShader(program, id);

	}

#pragma region Uints

	void OpenGLShader::SetUint(const std::string& name, uint32_t value)
	{
		UploadUniformUint(name, value);
	}

	void OpenGLShader::SetUintArray(const std::string& name, uint32_t* value, uint32_t count)
	{
		UploadUniformUintArray(name, value, count);
	}


	void OpenGLShader::SetUint2(const std::string& name, const glm::uvec2& value)
	{
		UploadUniformUint2(name, value);
	}

	void OpenGLShader::SetUint2Array(const std::string& name, uint32_t* value, uint32_t count)
	{
		UploadUniformUint2Array(name, value, count);
	}


	void OpenGLShader::SetUint3(const std::string& name, const glm::uvec3& value)
	{
		UploadUniformUint3(name, value);
	}

	void OpenGLShader::SetUint3Array(const std::string& name, uint32_t* value, uint32_t count)
	{
		UploadUniformUint3Array(name, value, count);
	}


	void OpenGLShader::SetUint4(const std::string& name, const glm::uvec4& value)
	{
		UploadUniformUint4(name, value);
	}

	void OpenGLShader::SetUint4Array(const std::string& name, uint32_t* value, uint32_t count)
	{
		UploadUniformUint4Array(name, value, count);
	}
	
#pragma endregion

#pragma region Ints

	void OpenGLShader::SetInt(const std::string& name, int32_t value)
	{
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int32_t* value, uint32_t count)
	{
		UploadUniformIntArray(name, value, count);
	}


	void OpenGLShader::SetInt2(const std::string& name, const glm::ivec2& value)
	{
		UploadUniformInt2(name, value);
	}

	void OpenGLShader::SetInt2Array(const std::string& name, int32_t* value, uint32_t count)
	{
		UploadUniformInt2Array(name, value, count);
	}


	void OpenGLShader::SetInt3(const std::string& name, const glm::ivec3& value)
	{
		UploadUniformInt2(name, value);
	}

	void OpenGLShader::SetInt3Array(const std::string& name, int32_t* value, uint32_t count)
	{
		UploadUniformInt3Array(name, value, count);
	}


	void OpenGLShader::SetInt4(const std::string& name, const glm::ivec4& value)
	{
		UploadUniformInt4(name, value);
	}

	void OpenGLShader::SetInt4Array(const std::string& name, int32_t* value, uint32_t count)
	{
		UploadUniformInt4Array(name, value, count);
	}

#pragma endregion

#pragma region Floats

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloatArray(const std::string& name, float* value, uint32_t count)
	{
		UploadUniformFloatArray(name, value, count);
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		UploadUniformFloat2(name, value);
	}

	void OpenGLShader::SetFloat2Array(const std::string& name, float* value, uint32_t count)
	{
		UploadUniformFloat2Array(name, value, count);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat3Array(const std::string& name, float* value, uint32_t count)
	{
		UploadUniformFloat3Array(name, value, count);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetFloat4Array(const std::string& name, float* value, uint32_t count)
	{
		UploadUniformFloat4Array(name, value, count);
	}

	void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& value)
	{
		UploadUniformMat3(name, value);
	}

	void OpenGLShader::SetMat3Array(const std::string& name, float* value, uint32_t count)
	{
		UploadUniformMat3Array(name, value, count);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::SetMat4Array(const std::string& name, float* value, uint32_t count)
	{
		UploadUniformMat4Array(name, value, count);
	}

#pragma endregion



	

#pragma region Uint_Unifoms

	void OpenGLShader::UploadUniformUint(const std::string& name, uint32_t values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1uiv(location, 1, &values);
	}

	void OpenGLShader::UploadUniformUintArray(const std::string& name, uint32_t* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1uiv(location, count, values);
	}

	void OpenGLShader::UploadUniformUint2(const std::string& name, const glm::uvec2& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2uiv(location, 1, glm::value_ptr(values));
	}

	void OpenGLShader::UploadUniformUint2Array(const std::string& name, uint32_t* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2uiv(location, count, values);
	}

	void OpenGLShader::UploadUniformUint3(const std::string& name, const glm::uvec3& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3uiv(location, 1, glm::value_ptr(values));
	}

	void OpenGLShader::UploadUniformUint3Array(const std::string& name, uint32_t* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3uiv(location, count, values);
	}

	void OpenGLShader::UploadUniformUint4(const std::string& name, const glm::uvec4& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4uiv(location, 1, glm::value_ptr(values));
	}

	void OpenGLShader::UploadUniformUint4Array(const std::string& name, uint32_t* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4uiv(location, count, value);
	}

#pragma endregion

#pragma region Int_Unifoms

	void OpenGLShader::UploadUniformInt(const std::string& name, int32_t values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, values);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, void* values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, 1, static_cast<int*>(values));
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int32_t* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

#if 0
	void OpenGLShader::UploadUniformIntArray(const std::string& name, void* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, static_cast<int32_t*>(values));
	}
#endif

	void OpenGLShader::UploadUniformInt2(const std::string& name, const glm::ivec2& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2iv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::UploadUniformInt2Array(const std::string& name, int32_t* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2iv(location, count, value);
	}


	void OpenGLShader::UploadUniformInt3(const std::string& name, const glm::ivec3& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3iv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::UploadUniformInt3Array(const std::string& name, int32_t* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3iv(location, count, value);
	}


	void OpenGLShader::UploadUniformInt4(const std::string& name, const glm::ivec4& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4iv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::UploadUniformInt4Array(const std::string& name, int32_t* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4iv(location, count, value);
		
	}

#pragma endregion

#pragma region Float_Unifoms

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

	void OpenGLShader::UploadUniformFloatArray(const std::string& name, float* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1fv(location, count, value);
	}

	void OpenGLShader::UploadUniformFloatArray(const std::string& name, void* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1fv(location, count, static_cast<float*>(value));
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

	void OpenGLShader::UploadUniformFloat2Array(const std::string& name, float* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2fv(location, count, value);
	}

	void OpenGLShader::UploadUniformFloat2Array(const std::string& name, void* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2fv(location, count, static_cast<float*>(values));
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

	void OpenGLShader::UploadUniformFloat3Array(const std::string& name, float* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3fv(location, count, value);
	}

	void OpenGLShader::UploadUniformFloat3Array(const std::string& name, void* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3fv(location, count, static_cast<float*>(values));
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

	void OpenGLShader::UploadUniformFloat4Array(const std::string& name, float* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4fv(location, count, value);
	}

	void OpenGLShader::UploadUniformFloat4Array(const std::string& name, void* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4fv(location, count, static_cast<float*>(values));
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

	void OpenGLShader::UploadUniformMat3Array(const std::string& name, float* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, count, GL_FALSE, value);
	}

	void OpenGLShader::UploadUniformMat3Array(const std::string& name, void* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, count, GL_FALSE, static_cast<float*>(values));
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

	void OpenGLShader::UploadUniformMat4Array(const std::string& name, float* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, count, GL_FALSE, value);
	}

	void OpenGLShader::UploadUniformMat4Array(const std::string& name, void* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, count, GL_FALSE, static_cast<float*>(values));
	}

#pragma endregion

}
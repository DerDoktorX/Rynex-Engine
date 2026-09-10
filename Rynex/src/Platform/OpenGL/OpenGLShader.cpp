#include "rypch.h"
#include "OpenGLShader.h"


#include <Platform/OpenGL/OpenGLBase.h>
#include <glad/glad.h>

#include <algorithm>
#include <vector>
#include <numeric>
#define GL_ARB_separate_shader_objects


#define RY_ENABLE_BINDLES_GLSL_INLINE_TEXTURE RY_ENABLE_BINDLES_TEXTURE && 0
namespace Rynex {

	namespace SyntaxGlsl {
		constexpr const char* COMMENT_LINE = "//";
		constexpr const char* COMMENT_FIST = "/*";
		constexpr const char* COMMENT_END = "*/";

		constexpr const char* MACRO_DEFINE = "#define ";
		constexpr const char* MACRO_IF = "#if ";
		constexpr const char* MACRO_IFDEF = "#ifdef ";
		constexpr const char* MACRO_ENDIF = "#endif";
	}

	namespace Utils {


		static std::vector<size_t> GetFoundPos(const std::string& shaderCode, const std::string& searchStr)
		{
			uint32_t count = 0;
			std::vector<size_t> posFoundVec;
			size_t offset = searchStr.length();
			size_t pos = 0ull;
			pos = shaderCode.find(searchStr, pos);
			
			while (pos != std::string::npos)
			{				
				posFoundVec.emplace_back(pos);

				size_t posOffset = offset + pos;
				size_t nextPos = shaderCode.find(searchStr, posOffset);
				pos = nextPos;
			}
			return posFoundVec;
		}

		static size_t GetFoundCount(const std::string& shaderCode, const std::string& searchStr)
		{
			size_t offset = searchStr.length();
			size_t pos = 0ull;
			pos = shaderCode.find(searchStr, pos);
			size_t count = 0ull;
			while (pos != std::string::npos)
			{
				count++;
				size_t posOffset = offset + pos;
				size_t nextPos = shaderCode.find(searchStr, posOffset);
				pos = nextPos;
			}
			return count;
		}

		static bool CheckRegularExpressInCommentPart(const std::string& shaderCode, size_t lineBegin, size_t posBeginRegularExpresion)
		{
			size_t commentFirst = shaderCode.find(SyntaxGlsl::COMMENT_FIST);
			size_t pos = commentFirst;
			bool isKommentClosed = commentFirst == std::string::npos;

			while (pos != std::string::npos && pos < posBeginRegularExpresion)
			{
				isKommentClosed = false;
				pos = shaderCode.find(SyntaxGlsl::COMMENT_END, pos);

				if (pos == std::string::npos)
					break;

				isKommentClosed = true;
				pos = shaderCode.find(SyntaxGlsl::COMMENT_FIST, pos);
			}
			
			return isKommentClosed;

		}

		static bool CheckLineSyntaxBeforPos(const std::string& shaderCode, size_t lineBegin, size_t posBeginRegularExpresion)
		{
			if (lineBegin > posBeginRegularExpresion)
				return false;

			if(!CheckRegularExpressInCommentPart(shaderCode, lineBegin, posBeginRegularExpresion))
				return false;
			


			size_t pos = shaderCode.find(SyntaxGlsl::COMMENT_LINE, lineBegin);
			return pos >= posBeginRegularExpresion;
		}

		static constexpr bool IsCarkterNumber(char charcter)
		{
			constexpr char zeroChar = '0';
			constexpr char nineChar = '9';

			return zeroChar <= charcter && charcter > nineChar;

		}

		static constexpr bool IsNotCarkterNumber(char charcter)
		{
			constexpr char zeroChar = '0';
			constexpr char nineChar = '9';

			return zeroChar > charcter && charcter < nineChar;

		}

		static int GetNumberInt(const std::string& name, size_t first, size_t end = std::string::npos)
		{
			constexpr char zeroChar = '0';
			constexpr char nineChar = '9';
			constexpr char spaceChar = ' ';

			int number = 0;
			size_t pos = first;
			bool hasFoundNumber = false;
			if (std::string::npos==end)
				end = name.size();

			while (pos < end)
			{
				
				char charekter = name.at(pos);
				if (IsNotCarkterNumber(charekter) && hasFoundNumber)
				{
					return number;
				}
				else
				{
					hasFoundNumber = true;
					number = number * 10 + (charekter - zeroChar);
				}

				pos++;
			}
			return number;
		}

		static void GetVertexOutPut(const std::string& shaderCode, std::vector<BufferElement>& bufferElementVec)
		{
			RY_CORE_NOT_IMPL();
			RY_REMBER_FUNC_CHANGE("I complet forget that on word layout is also the word out!");
			constexpr const char* searchFor = "gl_Position";
			constexpr const char* name = "Depth-Buffer";
			std::vector<size_t> posFoundVec = GetFoundPos(shaderCode, searchFor);
			if (posFoundVec.empty())
				return;

			std::string sub = "\n";
			size_t offset = sub.length();
			size_t pos = shaderCode.find(sub);
			size_t posName = pos;
			size_t index = 0u;
			size_t curentFoundPos = posFoundVec.at(index);
			size_t lineCount = 0ull;
			
			while (pos != std::string::npos)
			{
				
				size_t lineBegin = pos + offset;
				size_t lineEnd = shaderCode.find(sub, lineBegin);
				
				if (curentFoundPos < lineEnd)
				{
					if (CheckLineSyntaxBeforPos(shaderCode, lineBegin, curentFoundPos))
					{
						bufferElementVec.emplace_back(ShaderDataType::Float, name);
						return;
					}

					index++;
					if (posFoundVec.size() <= index)
						break;

					RY_CORE_ASSERT(curentFoundPos < posFoundVec.at(index), "Not larger pos!");
					curentFoundPos = posFoundVec.at(index);
				}
				pos = lineEnd;
				lineCount++;
			}

		}

		static ShaderDataType GetNextDataType(const std::string& shaderCode, size_t first, size_t end = std::string::npos)
		{
			constexpr const char* dataTypeList[] = {
				"float","vec2","vec3","vec4", "mat3", "mat4",
				"int", "ivec2", "ivec3", "ivec4", "imat3", "imat4",
				"uint","uvec2", "uvec3", "uvec4", "umat3", "umat4"
			};

			constexpr ShaderDataType shaderDataTypeList[]{
				SDT::Float, SDT::Float2, SDT::Float3, SDT::Float4, SDT::Float3x3, SDT::Float4x4,
				SDT::Int, SDT::Int2, SDT::Int3, SDT::Int4, SDT::Int3x3, SDT::Int4x4,
				SDT::Uint, SDT::Uint2, SDT::Uint3, SDT::Uint4, SDT::Uint3x3, SDT::Uint4x4,
			};

			if (std::string::npos == end)
				end = shaderCode.size();

			uint32_t index = 0u;
			for (const char* dataType : dataTypeList)
			{
				size_t pos = shaderCode.find(dataType, first);
				if (pos < end)
				{
					return shaderDataTypeList[index];
				}
				index++;
			}
			return SDT::None;
		}

		static void GetFragmentOutPut(const std::string& shaderCode, std::vector<BufferElement>& bufferElementVec)
		{
			RY_CORE_NOT_IMPL();
			RY_REMBER_FUNC_CHANGE("I complet forget that on word layout is also the word out!");

			constexpr const char* searchFor = "out";

			constexpr const char* nameBuffer = "Color-Buffer";
			constexpr const char* locatioIndex[] = {
				"location",
				"="
			};
			std::vector<size_t> posFoundVec = GetFoundPos(shaderCode, searchFor);
			if (posFoundVec.empty())
				return;

			std::string sub = "\n";
			size_t offset = sub.length();
			size_t pos = shaderCode.find(sub);
			size_t posName = pos;
			size_t index = 0u;
			size_t curentFoundPos = posFoundVec.at(index);
			size_t lineCount = 0ull;

			while (pos != std::string::npos)
			{

				size_t lineBegin = pos + offset;
				size_t lineEnd = shaderCode.find(sub, lineBegin);

				if (curentFoundPos < lineEnd)
				{

					if (CheckLineSyntaxBeforPos(shaderCode, lineBegin, curentFoundPos))
					{
						ShaderDataType type = GetNextDataType(shaderCode, curentFoundPos, lineEnd);
						size_t indexPos = lineBegin;
						for (const char* name : locatioIndex)
						{
							indexPos = shaderCode.find(name, indexPos);
						}
						if(indexPos != lineBegin)
						{
							int locationNumber = GetNumberInt(shaderCode, indexPos, curentFoundPos);
							size_t sizeBuffer = bufferElementVec.size();
							locationNumber += 1; // offset becouse off Depth-Buffer
							if (locationNumber <= sizeBuffer)
							{
								sizeBuffer = locationNumber + 1ull;
								bufferElementVec.resize(sizeBuffer);
							}
							BufferElement& e = bufferElementVec.at(locationNumber);
							e = BufferElement(type, nameBuffer);
						}
						else
						{
							ShaderDataType type = GetNextDataType(shaderCode, curentFoundPos, lineEnd);
							bufferElementVec.emplace_back(type, nameBuffer);
						}
					}

					index++;
					if (posFoundVec.size() <= index)
						break;

					RY_CORE_ASSERT(curentFoundPos < posFoundVec.at(index), "Not larger pos!");
					curentFoundPos = posFoundVec.at(index);
				}

				lineCount++;
				pos = lineEnd;

			}

		}

		static void GetOutPut(const std::string& shaderCode, ShaderType::ShaderType type, std::vector<BufferElement>& bufferElementVec)
		{
			switch (type)
			{
			case ShaderType::Vertex:
				GetVertexOutPut(shaderCode, bufferElementVec);
				break;
			case ShaderType::Fragment:
				GetFragmentOutPut(shaderCode, bufferElementVec);
				break;
			default:
				return;
			}
		}

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == OpenGLShader::g_VertexShaderToken)				return GL_VERTEX_SHADER;
			if (type == OpenGLShader::g_FragementShaderToken[0] 
				|| type == OpenGLShader::g_FragementShaderToken[1])		return GL_FRAGMENT_SHADER;
			if (type == OpenGLShader::g_GemotryShaderToken)				return GL_GEOMETRY_SHADER;
			if (type == OpenGLShader::g_TeseltionControllShaderToken)	return GL_TESS_CONTROL_SHADER;
			if (type == OpenGLShader::g_TeseltionEvalutionShaderToken)	return GL_TESS_EVALUATION_SHADER;
			if (type == OpenGLShader::g_ComputeShaderToken)				return GL_COMPUTE_SHADER;

			RY_CORE_ASSERT(false, "Unkowne Shader Type!");
			return 0;
		}

		static GLenum ShaderTypeFrom(ShaderType::ShaderType type)
		{
			switch (type)
			{
			case ShaderType::Vertex:					return GL_VERTEX_SHADER;
			case ShaderType::Fragment:				return GL_FRAGMENT_SHADER;
			case ShaderType::TeselationControl:		return GL_TESS_CONTROL_SHADER;
			case ShaderType::TeselationEvelution:		return GL_TESS_EVALUATION_SHADER;
			case ShaderType::Compute:					return GL_COMPUTE_SHADER;
			case ShaderType::Geometry:				return GL_GEOMETRY_SHADER;
			case ShaderType::MeshShader:		
				RY_CORE_ASSERT(false, "Not Impl Shader Type MeshShader");		
				return 0;
			case ShaderType::None:					
				RY_CORE_ASSERT(false, "Unkowne Shader Type! None"); 
				return 0;
			default:
				break;
			}
			RY_CORE_ASSERT(false, "Unkowne Shader Type!");
			return 0;
		}

		static ShaderType::ShaderType ShaderTypeEnumFromString(const std::string& type)
		{
			if (type == OpenGLShader::g_VertexShaderToken)				return ShaderType::Vertex;
			if (type == OpenGLShader::g_FragementShaderToken[0] 
				|| type == OpenGLShader::g_FragementShaderToken[1])		return ShaderType::Fragment;
			if (type == OpenGLShader::g_GemotryShaderToken)				return ShaderType::Geometry;
			if (type == OpenGLShader::g_TeseltionControllShaderToken)	return ShaderType::TeselationControl;
			if (type == OpenGLShader::g_TeseltionEvalutionShaderToken)	return ShaderType::TeselationEvelution;
			if (type == OpenGLShader::g_ComputeShaderToken)				return ShaderType::Compute;

			RY_CORE_ASSERT(false, "Unkowne Shader Type!");
			return ShaderType::None;
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
			RY_CORE_WARN("Error msg can only interpet Nvidea Error msg!");
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
						(*places)++;
						break;

					}
				}
			}
			return 0xFFFFFFFF;
		}

		static uint32_t ChangeLocaleLineInGlobleLine(std::vector<GLchar>& infoLog, uint32_t shaderLineOffset)
		{
			if (shaderLineOffset == 0)
				return 0u;
			uint32_t places = 0, startIndex = 0;
			uint32_t number = GetErrorMassgaeLine(infoLog, &places, &startIndex);
			if(number != 0xFFFFFFFF)
			{
				uint32_t expextedLine = number + shaderLineOffset;

				RY_CORE_FATAL("Mayby Line {} is mean!", expextedLine);
			}
			else
			{
				RY_CORE_FATAL("No Nummber found on info message but its a multy shader!");

				number = 0u;
			}
			return number;

		}

		static void PrintAroundLine(const std::string& shader, uint32_t line, uint32_t aroundRange = 3)
		{
			uint32_t count = 0;
			std::string sub = "\n";
			size_t lastLineBegin = 0ull;
			size_t lastPos = 0ull;
			size_t pos = shader.find(sub);
			uint32_t subCount = sub.length();
			size_t lineBegin = pos + subCount;

			uint32_t rangeBegin = line - aroundRange;
			uint32_t rangeEnde = line + aroundRange;
			
			while (pos != std::string::npos)
			{
				if (rangeBegin <= count && count <= rangeEnde)
				{
					uint32_t lineCharBeginCount = lineBegin - lastLineBegin - subCount;
					uint32_t lineCharCount = pos - lastPos;

					std::string lineStr = shader.substr(lastPos, lineCharCount);
					std::string lineBeginStr = shader.substr(lastLineBegin, lineCharBeginCount);

					RY_CORE_TRACE("{}: {}", count, lineBeginStr);
				}

				++count;
				lastLineBegin = lineBegin;
				lastPos = pos;
				pos = shader.find(sub, lineBegin);
				lineBegin = pos + subCount;
			}
#if RY_ENABLE_BINDLES_GLSL_INLINE_TEXTURE
			count - 2;
#endif
		}

		static bool CheckeShader(GLint shader, uint32_t shaderLineOffset, const std::string& shaderCode)
		{
			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				RY_OPENGL_SHADER_ID_SCOPE_LOCK();
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				RY_GRAFIC_DELETE(shader, OpenGLShader);
				glDeleteShader(shader);
				uint32_t erroLine = ChangeLocaleLineInGlobleLine(infoLog, shaderLineOffset);
				PrintAroundLine(shaderCode, erroLine);
				RY_CORE_ERROR("{0}", infoLog.data());
				
				RY_CORE_ASSERT(false, "Shader Compilation failure!");
				return false;
			}
			return true;
		}

		static GLint CreateShader(const std::string& shaderCode, GLenum type, uint32_t shaderLineOffset)
		{
			const GLchar* shaderSource = shaderCode.c_str();
			GLint shader;
			{
				RY_OPENGL_SHADER_ID_SCOPE_LOCK();

				shader = glCreateShader(type);
				RY_GRAFIC_CREATE(shader, OpenGLShader);

			}

			glShaderSource(shader, 1, &shaderSource, 0);
			glCompileShader(shader);
			RY_CORE_INFO("CheckeShader -> {0}", StringFromShaderType(type).c_str());
			if (CheckeShader(shader, shaderLineOffset, shaderCode))
				return shader;
			return -1;
		}

		static bool IsProgrammLinking(uint32_t& program, std::array<GLenum, 4>& glShaderIDs)
		{
			GLint isLinked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				if(!infoLog.empty())
				{
					glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
					RY_GRAFIC_DELETE(program, OpenGLShaderProgram);
					
					for (auto& id : glShaderIDs)
					{
						if (id != 0)
							break;

						RY_GRAFIC_DELETE(id, OpenGLShader);
						glDetachShader(program, id);
						glDeleteShader(id);
					}
					
					glDeleteProgram(program);
					program = 0u;

					RY_CORE_ERROR("{0}", infoLog.data());
					RY_CORE_ASSERT(false, "Linked Program Compilation failure!");
					return false;
				}
				else
				{
					
					
					for (auto& id : glShaderIDs)
					{

						if (id != 0)
							break;


						RY_GRAFIC_DELETE(id, OpenGLShader);
						glDetachShader(program, id);
						glDeleteShader(id);
					}
					RY_GRAFIC_DELETE(program, OpenGLShaderProgram);
					glDeleteProgram(program);

					RY_CORE_ASSERT(false, "Empty Error Msg String");
					return false;
				}
			}
			return true;
		}

		static void GetUniformList(const std::string& shaderCode,std::map<std::string, std::string>& uniformMap)
		{
			std::istringstream stream(shaderCode);
			std::string word;

			while (stream >> word) 
			{
				if (word == "uniform") 
				{
					std::string type, name;
					if (stream >> type >> name) 
					{
						if (!name.empty() && name.back() == ';') 
						{
							name.pop_back();
						}
						uniformMap[name] = type;
					}
				}
			}
		}

		static void SetubDefine(std::string& shaderCode)
		{
			uint32_t count = 0;
			std::string sub = "#version ";
			size_t pos = shaderCode.find(sub);
			pos += 9;

			pos += 3; 
			
			pos += 7; // "core\r\n"

			
			std::string bindlesDefine = "#extension GL_ARB_bindless_texture : require\r\n#define RY_BINDLES_DETH_TEX\r\n";
			shaderCode.insert(pos, bindlesDefine);
			shaderCode += bindlesDefine;
			
		}


		static ShaderDataType GetTypeShaderDataType(GLenum type) {
			switch (type) {
		
			case GL_FLOAT: return SDT::Float;
			case GL_FLOAT_VEC2: return SDT::Float2;
			case GL_FLOAT_VEC3: return SDT::Float3;
			case GL_FLOAT_VEC4: return SDT::Float4;

			case GL_INT: return  SDT::Int;
			case GL_INT_VEC2: return SDT::Int2;
			case GL_INT_VEC3: return SDT::Int3;
			case GL_INT_VEC4: return SDT::Int4;
			
			case GL_UNSIGNED_INT: return SDT::Uint;
			case GL_UNSIGNED_INT_VEC2: return SDT::Uint2;
			case GL_UNSIGNED_INT_VEC3: return SDT::Uint3;
			case GL_UNSIGNED_INT_VEC4: return SDT::Uint4;

			
			case GL_FLOAT_MAT3: return SDT::Float3x3;
			case GL_FLOAT_MAT4: return SDT::Float4x4;

			default: 
				RY_CORE_ASSERT(false, "not vaild Shader data Type")
				return SDT::None;
			}
		}

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
#if RY_ENABLE_BINDLES_GLSL_INLINE_TEXTURE
			count - 2;
#endif
			return count;
		}


		static bool IsLineInCommantBlock(const std::string& shaderCode, size_t beginLine, size_t endLine)
		{
			RY_CORE_ASSERT(beginLine < endLine);
			size_t beginPos = 0;
			size_t endPos = 0;
			bool isVaildPos;
			bool blockEndIsBeforLine;
			bool blockBeginIsBeforLine;
			do
			{
				beginPos = shaderCode.find(SyntaxGlsl::COMMENT_FIST);
				endPos = shaderCode.find(SyntaxGlsl::COMMENT_END, beginPos);
				isVaildPos = beginPos < endPos;
				blockEndIsBeforLine = endPos < beginLine;
				blockBeginIsBeforLine = beginPos < beginLine;
			} 
			while (isVaildPos && blockEndIsBeforLine && blockBeginIsBeforLine);
				
			bool comantStartsInLine = beginLine < beginPos && beginPos > endLine;
			bool comantEndsInLine = beginLine < endPos && endPos > endLine;
			bool comantIsAroundLine = blockBeginIsBeforLine && endLine < endPos;
			bool comantBlockHasStartedAndEndInline = comantStartsInLine && comantEndsInLine;
			bool comantBlockcomesAfterLine = endLine < beginPos && endLine < endPos;
			bool comantStartsInLineAndEndsAfterLine = comantStartsInLine && endLine < endPos;
			bool comantEndsInLineStartBeforLine = comantEndsInLine && beginLine < beginLine;


			bool isLineInKommantBlock = comantIsAroundLine;
			if (isVaildPos)
				RY_CORE_ERROR("Invaild Commant area found!");

			return false;
		}

		static size_t IsLineInlineComented(const std::string& shaderCode, size_t beginLine, size_t endLine)
		{
			RY_CORE_ASSERT(beginLine < endLine);

			size_t pos = 0;

			bool isBeforMyLine;
			bool isAfterNotAfterLine;

			do
			{
				pos = shaderCode.find(SyntaxGlsl::COMMENT_LINE, pos);
				isBeforMyLine;
				isAfterNotAfterLine;

			} while (isBeforMyLine && isAfterNotAfterLine);
			return pos;
		}


		static void AddDefineVecToShaderCode(const OpenGLShader::ShaderDefineVec& shaderDefineVec, std::string& shaderCode)
		{			
			RY_REMBER_FUNC_CHANGE("Fix at some point the define finding situation! Space");
			using ShaderDefine = OpenGLShader::ShaderDefine;
			using StringPtrDiffernz = std::string::difference_type;
			constexpr const char* versionStr = "#version";
			constexpr const char* lineEnd = "\n";
			constexpr const char* emptyStr = "";


			size_t pos = shaderCode.find(versionStr);
			pos = shaderCode.find(lineEnd, pos);
			pos += 1ull;
			std::string defineLine(emptyStr, 50);
			std::string defineInShaderCodeDeclarted(emptyStr, 50);

			for (const ShaderDefine& shaderDefine : shaderDefineVec)
			{
				constexpr const char* typeChar = "#define";
				constexpr const char* betweenTypeAndName = " ";
				const std::string& nameDefine = shaderDefine.first;
				constexpr const char* betweenNameAndValue = " ";
				const std::string& valueDefine = shaderDefine.second;

				defineLine.clear();
				defineLine += lineEnd;
				defineLine += typeChar;
				defineLine += betweenTypeAndName;
				defineLine += nameDefine;
				size_t definLineTypeNameCount = defineLine.size();
				defineLine += valueDefine;
				defineLine += lineEnd;

				size_t newCountLine = UINT64_MAX;
				defineInShaderCodeDeclarted.clear();
				defineInShaderCodeDeclarted += lineEnd;
				defineInShaderCodeDeclarted += typeChar;
				defineInShaderCodeDeclarted += betweenTypeAndName;
				defineInShaderCodeDeclarted += nameDefine;

				size_t shaderDefineOldPos = shaderCode.find(defineInShaderCodeDeclarted);
				size_t shaderCodeCount = shaderCode.size();
				if (shaderDefineOldPos <= shaderCodeCount)
				{

					size_t shaderDefineEndLineOldPos = shaderCode.find(lineEnd, shaderDefineOldPos + 1ull);

					const char* newlineStr = defineLine.c_str();
					newCountLine = defineLine.size();
					size_t oldLineCount = shaderDefineEndLineOldPos - shaderDefineOldPos;
					if (definLineTypeNameCount == oldLineCount)
					{
						shaderCode.replace(shaderDefineOldPos, newCountLine, newlineStr);
					}
					else 
					{
						shaderCode.erase(shaderDefineOldPos, oldLineCount);
						shaderCode.insert(shaderDefineOldPos, newlineStr, newCountLine);
					}
				}
				else
				{
					const char* newlineStr = defineLine.c_str();
					newCountLine = defineLine.size();
					shaderCode.insert(pos, newlineStr, newCountLine);
				}
				pos += newCountLine;
			}
		}
	
		static BufferLayout GetOutputLayoute(GLuint program)
		{
			GLint numOutputs;
			glGetProgramInterfaceiv(program, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &numOutputs);
			std::vector<BufferElement> bufferElementVec;
			int maxLocation = 0;
			bufferElementVec.reserve(numOutputs);
			for (int i = 0; i < numOutputs; i++) {
				char name[256];
				glGetProgramResourceName(program, GL_PROGRAM_OUTPUT, i, sizeof(name), NULL, name);

				GLenum props[] = { GL_TYPE, GL_LOCATION, GL_ARRAY_SIZE };
				GLint values[3];
				glGetProgramResourceiv(program, GL_PROGRAM_OUTPUT, i, 3, props, 3, NULL, values);

				GLenum type = values[0];
				GLint location = values[1];
				GLint arraySize = values[2];
				SDT shaderType = GetTypeShaderDataType(type);
				if (location < 0)
					continue;
				if (maxLocation <= location)
					maxLocation = location + 1;

				BufferElement& ellments = bufferElementVec.emplace_back(shaderType, name);
				ellments.offset = location;
			}
			size_t count = bufferElementVec.size();
			if (1u == count)
			{
				return BufferLayout(bufferElementVec);
			}

			std::sort(bufferElementVec.begin(), bufferElementVec.end(),
				[&](BufferElement& a, BufferElement& b)
				{
					return a.offset < b.offset;
				}
			);
			return BufferLayout(bufferElementVec);
		}

		static BufferLayout GetInputLayoute(GLuint program)
		{
			GLint numOutputs;
			glGetProgramInterfaceiv(program, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numOutputs);
			std::vector<BufferElement> bufferElementVec;
			std::vector<uint32_t> numberElementVec;

			bufferElementVec.reserve(numOutputs);

			int maxLocation = 0;

			for (int i = 0; i < numOutputs; i++) {
				char name[256];
				glGetProgramResourceName(program, GL_PROGRAM_INPUT, i, sizeof(name), NULL, name);

				GLenum props[] = { GL_TYPE, GL_LOCATION, GL_ARRAY_SIZE };
				GLint values[3];
				glGetProgramResourceiv(program, GL_PROGRAM_INPUT, i, 3, props, 3, NULL, values);

				GLenum type = values[0];
				GLint location = values[1];
				GLint arraySize = values[2];
				SDT shaderType = GetTypeShaderDataType(type);

				if (location < 0)
					continue;
				if (maxLocation <= location)
					maxLocation = location + 1;
				BufferElement& ellments = bufferElementVec.emplace_back(shaderType, name);
				ellments.count = location;
			}
			size_t count = bufferElementVec.size();
			if(1u == count)
			{
				return BufferLayout(bufferElementVec);
			}

			std::sort(bufferElementVec.begin(), bufferElementVec.end(),
				[&](BufferElement& a, BufferElement& b)
				{
					return a.count < b.count;
				}
			);
			

			return BufferLayout(bufferElementVec);
		}

	}

	uint32_t OpenGLShader::s_LastBindShaderID = 0u;

	OpenGLShader::OpenGLShader(std::string&& source)
		: m_Source()
		, m_Name("Unknown")
	{
		std::string shaderSource = std::move(source);
		Invalidate(shaderSource);
	}

	OpenGLShader::OpenGLShader(const std::string& source, const std::string& name)
		: m_Name(name), m_Source()
	{
		std::string shaderSource = source;
		Invalidate(shaderSource);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name), m_Source()
	{
		std::string shaderSource = vertexSrc + fragmentSrc;
		Invalidate(shaderSource);

	}

	OpenGLShader::~OpenGLShader()
	{
		DestroyID();
	}

	void OpenGLShader::CreateID()
	{
		DestroyID();

		RY_CORE_ASSERT(0u == m_RendererID);

		RY_OPENGL_SHADER_ID_SCOPE_LOCK();
		m_RendererID = glCreateProgram();
		
		RY_CORE_ASSERT(0u != m_RendererID);

		GL_CHECK();
	}

	void OpenGLShader::DestroyID()
	{
		RY_CORE_ASSERT(OpenGLThreadContext::IsActive());
		constexpr uint32_t programResetValue = 0u;

		if (programResetValue == m_RendererID)
			return;

		RY_OPENGL_SHADER_ID_SCOPE_LOCK();
		RY_GRAFIC_DELETE(m_RendererID, OpenGLShaderProgram);
		glDeleteProgram(m_RendererID);
		m_RendererID = programResetValue;

		GL_CHECK();
	}

	uint32_t OpenGLShader::CreateLodingRenderID()
	{
		uint32_t renderID;
		if (0u == m_RendererID)
		{
			CreateID();
			renderID = m_RendererID;
		}
		else
		{
			renderID = glCreateProgram();
		}
		RY_CORE_ASSERT(0u != m_RendererID, "if there is no renderID you are not allowd to create a loding handle!");
		return renderID;
	}

	void OpenGLShader::CreateIDFromLodingRenderID(uint32_t renderID)
	{
		RY_CORE_ASSERT(0u != renderID, "no Render ID!");
		if (renderID == m_RendererID)
		{
			return;
		}
		
		

		m_RendererID = renderID;
		RY_CORE_TRACE("Set shader render ID on Element");
	}

	void OpenGLShader::Invalidate(std::string& shaderSounrce)
	{
		if (m_Source == shaderSounrce)
		{
			RY_CORE_INFO("Shader are alrady equela!");
			return;
		}

		RY_EXE_ON_MAIN_THREAD_RESUME_ARGS(OpenGLShader::Invalidate, std::string(shaderSounrce));
		constexpr int resetShaderType = 0;
		std::map<uint32_t, std::string> shaderSource;
		std::map<ShaderType::ShaderType, std::string> shaderMap;
		int shaderTypeFlag = PreProcess(shaderSounrce, shaderSource, shaderMap);

		if (Compile(shaderSource))
		{
			m_Source = std::move(shaderSounrce);
			m_ShaderMap = std::move(shaderMap);	
			m_ShaderSources = std::move(shaderSource);
			m_UnifromLocation.clear();
			m_sUniformLayoute.clear();
			m_ShaderType = shaderTypeFlag;
		}

	}



	void OpenGLShader::ReganrateShader(std::string& source)
	{
		Invalidate(source);
	}

	void OpenGLShader::ReganrateShader(std::string&& source)
	{		
		std::string shaderSounrce = std::move(source);
		Invalidate(shaderSounrce);
	}

	void OpenGLShader::Bind() const
	{
		RY_CORE_ASSERT(0u != m_RendererID);
		OpenGLRenderCommand::BindShader(m_RendererID);
	}

	void OpenGLShader::UnBind() const
	{
		RY_REMBER_FUNC_CHANGE("Maybe Delting the funtion and use a Globel Reset Binding Function that can be calld by RenderCommand");
		RY_CORE_ASSERT(0u != m_RendererID);

		OpenGLRenderCommand::BindShader(0u);
	}

	void OpenGLShader::AddShader(const std::string& shader, ShaderType::ShaderType shaderType)
	{
		RY_REMBER_FUNC_CHANGE("Maybe Delting the funtion becaus it has no Purpes curently!");
		RY_CORE_ASSERT(false, "Not Rady!");
	}

	void OpenGLShader::SetPatcheVertecies(uint32_t count)
	{
		RY_CORE_ASSERT(count != 0 , "Patch Verticies need more then 0 Verticies!");
		constexpr int tessCombValue = static_cast<int>(ShaderType::TeselationControl) | static_cast<int>(ShaderType::TeselationEvelution);
		int valueTessControl = m_ShaderType & tessCombValue;
		bool result = valueTessControl;

		RY_CORE_ASSERT(result, "Shader need to be a TeselationControl and TeselationEvelution Shader!");
		glPatchParameteri(GL_PATCH_VERTICES , count);
		GL_CHECK_LOOP();
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
			UploadUniformIntArray(name, value, count);
			break;
		}
		default:
			break;
		}
	}


	ShaderTypeType OpenGLShader::PreProcess(const std::string& source, std::map<uint32_t, std::string>& shaderSource, std::map<ShaderType::ShaderType, std::string>& shaderMap) const
	{
		ShaderTypeType shaderTypeFlag = 0;
		const char* typeToken = "#type";
		size_t typeTokenLeangth = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			RY_CORE_ASSERT(eol != std::string::npos, "Sytex error");
			size_t begin = pos + typeTokenLeangth + 1;
			std::string type = source.substr(begin, eol - begin);
			{

				RY_CORE_ASSERT(type == g_ShaderTokenList[0]
					|| type == g_ShaderTokenList[1]
					|| type == g_ShaderTokenList[2]
					|| type == g_ShaderTokenList[3]
					|| type == g_ShaderTokenList[4]
					|| type == g_ShaderTokenList[5]
					|| type == g_ShaderTokenList[6], "Invadlid shader type specification");
			}

			size_t nextLinePos = source.find_first_of("\r\n",eol);
			pos = source.find(typeToken, nextLinePos);
			size_t shaderCharCount = pos - (std::string::npos == nextLinePos? source.size() - 1 : nextLinePos);
			std::string shader = source.substr(nextLinePos, shaderCharCount);
			Utils::AddDefineVecToShaderCode(m_ShaderDefineVec, shader);
			ShaderType::ShaderType shaderType = Utils::ShaderTypeEnumFromString(type);
			shaderMap[shaderType] = shader;
			uint32_t typeShaderGL = Utils::ShaderTypeFromString(type);
			shaderSource[typeShaderGL] = shader;
			shaderTypeFlag |= shaderType;
		}
		
		return shaderTypeFlag;
	}

	bool OpenGLShader::Compile(std::map<GLenum, std::string>& shadersSources)
	{
		
		GLuint compileRenderID = CreateLodingRenderID();
		RY_CORE_ASSERT(shadersSources.size() <= 4, "only 4 Shaders for now!");

		std::array<GLenum, 4> glShaderIDs = {
			0,
			0,
			0,
			0,
		};
		
		int glShaderIndex = 0;
		uint32_t shaderLineOffset = 0;
		using ShaderPair = std::unordered_map<uint32_t, std::string>::value_type;
		for (ShaderPair& shaderSrc : shadersSources)
		{
			GLenum type = shaderSrc.first;
			const std::string& source = shaderSrc.second;

			GLint shader = Utils::CreateShader(source, type, shaderLineOffset);
			if (shader == -1)
			{
				

				for (uint32_t& id : glShaderIDs)
				{
					RY_OPENGL_SHADER_ID_SCOPE_LOCK();
					RY_GRAFIC_DELETE(id, OpenGLShader);
					glDetachShader(m_RendererID, id);
					glDeleteShader(id);
				}
				glDeleteProgram(compileRenderID);
				compileRenderID = 0u;

				return false;
			}
			shaderLineOffset += Utils::GetLineCount(source);
			glAttachShader(compileRenderID, shader);
			glShaderIDs[glShaderIndex]=shader;
			glShaderIndex++;
			
			
		}


		glLinkProgram(compileRenderID);

		
		if (Utils::IsProgrammLinking(compileRenderID, glShaderIDs))
		{
			for (uint32_t i = 0; i < glShaderIndex; i++)
			{
				uint32_t& id = glShaderIDs[i];
				RY_OPENGL_SHADER_ID_SCOPE_LOCK();

				glDetachShader(compileRenderID, id);
				glDeleteShader(id);
			}
			m_OutPutLayout = Utils::GetOutputLayoute(compileRenderID);
			m_InPutLayout = Utils::GetInputLayoute(compileRenderID);


			CreateIDFromLodingRenderID(compileRenderID);
			GL_CHECK();
			return true;
		}
		
	
		
		return false;
	}

	void OpenGLShader::SetDefine(const std::string& name)
	{
		for (ShaderDefine& shaderDefine : m_ShaderDefineVec)
		{
			std::string& defineName = shaderDefine.first;
			if (name == defineName && !shaderDefine.second.empty())
			{
				shaderDefine.second.clear();
				std::string shaderSource = std::move(m_Source);
				m_Source = "";
				Invalidate(shaderSource);
				return;
			}
		}
		m_ShaderDefineVec.emplace_back(std::make_pair(name, ""));
		std::string shaderSource = std::move(m_Source);
		m_Source = "";
		Invalidate(shaderSource);
	}

	void OpenGLShader::SetDefine(const std::string& name, const std::string& value)
	{
		for (ShaderDefine& shaderDefine : m_ShaderDefineVec)
		{
			std::string& defineName = shaderDefine.first;
			if (name == defineName && value != shaderDefine.second)
			{
				shaderDefine.second = value;

				std::string shaderSource = std::move(m_Source);
				m_Source = "";
				Invalidate(shaderSource);
				return;
			}

		}
		m_ShaderDefineVec.emplace_back(std::make_pair(name, value));
		std::string shaderSource = std::move(m_Source);
		m_Source = "";
		Invalidate(shaderSource);
	}

	void OpenGLShader::RemoveDefine(const std::string& name)
	{
		uint32_t index = 0u;
		for (ShaderDefine& shaderDefine : m_ShaderDefineVec)
		{
			std::string& defineName = shaderDefine.first;
			if (name == defineName)
				break;
			index++;
		}
		uint32_t count = m_ShaderDefineVec.size();
		if(index < count)
		{				
			using ShaderDefineVecContIterator = ShaderDefineVec::const_iterator;
			using ShaderDefineVecPtrDiffernz = ShaderDefineVec::difference_type;

			ShaderDefineVecContIterator it = m_ShaderDefineVec.begin();
			it = it + static_cast<ShaderDefineVecPtrDiffernz>(index);
			m_ShaderDefineVec.erase(it);
			std::string shaderSource = std::move(m_Source);
			m_Source = "";
			Invalidate(shaderSource);
			return;
		}

		RY_CORE_ERROR("We Dident Find Any Define with the Name {}", name);
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
		glUniform1uiv(GetLocation(name), 1, &values);
	}

	void OpenGLShader::UploadUniformUintArray(const std::string& name, uint32_t* values, uint32_t count)
	{
		glUniform1uiv(GetLocation(name), count, values);
	}

	void OpenGLShader::UploadUniformUint2(const std::string& name, const glm::uvec2& values)
	{
		glUniform2uiv(GetLocation(name), 1, glm::value_ptr(values));
	}

	void OpenGLShader::UploadUniformUint2Array(const std::string& name, uint32_t* values, uint32_t count)
	{
		glUniform2uiv(GetLocation(name), count, values);
	}

	void OpenGLShader::UploadUniformUint3(const std::string& name, const glm::uvec3& values)
	{
		glUniform3uiv(GetLocation(name), 1, glm::value_ptr(values));
	}

	void OpenGLShader::UploadUniformUint3Array(const std::string& name, uint32_t* values, uint32_t count)
	{
		glUniform3uiv(GetLocation(name), count, values);
	}

	void OpenGLShader::UploadUniformUint4(const std::string& name, const glm::uvec4& values)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4uiv(location, 1, glm::value_ptr(values));
	}

	void OpenGLShader::UploadUniformUint4Array(const std::string& name, uint32_t* value, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4uiv(GetLocation(name), count, value);
	}

#pragma endregion

#pragma region Int_Unifoms

	void OpenGLShader::UploadUniformInt(const std::string& name, int32_t values)
	{
		glUniform1i(GetLocation(name), values);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, void* values)
	{
		glUniform1iv(GetLocation(name), 1, static_cast<int*>(values));
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int32_t* values, uint32_t count)
	{
		glUniform1iv(GetLocation(name), count, values);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, void* values, uint32_t count)
	{
		glUniform1iv(GetLocation(name), count, static_cast<int32_t*>(values));
	}


	void OpenGLShader::UploadUniformInt2(const std::string& name, const glm::ivec2& value)
	{
		glUniform2iv(GetLocation(name), 1, glm::value_ptr(value));
	}

	void OpenGLShader::UploadUniformInt2Array(const std::string& name, int32_t* value, uint32_t count)
	{
		glUniform2iv(GetLocation(name), count, value);
	}


	void OpenGLShader::UploadUniformInt3(const std::string& name, const glm::ivec3& value)
	{
		glUniform3iv(GetLocation(name), 1, glm::value_ptr(value));
	}

	void OpenGLShader::UploadUniformInt3Array(const std::string& name, int32_t* value, uint32_t count)
	{
		glUniform3iv(GetLocation(name), count, value);
	}


	void OpenGLShader::UploadUniformInt4(const std::string& name, const glm::ivec4& value)
	{
		glUniform4iv(GetLocation(name), 1, glm::value_ptr(value));
	}

	void OpenGLShader::UploadUniformInt4Array(const std::string& name, int32_t* value, uint32_t count)
	{
		glUniform4iv(GetLocation(name), count, value);
		
	}

#pragma endregion

#pragma region Float_Unifoms

	void OpenGLShader::UploadUniformFloat(const std::string& name, float values)
	{
		glUniform1f(GetLocation(name), values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, void* values)
	{
		glUniform1fv(GetLocation(name), 1, static_cast<float*>(values));
	}

	void OpenGLShader::UploadUniformFloatArray(const std::string& name, float* value, uint32_t count)
	{
		glUniform1fv(GetLocation(name), count, value);
	}

	void OpenGLShader::UploadUniformFloatArray(const std::string& name, void* value, uint32_t count)
	{
		glUniform1fv(GetLocation(name), count, static_cast<float*>(value));
	}


	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		glUniform2f(GetLocation(name), values.x, values.y);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, void* values)
	{
		glUniform2fv(GetLocation(name), 1, static_cast<float*>(values));
	}

	void OpenGLShader::UploadUniformFloat2Array(const std::string& name, float* value, uint32_t count)
	{
		glUniform2fv(GetLocation(name), count, value);
	}

	void OpenGLShader::UploadUniformFloat2Array(const std::string& name, void* values, uint32_t count)
	{
		glUniform2fv(GetLocation(name), count, static_cast<float*>(values));
	}


	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		glUniform3f(GetLocation(name), values.x, values.y, values.z);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, void* values)
	{
		glUniform3fv(GetLocation(name), 1, static_cast<float*>(values));
	}

	void OpenGLShader::UploadUniformFloat3Array(const std::string& name, float* value, uint32_t count)
	{
		glUniform3fv(GetLocation(name), count, value);
	}

	void OpenGLShader::UploadUniformFloat3Array(const std::string& name, void* values, uint32_t count)
	{
		glUniform3fv(GetLocation(name), count, static_cast<float*>(values));
	}


	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		glUniform4f(GetLocation(name), values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, void* values)
	{
		glUniform4fv(GetLocation(name), 1, static_cast<float*>(values));
	}

	void OpenGLShader::UploadUniformFloat4Array(const std::string& name, float* value, uint32_t count)
	{
		glUniform4fv(GetLocation(name), count, value);
	}

	void OpenGLShader::UploadUniformFloat4Array(const std::string& name, void* values, uint32_t count)
	{
		glUniform4fv(GetLocation(name), count, static_cast<float*>(values));
	}


	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, void* values)
	{
		glUniformMatrix3fv(GetLocation(name), 1, GL_FALSE, static_cast<float*>(values));
	}

	void OpenGLShader::UploadUniformMat3Array(const std::string& name, float* value, uint32_t count)
	{
		glUniformMatrix3fv(GetLocation(name), count, GL_FALSE, value);
	}

	void OpenGLShader::UploadUniformMat3Array(const std::string& name, void* values, uint32_t count)
	{
		glUniformMatrix3fv(GetLocation(name), count, GL_FALSE, static_cast<float*>(values));
	}


	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, void* values)
	{
		glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, static_cast<float*>(values));
	}

	void OpenGLShader::UploadUniformMat4Array(const std::string& name, float* value, uint32_t count)
	{
		glUniformMatrix4fv(GetLocation(name), count, GL_FALSE, value);
	}

	void OpenGLShader::UploadUniformMat4Array(const std::string& name, void* values, uint32_t count)
	{
		
		glUniformMatrix4fv(GetLocation(name), count, GL_FALSE, static_cast<float*>(values));
	}

	inline int32_t OpenGLShader::GetLocation(const std::string& name)
	{
		
		if(m_UnifromLocation.find(name) != m_UnifromLocation.end())
			return m_UnifromLocation.at(name);

		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		m_UnifromLocation[name] = location;
		return location;

		
	}

#pragma endregion

}
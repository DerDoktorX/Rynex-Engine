#pragma once

#include "Rynex/Renderer/API/Shader.h"
#include "Rynex/Asset/Base/Asset.h"

#include <glm/glm.hpp>

typedef unsigned int GLenum;


namespace Rynex {



	class OpenGLShader : public Shader
	{
	public:
		//OpenGLShader(const std::string& filePath);
#if 1
		OpenGLShader() = default;
#endif // TODO: after test dealating!
		OpenGLShader(const std::string& source, const std::string& name);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void ReganrateShader(const std::string& source) override;

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void AddShader(const std::string& shader, Type shaderType) override;

		virtual void SetPatcheVertecies(uint32_t count) override;

		virtual void SetUniformValue(const std::string& name, void* value, ShaderDataType type);

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* value, uint32_t count) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual void SetAlgorithm(Algorithm ) override;
		virtual Algorithm GetAlgorithm() override;

		virtual std::map<std::string, std::string>& GetUniformLayoute() override { return m_sUniformLayoute; };

		virtual const std::string& GetName() const override { return m_Name;  };

		virtual bool operator==(const Shader& other)const override
		{
			return m_RendererID == ((OpenGLShader&)other).m_RendererID;
		};

		void UploadUniformInt(const std::string& name, int values);
		void UploadUniformInt(const std::string& name, void* values);

		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void UploadUniformIntArray(const std::string& name, void* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, float values);
		void UploadUniformFloat(const std::string& name, void* values);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void UploadUniformFloat2(const std::string& name, void* values);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat3(const std::string& name, void* values);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values);
		void UploadUniformFloat4(const std::string& name, void* values);
		
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat3(const std::string& name, void* values);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		void UploadUniformMat4(const std::string& name, void* values);

		virtual const RendererAPI::API GetRendererAPI() const override { return RendererAPI::API::OpenGL; };

	private:
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
		std::map<std::string, std::string> m_sUniformLayoute;
		BufferLayout m_BufferLayout;
		int m_ShaderType = 0;

	};
}



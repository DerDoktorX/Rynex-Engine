#pragma once

#include "Rynex/Renderer/Shader.h"

#include <glm/glm.hpp>
#include <Rynex/Asset/Base/Asset.h>

typedef unsigned int GLenum;


namespace Rynex {
	class OpenGLShader : public Shader
	{
	public:
		//OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& source, const std::string& name);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void AddShader(const std::string& shader, Type shaderType) override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* value, uint32_t count) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual void SetAlgorithm(Algorithm ) override;
		virtual Algorithm GetAlgorithm() override;

		virtual void SetLayouteData(BufferLayout& bufferLayout, Type shaderType, void* value, uint32_t layoute = 0) override;
		virtual const BufferLayout& GetLayout(Type shaderType, uint32_t layoute = 0) override;

		virtual const std::string& GetName() const override { return m_Name;  };


		void UploadUniformInt(const std::string& name, int values);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, float values);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values);
		
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

		// Asset
		// static AssetType GetStaticType() { return AssetType::Shader; }
		// virtual AssetType GetType() const { return GetStaticType(); }
		// virtual AssetHandle GetHandle() const { return m_Handle; };
	private:
		//std::string ReadFile(const std::string& fiilePath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_RendererID;
		std::string m_Name;
		BufferLayout m_Layouts;
	};
}



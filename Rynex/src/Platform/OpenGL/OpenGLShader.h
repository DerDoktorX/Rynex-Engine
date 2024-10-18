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
		virtual void SetUniformValueArray(const std::string& name, void* value, ShaderDataType type, uint32_t count);

		virtual void SetUint(const std::string& name, uint32_t value) override;
		virtual void SetUintArray(const std::string& name, uint32_t* value, uint32_t count) override;
		virtual void SetUint2(const std::string& name, const glm::uvec2& value) override;
		virtual void SetUint2Array(const std::string& name, uint32_t* value, uint32_t count) override;
		virtual void SetUint3(const std::string& name, const glm::uvec3& value) override;
		virtual void SetUint3Array(const std::string& name, uint32_t* value, uint32_t count) override;
		virtual void SetUint4(const std::string& name, const glm::uvec4& value) override;
		virtual void SetUint4Array(const std::string& name, uint32_t* value, uint32_t count)override;

		virtual void SetInt(const std::string& name, int32_t value) override;
		virtual void SetIntArray(const std::string& name, int32_t* value, uint32_t count) override;
		virtual void SetInt2(const std::string& name, const glm::ivec2& value) override;
		virtual void SetInt2Array(const std::string& name, int32_t* value, uint32_t count) override;
		virtual void SetInt3(const std::string& name, const glm::ivec3& value) override;
		virtual void SetInt3Array(const std::string& name, int32_t* value, uint32_t count) override;
		virtual void SetInt4(const std::string& name, const glm::ivec4& value) override;
		virtual void SetInt4Array(const std::string& name, int32_t* value, uint32_t count) override;

		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloatArray(const std::string& name, float* value, uint32_t count) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat2Array(const std::string& name, float* value, uint32_t count) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat3Array(const std::string& name, float* value, uint32_t count) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetFloat4Array(const std::string& name, float* value, uint32_t count) override;

		virtual void SetMat3(const std::string& name, const glm::mat3& value) override;
		virtual void SetMat3Array(const std::string& name, float* value, uint32_t count) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
		virtual void SetMat4Array(const std::string& name, float* value, uint32_t count) override;


		virtual std::map<std::string, std::string>& GetUniformLayoute() override { return m_sUniformLayoute; };

		virtual const std::string& GetName() const override { return m_Name;  };

		virtual bool operator==(const Shader& other)const override
		{
			return m_RendererID == ((OpenGLShader&)other).m_RendererID;
		};

	private:
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(std::unordered_map<GLenum, std::string>& shaderSources);

		void UploadUniformUint(const std::string& name, uint32_t value);
		void UploadUniformUint(const std::string& name, void* value);
		void UploadUniformUintArray(const std::string& name, uint32_t* value, uint32_t count);
		void UploadUniformUintArray(const std::string& name, void* value, uint32_t count);

		void UploadUniformUint2(const std::string& name, const glm::uvec2& value);
		void UploadUniformUint2(const std::string& name, void* value);
		void UploadUniformUint2Array(const std::string& name, uint32_t*, uint32_t count);
		void UploadUniformUint2Array(const std::string& name, void* value, uint32_t count);

		void UploadUniformUint3(const std::string& name, const glm::uvec3& value);
		void UploadUniformUint3(const std::string& name, void* value);
		void UploadUniformUint3Array(const std::string& name, uint32_t* value, uint32_t count);
		void UploadUniformUint3Array(const std::string& name, void* value, uint32_t count);

		void UploadUniformUint4(const std::string& name, const glm::uvec4& value);
		void UploadUniformUint4(const std::string& name, void* value);
		void UploadUniformUint4Array(const std::string& name, uint32_t* value, uint32_t count);
		void UploadUniformUint4Array(const std::string& name, void* value, uint32_t count);


		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformInt(const std::string& name, void* value);
		void UploadUniformIntArray(const std::string& name, int32_t* value, uint32_t count);
		void UploadUniformIntArray(const std::string& name, void* value, uint32_t count);
		
		void UploadUniformInt2(const std::string& name, const glm::ivec2& value);
		void UploadUniformInt2(const std::string& name, void* value);
		void UploadUniformInt2Array(const std::string& name, int32_t* value, uint32_t count);
		void UploadUniformInt2Array(const std::string& name, void* value, uint32_t count);

		void UploadUniformInt3(const std::string& name, const glm::ivec3& value);
		void UploadUniformInt3(const std::string& name, void* value);
		void UploadUniformInt3Array(const std::string& name, int32_t* value, uint32_t count);
		void UploadUniformInt3Array(const std::string& name, void* value, uint32_t count);

		void UploadUniformInt4(const std::string& name, const glm::ivec4& value);
		void UploadUniformInt4(const std::string& name, void* value);
		void UploadUniformInt4Array(const std::string& name, int32_t* value, uint32_t count);
		void UploadUniformInt4Array(const std::string& name, void* value, uint32_t count);


		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat(const std::string& name, void* value);
		void UploadUniformFloatArray(const std::string& name, float* value, uint32_t count);
		void UploadUniformFloatArray(const std::string& name, void* value, uint32_t count);

		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat2(const std::string& name, void* value);
		void UploadUniformFloat2Array(const std::string& name, float* value, uint32_t count);
		void UploadUniformFloat2Array(const std::string& name, void* value, uint32_t count);

		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat3(const std::string& name, void* value);
		void UploadUniformFloat3Array(const std::string& name, float* value, uint32_t count);
		void UploadUniformFloat3Array(const std::string& name, void* value, uint32_t count);

		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformFloat4(const std::string& name, void* value);
		void UploadUniformFloat4Array(const std::string& name, float* value, uint32_t count);
		void UploadUniformFloat4Array(const std::string& name, void* value, uint32_t count);
		
		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat3(const std::string& name, void* value);
		void UploadUniformMat3Array(const std::string& name, float* matrix, uint32_t count);
		void UploadUniformMat3Array(const std::string& name, void* value, uint32_t count);

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
		void UploadUniformMat4(const std::string& name, void* value);
		void UploadUniformMat4Array(const std::string& name, float* matrix, uint32_t count);
		void UploadUniformMat4Array(const std::string& name, void* value, uint32_t count);

	
	private:
		uint32_t m_RendererID;
		std::string m_Name;
		std::map<std::string, std::string> m_sUniformLayoute;
		BufferLayout m_BufferLayout;
		int m_ShaderType = 0;

	};
}



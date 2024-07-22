#pragma once


#include "Rynex/Renderer/Buffer.h"
#include "Rynex/Asset/Base/Asset.h"

#include <glm/glm.hpp>
#include <string>


namespace Rynex{

	class Shader : public Asset
	{
	public:
	enum class Type
	{
		None = 0,
		Fragment = BIT(0),
		Vertex = BIT(1),
		Compute = BIT(2),
		Geometry = BIT(3),
		TeselationControl = BIT(4),
		TeselationEvelution = BIT(5)
	};

	enum class Algorithm
	{
		Nono = 0,
		Z_Buffer = BIT(0), Depth_Buffer = BIT(0),
		A_Buffer = BIT(1),
		Blend = BIT(2),
		DobbleSide = BIT(3),
		ClockWise = BIT(4),
	};

	public:
		~Shader() = default;
		static Ref<Shader> Create(const std::string& filePath, const std::string& name);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);


		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void AddShader(const std::string& shader, Shader::Type shaderType) = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* value, uint32_t count) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual void SetLayouteData(BufferLayout& bufferLayout, Type shaderType, void* value, uint32_t layoute = 0) = 0;
		virtual const BufferLayout& GetLayout(Type shaderType, uint32_t layoute = 0) = 0;
		
		virtual void SetAlgorithm(Shader::Algorithm) = 0;
		virtual Algorithm GetAlgorithm() = 0;

		virtual const std::string& GetName() const = 0;

		// Asset
		static AssetType GetStaticType() { return AssetType::Shader; }
		AssetType GetType() const { return GetStaticType(); }
		// virtual AssetHandle GetHandle() const { return m_Handle; };
		

	private:
		uint32_t m_RenderID;
	};

	class ShaderLibary
	{
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);

		Ref<Shader> Load(const std::filesystem::path& path);
		Ref<Shader> Load(const std::string& vertexSrc, const std::string& fragmentSrc);

		Ref<Shader> Get(const std::string& name);

		bool Exist(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};
}

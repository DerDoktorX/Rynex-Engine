#pragma once
#include "Rynex/Asset/Base/Asset.h"

#include "Rynex/Renderer/API/Framebuffer.h"
#include "Rynex/Renderer/API/Buffer.h"

#include <glm/glm.hpp>

namespace Rynex{

	enum class ShaderResourceType
	{
		None = 0,
		LocalModel, LocalColor, MainCamerPos, EnitiyID, 
		MainCameraViewMatrix, MainCamerProjectionMatrix, MainCameraViewProjectionMatrix,
		GlobleResource,
		AmbientLigths, PointLigths, SpotLigths, DrirektionLigths,
		ShadowTexture
	};

	enum class ShaderBufferType
	{
		None = 0,
		VertexIn, 
		Uniform,
		FragmentOut
	};
	
	using ShBuTy = ShaderBufferType;
	using ShReTy = ShaderResourceType;

	typedef uint8_t ShaderTypeType;
	namespace ShaderType
	{
		enum ShaderType : uint8_t
		{
			None = 0,
			Fragment = BIT(0),
			Vertex = BIT(1),
			Compute = BIT(2),
			Geometry = BIT(3),
			TeselationControl = BIT(4),
			TeselationEvelution = BIT(5),
			MeshShader = BIT(6)
		};
		static constexpr const size_t s_Count = 8;
	}


	class RYNEX_API Shader : public Asset
	{
	public:
		
	public:
		~Shader() = default;

		static Ref<Shader> Create(const std::string& source, const std::string& name);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		static Ref<Shader> CreateAsync(std::string&& source);
		static Ref<Shader> Default();
		static void Shutdown();

		virtual void ReganrateShader(std::string& source) = 0;
		virtual void ReganrateShader(std::string&& source) = 0;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void AddShader(const std::string& shader, ShaderType::ShaderType shaderType) = 0;

		// For Teselation and Evelation Shaders!
		virtual void SetPatcheVertecies(uint32_t count) = 0;

		
		virtual void SetUniformValue(const std::string& name, void* value, ShaderDataType type) = 0;

		virtual void SetUint(const std::string& name, uint32_t value) = 0;
		virtual void SetUintArray(const std::string& name, uint32_t* value, uint32_t count) = 0;
		virtual void SetUint2(const std::string& name, const glm::uvec2& value) = 0;
		virtual void SetUint2Array(const std::string& name, uint32_t* value, uint32_t count) = 0;
		virtual void SetUint3(const std::string& name, const glm::uvec3& value) = 0;
		virtual void SetUint3Array(const std::string& name, uint32_t* value, uint32_t count) = 0;
		virtual void SetUint4(const std::string& name, const glm::uvec4& value) = 0;
		virtual void SetUint4Array(const std::string& name, uint32_t* value, uint32_t count) = 0;

		virtual void SetInt(const std::string& name, int32_t value) = 0;
		virtual void SetIntArray(const std::string& name, int32_t*, uint32_t count) = 0;
		virtual void SetInt2(const std::string& name, const glm::ivec2& value) = 0;
		virtual void SetInt2Array(const std::string& name, int32_t* value, uint32_t count) = 0;
		virtual void SetInt3(const std::string& name, const glm::ivec3& value) = 0;
		virtual void SetInt3Array(const std::string& name, int32_t* value, uint32_t count) = 0;
		virtual void SetInt4(const std::string& name, const glm::ivec4& value) = 0;
		virtual void SetInt4Array(const std::string& name, int32_t* value, uint32_t count) = 0;

		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloatArray(const std::string& name, float* value, uint32_t count) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat2Array(const std::string& name, float* value, uint32_t count) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat3Array(const std::string& name, float* value, uint32_t count) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetFloat4Array(const std::string& name, float* value, uint32_t count) = 0;

		virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
		virtual void SetMat3Array(const std::string& name, float* value, uint32_t count) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetMat4Array(const std::string& name, float* value, uint32_t count) = 0;

		virtual void SetDefine(const std::string& name) = 0;
		virtual void SetDefine(const std::string& name, const std::string& value) = 0;
		virtual void RemoveDefine(const std::string& name) = 0;

		virtual const std::vector<std::pair<std::string, std::string>>& GetShaderDefineVec() const = 0;

		virtual std::map<std::string, std::string>& GetUniformLayoute() = 0;
		virtual const std::map<ShaderType::ShaderType, std::string>& GetShaderMap() const = 0;

		virtual const BufferLayout& GetOutPut() const = 0;
		virtual const BufferLayout& GetInPut() const = 0;

		virtual bool operator==(const Shader& other)const = 0;

		virtual const std::string& GetName() const = 0;

		// Asset
		static AssetType GetStaticType() { return AssetType::Shader; }
		
		AssetType GetType() const { return GetStaticType(); }
	};

	class RYNEX_API ShaderLibary
	{
	public:
		void Add(const std::string& name, const Ref <Shader>& shader);
		void Add(const Ref<Shader>& shader);

		Ref<Shader> Load(const std::filesystem::path& path);
		Ref<Shader> Load(const std::string& vertexSrc, const std::string& fragmentSrc);

		Ref<Shader> Get(const std::string& name);

		bool Exist(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};


	
}

#pragma once
#include <Rynex/Renderer/API/Shader.h>


namespace Rynex {
	
	struct ShaderMaterialDefaultNames;

	class Material : public Asset
	{
	public:

		virtual ~Material() {};

		virtual void SetColor(const glm::vec3& color) = 0;
		virtual const glm::vec3& GetColor() const;

		virtual void SetAlpha(float v) = 0;
		virtual float GetAlpha() const;

		virtual void SetShader(const Ref<Shader>& shader) = 0;
		virtual const Ref<Shader>& GetShader() const;

		virtual void SetFlage(int flage) = 0;
		virtual int GetFlage();

		virtual void SetShadowTextures(const std::array<Ref<Texture>, 72>& shadowsTex, uint32_t size) = 0;
		virtual void SetMatrix(const glm::mat4& matrix) = 0;
		virtual const glm::mat4& GetMatrix() const;

		virtual void BindShadow() = 0;
		virtual void Bind(int* entityIDs, uint32_t size = 1) = 0;
		virtual void UnBind() = 0;
		virtual bool IsRady();
		virtual void InitAsync() = 0;

		virtual void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer) = 0;
		virtual const Ref<UniformBuffer>& GetUniformBuffer() const = 0;

		virtual void SetUniformValue(const BufferElement& element, void* data) = 0;
		
		virtual void* GetUniformData() = 0;
		virtual Ref<Shader> GetShadowShader() = 0;
		// static AssetType GetStaticType() { return AssetType::Material; }
		// virtual AssetType GetType() const = 0;
	
		static Ref<Material> CreateBasic(const std::vector<std::filesystem::path>& paths);
		static Ref<Material> CreateDefeard();
		static Ref<Material> CreateShader(ShaderMaterialDefaultNames& names);

		static Ref<Material> CreateBasicAsysnc(std::vector<std::filesystem::path>&& paths);
		static Ref<Material> CreateDefeardAsysnc();
		static Ref<Material> CreateShaderAsysnc();

		
	};

}
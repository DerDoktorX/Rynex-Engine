#pragma once

#include "Rynex/Renderer/Materials/Material.h"
#include <Rynex/Renderer/API/Shader.h>

namespace Rynex {

	
	struct ShaderMaterialDefaultNames
	{
		BufferElement Color;
		BufferElement Alpha;
		BufferElement ModelMatrix;

		ShaderMaterialDefaultNames(BufferElement color, BufferElement alpha, BufferElement modelMatrix)
			: Color(color), Alpha(alpha), ModelMatrix(modelMatrix)
		{ }
	};

	class ShaderMaterial : public Material
	{
	public:
		ShaderMaterial(const ShaderMaterialDefaultNames& names);
		virtual ~ShaderMaterial();

		virtual void SetColor(const glm::vec3& color) override;

		virtual void SetAlpha(float v) override;

		virtual void SetShader(const Ref<Shader>& shader) override;
		virtual const Ref<Shader>& GetShader() const override;

		virtual void SetFlage(int flage) override;
		virtual int GetFlage() override;

		virtual void SetMatrix(const glm::mat4& matrix) override;
		virtual void SetShadowTextures(const std::array<Ref<Texture>, 72>& shadowsTex, uint32_t size) override;
		virtual void BindShadow(CameraData& ligthCam) override;

		virtual void Bind(int* entityIDs, uint32_t size, CameraData& camera, CameraData& ligthCam) override;
		virtual void UnBind() override;

		virtual void InitAsync() override;

		virtual void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer) override;

		virtual void SetUniformValue(const BufferElement& element, void* data) override;
		virtual const Ref<UniformBuffer>& GetUniformBuffer() const  override;

		void SetUniforme(ShaderMaterialDefaultNames names);
		const ShaderMaterialDefaultNames& GetUniforme() const;
		virtual Ref<Shader> GetShadowShader() override;
		virtual AssetType GetType() const override { return AssetType::BasicMaterial; }

		virtual void* GetUniformData() override;
	private:
		void SetDefaultName(BufferElement& elementName, const void* data);
	private:
		int m_Flags;
		Ref<Shader> m_Shader;
		Ref<Shader> m_ShadowShader;
		Ref<UniformBuffer> m_Uniform;
		ShaderMaterialDefaultNames m_Names;

	};

}


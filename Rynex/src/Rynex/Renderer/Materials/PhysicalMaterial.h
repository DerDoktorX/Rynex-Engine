#pragma once
#include "Rynex/Renderer/Materials/Material.h"

namespace Rynex {

    struct UniformDataPhyicel
    {
        float ModelMatrix[16];
        float Color[3];
        float Alpha;        
        float Emission;
        float Matalness;
        float Roughness;
        float Shinines;
        float Specular;

        int EntityID;
        int Empty[2];
        
    };

	class PhysicalMaterial : public Material
	{
    public:
        PhysicalMaterial(bool async);
        virtual ~PhysicalMaterial();

        virtual void SetColor(const glm::vec3& color) override;
        virtual const glm::vec3& GetColor() const override;

        virtual void SetAlpha(float v) override;
        virtual float GetAlpha() const override;

        virtual void SetShader(const Ref<Shader>& shader) override;
        virtual const Ref<Shader>& GetShader() const override;

        virtual void SetMatrix(const glm::mat4& matrix) override;
        virtual void SetShadowTextures(const std::array<Ref<Texture>, 72>& shadowsTex, uint32_t size) override;
        virtual void BindShadow(CameraData& ligthCam) override;

        virtual void SetFlage(int flage) override;
        virtual int GetFlage() override;

        virtual void Bind(int* entityIDs, uint32_t size, CameraData& camera, CameraData& ligthCam) override;
        virtual void UnBind() override;

        virtual void InitAsync() override;

        virtual void SetUniformBuffer(Ref<UniformBuffer> uniformBuffer) override;

        virtual void SetUniformValue(const BufferElement& element, void* data) override;
        virtual const Ref<UniformBuffer>& GetUniformBuffer() const  override;
        virtual Ref<Shader> GetShadowShader() override;
        virtual AssetType GetType() const override { return AssetType::BasicMaterial; }

        virtual void* GetUniformData() override;
    private:
        UniformDataPhyicel* m_BufferData;
        int m_Flage;

        Ref<Shader> m_Shader;
        Ref<Shader> m_ShadowShader;
        Ref<UniformBuffer> m_UniformBuffer;

        Ref<Texture> m_AlbedoMap;
        Ref<Texture> m_NormalMap;
        Ref<Texture> m_RoughnessMap;
        Ref<Texture> m_MetallicMap;
	};

}



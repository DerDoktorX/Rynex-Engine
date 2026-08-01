#pragma once
#include "Rynex/Renderer/Materials/Material.h"

#define RY_DEFAULT_MATERIEL_WEAK 1
namespace Rynex {

    struct MaterielShaderData
    {
        glm::vec3 Color = glm::vec3{ 1.0f, 0.0f, 1.0f };
        float Alpha = 1.0f;
        float Shinines = -1.0f;
        float Specular = -1.0f;
        float AmbientLigthe = -1.0f;
        int UseTexure = -1;

        static BufferLayout GetLayoute();
    };
    
    class DefaultMaterial : public Material
    {
    public:
        using PackageArrayMap = typename StorageMapPackage<Ref<Material>, MaterielShaderData>;
        using PackageArrayDynamic = typename StorageArrayPackage<MaterielShaderData>;
        using PackageArrayStatic = typename UniformArrayPackage<MaterielShaderData>;
        using Package = typename UniformDataPackage<MaterielShaderData>;
    public:
        DefaultMaterial();
        DefaultMaterial(const MaterielShaderData& data, const std::vector<Pass>& vec);

        DefaultMaterial(std::vector<std::filesystem::path>&& vecPath, const Ref<Shader>& shaderShade, const Ref<Shader>& shaderDepth);
        DefaultMaterial(const MaterielShaderData& data, Ref<Texture> tex, const Ref<Shader>& shaderShade, const Ref<Shader>& shaderDepth);
        virtual ~DefaultMaterial();

        virtual void SetColor(const glm::vec3& color) override { m_Change = true; m_BufferData.Color = color; }
        virtual glm::vec3& GetColor() override { m_Change = true; return m_BufferData.Color; }
        virtual const glm::vec3& GetColor() const override { return m_BufferData.Color; }

        virtual void SetAlpha(float v) override { m_Change = true; m_BufferData.Alpha = v; }
        virtual float& GetAlpha() override { m_Change = true; return m_BufferData.Alpha; }
        virtual float GetAlpha() const override { return m_BufferData.Alpha; }
       

        virtual bool IsRady() const override { return nullptr != m_DefaultMap; }
        virtual bool UpdateMaterielData() const { return m_Change; }

        virtual int GetShadeRenderMode() const override { return m_ShadeRenderMode; }
        virtual int GetDepthRenderMode() const override { return m_DepthRenderMode; }
        
        virtual void* GetMaterielDataPtr() override { return &m_BufferData; }
        virtual const void* GetMaterielDataPtr() const override { return &m_BufferData; }
        virtual uint32_t GetMaterielDataByteSize() const override { return sizeof(MaterielShaderData); }
        
        virtual void UpdateMaterielData(void* materielArrayData, uint32_t offset, uint32_t byteSize) const override;

        virtual int AddMaterielAlbedoTextures(Ref<BindlesTextureArray>& bindlesTexures) const override;
        virtual int GetMaterielAlbedoTextures(Ref<BindlesTextureArray>& bindlesTexures) const override;
        virtual Ref<UniformBuffer> GetMaterielUniformBuffer() override;

        virtual AssetType GetType() const override { return AssetType::BasicMaterial; }

        virtual Ref<Texture> GetAlbedoTextures() const override { return m_DefaultMap; }

        virtual bool HasSpecForDraw(const BufferLayout& layout, int lodTier) const override;
        

        virtual const std::vector<Ref<Texture>>& GetTextureForDraw(const BufferLayout& layout, int lodTier) const override;
        virtual const Ref<Shader>& GetShaderForDraw(const BufferLayout& layout, int lodTier) const override;
        virtual DrawSpecification::BatchConfig GetDrawSpecification(const BufferLayout& layout, int lodTier) const override;
        virtual int GetRenderMode(const BufferLayout& layout, int lodTier) const override;
        virtual int GetLayaoutIndex(const BufferLayout& layout, int lodTier) const override;
        
        virtual bool HasSpecForDraw(const BufferLayout& layout, int lodTier) override;
        virtual const std::vector<Ref<Texture>>& GetTextureForDraw() override;
        virtual const Ref<Shader>& GetShaderForDraw() override;
        virtual DrawSpecification::BatchConfig GetDrawSpecification() override;
        virtual int GetRenderMode() override;
        virtual int GetLayaoutIndex() override;

    private:
        Ref<Texture> m_DefaultMap;
        Pass* m_LastPassPtr;
#if RY_DEFAULT_MATERIEL_WEAK
        Weak<UniformBuffer> m_UniformMaterial;
#else
        Ref<UniformBuffer> m_UniformMaterial;
#endif
        MaterielShaderData m_BufferData;
        int m_ShadeRenderMode;
        int m_DepthRenderMode;


        std::vector<Pass> m_PassesVec;
        bool m_Change;

        // Ref<Texture> m_AlbedoMap;
        // Ref<Texture> m_NormalMap;
        // Ref<Texture> m_RoughnessMap;
        // Ref<Texture> m_MetallicMap;
    };


}
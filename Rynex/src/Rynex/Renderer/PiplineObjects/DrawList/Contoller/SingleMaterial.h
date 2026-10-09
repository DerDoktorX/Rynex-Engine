#pragma once
// SingleMaterial created in Project Rynex-Rendering on 09/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/ChachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/PiplineObjects/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>

namespace Rynex::DrawListPolicy {

    struct NoIndices{ };

    template<typename PassResources>
    class SingleMaterial
    {
    public:
        using Resources = Ref<Material>;
        using Indices = NoIndices;
        static constexpr uint32_t s_InstanceFields = InstanceField_None;

        // --- public member methods -----------------------------------------------------------------------------------
        bool Set(const Resources& material)
        {
            RY_CORE_ASSERT(nullptr != material, "Material is not valid!");
            const bool otherMaterial = m_Current != material;
            m_Current = material;
            return otherMaterial;
        }

        void Commit() { m_GroupMaterialVec.push_back(m_Current); }

        const Indices& GetIndices() const { return m_Indices; }


        void Bind(ShaderDrawResource& base, const DrawChunk& chunk, CachedBufferRegister& cachedBufferRegister)
        {
            const Ref<Material>& material = m_GroupMaterialVec[chunk.m_Group];
            m_PassResources.AcquireFromMaterial(material);
            m_PassResources.Bind(base);
            base.m_RenderMode = m_PassResources.GetDrawMode(material);
        }

        void Clear()
        {
            m_GroupMaterialVec.clear();
            m_Current = nullptr;
        }

        PassResources& GetPassResources() { return m_PassResources; }

    private:
        // --- private member variables --------------------------------------------------------------------------------
        std::vector<Ref<Material>> m_GroupMaterialVec;
        Ref<Material> m_Current;
        PassResources m_PassResources;
        Indices m_Indices;
    };
    
}
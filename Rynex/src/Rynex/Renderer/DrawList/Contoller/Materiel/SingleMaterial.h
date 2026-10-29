#pragma once
// SingleMaterial created in Project Rynex-Rendering on 09/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/CachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/DrawList/SectionList.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>

namespace Rynex {

    class SingleMaterial
    {
    public:
        enum SingleMaterialBufferSlot : uint32_t
        {
            None = 0,
            SingleMaterialBufferSlot_Texture = 0,
            SingleMaterialBufferSlot_Uniform = 1,
        };
        static constexpr uint32_t s_InstanceFields = InstanceField_None;
    // public member alias ----------------------------------------------------------------------------------------------------
        using Resources = Ref<Material>;
        using Indices = NoIndices;


        // --- public member methods -----------------------------------------------------------------------------------
        bool Set(const Resources& material)
        {
            RY_CORE_ASSERT(nullptr != material, "Material is not valid!");
            const Ref<Material>& currentMaterial = m_SectionList.Current();
            if(currentMaterial != material)
            {
                m_SectionList.Add(material);
                return true;
            }

            return false;
        }

        const Indices& GetIndices() const { return m_Indices; }


        template<typename Output, typename VertexElementArray, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, VertexElementArray& vertexElementArray, BufferRegister& bufferRegister)
        {
            Bind(section, output, bufferRegister);
        }

        template<typename Output, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, BufferRegister& bufferRegister)
        {
            Ref<Material>& materiel = m_SectionList.Current();
            output.GetBindTextures()[SingleMaterialBufferSlot_Texture] =  materiel->GetAlbedoTextures();
            output.GetBindUniform()[SingleMaterialBufferSlot_Uniform] =  materiel->GetMaterielUniformBuffer();
        }

        void Clear()
        {
            m_SectionList.Clear();
        }

    private:
        SectionList<Ref<Material>> m_SectionList;
        Indices m_Indices;
    };
    
}
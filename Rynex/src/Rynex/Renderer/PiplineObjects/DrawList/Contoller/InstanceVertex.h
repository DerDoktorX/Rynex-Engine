#pragma once
// InstanceVertexArray created in Project Rynex-Rendering on 05/10/2026.
#include <Rynex/Renderer/ChachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/PiplineObjects/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>

namespace Rynex {
    template<uint32_t FieldMask>
    class InstanceVertexArray
    {
    public:
        struct Instance
        {
            glm::mat4 m_Model;
            glm::mat4 m_Normal;
            int32_t m_Entity;
            int32_t m_MaterialIndex;
            int32_t m_ParameterIndex;
            int32_t m_TextureIndex;

            static BufferLayout GetLayout()
            {
                BufferLayout layout({
                    {SDT::Float4x4, "m_Model"},
                    {SDT::Float4x4, "m_Normal"},
                    {SDT::Int, "m_Entity"},
                    {SDT::Int, "m_MaterialIndex"},
                    {SDT::Int, "m_ParameterIndex"},
                    {SDT::Int, "m_TextureIndex"},
                });
                layout.SetAutoCompress(true);
                return layout;
            }
        };


        using BufferOutPut = BufferOutPut<0, 1>;


    // public static constexpr ------------------------------------------------------------------------------------------------
        static constexpr uint32_t s_Fields = FieldMask;
    // public static methode --------------------------------------------------------------------------------------------------
        static uint32_t GetMaxCapacity()
        {
            return std::numeric_limits<uint32_t>::max();
        }


        template<typename Indices>
        void Make(const glm::mat4& model, const uint32_t entityID, const Indices& indices)
        {
            const glm::mat4 inverse = glm::inverse(model);
            Instance instance;
            instance.m_Model = model;
            instance.m_Normal = glm::transpose(inverse);
            instance.m_Entity = static_cast<int32_t>(entityID);
            instance.m_MaterialIndex = -1;
            instance.m_ParameterIndex = -1;
            instance.m_TextureIndex = -1;
            if constexpr (0u != (FieldMask & InstanceField_MaterialIndex))
                instance.m_MaterialIndex = indices.m_Material;
            if constexpr (0u != (FieldMask & InstanceField_ParameterIndex))
                instance.m_ParameterIndex = indices.m_Parameter;
            if constexpr (0u != (FieldMask & InstanceField_TextureIndex))
                instance.m_TextureIndex = indices.m_Texture;
             m_Instances.emplace_back(instance);
        }


        void GetOutPutBuffer(CachedBufferRegister& bufferRegister)
        {
            constexpr uint32_t elementByteSize = sizeof(Instance);
            const uint32_t count = m_Instances.size();
            const uint32_t bytesSize = count * elementByteSize;

            const Ref<VertexBuffer>& instancesVertexBuffer = bufferRegister.AcquireVertexBuffer(m_Instances);

        }

    private:
        std::vector<Instance> m_Instances;
        uint32_t m_
    };
}

#pragma once
// InstanceVertexArray created in Project Rynex-Rendering on 05/10/2026.
#include <rypch.h>
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
        };
    // public static constexpr ------------------------------------------------------------------------------------------------
        static constexpr uint32_t s_Fields = FieldMask;
    // public static methode --------------------------------------------------------------------------------------------------
        static uint32_t GetMaxCapacity() { return std::numeric_limits<uint32_t>::max(); }


        template<typename Indices>
        static Instance Make(const glm::mat4& model, const uint32_t entityID, const Indices& indices)
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
            return instance;
        }

        template<typename BufferRegister>
        static void Upload(const Span<Instance>& instances, const Span<DrawElementsIndirectCommand>& commands,  const ShaderDrawResource& base, std::vector<ShaderDrawResource>& out, BufferRegister& bufferRegister)
        {
            constexpr uint32_t elementByteSize = sizeof(Instance);
            const uint32_t count = instances.m_Count;
            const uint32_t bytesSize = count * elementByteSize;

            const Ref<VertexBuffer>& instancesVertexBuffer = bufferRegister.template AcquireVertexBuffer<Instance>(instances);
            // TODO make own Implemtion. (no base.m_VAO shit).
            const Ref<VertexBuffer>& vertexBuffer = nullptr;
            const Ref<VertexBuffer>& indexBuffer = nullptr;
            const Ref<VertexArray>& vertexBufferObject = bufferRegister.AquireInstanceVertexArray(indexBuffer, vertexBuffer, instancesVertexBuffer);

            for (const DrawElementsIndirectCommand& command : commands)
            {
                ShaderDrawResource draw = base;
                draw.m_VAO = vertexBufferObject;
                draw.m_DrawElement = ToPerDrawObject(command);
                out.emplace_back(std::move(draw));
            }
        }
    private:

    };
}

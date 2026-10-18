#pragma once
// InstanceVertexArray created in Project Rynex-Rendering on 05/10/2026.

#include <Rynex/Core/KeyCodes.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/DrawList/SectionList.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>


namespace Rynex {

    class InstanceVertex
    {
    public:
        enum VertexBufferSlot : uint32_t
        {
            VertexBufferSlot_Instance = 1u,
        };
        static constexpr uint32_t s_VertexBufferCount = 1u;
        static constexpr uint32_t s_UniformBufferCount = 0u;
        static constexpr uint32_t s_StorageBufferCount = 0u;
        static constexpr uint32_t s_AttributeLocationMask = BIT(VertexBufferSlot_Instance);
        static constexpr uint32_t s_UniformSlotMask = 0u;
        static constexpr uint32_t s_StorageSlotMask = 0u;
        static constexpr uint32_t s_TextureSlotMask = 0u;
        static constexpr bool s_GpuReadable = false;
        static constexpr bool s_SupportsRetained = false;
    // public member struct ---------------------------------------------------------------------------------------------------
        struct Instance
        {
            glm::mat4 m_Model;
            glm::mat4 m_Normal;
            int32_t m_Entity;
            int32_t m_MaterialIndex;
            int32_t m_ParameterIndex;
            int32_t m_TextureIndex;

            Instance() = default;
            Instance(const Instance&) = default;
            Instance(Instance&& instance) noexcept
                : m_Model(instance.m_Model)
                , m_Normal(instance.m_Normal)
                , m_Entity(instance.m_Entity)
                , m_MaterialIndex(instance.m_MaterialIndex)
                , m_ParameterIndex(instance.m_ParameterIndex)
                , m_TextureIndex(instance.m_TextureIndex)
            {
            }

            static Instance Make(const InstanceSource& instanceSource)
            {
                Instance instance;
                instance.m_Model = instanceSource.m_Model;
                instance.m_Normal = glm::transpose(instanceSource.m_Model);
                instance.m_Entity = instanceSource.m_EntityID;
                instance.m_MaterialIndex = -1;
                instance.m_ParameterIndex = -1;
                instance.m_TextureIndex = -1;
                return instance;
            }

            template<typename InstanceMateriel>
            static Instance Make(const InstanceSource& instanceSource, const InstanceMateriel& instanceMateriel)
            {
                Instance instance = Make(instanceSource);
                if constexpr (0u != (InstanceMateriel::s_FieldMask & InstanceField_MaterialIndex))
                    instance.m_MaterialIndex = instanceMateriel.m_MaterialIndex;
                if constexpr (0u != (InstanceMateriel::s_FieldMask & InstanceField_ParameterIndex))
                    instance.m_ParameterIndex = instanceMateriel.m_ParameterIndex;
                if constexpr (0u != (InstanceMateriel::s_FieldMask & InstanceField_TextureIndex))
                    instance.m_TextureIndex = instanceMateriel.m_TextureIndex;

                return instance;
            }

            static BufferLayout GetLayout()
            {
                constexpr uint32_t instanceIncrease = 1;
                BufferLayout layout({
                    {SDT::Float4x4, "m_Model"},
                    {SDT::Float4x4, "m_Normal"},
                    {SDT::Int, "m_Entity"},
                    {SDT::Int, "m_MaterialIndex"},
                    {SDT::Int, "m_ParameterIndex"},
                    {SDT::Int, "m_TextureIndex"},
                }, instanceIncrease);
                layout.SetAutoCompress(true);

                return layout;
            }
        };
        using Section = std::vector<Instance>;
    // public member methode --------------------------------------------------------------------------------------------------
        InstanceVertex()
            : m_SectionList()
        {
        }



        DrawListControllerResult Add(const InstanceSource& instanceSource)
        {
            std::vector<Instance>& instanceVec = m_SectionList.Current();
            Instance instance = Instance::Make(instanceSource);
            instanceVec.emplace_back(instance);
            return DrawListControllerResult::Same;
        }

        template<typename InstanceMateriel>
        DrawListControllerResult Add(const InstanceSource& instanceSource, const InstanceMateriel& instanceMateriel)
        {
            std::vector<Instance>& instanceVec = m_SectionList.Current();
            instanceVec = Instance::Make(instanceSource, instanceMateriel);
            return DrawListControllerResult::Same;
        }

        uint32_t GetSectionCount() const
        {
            return m_SectionList.Count();
        }

        template<typename Output, typename VertexElementArray, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, VertexElementArray& vertexElementArray, BufferRegister& bufferRegister)
        {
            const std::vector<Instance>& instanceVec = m_SectionList.Get(section);
            const Span<const Instance> instanceSpan(instanceVec.data(), instanceVec.size());
            Ref<VertexBuffer> vertexBuffer = bufferRegister.AcquireVertexBuffer(instanceSpan);
            vertexElementArray[VertexBufferSlot_Instance] = VertexArray::VertexElements{ Instance::GetLayout(), vertexBuffer };
        }

        template<typename Output, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, BufferRegister& bufferRegister)
        {
            static_assert(false, "we need to use the methoed implemtion with VertexElementArray");
        }

        void Clear()
        {
            m_SectionList.Clear();
        }

        uint32_t GetCapacity() const
        {
            return std::numeric_limits<uint32_t>::max();
        }

        uint32_t Allocate()
        {
            static_assert(false, "this mode methode is not implemted!");
            return std::numeric_limits<uint32_t>::max();
        }

        void Write(uint32_t slot, const Instance& instance)
        {
            static_assert(false, "this mode methode is not implemted!");
        }

        void Free(uint32_t slot)
        {
            static_assert(false, "this mode methode is not implemted!");
        }

    private:
        SectionList<Section> m_SectionList;
    };
}

#pragma once
// MeshGeometry created in Project Rynex-Rendering on 10/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/DrawList/SectionList.h>
#include <Rynex/Renderer/Mesh/Mesh.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>

namespace Rynex {



    template<typename PassGeometry, bool Instanced>
    class MeshGeometry
    {
    public:
        enum VertexBufferSlot : uint32_t
        {
            VertexBufferSlot_Vertex = 0u,
        };
        static constexpr uint32_t s_VertexBufferCount = 1u;
        static constexpr uint32_t s_UniformBufferCount = 0u;
        static constexpr uint32_t s_StorageBufferCount = 0u;
        static constexpr uint32_t s_AttributeLocationMask = BIT(VertexBufferSlot_Vertex);
        static constexpr uint32_t s_UniformSlotMask = 0u;
        static constexpr uint32_t s_StorageSlotMask = 0u;
        static constexpr uint32_t s_TextureSlotMask = 0u;
        static constexpr bool s_HasBounds = true;
    // public member alias ----------------------------------------------------------------------------------------------------
        using Resources = Ref<MeshSingle>;
        using DrawObject = DrawElementsIndirectCommand;
        using Bounds = BoundingVolume;
    // public member methods --------------------------------------------------------------------------------------------------
        DrawSplit Set(const Resources& mesh)
        {
            RY_CORE_ASSERT(nullptr != mesh, "Mesh is not valid!");
            if (m_SectionList.IsEmpty())
            {
                m_SectionList.Add(mesh);
                return DrawSplit::NewDraw;
            }
            const Ref<MeshSingle>& currentMesh = m_SectionList.Current();

            if (currentMesh != mesh) // every MeshSingle owns its VB / IB
            {
                m_SectionList.Add(currentMesh);
                return DrawSplit::NewDraw;
            }
            return DrawSplit::Same;
        }

        Bounds GetBounds(uint32_t section) const
        {
            const Ref<MeshSingle>& meshSingle = m_SectionList.Get(section);
            return meshSingle->GetBoundingVolume();
        }

        void Clear()
        {
            m_SectionList.Clear();
        }

        uint32_t GetSectionCount() const
        {
            return m_SectionList.Count();
        }

        template<typename RenderPassTarget>
        const Ref<IndexBuffer>& GetIndexBuffer(uint32_t section) const
        {
            const Ref<MeshSingle>& singleMesh = m_SectionList.Get(section);
            return RenderPassTarget::GetIndexFromMeshSingle(singleMesh);
        }

        uint32_t GetCapacity() const
        {
            return std::numeric_limits<uint32_t>::max();
        }

        template<typename Output, typename VertexElementArray, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, VertexElementArray& vertexElementArray, BufferRegister& bufferRegister)
        {
            const Ref<MeshSingle>& singleMesh = m_SectionList.Get(section);
            const Ref<VertexBuffer>& vertexBuffer = singleMesh->GetVertexBuffer();
            vertexElementArray[VertexBufferSlot_Vertex] = VertexArray::VertexElements{ vertexBuffer };
        }

        template<typename Output, typename BufferRegister>
        void Bind(const uint32_t section, Output& output, BufferRegister& bufferRegister)
        {
            static_assert(false, "we need to use the methoed implemtion with VertexElementArray");
        }

        void OpenSection()
        {
            m_SectionList.Open();
        }

        void Clear()
        {
            m_SectionList.Clear();
            m_CommandOpen = false;
        }

    private:
    // --- private member variables -------------------------------------------------------------------------------------------
        SectionList<Resources> m_SectionList;
        bool m_CommandOpen = false;
    };
    
}

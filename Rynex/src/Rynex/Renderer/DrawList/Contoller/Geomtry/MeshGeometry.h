#pragma once
// MeshGeometry created in Project Rynex-Rendering on 10/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/DrawList/ControlSlot.h>
#include <Rynex/Renderer/DrawList/Contoller/CommandBuildBase.h>
#include <Rynex/Renderer/DrawList/Contoller/InterfaceBase.h>
#include <Rynex/Renderer/Mesh/Mesh.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>

namespace Rynex::Control {

    template<typename Interface, typename CommandBuild>
    class GeometryControlImpl
    {
    public:
        using Source = typename Interface::Source;
        using Indices = typename Interface::Indices;
        using CommandBuild = CommandBuild;
    };

    namespace GeometryDefault {


#if 0
        class Interface
        {
        public:
            using Source = Ref<MeshSingle>;
            using Section = Ref<MeshSingle>;
            using Indices = NoIndices;
            using Unit = uint32_t;

            Interface()
                : m_SectionPtr(nullptr)
            {}



            void SetSection(Section* sectionPtr) { m_SectionPtr = sectionPtr;}
            Section* GetSection() const { return m_SectionPtr; }

            /**
             * Clear don't mean clear the stored address, but the date stored on this address.
             */
            void ClearSection()
            {
                RY_CORE_ASSERT(nullptr != m_SectionPtr, "SectionPtr is nullptr!");

                Ref<MeshSingle>& meshPtr = *m_SectionPtr;
                RY_DESTROY_REF(meshPtr);
            }

            Indices GetInstance() { return Indices{}; }

            DrawSplit HasStorage(const Source& source)
            {
                if (nullptr == m_SectionPtr)
                    return DrawSplit::NewDraw;

                if (!IsSourceEqual(source, *m_SectionPtr))
                    return DrawSplit::NewDraw;

                return DrawSplit::Same;
            }

            void Set(const Source& source)
            {
                RY_CORE_ASSERT(nullptr != m_SectionPtr, "SectionPtr is nullptr!");
                *m_SectionPtr = source;
            }


            [[nodiscard]] Span<const Byte> GetBufferData(Unit index) const
            {
                static_assert(false, "no buffer data, Stored! (Use Buffers)");
                return Span<const Byte>{ nullptr, 0 };
            }

            [[nodiscard]] Ref<VertexBuffer> GetVertexBuffer(const Unit index) const
            {
                RY_CORE_ASSERT(nullptr != m_SectionPtr, "SectionPtr is nullptr!");
                const Section& section = *m_SectionPtr;
                switch (index)
                {
                case 0: return section->GetVertexBuffer();
                default:
                    RY_CORE_ASSERT(false, "Invalid index!");
                    return Ref<VertexBuffer>{ nullptr };
                }
            }

            [[nodiscard]] Ref<IndexBuffer> GetIndexBuffer(const Unit index) const
            {
                RY_CORE_ASSERT(nullptr != m_SectionPtr, "SectionPtr is nullptr!");
                const Section& section = *m_SectionPtr;
                switch (index)
                {
                case 0: return section->GetShadeIndexBuffer();
                case 1: return section->GetDepthIndexBuffer();
                default:
                    RY_CORE_ASSERT(false, "Invalid index!");
                    return Ref<IndexBuffer>{ nullptr };
                }
            }

            [[nodiscard]] Ref<StorageBuffer> GetStorageBuffer(const Unit index) const
            {
                static_assert(false, "no buffer data, Stored! (Use Buffers)");
                return Ref<StorageBuffer>{ nullptr };
            }

            [[nodiscard]] Ref<UniformBuffer> GetUinformBuffer(const Unit index) const
            {
                static_assert(false, "no buffer data, Stored! (Use Buffers)");
                return Ref<UniformBuffer>{ nullptr };
            }


            static bool IsSourceEqual(const Source& a, const Source& b)
            {
                return a == b;
            }

            static bool IsSourceEmpty(const Source& source)
            {
                return nullptr == source;
            }
        private:
            Section* m_SectionPtr;
        };
#else
        class Interface : public InterfaceRefBase<MeshSingle>
        {
        public:
            Interface()
                : InterfaceRefBase<MeshSingle>()
            {
            }

            ~Interface() override
            {
                this->m_SectionPtr = nullptr;
            }

            virtual BufferLayout GetBufferLayout(const Unit index) const override
            {
                const Ref<VertexBuffer> vertexBuffer = GetVertexBuffer(index);
                const BufferLayout& bufferLayout = vertexBuffer->GetLayout();
                return bufferLayout;
            }

            virtual Ref<VertexBuffer> GetVertexBuffer(const Unit index) const override
            {
                RY_CORE_ASSERT(0 == index, "index is invalid! valid index -> ( 0 )");
                const Ref<MeshSingle>& meshSingleRef = Get();
                const Ref<VertexBuffer>& vertexBuffer = meshSingleRef->GetVertexBuffer();
                return vertexBuffer;
            }

            virtual Ref<IndexBuffer> GetIndexBuffer(const Unit index) const override
            {
                switch (index)
                {
                case 0: return Get()->GetShadeIndexBuffer();
                case 1: return Get()->GetDepthIndexBuffer();
                default:
                    RY_CORE_ASSERT(false, "index is invalid! valid index -> ( 0, 1 )");
                }
                return Ref<IndexBuffer>{ nullptr };
            }

        private:
            const Ref<MeshSingle>& Get() const
            {
                Ref<MeshSingle>* meshSinglePtr = this->m_SectionPtr;
                RY_CORE_ASSERT(nullptr != meshSinglePtr, "Section is nullptr");
                return (*meshSinglePtr);
            }
        };
#endif





        class CommandBuild : public CommandBuildBase<Interface, true, false>
        {
        public:
            enum UniformBufferSlot : uint32_t
            {
                VertexBufferSlot_Vertex = 0u
            };

        // public member method -----------------------------------------------------------------------------------------------
            explicit CommandBuild(CachedBufferRegister& cache, Interface& interfaceType, ShaderDrawResource& drawResource)
                : CommandBuildBase<Interface, true, false>(cache, interfaceType, drawResource)
            {
            }

            virtual void CollectVertexBuffer(Span<VertexArray::VertexElements>& vertexElementsSpan) override
            {
                const Ref<VertexBuffer> vertexBuffer = m_Interface.GetVertexBuffer(0);
                vertexElementsSpan[VertexBufferSlot_Vertex] = VertexArray::VertexElements{ vertexBuffer };
            }

            virtual void BindCommand() override
            {
                static_assert(false, "BindCommand!");
            }
        };

        using DefaultCommand = GeometryControlImpl<Interface, CommandBuild>;


    }

#if 0
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
#endif

}

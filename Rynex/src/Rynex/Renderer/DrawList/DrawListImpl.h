#pragma once
// DrawListImpl created in Project Rynex-Rendering on 03/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>

namespace Rynex {

    RY_DEFINE_HAS_STATIC_MEMBER(s_Fields, InstanceControlVaribleFields);
    RY_DEFINE_HAS_STATIC_MEMBER(s_InstanceFields, MaterielControlVaribleInstanceFields);

    /**
     * Holds the data of a draw list, nothing else.
     *
     * InstanceControl: using Instance; static constexpr uint32_t s_Fields; static uint32_t GetCapacity();
     *                  template<Indices> static Instance Make(model, entityID, const Indices&);
     *                  template<BufferRegister> static void Upload(DrawSpan<Instance>, DrawSpan<Command>, base, out, reg);
     * GeometryControl: using Resources; bool Set(const Resources&); void Commit(); void Clear();
     *                  void OnInstance(commands, instanceIndex); template<BufferRegister> void Bind(base, chunk, reg);
     * MaterielControl: using Resources; using Indices; static constexpr uint32_t s_InstanceFields;
     *                  bool Set(const Resources&); void Commit(); void Clear(); const Indices& GetIndices() const;
     *                  template<BufferRegister> void Bind(base, chunk, reg);
     *
     * Set() returns true when the new geometry / material cannot share the current draw call (other VAO, other shader state).
     * Commit() is called when a new group opens: the control stores its current state as the state of that group.
     */
    template<typename InstanceControl, typename GeometryControl, typename MaterielControl, typename TemplateSlots = NoTemplateSlots>
    class DrawListImpl
    {
        static_assert(std::is_trivially_copyable_v<typename MaterielControl::Instance>, "No instance data type provided by InstanceControl!");
        static_assert(0u < sizeof(typename InstanceControl::Instance), "No data in InstanceValue!");
        static_assert(AreMasksDisjoint(GeometryControl::s_AttributeLocationMask, MaterielControl::s_AttributeLocationMask, InstanceControl::s_AttributeLocationMask, TemplateSlots::s_AttributeLocationMask), "Vertex attribute locations overlap between the controls!");
        static_assert(AreMasksDisjoint(GeometryControl::s_UniformSlotMask, MaterielControl::s_UniformSlotMask, InstanceControl::s_UniformSlotMask, TemplateSlots::s_UniformSlotMask), "Uniform buffer slots overlap between the controls!");
        static_assert(AreMasksDisjoint(GeometryControl::s_StorageSlotMask, MaterielControl::s_StorageSlotMask, InstanceControl::s_StorageSlotMask, TemplateSlots::s_StorageSlotMask), "Storage buffer slots overlap between the controls!");
        static_assert(AreMasksDisjoint(GeometryControl::s_TextureSlotMask, MaterielControl::s_TextureSlotMask, InstanceControl::s_TextureSlotMask, TemplateSlots::s_TextureSlotMask), "Texture slots overlap between the controls!");
        static_assert(0u == (MaterielControl::s_InstanceFields & ~InstanceControl::s_Fields), "MaterielControl hands out indices the instance record cannot store!");

    public:
        using GeometryResources = typename GeometryControl::Resources;      // Ref<MeshSingle>, std::vector<Vertex>, AssetHandle, ...
        using MaterielResources = typename MaterielControl::Resources;      // Ref<Material>, AssetHandle, ...
        using MaterielIndices = typename MaterielControl::Indices;          // per instance indices, NoIndices if none
        using Unit = uint32_t;

    // --- public static variables -------------------------------------------------------------------------------------
        static constexpr uint32_t s_VertexBufferCount = GeometryControl::s_VertexBufferCount + MaterielControl::s_VertexBufferCount + InstanceControl::s_VertexBufferCount;
        static constexpr uint32_t s_MinimumGpuFiledByteCount = 4u; // no 1 byte filed like bool are allowed bool values can be used in a int. Expect 1 byte has 8 bit.
        static constexpr bool s_MaterielIndicesFieldEnabled = s_MinimumGpuFiledByteCount < sizeof(MaterielIndices);
    // --- public member methods ---------------------------------------------------------------------------------------
        DrawListImpl()
            : m_InstanceControl()
            , m_GeometryControl()
            , m_MaterielControl()
            , m_Template()
            , m_Stats()
            , m_Remaining(0u)
            , m_NewGroup(true)
        {
        }

        ~DrawListImpl()
        {
        }

        // Shader, camera and other fixed bindings. Copied once per chunk.
        void SetTemplate(const ShaderDrawResource& templ)
        {
            m_Template = templ;
        }

        void SetGeometry(const GeometryResources& geometry)
        {
            if (m_GeometryControl.Set(geometry))
                RequestNewGroup();
        }

        void SetMateriel(const MaterielResources& material)
        {
            if (m_MaterielControl.Set(material))
                RequestNewGroup();
        }

        // Hot path: one compare, no allocation, no map, no virtual call. The geometry control is not involved.
        void Add(const glm::mat4& modelMatrix, const uint32_t entityID)
        {
            if (0u == m_Remaining)
                OpenChunk();

            m_Remaining--;
            const InstanceSource source{ modelMatrix, entityID };
            if constexpr (s_MaterielIndicesFieldEnabled)
            {
                const MaterielIndices& indices = m_MaterielControl.GetIndices();
                m_InstanceControl.Add(source, indices);
            }
            else
            {
                m_InstanceControl.Add(source);
            }
        }

        // Smallest capacity of all controls for the open (or the next) chunk. No side effect.
        Unit GetEffectiveCapacity() const
        {
            const Unit geometryCapacity = m_GeometryControl.GetCapacity();
            const Unit materielCapacity = m_MaterielControl.GetCapacity();
            const Unit instanceCapacity = m_InstanceControl.GetCapacity();
            const Unit minGeometryMaterielCapacity = std::min(geometryCapacity, materielCapacity);
            const Unit minCapacity = std::min(minGeometryMaterielCapacity, instanceCapacity);
            return minCapacity;
        }

        Unit GetCount() const
        {
            return m_InstanceControl.GetCount();
        }

        bool IsEmpty() const
        {
            return 0u == GetCount();
        }

        // Frame reset. Controls keep their allocations, the template stays.
        void Clear()
        {
            m_InstanceControl.Clear();
            m_GeometryControl.Clear();
            m_MaterielControl.Clear();
            m_Stats = DrawListStats();
            m_Remaining = 0u;
            m_NewGroup = true;
        }

        // One ShaderDrawResource per chunk is appended to out. No cutting here, the chunks already exist.
        template<typename BufferRegister>
        void Flush(std::vector<ShaderDrawResource>& out, BufferRegister& bufferRegister)
        {
            const Unit chunkCountInstance = m_MaterielControl.GetSectionCount();
            const Unit chunkCountGeometry = m_GeometryControl.GetSectionCount();
            const Unit chunkCountMateriel = m_InstanceControl.GetSectionCount();
            RY_CORE_ASSERT(chunkCountInstance == chunkCountGeometry, "Geometry sections do not match the chunks!");
            RY_CORE_ASSERT(chunkCountInstance == chunkCountMateriel, "Materiel sections do not match the chunks!");
            const Unit chunkCount = chunkCountInstance;

            const size_t outSize = out.size();
            const size_t outReserve = outSize + chunkCount;
            out.reserve(outReserve);
            for (Unit i = 0u; i < chunkCount; i++)
            {
                ShaderDrawResource draw = m_Template;
                BuildDraw(i, draw, bufferRegister);
                out.emplace_back(std::move(draw));
            }
        }

    private:
        using VertexElementArray = std::array<VertexArray::VertexElements, s_VertexBufferCount>;
    // --- private member method ---------------------------------------------------------------------------------------

        // Forces the next Add() into the slow path. The group is counted when the chunk opens, not here.
        void RequestNewGroup()
        {
            m_NewGroup = true;
            m_Remaining = 0u;
        }

        // New draw call. Fixed order: Geometry, Materiel, Instance. Every control stores its current state as the new section.
        void OpenChunk()
        {
            m_GeometryControl.OpenSection();
            m_MaterielControl.OpenSection();
            m_InstanceControl.OpenSection();

            m_Remaining = GetEffectiveCapacity();
            RY_CORE_ASSERT(0u < m_Remaining, "A control has no space for a single instance!");

            m_Stats.m_ChunkCount++;
            if (m_NewGroup)
                m_Stats.m_GroupCount++;
            m_NewGroup = false;
        }

        template<typename BufferRegister>
        void BuildVertexDraw(const Unit sectionIndex, ShaderDrawResource& draw, BufferRegister& bufferRegister)
        {
            const Unit instanceCount = m_InstanceControl.GetSectionInstanceCount(sectionIndex);
            const DrawChunk chunk{ sectionIndex, instanceCount };

            VertexElementArray elements;
            m_GeometryControl.Bind(chunk, draw, elements, bufferRegister);
            m_MaterielControl.Bind(chunk, draw, elements, bufferRegister);
            m_InstanceControl.Bind(chunk, draw, elements, bufferRegister);

            // check for not matching vertex buffer element count. is for developing off DrawListImpl.
            if constexpr (0u < MaterielControl::s_VertexBufferCount && 0u < InstanceControl::s_VertexBufferCount)
                CheckVertexBufferOffElementCount(elements, instanceCount);

            draw.m_VAO = AcquireVertexArray(sectionIndex, elements, bufferRegister);
        }

        // check for elements count differences in Vertex Buffer and checks instance increase the have the necessary element count.
        static constexpr void CheckVertexBufferOffElementCount(const VertexElementArray& elements, const uint32_t minInstanceCount)
        {
            uint32_t perVertex = 0u;

            for (auto&[layout, buffer] : elements)
            {
                const uint32_t byteSize = buffer->GetByteSize();
                const uint32_t stride = layout.GetStride();
                const uint32_t count = byteSize / stride;
                const uint32_t instanceIncrease = layout.GetInstastancIncreas();
                if (0 == instanceIncrease)
                {
                    RY_CORE_ASSERT(count == perVertex || 0u == perVertex, "Equal not per Instance Increase, but not not equal Ellements in call. Worse case Can run Out off Memory!");
                    perVertex = count;
                }
                else
                {
                    const uint32_t instanceCount = instanceIncrease * count;
                    RY_CORE_ASSERT(minInstanceCount <= instanceCount, "Equal per Instance Increase, but not not equal Ellements in call. Can run Out off Memory!");
                }
            }
        }

        // Only place that builds the VAO. VAO hash with per frame instance buffers.
        template<typename BufferRegister>
        Ref<VertexArray> AcquireVertexArray(const Unit sectionIndex, const VertexElementArray& elements, BufferRegister& bufferRegister) const
        {
            Ref<IndexBuffer> indexBuffer = nullptr;
            if constexpr (0u < GeometryControl::s_IndexBufferCount)
                indexBuffer = m_GeometryControl.GetIndexBuffer(sectionIndex);

            const Span<const VertexArray::VertexElements> elementSpan(elements.data(), elements.size());
            auto [hash, vertexArray] = bufferRegister.AcquireUniqueVertexArray(indexBuffer, elementSpan);
            return vertexArray;
        }


    // --- private member variables ------------------------------------------------------------------------------------
        InstanceControl m_InstanceControl;
        GeometryControl m_GeometryControl;
        MaterielControl m_MaterielControl;
        ShaderDrawResource m_Template;
        DrawListStats m_Stats;
        uint32_t m_Remaining;
        bool m_NewGroup;
    };

}
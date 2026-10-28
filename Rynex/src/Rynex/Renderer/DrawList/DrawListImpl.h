#pragma once
// DrawListImpl created in Project Rynex-Rendering on 03/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>

namespace Rynex {

    template<typename InstanceControl, typename GeometryControl, typename MaterielControl, typename DrawController, typename ChunkPolicy>
    class DrawListImpl
    {
    public:
        using InstanceResources = typename InstanceControl::Resources;
        using GeometryResources = typename GeometryControl::Resources;      // Ref<MeshSingle>, std::vector<Vertex>, AssetHandle, ...
        using MaterielResources = typename MaterielControl::Resources;      // Ref<Material>, AssetHandle, ...
        using DrawResources = typename DrawController::Resources;

        using InstanceChunk = typename InstanceControl::Chunk;
        using GeometryChunk = typename GeometryControl::Chunk;
        using MaterielChunk = typename MaterielControl::Chunk;
        using DrawResources = typename DrawController::Chunk;

        using MaterielIndices = typename MaterielControl::Indices;          // per instance indices, NoIndices if none
        using GeometryIndices = typename GeometryControl::Indices;
        using DrawIndices = typename DrawController::Indices;

        using Unit = uint32_t;

    // --- public static variables -------------------------------------------------------------------------------------
        static constexpr uint32_t s_VertexBufferCount = GeometryControl::s_VertexBufferCount + MaterielControl::s_VertexBufferCount + InstanceControl::s_VertexBufferCount + DrawController::s_VertexBufferCount;

        static constexpr uint32_t s_MinimumGpuFiledByteCount = 4u; // no 1 byte filed like bool are allowed bool values can be used in a int. Expect 1 byte has 8 bit.
        static constexpr bool s_MaterielIndicesFieldEnabled = s_MinimumGpuFiledByteCount < sizeof(MaterielIndices);
    // --- public member methods ---------------------------------------------------------------------------------------
        DrawListImpl()
        {
        }

        ~DrawListImpl()
        {
        }

        // Shader, camera and other fixed bindings. Copied once per chunk.
        void SetTemplate(const DrawResources& templ)
        {
        }

        void SetGeometry(const GeometryResources& geometry)
        {
        }

        void SetMateriel(const MaterielResources& material)
        {
        }

        // Hot path: one compare, no allocation, no map, no virtual call. The geometry control is not involved.
        void Add(const InstanceResources& instance)
        {
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
        }

        // New draw call. Fixed order: Geometry, Materiel, Instance. Every control stores its current state as the new section.
        void OpenChunk()
        {
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

    };

}
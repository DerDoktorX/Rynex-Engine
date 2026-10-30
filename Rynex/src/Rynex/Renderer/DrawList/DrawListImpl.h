#pragma once
// DrawListImpl created in Project Rynex-Rendering on 03/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/DrawList/ControlSlot.h>

#include <Rynex/Renderer/Rendering/ShaderDrawList.h>



namespace Rynex {

    template<typename InstanceControl, typename GeometryControl, typename MaterielControl, typename CommandController, typename ChunkPolicy>
    class DrawListImpl
    {
    public:
        using InstanceInterface = typename InstanceControl::Interface;
        using GeometryInterface = typename GeometryControl::Interface;      // Ref<MeshSingle>, std::vector<Vertex>, AssetHandle, ...
        using MaterielInterface = typename MaterielControl::Interface;      // Ref<Material>, AssetHandle, ...
        using CommandInterface = typename CommandController::Interface;

        using InstanceSource = typename InstanceControl::Source;
        using GeometrySource = typename GeometryControl::Source;      // Ref<MeshSingle>, std::vector<Vertex>, AssetHandle, ...
        using MaterielSource = typename MaterielControl::Source;      // Ref<Material>, AssetHandle, ...
        using CommandSource = typename CommandController::Source;

        using MaterielIndices = typename MaterielControl::Indices;          // per instance indices, NoIndices if none
        using GeometryIndices = typename GeometryControl::Indices;
        using CommandIndices = typename CommandController::Indices;

        using GeometryInfo = typename GeometryControl::Info;
        using Unit = uint32_t;

    // --- public static variables -------------------------------------------------------------------------------------
        static constexpr uint32_t s_VertexBufferCount = GeometryControl::s_VertexBufferCount + MaterielControl::s_VertexBufferCount + InstanceControl::s_VertexBufferCount + DrawController::s_VertexBufferCount;

        static constexpr uint32_t s_MinimumGpuFiledByteCount = 4u; // no 1 byte filed like bool are allowed bool values can be used in a int. Expect 1 byte has 8 bit.
        static constexpr bool s_MaterielIndicesFieldEnabled = s_MinimumGpuFiledByteCount < sizeof(MaterielIndices);
    // --- public member methods ---------------------------------------------------------------------------------------
        DrawListImpl()
            : m_InstanceSlot()
            , m_MaterielSlot()
            , m_GeometrySlot()
            , m_CommandSlot()
            , m_Stats(DrawSplit::Same)
        {
        }

        ~DrawListImpl()
        {
        }

        // Shader, camera and other fixed bindings. Copied once per chunk.
        void SetTemplate(CommandSource templ)
        {
            DrawSplit drawSplit = m_CommandSlot.SetOnInterface(geometry);
            StateChang(drawSplit);
        }

        void SetGeometry(GeometrySource geometry)
        {
            DrawSplit drawSplit = m_GeometrySlot.SetOnInterface(geometry);
            StateChang(drawSplit);
        }

        void SetMateriel(MaterielSource material)
        {
            DrawSplit drawSplit = m_MaterielSlot.SetOnInterface(material);
            StateChang(drawSplit);
        }

        void SectionHotPath()
        {
            switch (m_Stats)
            {
            case DrawSplit::Same:
            {
                break;
            }
            case DrawSplit::NewDraw:
            {
                FlushSection();
                OpenSection();
                break;
            }
            case DrawSplit::NewCommand:
            {
                FlushSection();
                break;
            }
            case DrawSplit::None:
            {
                RY_CORE_FATEL("DrawSplit::None is not expected");
                break;
            }
            default:
            {
                RY_CORE_ASSERT(false, "not valid DrawSplit State");
                break;
            }
            }
            m_Stats = DrawSplit::Same;
        }

        // Hot path: one compare, no allocation, no map, no virtual call. The geometry control is not involved.
        void Add(const InstanceSource& instance)
        {
            CommandInterface& commandInterface = m_CommandSlot.CurrentInterface();
            GeometryInterface& geometryInterface = m_GeometrySlot.CurrentInterface();
            MaterielInterface& materielInterface = m_MaterielSlot.CurrentInterface();
            instance.SetCommand(commandInterface.GetInstance());
            instance.SetGeometry(geometryInterface.GetInstance());
            instance.SetMateriel(materielInterface.GetInstance());

            if (!m_InstanceSlot.AddOnInterface(instance))
            {
                m_Stats = DrawSplit::NewDraw;
                SectionHotPath();
            }
        }

        // Smallest count of all controls for the open (or the next) chunk. No side effect.
        Unit GetEffectiveCount() const
        {
            const Unit geometrySectionCount = m_GeometrySlot.GetSectionCount();
            const Unit materielSectionCount = m_MaterielSlot.GetSectionCount();
            const Unit instanceSectionCount = m_InstanceSlot.GetSectionCount();
            const Unit commandSectionCount = m_CommandSlot.GetSectionCount();
            const Unit minCommandGeometryCount = std::min(commandSectionCount, geometrySectionCount);
            const Unit minMaterielInstanceCount = std::min(materielSectionCount, instanceSectionCount);
            const Unit minCount = std::min(minCommandGeometryCount, minMaterielInstanceCount);
            return minCount;
        }

        Unit GetCount() const
        {
            const Unit geometryCount = m_GeometrySlot.GetCount();
            const Unit instanceCount = m_InstanceSlot.GetCount();
            const Unit materielCount = m_MaterielSlot.GetCount();
            const Unit commandCount = m_CommandSlot.GetCount();
            const Unit commandGeometryMaxCount = std::max(geometryCount, commandCount);
            const Unit materielInstanceMaxCount = std::max(materielCount, instanceCount);
            const Unit count = std::max(commandGeometryMaxCount, materielInstanceMaxCount);
            return count;
        }

        bool IsEmpty() const
        {
            return 0u == GetCount();
        }

        // Frame reset. Controls keep their allocations, the template stays.
        void Clear()
        {
            m_InstanceSlot.Clear();
            m_MaterielSlot.Clear();
            m_GeometrySlot.Clear();
            m_CommandSlot.Clear();
            m_Stats = DrawSplit::Same;
        }

        // One ShaderDrawResource per chunk is appended to out. No cutting here, the chunks already exist.
        void FlushCommands(std::vector<ShaderDrawResource>& out, CachedBuffer& bufferRegister)
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
        void StateChang(DrawSplit drawSplit)
        {
            switch (drawSplit)
            {
            case DrawSplit::None:
            {
                RY_CORE_FATEL("DrawSplit::None is not expected");
                break;
            }
            case DrawSplit::Same:
            {
                break;
            }
            case DrawSplit::NewDraw:
            {
                m_Stats = DrawSplit::NewDraw;
                break;
            }
            case DrawSplit::NewCommand:
            {
                if (DrawSplit::NewDraw != m_Stats)
                    m_Stats = DrawSplit::NewCommand;
                break;
            }
            default:
            {
                RY_CORE_ASSERT(false, "not valid DrawSplit State");
                break;
            }
            }
            m_Stats = DrawSplit::Same;
        }

        /**
         * alle processors, that need to be fished bevor open a new section or bevor start a new indirect element.
         */
        void FlushSection()
        {
            Unit instanceCount = m_InstanceSlot.GetSectionCount();
            Unit materielCount = m_MaterielSlot.GetSectionCount();
            GeometryCommandInfo geometryInfo = m_GeometrySlot.GetGeometryInfo();

            CommandInterface& commandInterface = m_CommandSlot.CurrentInterface();
            commandInterface.Flush(geometryInfo, instanceCount, materielCount);
        }

        // New draw call. Fixed order: Geometry, Materiel, Instance. Every control stores its current state as the new section.
        void OpenSection()
        {
            m_InstanceSlot.OpenNextSection();
            m_MaterielSlot.OpenNextSection();
            m_GeometrySlot.OpenNextSection();
            m_CommandSlot.OpenNextSection();

            m_InstanceSlot.ClearSection();
            m_MaterielSlot.ClearSection();
            m_GeometrySlot.ClearSection();
            m_CommandSlot.ClearSection();

            m_InstanceSlot.UploadCacheOnInterface();
            m_MaterielSlot.UploadCacheOnInterface();
            m_GeometrySlot.UploadCacheOnInterface();

            m_Stats = DrawSplit::Same;
        }

        // Only place that builds the VAO. VAO hash with per frame instance buffers.
        Ref<VertexArray> AcquireVertexArray(const Unit sectionIndex, const VertexElementArray& elements, CachedBufferRegister& bufferRegister) const
        {
            Ref<IndexBuffer> indexBuffer = nullptr;
            if constexpr (0u < GeometryControl::s_IndexBufferCount)
                indexBuffer = m_GeometryControl.GetIndexBuffer(sectionIndex);

            const Span<const VertexArray::VertexElements> elementSpan(elements.data(), elements.size());
            auto [hash, vertexArray] = bufferRegister.AcquireUniqueVertexArray(indexBuffer, elementSpan);
            return vertexArray;
        }


    // --- private member variables ------------------------------------------------------------------------------------
        ControlSlot<InstanceControl> m_InstanceSlot;
        ControlSlot<MaterielControl> m_MaterielSlot;
        ControlSlot<GeometryControl> m_GeometrySlot;
        ControlSlot<CommandController> m_CommandSlot;
        DrawSplit m_Stats;
    };

}

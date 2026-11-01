#pragma once
// DrawListImpl created in Project Rynex-Rendering on 03/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/DrawList/ControlSlot.h>

#include <Rynex/Renderer/Rendering/ShaderDrawList.h>



namespace Rynex {

    template<typename InstanceControl, typename GeometryControl, typename MaterielControl, typename CommandControl>
    class DrawListImpl
    {
    public:
        using CommandSlot =  ControlSlot<CommandControl>;
        using GeometrySlot = ControlSlot<GeometryControl>;
        using MaterielSlot = ControlSlot <MaterielControl>;
        using InstanceSlot = ControlSlot <InstanceControl>;

        using InterfaceInstance = typename InstanceControl::Interface;
        using InterfaceGeometry = typename GeometryControl::Interface;      // Ref<MeshSingle>, std::vector<Vertex>, AssetHandle, ...
        using InterfaceMateriel = typename MaterielControl::Interface;      // Ref<Material>, AssetHandle, ...
        using InterfaceCommand = typename CommandControl::Interface;

        using SourceInstance = typename InstanceControl::Source;
        using SourceGeometry = typename GeometryControl::Source;      // Ref<MeshSingle>, std::vector<Vertex>, AssetHandle, ...
        using SourceMateriel = typename MaterielControl::Source;      // Ref<Material>, AssetHandle, ...
        using SourceCommand = typename CommandControl::Source;

        using IndicesMateriel = typename MaterielControl::Indices;          // per instance indices, NoIndices if none
        using IndicesGeometry = typename GeometryControl::Indices;
        using IndicesCommand = typename CommandControl::Indices;

        using BuildCommand = typename CommandControl::CommandBuild;
        using BuildGeometry = typename GeometryControl::CommandBuild;
        using BuildMateriel = typename MaterielControl::CommandBuild;
        using BuildInstance = typename InstanceControl::CommandBuild;

        using Unit = uint32_t;

    // --- public static variables -------------------------------------------------------------------------------------
        static constexpr uint32_t s_VertexBufferCount = GeometryControl::s_VertexBufferCount + MaterielControl::s_VertexBufferCount + InstanceControl::s_VertexBufferCount + CommandControl::s_VertexBufferCount;
        using VertexElementArray = std::array<VertexArray::VertexElements, s_VertexBufferCount>;

        static constexpr uint32_t s_MinimumGpuFiledByteCount = 4u; // no 1 byte filed like bool are allowed bool values can be used in a int. Expect 1 byte has 8 bit.
        static constexpr bool s_MaterielIndicesFieldEnabled = s_MinimumGpuFiledByteCount < sizeof(IndicesMateriel);
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
        void SetTemplate(SourceCommand& templ)
        {
            const DrawSplit drawSplit = m_CommandSlot.SetOnInterface(templ);
            StateChang(drawSplit);
        }

        void SetGeometry(SourceGeometry& geometry)
        {
            const DrawSplit drawSplit = m_GeometrySlot.SetOnInterface(geometry);
            StateChang(drawSplit);
        }

        void SetMateriel(SourceMateriel& material)
        {
            const DrawSplit drawSplit = m_MaterielSlot.SetOnInterface(material);
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
                if(FlushSection())
                {
                    RY_CORE_FATAL("FlushSection forced to push to next Section, instead off new command!");
                    OpenSection();
                }
                break;
            }
            case DrawSplit::None:
            {
                RY_CORE_FATAL("DrawSplit::None is not expected");
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
        void Add(const SourceInstance& instance)
        {
            InterfaceCommand& commandInterface = m_CommandSlot.CurrentInterface();
            InterfaceGeometry& geometryInterface = m_GeometrySlot.CurrentInterface();
            InterfaceMateriel& materielInterface = m_MaterielSlot.CurrentInterface();
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
        void FlushCommands(std::vector<ShaderDrawResource>& out, CachedBufferRegister& bufferRegister)
        {
            const Unit sectionCountInstance = m_InstanceSlot.Count();
            const Unit sectionCountMateriel = m_MaterielSlot.Count();
            const Unit sectionCountGeometry = m_GeometrySlot.Count();
            const Unit sectionCountCommand = m_CommandSlot.Count();
            RY_CORE_ASSERT(sectionCountInstance == sectionCountCommand, "Command sections do not match the indices section!");
            RY_CORE_ASSERT(sectionCountInstance == sectionCountMateriel, "Geometry sections do not match the indices section!");
            RY_CORE_ASSERT(sectionCountInstance == sectionCountGeometry, "Materiel sections do not match the indices section!");
            const Unit chunkCount = sectionCountInstance;

            const size_t outSize = out.size();
            const size_t outReserve = outSize + chunkCount;
            out.reserve(outReserve);
            for (Unit i = 0u; i < chunkCount; i++)
            {
                SetInterfaceIndex(i);
                ShaderDrawResource drawResource;
                UploadDrawList(drawResource, bufferRegister);
                out.emplace_back(std::move(drawResource));
            }
        }

    private:


    // --- private member method ---------------------------------------------------------------------------------------
        void StateChang(DrawSplit drawSplit)
        {
            switch (drawSplit)
            {
            case DrawSplit::None:
            {
                RY_CORE_FATAL("DrawSplit::None is not expected");
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
        bool FlushSection()
        {
            Unit instanceCount = m_InstanceSlot.GetSectionCount();
            Unit materielCount = m_MaterielSlot.GetSectionCount();
            GeometryCommandInfo geometryInfo = m_GeometrySlot.GetGeometryInfo();

            InterfaceCommand& commandInterface = m_CommandSlot.CurrentInterface();
            return commandInterface.SectionFlush(geometryInfo, instanceCount, materielCount);
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


        void SetInterfaceIndex(Unit index)
        {
            m_InstanceSlot.SetCurrentInterface(index);
            m_MaterielSlot.SetCurrentInterface(index);
            m_GeometrySlot.SetCurrentInterface(index);
            m_CommandSlot.SetCurrentInterface(index);
        }

        void UploadDrawList(ShaderDrawResource& drawResource, CachedBufferRegister& bufferRegister)
        {
            BuildCommand buildCommand = m_CommandSlot.CreateBuild(drawResource,bufferRegister);
            BuildGeometry buildGeometry = m_GeometrySlot.CreateBuild(drawResource,bufferRegister);
            BuildMateriel buildMateriel = m_MaterielSlot.CreateBuild(drawResource,bufferRegister);
            BuildInstance buildInstance = m_InstanceSlot.CreateBuild(drawResource,bufferRegister);

            if constexpr (0u < s_VertexBufferCount)
                drawResource.m_VAO = CollectVertexBuffer(buildCommand, buildGeometry, buildMateriel, buildInstance, bufferRegister);

            CommandSlot::BindCommand(buildCommand);
            GeometrySlot::BindCommand(buildGeometry);
            MaterielSlot::BindCommand(buildMateriel);
            InstanceSlot::BindCommand(buildMateriel);
        }

        Ref<VertexArray> CollectVertexBuffer(BuildCommand& command, BuildGeometry& geometry, BuildMateriel& material, BuildInstance& instance, CachedBufferRegister& bufferRegister)
        {
            VertexElementArray vertexElementArray;
            Span<VertexElementArray> vertexElementSpan(vertexElementArray.data(), vertexElementArray.size());
            CommandSlot::CollectVertexBuffer(command, vertexElementSpan);
            GeometrySlot::CollectVertexBuffer(geometry, vertexElementSpan);
            MaterielSlot::CollectVertexBuffer(material, vertexElementSpan);
            InstanceSlot::CollectVertexBuffer(instance, vertexElementSpan);
            return AcquireVertexArray(vertexElementArray, bufferRegister);
        }

        // Only place that builds the VAO. VAO hash with per frame instance buffers.
        Ref<VertexArray> AcquireVertexArray(const VertexElementArray& elements, CachedBufferRegister& bufferRegister)
        {
            Ref<IndexBuffer> indexBuffer = nullptr;
            if constexpr (0u < GeometryControl::s_IndexBufferCount)
            {
                InterfaceGeometry& geometryInterface = m_GeometrySlot.CurentInterface();
                indexBuffer = geometryInterface.GetIndexBuffer(indexBuffer);
            }

            const Span<const VertexArray::VertexElements> elementSpan(elements.data(), elements.size());
            auto [hash, vertexArray] = bufferRegister.AcquireUniqueVertexArray(indexBuffer, elementSpan);
            return vertexArray;
        }


    // --- private member variables ------------------------------------------------------------------------------------
        InstanceSlot m_InstanceSlot;
        MaterielSlot m_MaterielSlot;
        GeometrySlot m_GeometrySlot;
        CommandSlot m_CommandSlot;
        DrawSplit m_Stats;
    };

}

#pragma once
// DrawListImpl created in Project Rynex-Rendering on 03/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/ChachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/PiplineObjects/DrawList/DrawListTypes.h>
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
    template<typename InstanceControl, typename GeometryControl, typename MaterielControl>
    class DrawListImpl
    {
        static_assert(std::is_trivially_copyable_v<typename InstanceControl::Instance>, "No instance data type provided by InstanceControl!");
        static_assert(0u < sizeof(typename InstanceControl::Instance), "No data in InstanceValue!");
        static_assert(HasMemberStatic_InstanceControlVaribleFields_v<InstanceControl>, "No static s_Fields in InstanceControl!");
        static_assert(HasMemberStatic_MaterielControlVaribleInstanceFields_v<MaterielControl>, "No static s_InstanceFields in MaterielControl!");
        static_assert(std::is_trivially_copyable_v<typename MaterielControl::Indices>, "No indices data type provided by MaterielControl!");
        static_assert(0u == (MaterielControl::s_InstanceFields & ~InstanceControl::s_Fields), "MaterielControl hands out indices the instance record cannot store!");

    public:
        using InstanceValue = typename InstanceControl::Instance;
        using GeometryResources = typename GeometryControl::Resources;      // Ref<MeshSingle>, std::vector<Vertex>, AssetHandle, ...
        using MaterielResources = typename MaterielControl::Resources;      // Ref<Material>, AssetHandle, ...
        using MaterielIndices = typename MaterielControl::Indices;          // per instance indices, NoIndices if none
        using Unit = uint32_t;

    // --- public static variables -------------------------------------------------------------------------------------
        static constexpr uint32_t s_InstanceFields = InstanceControl::s_Fields;
        static constexpr uint32_t s_MaterielFields = MaterielControl::s_InstanceFields;

    // --- public member methods ---------------------------------------------------------------------------------------
        DrawListImpl()
            : m_InstanceVec()
            , m_Commands()
            , m_ChunkCommands()
            , m_GroupStartVec()
            , m_Template()
            , m_GeometryControl()
            , m_MaterielControl()
            , m_GroupPending(true)
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
            const bool startsGroup = m_GeometryControl.Set(geometry);
            if (true == startsGroup)
                m_GroupPending = true;
        }

        void SetMateriel(const MaterielResources& material)
        {
            const bool startsGroup = m_MaterielControl.Set(material);
            if (true == startsGroup)
                m_GroupPending = true;
        }

        void Add(const glm::mat4& modelMatrix, const uint32_t entityID)
        {
            if (true == m_GroupPending)
                OpenGroup();

            const uint32_t instanceIndex = GetCount();
            const MaterielIndices& indices = m_MaterielControl.GetIndices();
            m_InstanceVec.emplace_back(InstanceControl::Make(modelMatrix, entityID, indices));
            m_GeometryControl.OnInstance(m_Commands, instanceIndex);
        }

        Unit GetCount() const
        {
            return static_cast<Unit>(m_InstanceVec.size());
        }

        Unit GetByteSize() const
        {
            return GetCount() * static_cast<Unit>(sizeof(InstanceValue));
        }

        bool IsEmpty() const
        {
            return 0u == GetCount();
        }

        void Clear()
        {
            m_InstanceVec.clear();
            m_Commands.clear();
            m_ChunkCommands.clear();
            m_GroupStartVec.clear();
            m_GeometryControl.Clear();
            m_MaterielControl.Clear();
            m_GroupPending = true;
        }

        /**
         * Cuts the CPU data into draw calls and appends them to out. Every group is at least one draw call,
         * a group is cut further when InstanceControl::GetCapacity() is smaller than the group.
         * @tparam BufferRegister Register to get or create buffers, only forwarded to the controls.
         * @param out a output vector off draw calls
         * @param bufferRegister register to get or create Buffer
         */
        void UploadCommands(std::vector<ShaderDrawResource>& out, CachedBufferRegister& bufferRegister)
        {
            if (true == IsEmpty())
                return;

            const uint32_t capacity = InstanceControl::GetCapacity();
            RY_CORE_ASSERT(0u < capacity, "InstanceControl has no space for a single instance!");

            const uint32_t groupCount = static_cast<uint32_t>(m_GroupStartVec.size());
            uint32_t commandCursor = 0u;
            for (uint32_t group = 0u; group < groupCount; group++)
            {
                uint32_t begin = m_GroupStartVec[group];
                const uint32_t groupEnd = GetGroupEnd(group);
                while (begin < groupEnd)
                {
                    SkipFinishedCommands(begin, commandCursor);
                    const uint32_t end = FindChunkEnd(begin, groupEnd, capacity, commandCursor);
                    const uint32_t count = end - begin;
                    const DrawChunk chunk{ group, begin, count };
                    BuildChunkCommands(chunk, commandCursor);
                    UploadChunk(chunk, out, bufferRegister);
                    begin = end;
                }
            }
        }

    private:
    // --- private member methods --------------------------------------------------------------------------------------
        void OpenGroup()
        {
            m_GroupStartVec.push_back(GetCount());
            m_GeometryControl.Commit();
            m_MaterielControl.Commit();
            m_GroupPending = false;
        }

        uint32_t GetGroupEnd(const uint32_t group) const
        {
            const uint32_t next = group + 1u;

            if (next < m_GroupStartVec.size())
                return m_GroupStartVec[next];
            return GetCount();
        }

        // commands are ordered by m_BaseInstance, move the cursor to the command that contains 'begin'
        void SkipFinishedCommands(const uint32_t begin, uint32_t& commandCursor) const
        {
            const uint32_t commandCount = static_cast<uint32_t>(m_Commands.size());
            while (commandCursor < commandCount)
            {
                const DrawElementsIndirectCommand& command = m_Commands[commandCursor];
                const uint32_t commandEnd = command.m_BaseInstance + command.m_InstancesCount;
                if (begin < commandEnd)
                    return;
                commandCursor++;
            }
        }

        // whole group when it fits, else the last command border inside the capacity, else cut inside a command
        uint32_t FindChunkEnd(const uint32_t begin, const uint32_t groupEnd, const uint32_t capacity,
            const uint32_t commandCursor) const
        {
            const uint32_t remaining = groupEnd - begin;
            if (remaining <= capacity)
                return groupEnd;

            const uint32_t limit = begin + capacity;
            const uint32_t commandCount = static_cast<uint32_t>(m_Commands.size());
            uint32_t cut = limit;
            for (uint32_t i = commandCursor; i < commandCount; i++)
            {
                const uint32_t base = m_Commands[i].m_BaseInstance;
                if (limit < base)
                    break;
                if (begin < base)
                    cut = base;
            }
            return cut;
        }

        // copies the commands of [chunk begin, chunk end) and rebases m_BaseInstance to the chunk's own instance buffer
        void BuildChunkCommands(const DrawChunk& chunk, const uint32_t commandCursor)
        {
            m_ChunkCommands.clear();
            const uint32_t begin = chunk.m_FirstInstance;
            const uint32_t end = begin + chunk.m_InstanceCount;
            const uint32_t commandCount = static_cast<uint32_t>(m_Commands.size());
            for (uint32_t i = commandCursor; i < commandCount; i++)
            {
                const DrawElementsIndirectCommand& source = m_Commands[i];
                if (end <= source.m_BaseInstance)
                    break;

                const uint32_t sourceEnd = source.m_BaseInstance + source.m_InstancesCount;
                const uint32_t clipBegin = std::max(source.m_BaseInstance, begin);
                const uint32_t clipEnd = std::min(sourceEnd, end);

                DrawElementsIndirectCommand clipped = source;
                clipped.m_BaseInstance = clipBegin - begin;
                clipped.m_InstancesCount = clipEnd - clipBegin;
                m_ChunkCommands.push_back(clipped);
            }
        }

        template<typename BufferRegister>
        void UploadChunk(const DrawChunk& chunk, std::vector<ShaderDrawResource>& out, BufferRegister& bufferRegister)
        {
            ShaderDrawResource base = m_Template;
            m_GeometryControl.Bind(base, chunk, bufferRegister);
            m_MaterielControl.Bind(base, chunk, bufferRegister);

            const InstanceValue* instanceData = m_InstanceVec.data() + chunk.m_FirstInstance;
            const Span<InstanceValue> instances{ instanceData, chunk.m_InstanceCount };
            const Span<DrawElementsIndirectCommand> commands( m_ChunkCommands.data(), m_ChunkCommands.size() );
            InstanceControl::Upload(instances, commands, base, out, bufferRegister);
        }

    // --- private member variables ------------------------------------------------------------------------------------
        std::vector<InstanceValue> m_InstanceVec;
        std::vector<DrawElementsIndirectCommand> m_Commands;
        std::vector<DrawElementsIndirectCommand> m_ChunkCommands;       // scratch, reused for every chunk
        std::vector<uint32_t> m_GroupStartVec;                          // first instance index of every group
        ShaderDrawResource m_Template;
        GeometryControl m_GeometryControl;
        MaterielControl m_MaterielControl;
        bool m_GroupPending;
    };

}
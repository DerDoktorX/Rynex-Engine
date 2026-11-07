#pragma once
// DefaultCommand created in Project Rynex-Rendering on 16/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/DrawList/Contoller/CommandBuildBase.h>
#include <Rynex/Renderer/DrawList/Contoller/InterfaceBase.h>
#include <Rynex/Renderer/DrawList/Contoller/VectorInterface.h>


namespace Rynex::Control {

    template<typename Interface, typename Source, typename CommandBuild>
    class CommandControlImpl
    {
    public:
        using Section = typename Interface::Section;

    };


    namespace CommandDefault {

        struct SourceType
        {
            SourceType()
                : m_Shader(nullptr)
                , m_Camera(nullptr)
            {
            }

            SourceType(const SourceType& source) = default;
            SourceType(SourceType&& source) = default;
            SourceType(const Ref<UniformBuffer>& camera, const Ref<Shader>& shader)
                : m_Shader(shader)
                , m_Camera(camera)
            {}

            Ref<Shader> m_Shader;
            Ref<UniformBuffer> m_Camera;
            SourceType operator=(const SourceType& source) = default;

            bool operator==(const SourceType& right) const
            {
                return m_Shader == right.m_Shader && m_Camera == right.m_Camera;
            }
        };
#if 0
        template<typename Source>
        class Interface
        {
        public:
            using Section = DrawElementsIndirectCommand;
            using Indices = NoIndices;

            constexpr static bool s_MultyCommand = false;

            Interface()
                : m_SectionPtr(nullptr)
                , m_Source()
            {}

            using Unit = uint32_t;


            void SetSection(Section* sectionPtr) { m_SectionPtr = sectionPtr;}
            Section* GetSection() const { return m_SectionPtr; }

            /**
             * Clear don't mean clear the stored address, but the date stored on this address.
             */
            void ClearSection()
            {
                RY_CORE_ASSERT(nullptr != m_SectionPtr, "SectionPtr is nullptr!");

                m_SectionPtr->m_FirstIndex = 0u;
                m_SectionPtr->m_BaseVertex = 0;
                m_SectionPtr->m_BaseInstance = 0u;
                m_SectionPtr->m_IndexCount = 0u;
                m_SectionPtr->m_InstancesCount = 0u;
            }
            Indices GetInstance() { return Indices{}; }

            DrawSplit HasStorage(const Source& source)
            {
                if (IsSourceEqual(m_Source))
                    return DrawSplit::NewDraw;

                if (!IsSourceEqual(source, m_Source))
                {
                    RY_CORE_FATAL("No Source change allowed once Set");
                    return DrawSplit::NewDraw;
                }
                return DrawSplit::Same;
            }


            void Set(const Source& source)
            {
                m_Source = source;
            }

            /**
             * @param info holds the count off vertex and indices for Command.
             * @param instanceCount how many instances this draw command has.
             * @param materielCount how many materiel in this command are.
             * @return @{false} is for we are able to store more sections. @{true} we need to push to a next section (maby max count off commands are set).
             */
            bool SectionFlush(const GeometryCommandInfo& info, const Unit instanceCount, const Unit materielCount)
            {
                RY_CORE_ASSERT(nullptr != m_SectionPtr, "SectionPtr is nullptr!");

                m_SectionPtr->m_FirstIndex = 0u;
                m_SectionPtr->m_IndexCount = info.m_IndexCount;
                m_SectionPtr->m_BaseVertex = 0;
                m_SectionPtr->m_BaseInstance = 0u;
                m_SectionPtr->m_IndexCount = instanceCount;
                return true;
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

            static bool IsSourceEmpty(const Source& source);

            const Source& GetSource() const { return m_Source; }
        private:
            Section* m_SectionPtr;
            Source m_Source;
        };

        template<typename Source>
        bool Interface<Source>::IsSourceEmpty(const Source& source)
        {
            static_assert(false, "no default Type defetion!");
            return false;
        }

        template<>
        inline bool Interface<Source>::IsSourceEmpty(const Source& source)
        {
            return nullptr == source.m_Shader && nullptr == source.m_Camera;
        }
#else




        class Interface : public InterfaceBase<DrawElementsIndirectCommand>
        {
        public:
            using Source = GeometryCommandInfo;

            SectionFill AddSection(const Source& source)
            {
                Section& section = Current();

                section.m_IndexCount = source.m_IndexCount;
                section.m_BaseVertex = 0;
                section.m_BaseInstance = 0u;
                section.m_FirstIndex = 0u;
                section.m_InstancesCount = source.m_InstanceCount;
                return SectionFill::Full;
            }


            NoIndices GetIndices() const
            {
                return NoIndices{};
            }

            DrawSplit HasSection(const SourceType& source) const
            {
                if(nullptr == m_GlobalCommand.m_Shader && nullptr == m_GlobalCommand.m_Camera)
                    return DrawSplit::Same;

                if(source.m_Shader == m_GlobalCommand.m_Shader && source.m_Camera == m_GlobalCommand.m_Camera)
                    return DrawSplit::Same;

                return DrawSplit::NewDraw;
            }

            DrawSplit SetSection(const SourceType& source)
            {
                const DrawSplit split = HasSection(source);
                m_GlobalCommand = source;

                return split;
            }

            template<size_t Index>
            Ref<Shader> GetShader() const
            {
                static_assert(0u == Index, "Index out of range! -> ( 0 )");
                return m_GlobalCommand.m_Shader;
            }

            template<size_t Index>
            Ref<UniformBuffer> GetUniformBuffer() const
            {
                static_assert(0u == Index, "Index out of range! -> ( 0 )");
                return m_GlobalCommand.m_Camera;
            }

            const DrawElementsIndirectCommand& GetCommand() const
            {
                return this->Current();
            }
        private:
            SourceType m_GlobalCommand;
        };

#endif

        using Build = DeclaredBuild<
            BindingSingleCommand
            , BindingUniform<0, CacheBufferMode::None, 0>
            , BindingShader<0u>
        >;


        using DefaultCommand = ControlDescriptor<Interface, Build>;


    }






}


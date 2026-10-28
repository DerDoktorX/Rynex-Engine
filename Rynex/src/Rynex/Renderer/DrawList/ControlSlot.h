#pragma once
// SectionList created in Project Rynex-Rendering on 11/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/ChachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>


namespace Rynex {
    template<typename Control>
    class ControlSlot
    {
    public:
        using Source = typename Control::Source;        // a memory block that stores input data.
        using Section = typename Control::Section;      // a memory block for on draw call.
        using Interface = typename Control::Interface;  // a interface for store current section ptr and has alle methods for interact withe per Section data. (Set/Add, Capacity, HasStorage, SetSection, GetSection, IsSectionEmpty, GetStorage)
                                                        // Should have static methods that can check that Source is valid or make it empty. (IsSourceEmpty, {ClearSource})
        using Build = typename Control::CommandBuild;   // a interface for methods build render command. It stores the reference from the Chunk Object and from CachedBufferRegister (CollectVertexBuffer, CommandUpload)

        using Container = std::vector<Section>;
        using Unit = uint32_t;
    // public member methode ---------------------------------------------------------------------------------------------------
        ControlSlot()
            : m_SectionVec(7)
            , m_Interface()
            , m_Cached(Source{})
            , m_Used(0)
        {
            Section* sectionPtr = m_SectionVec.data();
            m_Interface.SetSection(sectionPtr);
        }
        
        ~ControlSlot()
        {
        }

        /**
         * for hot path designed No Checks of empty Source Data.
         * @param source date parameter
         * @return element is stored but last element. (expect before next AddOnInterface action to open next section)
         */
        bool AddOnInterface(const Source& source)
        {
            return m_Interface.Add(source);
        }

        // set the source data on Controller/Interface
        /**
         * that methode cached the new source data, after that the Interface should be reporting if the in the current section we can store it.
         * @param source date parameter that is
         * @return reports whether a new section needs to be created, whether only a cache upload is required, or whether neither of these actions is necessary.
         */
        DrawSplit SetOnInterface(const Source& source)
        {
            RY_CORE_ASSERT(!Interface::IsEmpty(m_Cached), "Cache is Empty!");
            m_Cached = source;
            return m_Interface.HasStorage(m_Cached);
        }

        /**
         * uploade the last submitted source data throw the Interface. Usefully if there is just overrides only the last set cache will be submitted or if there is a new section we have the last needed source data.
         */
        void UploadCacheOnInterface()
        {
            RY_CORE_ASSERT(!Interface::IsEmpty(m_Cached), "Cache is Empty!");
            m_Interface.Set(m_Cached);
        }

        /**
         * open section, select the next element in the section list or allocate a new section range and set the current section element in the Interface.
         */
        void OpenNextSection()
        {
            const Unit allocated = static_cast<Unit>(m_SectionVec.size());
#if 0
            if (m_Used == allocated)
            {
                m_SectionVec.emplace_back();
                const Unit count = static_cast<Unit>(m_SectionVec.size());
                RY_CORE_ASSERT(m_Used < allocated, "Section index out of range!");
            }
#else
            if (allocated == m_Used)
                m_SectionVec.emplace_back();

            RY_CORE_ASSERT(m_Used < m_SectionVec.size(), "Section index out of range!");
#endif

            Section& section = m_SectionVec[m_Used];
            m_Used++;
            Section& sectionPtr = &section;
            m_Interface.SetSection(sectionPtr);
        }

        /**
         * how many ellements the in the current Section are.
         * @return element count, from Interface.
         */
        uint32_t GetSectionCount()
        {
            return m_Interface.GetStoredCount();
        }

        /**
         * get the Section on the last spot, maby not the same as Interface has.
         * @return Reference to the current Section. For direct access.
         */
        Section& Current()
        {
            RY_CORE_ASSERT(0u < m_Used, "No open section!");
            const Unit last = m_Used - 1u;
            return m_SectionVec[last];
        }

        /**
         * the interface is forced to use index of the Section. (expected use is inside)
         * @param index which Section should be used in Interface
         */
        void SetCurrentInterface(const Unit index)
        {
            Section& section = Get(index);
            Section* sectionPtr = std::addressof(section);
            m_Interface.SetSection(sectionPtr);
        }

        /**
         * the interface is forced to use index of the Section. (expected use is inside)
         * so the change is kind a permanently. Until some changes again.
         * @param index
         * @return 
         */
        Interface& GetInterface(const Unit index)
        {
            SetCurrentInterface(index);
            return m_Interface;
        }

        Interface& CurrentInterface()
        {
            return m_Interface;
        }

        Section& Get(const Unit index)
        {
            RY_CORE_ASSERT(index < m_Used, "Section index out of range!");
            return m_SectionVec[index];
        }

        bool IsEmpty() const
        {
            return 0u == m_Used;
        }

        void Clear()
        {
            m_Used = 0u;
        }

        Unit Count() const
        {
            return m_Used;
        }

        Build CreateBuild(ShaderDrawResource& draw, CachedBufferRegister& cache)
        {
            Section* sectionPtr = m_Interface.GetSection();
            return Build{ cache, *sectionPtr, draw };
        }

        static void CollectVertexBuffer([[maybe_unused]] Build& build, [[maybe_unused]] Span<Ref<VertexBuffer>>& vertexBufferSpan)
        {
            if constexpr (Build::s_HasVertexBuffer)
                build.CollectVertexBuffer(vertexBufferSpan);
        }

        /**
         * execute Bind action withe current Build alias if there are bind action available
         * @param build
         */
        static void BindCommand([[maybe_unused]] Build& build)
        {
            if constexpr (Build::s_HasBindBuffers)
                build.BindCommand();
        }
    private:
        Container m_SectionVec;
        Interface m_Interface;
        Source m_Cached;
        Unit m_Used;
    };


}

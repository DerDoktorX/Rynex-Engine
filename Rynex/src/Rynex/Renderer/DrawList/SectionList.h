#pragma once
// SectionList created in Project Rynex-Rendering on 11/10/2026.
#include <rypch.h>



namespace Rynex {
    template<typename Section>
    class SectionList 
    {
    public:
        using Value = Section;
        using Container = std::vector<Value>;
        using Unit = uint32_t;
    // public member methode ---------------------------------------------------------------------------------------------------
        SectionList()
            : m_SectionVec()
            , m_Used(0)
        {
        }
        
        ~SectionList()
        {
        }

        Section& Open()
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
            if (m_Used == allocated)
                m_SectionVec.emplace_back();

            RY_CORE_ASSERT(m_Used < m_SectionVec.size(), "Section index out of range!");
#endif

            Section& section = m_SectionVec[m_Used];
            m_Used++;
            return section;
        }

        Section& Current()
        {
            RY_CORE_ASSERT(0u < m_Used, "No open section!");
            const Unit last = m_Used - 1u;
            return m_SectionVec[last];
        }

        Section& Get(const Unit index)
        {
            RY_CORE_ASSERT(index < m_Used, "Section index out of range!");
            return m_SectionVec[index];
        }

        const Section& Get(const Unit index) const
        {
            RY_CORE_ASSERT(index < m_Used, "Section index out of range!");
            return m_SectionVec[index];
        }

        void Add(const Value& value)
        {
            m_SectionVec.emplace_back(value);
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
    private:
        Container m_SectionVec;
        Unit m_Used;
    };


}
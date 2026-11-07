#pragma once
// InterfaceBase created in Project Rynex-Rendering on 01/11/2026.
#include <rypch.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>


namespace Rynex {


    template<typename SectionType>
    class InterfaceBase
    {
    public:
        using Unit = uint32_t;
        using Section = SectionType;
    // public member operator --------------------------------------------------------------------------------------------------
        InterfaceBase()
            : m_SectionPtr(nullptr)
        {
        }

        // methods to control sectionPtr
        void BindSection(Section* sectionPtr)
        {
            m_SectionPtr = sectionPtr;
        }

        Section* GetSection() const
        {
            return m_SectionPtr;
        }


    protected:
        Section& Current()
        {
            RY_CORE_ASSERT(nullptr != m_SectionPtr, "Section is nullptr");
            return *m_SectionPtr;
        }

        const Section& Current() const
        {
            RY_CORE_ASSERT(nullptr != m_SectionPtr, "Section is nullptr");
            return *m_SectionPtr;
        }
    private:
        Section* m_SectionPtr;
    };

    template<typename RefSection>
    class RefInterface : public InterfaceBase<Ref<RefSection>>
    {
    public:
        using Handle = Ref<RefSection>;


    // public member operator --------------------------------------------------------------------------------------------------
        DrawSplit HasSection(const Handle& source) const
        {
            const Handle* sectionPtr = this->m_SectionPtr;
            if (nullptr == sectionPtr)
                return DrawSplit::NewDraw;

            const Handle& current = *sectionPtr;
            if (current == source)
                return DrawSplit::Same;

            return DrawSplit::NewDraw;
        }

        void SetSection(const Handle& source)
        {
            Handle& current = this->Current();
            current = source;
        }

        void ClearSection()
        {
            Handle& current = this->Current();
            RY_DESTROY_REF(current);
        }
    protected:
        Handle& GetResource()
        {
            Handle& handle = this->Current();
            RY_CORE_ASSERT(nullptr != handle, "the stored resource is nullptr!");
            return *handle;
        }

    };

}

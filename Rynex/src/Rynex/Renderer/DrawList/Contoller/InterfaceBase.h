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
        using Source = Ref<RefSection>;


    // public member operator --------------------------------------------------------------------------------------------------
        DrawSplit HasSection(const Source& source) const
        {
            const Source* sectionPtr = this->m_SectionPtr;
            if (nullptr == sectionPtr)
                return DrawSplit::NewDraw;

            const Source& current = *sectionPtr;
            if (current == source)
                return DrawSplit::Same;

            return DrawSplit::NewDraw;
        }

        void SetSection(const Source& source)
        {
            Source& current = this->Current();
            current = source;
        }

        void ClearSection()
        {
            Source& current = this->Current();
            RY_DESTROY_REF(current);
        }

        static bool IsSourceEmpty(const Source& source)
        {
            return nullptr == source;
        }

        static bool IsSourceEqual(const Source& left, const Source& right)
        {
            return left == right;
        }
    protected:
        Source& GetResource()
        {
            Source& handle = this->Current();
            RY_CORE_ASSERT(nullptr != handle, "the stored resource is nullptr!");
            return handle;
        }

        const Source& GetResource() const
        {
            const Source& handle = this->Current();
            RY_CORE_ASSERT(nullptr != handle, "the stored resource is nullptr!");
            return handle;
        }

        RefSection& Get()
        {
            Source& handle = GetResource();
            return *(handle.get());
        }

        const Source& Get() const
        {
            const Source& handle = GetResource();
            return *(handle.get());
        }
    };

}

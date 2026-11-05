#pragma once
#include <rypch.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/DrawList/Contoller/InterfaceBase.h>

namespace Rynex {
    enum class SectionFill : uint8_t
    {
        HasRoom = 0u,   // element stored, next Add() fits into the same section
        Full            // element stored, the NEXT Add() needs a new section
    };



#if 1
    template<typename Policy>
    class VectorSection
    {
    public:
        using Source = typename Policy::Source;
        using Element = typename Policy::Element;

        static_assert(std::is_trivially_copyable_v<Element>, "records are uploaded as raw bytes, they must be trivially copyable");

        // --- public member methods ---------------------------------------------------------------------------------------
        SectionFill Add(const Source& source)
        {
            const size_t countBefore = m_Elements.size();
            RY_CORE_ASSERT(countBefore < Policy::s_MaxElements, "section is full, open a new section first!");

            m_Elements.emplace_back(Policy::Make(source));

            const size_t countAfter = m_Elements.size();
            if (Policy::s_MaxElements <= countAfter)
                return SectionFill::Full;

            return SectionFill::HasRoom;
        }

        // clear() keeps the capacity, so a reused section does not allocate again.
        void Clear()
        {
            m_Elements.clear();
        }


        uint32_t Count() const
        {
            return static_cast<uint32_t>(m_Elements.size());
        }

        template<size_t Index>
        Span<const Byte> GetBytes() const
        {
            static_assert(0ull == Index, "index out of range! -> ( 0 )");
            return AsBytes(m_Elements);
        }
    private:
        std::vector<Element> m_Elements;
    };
#else
    template<>
    class VectorSection<>
    {
    public:
        VectorSection() = default;


    };

    template<typename Source, typename... Policy>
    class VectorSection
    {
    public:
        // --- public member methods ---------------------------------------------------------------------------------------
        SectionFill Add(const Source& source)
        {
            const size_t countBefore = m_Elements.size();
            RY_CORE_ASSERT(countBefore < Policy::s_MaxElements, "section is full, open a new section first!");

            m_Elements.emplace_back(Policy::Make(source));

            const size_t countAfter = m_Elements.size();

            if (ThisPolicy::s_MaxElements <= countAfter)
                return SectionFill::Full;
            return SectionFill::HasRoom;
        }

        // clear() keeps the capacity, so a reused section does not allocate again.
        void Clear()
        {
            m_Elements.clear();
        }


        uint32_t Count() const
        {
            return static_cast<uint32_t>(m_Elements.size());
        }

        template<size_t Index>
        Span<const Byte> GetBytes() const
        {
            static_assert(0ull == Index, "index out of range! -> ( 0 )");
            return AsBytes(m_Elements);
        }
    private:
        std::tuple<std::vector<Element>> m_Elements;
    };
#endif

    template<typename Policy>
    class VectorInterface : public InterfaceBase<VectorSection<Policy>, typename Policy::Source, NoIndices>
    {
    public:
        using Source = typename Policy::Source;

        // --- public member methods ---------------------------------------------------------------------------------------
        // Elements never split a draw by themselves, the capacity does (SectionFill::Full).
        DrawSplit HasSection(const Source&) const
        {
            return DrawSplit::Same;
        }

        SectionFill AddSection(const Source& source)
        {
            return this->Current().Add(source);
        }

        void ClearSection()
        {
            this->Current().Clear();
        }

        template<size_t Index>
        Span<const Byte> GetBufferData() const
        {
            static_assert(0 == Index, "out of range! -> ( 0 )");
            return this->Current().GetBytes();
        }

        template<size_t Index>
        BufferLayout GetBufferLayout(const uint32_t index) const
        {
            static_assert(0 == Index, "out of range! -> ( 0 )");
            return Policy::GetLayout();
        }


    };
}

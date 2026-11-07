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



#if 0
    template<typename PolicyType>
    class VectorSection
    {
    public:
        using Policy = PolicyType;
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

        template<size_t Index>
        static TextureSpecification GetSpecification()
        {
            static_assert(0ull == Index, "index out of range! -> ( 0 )");
            return Policy::GetSpecification();
        }

        template<size_t Index>
        static BufferLayout GetLayout()
        {
            static_assert(0ull == Index, "index out of range! -> ( 0 )");
            return Policy::GetLayout();
        }
    private:
        std::vector<Element> m_Elements;
    };

    template<typename Policy>
    class VectorInterface : public InterfaceBase<VectorSection<Policy>>
    {
    public:
        using Source = typename Policy::Source;
#else


    template <typename T, typename... Ts>
    constexpr size_t index_of()
    {
        constexpr bool matches[] = { std::is_same_v<T, Ts>... };

        for (size_t i = 0; i < sizeof...(Ts); ++i)
        {
            if (matches[i])
            {
                return i;
            }
        }
        return sizeof...(Ts);
    }


    template<typename SourceType, typename ...Policy>
    class VectorInterface;

    template<typename Source, typename... Policies>
    class VectorSection
    {
        static_assert(sizeof...(Policies) > 0, "VectorSection need at max one Policy!");


    public:

        template <typename Policy>
        using Element_T = typename Policy::Element;

        template <std::size_t I>
        using PolicyAt = std::tuple_element_t<I, std::tuple<Policies...>>;


        // --- public member methods ---------------------------------------------------------------------------------------
        SectionFill Add(const Source& source)
        {
            SectionFill fill = SectionFill::HasRoom;
            ([&]()
            {
                using Element = Element_T<Policies>;
                std::vector<Element>& vector = std::get<index_of<Element>>(m_Elements);

                const size_t countBefore = vector.size();
                RY_CORE_ASSERT(countBefore < Policies::s_MaxElements, "section is full, open a new section first!");

                Element element = Policies::Make(source);
                vector.emplace_back(element);

                const size_t countAfter = vector.size();

                if (Policies::s_MaxElements <= countAfter)
                    fill = SectionFill::Full;
            }(), ...);
            return fill;
        }

        // clear() keeps the capacity, so a reused section does not allocate again.
        void Clear()
        {
            std::apply(
                [](auto&... vector)
                {
                    vector.clear();
                }, m_Elements);
        }


        uint32_t Count() const
        {
            uint32_t count = 0u;
            std::apply(
               [&count](const auto&... vector)
               {
                   const uint32_t size = vector.size();
                   RY_CORE_ASSERT(size == count || 0u == count, "not syc size!");
                   count = size;
               }, m_Elements);

            return count;
        }

        template<size_t Index>
        Span<const Byte> GetBytes() const
        {
            const auto& vector = std::get<Index>(m_Elements);
            return AsBytes(vector);
        }

        template<size_t Index>
        TextureSpecification GetSpecification()
        {
            using Policy = PolicyAt<Index>;
            return Policy::GetSpecification();
        }

        template<size_t Index>
        BufferLayout GetBufferLayout()
        {
            using Policy = PolicyAt<Index>;
            return Policy::GetBufferLayout();
        }
    protected:
        template<typename T>
        std::vector<T>& GetBuffer()
        {
            return std::get<index_of<T>>(m_Elements);
        }

        template<typename T>
        const std::vector<T>& GetBuffer() const
        {
            return std::get<index_of<T>>(m_Elements);
        }

        template<typename T>
        T& GetBufferEnd()
        {
            std::vector<T>& vec = GetBuffer<T>();
            return vec.back();
        }

        template<typename T>
        const T& GetBufferEnd() const
        {
            const std::vector<T>& vec = GetBuffer<T>();
            return vec.back();
        }
    private:
        std::tuple<std::vector<typename Policies::Element>...> m_Elements;


        friend VectorInterface<Source, Policies...>;
    };


    template<typename SourceType, typename ...Policy>
    class VectorInterface : public InterfaceBase<VectorSection<Policy...>>
    {
    public:

#endif
        // --- public member methods ---------------------------------------------------------------------------------------
        // Elements never split a draw by themselves, the capacity does (SectionFill::Full).

        SectionFill AddSection(const SourceType& source)
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
            using U = typename VectorInterface::Section;
            U& section = this->Current();
            const Span<const Byte> spanByteData = section.template GetBytes<Index>();
            return spanByteData;
        }

        template<size_t Index>
        static BufferLayout GetBufferLayout()
        {
            return VectorInterface::GetBufferLayout<Index>();
        }

        template<size_t Index>
        static TextureSpecification GetSpecification()
        {
            return VectorInterface::GetSpecification<Index>();
        }

    protected:

        template<typename T>
        std::vector<T>& GetBuffer()
        {
            using U = VectorSection<Policy...>;
            U& section = this->Current();

            return section.template GetBuffer<T>();
        }

        template<typename T>
        const std::vector<T>& GetBuffer() const
        {
            using U = VectorSection<Policy...>;
            U& section = this->Current();

            return section.template GetBuffer<T>();
        }

        template<typename T>
        T& GetBufferEnd()
        {
            using U = VectorSection<Policy...>;
            U& section = this->Current();

            return section.template GetBufferEnd<T>();
        }

        template<typename T>
        const T& GetBufferEnd() const
        {
            using U = VectorSection<Policy...>;
            U& section = this->Current();

            return section.template GetBufferEnd<T>();
        }
    };
}

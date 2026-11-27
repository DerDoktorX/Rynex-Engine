#pragma once
#include <rypch.h>
#include <Rynex/Renderer/DrawList/DrawListTypes.h>
#include <Rynex/Renderer/DrawList/Contoller/InterfaceBase.h>
#include <Rynex/Renderer/DrawList/Contoller/Instance/InstanceVertex.h>


namespace Rynex {




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
                return i;
        }
        return sizeof...(Ts);
    }


    template<typename SourceType, size_t MaxElementCount, typename ...Policy>
    class VectorInterface;

    template<typename Source, typename... Policies>
    class VectorSection
    {
    public:


        template <std::size_t I>
        using PolicyAt = std::tuple_element_t<I, std::tuple<Policies...>>;



        // --- public member methods ---------------------------------------------------------------------------------------
        void Add(const Source& source)
        {
            AddElement<0>(source, m_Elements);
#if 0
            ([&]()
            {
                using Element = Element_T<Policies>;
                std::vector<Element>& vector = std::get<index_of<Element>>(m_Elements);

                const size_t countBefore = vector.size();


                Element element = Policies::Make(source);
                vector.emplace_back(element);

                const size_t countAfter = vector.size();

                if (Policies::s_MaxElements <= countAfter)
                    fill = SectionFill::Full;
            }(), ...);
            return fill;
#endif


        }

        // clear() keeps the capacity, so a reused section does not allocate again.
        void Clear()
        {
#if 1

#else
            for (auto& vec : m_Elements)
            {
                vec.clear();
            }
#endif
        }



        uint32_t Count() const
        {
            uint32_t count = 0u;
#if 1
            std::apply( [&count](auto&&... vector)
            {
                ((  count = vector.size() ), ...);
            }, m_Elements);
#else
            ([&]()
            {
                using ElementVector = Element_T<Policies>;
                const ElementVector& vector = std::get<index_of<ElementVector>>(m_Elements);
                const uint32_t size = vector.size();
                RY_CORE_ASSERT(size == count || 0u == count, "not syc size!");
                count = size;
            }(), ...);
#endif
            return count;
        }

        template<size_t Index>
        Span<const Byte> GetBytes() const
        {
            const auto& vector = std::get<Index>(m_Elements);
            return AsBytes(vector);
        }

        template<size_t Index>
        TextureSpecification GetTextureSpecification()
        {
            using Policy = PolicyAt<Index>;
            return Policy::GetTextureSpecification();
        }

        template<size_t Index>
        BufferLayout GetBufferLayout()
        {
            using Policy = PolicyAt<Index>;
            return Policy::GetBufferLayout();
        }


        static bool IsSourceEqual(const Source& left, const Source& right)
        {
            return left == right;
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
        template<size_t Index = 0, typename... Ts>
        typename std::enable_if<Index == sizeof...(Ts), void>::type AddElement(const Source& source, std::tuple<std::vector<Ts>...>& tuple)
        {
            return;
        }

        template<size_t Index = 0, typename... Ts>
        typename std::enable_if<Index < sizeof...(Ts), void>::type AddElement(const Source& source, std::tuple<std::vector<Ts>...>& tuple)
        {
            using Policy = PolicyAt<Index>;
            using Element = typename Policy::Element;
            std::vector<Element>& vector = std::get<Index>(tuple);
            Element element = Policy::Make(source);
            vector.emplace_back(element);

            AddElement<Index + 1>(source, tuple);
            return;
        }

        std::tuple<std::vector<typename Policies::Element>...> m_Elements;

    };


    template<typename SourceType, size_t MaxElementCount, typename ...Policy>
    class VectorInterface : public InterfaceBase<VectorSection<SourceType, Policy...>>
    {
    public:
        static constexpr size_t s_MaxElements = MaxElementCount;
        using Source = SourceType;
#endif
        // --- public member methods ---------------------------------------------------------------------------------------
        // Elements never split a draw by themselves, the capacity does (SectionFill::Full).

        SectionFill AddStorage(const SourceType& source)
        {
            VectorSection<Source,Policy...>& sectionVector = this->Current();
            RY_CORE_ASSERT(sectionVector.Count() < s_MaxElements, "section is full, open a new section first!");
            sectionVector.Add(source);
            if (sectionVector.Count() < s_MaxElements)
                return SectionFill::HasRoom;
            return SectionFill::Full;
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

        bool IsSectionStorageEmpty() const
        {
            return 0u == GetSectionStorageCount();
        }

        uint32_t GetSectionStorageCount() const
        {
            const VectorSection<Source,Policy...>& sectionVector = this->Current();
            return sectionVector.Count();
        }

        DrawSplit HasStorage(const Source& source) const
        {
            if(GetSectionStorageCount() < s_MaxElements)
                return DrawSplit::Same;
            return DrawSplit::NewDraw;
        }



        static bool IsSourceEmpty(const Source& source)
        {
            return !source;
        }

        static bool IsSourceEqual(const Source& sourceA, const Source& sourceB)
        {
            return sourceA == sourceB;
        }

    protected:
        template<typename T>
        std::vector<T>& GetBuffer()
        {
            using U = VectorSection<Source, Policy...>;
            U& section = this->Current();
            return section.template GetBuffer<T>();
        }

        template<typename T>
        const std::vector<T>& GetBuffer() const
        {
            using U = VectorSection<Source, Policy...>;
            U& section = this->Current();
            return section.template GetBuffer<T>();
        }

        template<typename T>
        T& GetBufferEnd()
        {
            using U = VectorSection<Source, Policy...>;
            U& section = this->Current();
            return section.template GetBufferEnd<T>();
        }

        template<typename T>
        const T& GetBufferEnd() const
        {
            using U = VectorSection<Source, Policy...>;
            U& section = this->Current();
            return section.template GetBufferEnd<T>();
        }
    };
}

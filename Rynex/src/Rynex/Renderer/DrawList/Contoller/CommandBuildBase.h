#pragma once
// CommandBuildBase created in Project Rynex-Rendering on 01/11/2026.
#include <rypch.h>
#include <Rynex/Renderer/CachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>


namespace Rynex {
    class CachedBufferRegister;


    struct BuildContext
    {
        CachedBufferRegister& m_Cache;
        ShaderDrawResource& m_Draw;
    };
    enum class BindingKind : uint8_t
    {
        Vertex = 0u,
        Index,
        Texture,
        Uniform,
        Storage,
        Other       // no slot, for example the shader program
    };

    enum class CacheBufferMode : uint8_t
    {
        None = 0u,
        UniqueCachedHash, // ask the interface for hash value. Create his own if noe available.
        UniqueNoCache,
        AcquireBuffer,
    };

    namespace Utils {

        template<typename Buffer, size_t Index, bool StoreHash, typename Interface>
        static Ref<Buffer> CreateBufferUnique(const Span<const Byte>& dataBuffer, const Interface& interface, CachedBufferRegister& cache)
        {
            BufferLayout layout = interface.GetBufferLayout(Index);
            auto [hash, buffer] = cache.AcquireUniqueBuffer<Ref<Buffer>>(dataBuffer);
            if constexpr (StoreHash)
                interface.SetHash<Index>(hash);
            return buffer;
        }

        template<typename Buffer, CacheBufferMode Mode, size_t Index, typename Interface>
        static Ref<Buffer> AcquireBufferSource(Interface& interface, BuildContext& context)
        {
            if constexpr(CacheBufferMode::None == Mode)
            {
                return interface.GetBuffer<Ref<Buffer>, Index>();
            }

            if constexpr (CacheBufferMode::UniqueCachedHash == Mode)
            {
                Hash64 hash = interface.GetHash<Index>();
                if (Hash64::Zero() != hash)
                {
                    Ref<Buffer> buffer = context.m_Cache.AcquireUniqueBufferHash<Ref<Buffer>>(hash);
                    if (nullptr != buffer)
                        return buffer;
                }
                Span<const Byte> dataBuffer = interface.GetBufferData<Index>();
                return CreateBufferUnique<Buffer, Index, true>(dataBuffer, interface, context, context.m_Cache);
            }

            Span<const Byte> dataBuffer = interface.GetBufferData(Index);
            if constexpr (CacheBufferMode::AcquireBuffer == Mode)
            {
                return context.m_Cache.AcquireBuffer<Ref<Buffer>>(dataBuffer);
            }
            if constexpr (CacheBufferMode::UniqueNoCache == Mode)
            {
                return CreateBufferUnique<Buffer, Index, false>(dataBuffer, interface, context.m_Cache);
            }

            return Ref<Buffer>{ nullptr };
        }
    }

#if 0
    template<size_t Slot, size_t Index = 0u, CacheBufferMode Mode = CacheBufferMode::AcquireBuffer>
    struct BindingVertex
    {
        static constexpr BindingKind s_Kind = BindingKind::Vertex;
        static constexpr uint64_t s_Bit = 1ull << Index;

        using Element = VertexArray::VertexElements;
        template<typename Interface, typename ElementSpan>
        static void Collect(const Interface& interface, BuildContext& context, ElementSpan& elementSpan)
        {
            using Element = typename ElementSpan::value_type;
            Ref<VertexBuffer> buffer = Utils::AcquireBufferSource<VertexBuffer, Mode, Index, Interface>(interface, context);
            elementSpan[Slot] = Element{ buffer };
        }
    };

    template<size_t Slot, size_t Index = 0u, CacheBufferMode Mode = CacheBufferMode::AcquireBuffer>
    struct BindingTexture
    {
        static constexpr BindingKind s_Kind = BindingKind::Texture;
        static constexpr uint64_t s_Bit = 1ull << Index;

        using Element = Ref<Texture>;
        template<typename Interface, typename ElementSpan>
        static void Apply(const Interface& source, BuildContext& context, Span<Element>& elementSpan)
        {
            Element texture = Utils::AcquireBufferSource<Texture, Mode, Index, Interface>(interface, context);
            elementSpan[Slot] = texture;
        }
    };

    template<size_t Slot, size_t Index = 0u, CacheBufferMode Mode = CacheBufferMode::AcquireBuffer>
    struct BindingUniform
    {
        static constexpr BindingKind s_Kind = BindingKind::Uniform;
        static constexpr uint64_t s_Bit = 1ull << Index;

        using Element = Ref<UniformBuffer>;
        template<typename Interface, typename ElementSpan>
        static void Apply(const Interface& source, BuildContext& context, Span<Element>& elementSpan)
        {
            Ref<UniformBuffer> buffer = Utils::AcquireBufferSource<UniformBuffer, Mode, Index, Interface>(interface, context);
            elementSpan[Slot] = buffer;
        }
    };

    template<size_t Slot, size_t Index = 0u, CacheBufferMode Mode = CacheBufferMode::AcquireBuffer>
    struct BindingStorage
    {
        static constexpr BindingKind s_Kind = BindingKind::Storage;
        static constexpr uint64_t s_Bit = 1ull << Index;

        using Element = Ref<StorageBuffer>;
        template<typename Interface, typename ElementSpan>
        static void Apply(const Interface& source, BuildContext& context, Span<Element>& elementSpan)
        {
            Ref<StorageBuffer> buffer = Utils::AcquireBufferSource<UniformBuffer, Mode, Index, Interface>(interface, context);
            elementSpan[Slot] = buffer;
        }
    };

    template<size_t Slot, size_t Index = 0u, CacheBufferMode Mode = CacheBufferMode::AcquireBuffer>
    struct BindingIndex
    {
        static constexpr BindingKind s_Kind = BindingKind::Index;
        static constexpr uint64_t s_Bit = 1ull << Index;

        using Element = Ref<IndexBuffer>;
        template<typename Interface, typename ElementSpan>
        static void Apply(const Interface& source, BuildContext& context, Span<Element>& elementSpan)
        {
            Ref<IndexBuffer> buffer = Utils::AcquireBufferSource<IndexBuffer, Mode, Index, Interface>(interface, context);
            elementSpan[Slot] = buffer;
        }
    };
#else
    template<size_t Slot, size_t Index, CacheBufferMode Mode, typename  Buffer, BindingKind Kind>
    struct BindingGenericBufferCollect
    {
        static constexpr BindingKind s_Kind = Kind;
        static constexpr uint64_t s_Bit = 1ull << Index;

        using Element = Ref<Buffer>;
        template<typename Interface>
        static void Collect(const Interface& interface, BuildContext& context, Span<Element>& elementSpan)
        {
            Ref<Buffer> buffer = Utils::AcquireBufferSource<Buffer, Mode, Index, Interface>(interface, context);
            elementSpan[Slot] = buffer;
        }
    };

    template<size_t Slot, size_t Index, CacheBufferMode Mode, typename Buffer, BindingKind Kind>
    struct BindingGenericBufferApply
    {
        static constexpr BindingKind s_Kind = Kind;
        static constexpr uint64_t s_Bit = 1ull << Index;

        using Element = Ref<Buffer>;
        template<typename Interface>
        static void Apply(const Interface& interface, BuildContext& context, Span<Element>& elementSpan)
        {
            Ref<Buffer> buffer = Utils::AcquireBufferSource<Buffer, Mode, Index, Interface>(interface, context);
            elementSpan[Slot] = buffer;
        }
    };

    template<size_t Slot, CacheBufferMode Mode, size_t Index = 0ull>
    using BindingVertex = BindingGenericBufferCollect<Slot, Index, Mode, VertexBuffer, BindingKind::Vertex>;

    template<size_t Slot, CacheBufferMode Mode, size_t Index = 0ull>
    using BindingIndex = BindingGenericBufferApply<Slot, Index, Mode, IndexBuffer, BindingKind::Index>;

    template<size_t Slot, CacheBufferMode Mode, size_t Index = 0ull>
    using BindingUniform = BindingGenericBufferApply<Slot, Index, Mode, UniformBuffer, BindingKind::Uniform>;

    template<size_t Slot, CacheBufferMode Mode, size_t Index = 0ull>
    using BindingStorage = BindingGenericBufferApply<Slot, Index, Mode, StorageBuffer, BindingKind::Storage>;

    template<size_t Slot, CacheBufferMode Mode, size_t Index = 0ull>
    struct BindingTexture
    {
        static constexpr BindingKind s_Kind = BindingKind::Texture;
        static constexpr uint64_t s_Bit = 1ull << Index;

        using Element = Ref<Buffer>;

        template<typename Interface>
        static void Apply(const Interface& interface, BuildContext& context, Span<Element>& elementSpan)
        {
            Ref<Texture> texture = nullptr;
            if constexpr (CacheBufferMode::None == Mode)
            {
                texture = interface.GetTexture<Index>();
            }
            else if constexpr (CacheBufferMode::AcquireBuffer == Mode)
            {
                const TextureSpecification& spec = interface.GetSpecification<Index>();
                texture = context.m_Cache.AcquireTexture();
            }
            else
            {
                static_assert(false, "Unknown Cache Mode");
            }
            elementSpan[Slot] = texture;
        }

    };

    template<size_t Index = 0ull>
    struct BindingShader
    {
        static constexpr BindingKind s_Kind = BindingKind::Other;
        static constexpr uint64_t s_Bit = 1ull << Index;

        using Element = Ref<Shader>;

        template<typename Interface>
        static void Apply(const Interface& interface, BuildContext& context, Span<Element>& elementSpan)
        {
            Ref<Shader> shader = interface.GetShader<Index>();
            context.m_Draw.shaderProgram = shader;
        }
    };
#endif


    template<typename InterfaceType, typename... Bindings>
    class DeclaredBuild
    {
    public:
        using Interface = InterfaceType;
    // public member methods --------------------------------------------------------------------------------------------------
        DeclaredBuild() = delete;
        DeclaredBuild(const DeclaredBuild&) = delete;
        explicit DeclaredBuild(CachedBufferRegister& cache, Interface& interfaceType, ShaderDrawResource& drawResource)
            : m_Interface(interfaceType)
            , m_Context{ cache, drawResource }
        {}
        
        ~DeclaredBuild()
        {
        }


        void CollectVertexBuffer(Span<VertexArray::VertexElements>& elements)
        {
            (CollectOne<Bindings>(elements), ...);
        }

        void BindCommand()
        {
            (BindOne<Bindings>(), ...);
        }
    private:
        template<typename Binding, typename ElementSpan>
        void CollectOne(ElementSpan& elements)
        {
            if constexpr (BindingKind::Vertex == Binding::s_Kind)
                Binding::Collect(m_Interface, m_Context, elements);
        }

        template<typename Binding>
        void BindOne()
        {
            if constexpr (BindingKind::Vertex != Binding::s_Kind)
                Binding::Apply(m_Interface, m_Context);
        }
        // --- private member variables ------------------------------------------------------------------------------------
        Interface& m_Interface;
        BuildContext m_Context;
    };


}

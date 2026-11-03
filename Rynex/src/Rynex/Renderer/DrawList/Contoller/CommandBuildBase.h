#pragma once
// CommandBuildBase created in Project Rynex-Rendering on 01/11/2026.
#include <rypch.h>
#include <Rynex/Renderer/CachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>


namespace Rynex {


    struct BuildContext
    {
        CachedBufferRegister& m_Cache;
        ShaderDrawResource& m_Draw;
    };
    enum class BindingKind : uint8_t
    {
        Vertex = 0u,
        Texture,
        Uniform,
        Storage,
        Other       // no slot, for example the shader program
    };
    namespace Binding {

        template<size_t Slot, size_t Index = 0u>
        struct Vertex
        {
            static constexpr BindingKind s_Kind = BindingKind::Vertex;
            static constexpr uint64_t s_Bit = 1ull << Index;

            using Element = VertexArray::VertexElements;
            template<typename Interface, typename ElementSpan>
            static void Collect(const Interface& source, BuildContext& context, ElementSpan& elementSpan)
            {
                using Element = typename ElementSpan::value_type;
                Ref<VertexBuffer> buffer = source.GetVertexBuffer(Index);
                elementSpan[Slot] = Element{ buffer };
            }
        };

        template<size_t Slot, size_t Index = 0u>
        struct Texture
        {
            static constexpr BindingKind s_Kind = BindingKind::Texture;
            static constexpr uint64_t s_Bit = 1ull << Index;

            using Element = Ref<Rynex::Texture>;
            template<typename Interface, typename ElementSpan>
            static void Apply(const Interface& source, BuildContext& context, Span<Element>& elementSpan)
            {
                Element texture = source.GetTexture(Index);
                elementSpan[Slot] = texture;
            }
        };

        template<size_t Slot, size_t Index = 0u>
        struct Uniform
        {
            static constexpr BindingKind s_Kind = BindingKind::Uniform;
            static constexpr uint64_t s_Bit = 1ull << Index;

            using Element = Ref<UniformBuffer>;
            template<typename Interface, typename ElementSpan>
            static void Apply(const Interface& source, BuildContext& context, Span<Element>& elementSpan)
            {
                Element uniformBuffer = source.GetUniformBuffer(Index);
                elementSpan[Slot] = uniformBuffer;
            }
        };

        template<size_t Slot, size_t Index = 0u>
        struct Storage
        {
            static constexpr BindingKind s_Kind = BindingKind::Storage;
            static constexpr uint64_t s_Bit = 1ull << Index;

            using Element = Ref<StorageBuffer>;
            template<typename Interface, typename ElementSpan>
            static void Apply(const Interface& source, BuildContext& context, Span<Element>& elementSpan)
            {
                Element storageBuffer = source.GetStorageBuffer(Index);
                elementSpan[Slot] = storageBuffer;
            }
        };
    }

    namespace RegisterBinding {

        template<size_t Slot, size_t Index = 0u>
        struct Vertex
        {
            static constexpr BindingKind s_Kind = BindingKind::Vertex;
            static constexpr uint64_t s_Bit = 1ull << Index;

            using Element = VertexArray::VertexElements;

            template<typename Interface>
            static void Collect(const Interface& source, BuildContext& context, Span<Element>& vertexElementsSpan)
            {
                const Span<const Byte> data = source.GetBufferData(Index);
                const BufferLayout layout = source.GetBufferLayout(Index);
                Ref<VertexBuffer> buffer = context.m_Cache.AcquireVertexBuffer(data);
                elements[Slot] = Element{ layout, buffer };
            }
        };

        template<size_t Slot, size_t Index = 0u>
        struct Texture
        {
            static constexpr BindingKind s_Kind = BindingKind::Texture;
            static constexpr uint64_t s_Bit = 1ull << Index;

            using Element = Ref<Rynex::Texture>;

            template<typename Interface>
            static void Apply(const Interface& source, BuildContext& context, Span<Element>& vertexElementsSpan)
            {
                const Span<const Byte> data = source.GetBufferData(Index);
                const TextureSpecification specifcation = source.GetTextureSpec(Index);
                Ref<Rynex::Texture> texture = context.m_Cache.AcquireTexture(specifcation, data);
                elements[Slot] = texture;
            }
        };

        template<size_t Slot, size_t Index = 0u>
        struct Uniform
        {
            static constexpr BindingKind s_Kind = BindingKind::Uniform;
            static constexpr uint64_t s_Bit = 1ull << Index;

            using Element = Ref<UniformBuffer>;
            template<typename Interface, typename ElementSpan>
            static void Apply(const Interface& source, BuildContext& context, Span<Element>& elementSpan)
            {
                Element uniformBuffer = source.GetUniformBuffer(Index);
                elementSpan[Slot] = uniformBuffer;
            }
        };

        template<size_t Slot, size_t Index = 0u>
        struct Storage
        {
            static constexpr BindingKind s_Kind = BindingKind::Storage;
            static constexpr uint64_t s_Bit = 1ull << Index;

            using Element = Ref<StorageBuffer>;
            template<typename Interface, typename ElementSpan>
            static void Apply(const Interface& source, BuildContext& context, Span<Element>& elementSpan)
            {
                Span<Byte> data = source.GetBufferData(Index);
                BufferLayout layout = source.GetBufferLayout(Index);
                
                Element storageBuffer = context.m_Cache.AcquireStorageBuffer();
                elementSpan[Slot] = storageBuffer;
            }
        };
    }

    template<typename Interface, bool VertexBuffer = false, bool BindBuffers = true>
    class CommandBuildBase 
    {
    public:
        static constexpr bool s_HasVertexBuffer = VertexBuffer;
        static constexpr bool s_HasBindBuffers = BindBuffers;
    // public member methods --------------------------------------------------------------------------------------------------
        CommandBuildBase() = delete;
        CommandBuildBase(const CommandBuildBase&) = delete;
        explicit CommandBuildBase(CachedBufferRegister& cache, Interface& interfaceType, ShaderDrawResource& drawResource)
            : m_Interface(interfaceType)
            , m_Cache(cache)
            , m_DrawResource(drawResource)
        {}
        
        ~CommandBuildBase()
        {
        }


    protected:
        Interface& m_Interface;
        CachedBufferRegister& m_Cache;
        ShaderDrawResource& m_DrawResource;
    };


}

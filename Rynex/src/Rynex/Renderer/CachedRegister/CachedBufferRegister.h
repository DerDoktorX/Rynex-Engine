#pragma once
// CachedBufferRegister created in Project Rynex-Rendering on 05/10/2026.
#include <rypch.h>

#include <Rynex/Renderer/API/BindlessTextureArray.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/API/VertexArray.h>

#include <Rynex/Renderer/CachedRegister/BufferControlller.h>
#include <Rynex/Renderer/CachedRegister/StoredBufferRegister.h>
#include <Rynex/Renderer/CachedRegister/StoredKey.h>
#include <Rynex/Renderer/CachedRegister/UniqueBufferRegister.h>
#include <Rynex/Renderer/CachedRegister/UniqueKey.h>


// #define RY_VERTEX_BUFFER_REGISTER
#define RY_TEXTURE_REGISTER
namespace Rynex {


    class CachedBufferRegister
    {
    public:
        using Unit = uint32_t;

        static constexpr uint64_t s_DefaultGarbageInterval = 600ull;
        static constexpr uint64_t s_DefaultMaxUnusedFrames = 600ull;
    // public member methode --------------------------------------------------------------------------------------------------
        CachedBufferRegister();
        ~CachedBufferRegister();

        template<typename Buffer>
        Ref<Buffer> AcquireBuffer(const Span<const Byte>& bufferData, BufferFlagGPU flag);

        template<typename Buffer>
        Ref<Buffer> AcquireUniqueBufferHash(Hash64 hash);

        template<typename Buffer>
        std::pair<Hash64, Ref<Buffer>> AcquireUniqueBuffer(const Span<const Byte>& bufferData, const BufferLayout& layout, const BufferFlagGPU flag);


        template<typename T>
        Ref<StorageBuffer> AcquireStorageBuffer(const Span<T>& bufferData)
        {
            const Span<const Byte> bytes = AsBytes(bufferData);
            return AcquireStorageBuffer(bytes);
        }
        Ref<StorageBuffer> AcquireStorageBuffer(const Span<const Byte>& bufferData);

        template<typename T>
        Ref<VertexBuffer> AcquireVertexBuffer(const Span<T>& bufferData)
        {
            const Span<const Byte> bytes = AsBytes(bufferData);
            return AcquireVertexBuffer(bytes);
        }
        Ref<VertexBuffer> AcquireVertexBuffer(const Span<const Byte>& bufferData);


        template<typename T>
        Ref<UniformBuffer> AcquireUniformBuffer(const Span<T>& bufferData, const BufferFlagGPU flag)
        {
            const Span<const Byte> bytes = AsBytes(bufferData);
            return AcquireUniformBuffer(bytes, flag);
        }
        template<typename T>
        Ref<UniformBuffer> AcquireUniformBuffer(const T& bufferData, const BufferFlagGPU flag)
        {
            const Span<const Byte> bytes( &bufferData, sizeof(T) );
            return AcquireUniformBuffer(bytes, flag);
        }
        Ref<UniformBuffer> AcquireUniformBuffer(const Span<const Byte>& bufferData, BufferFlagGPU flag);


        template<typename T>
        Ref<Texture> AcquireTexture(const TextureSpecification& spec, const Span<T>& bufferData);
        Ref<Texture> AcquireTexture(const TextureSpecification& spec, const Span<const Byte>& bufferData);
        Ref<Texture> AcquireTexture(const TextureSpecification& spec);



        template<typename T>
        std::pair<Hash64, Ref<UniformBuffer>> AcquireUniqueUniformBuffer(const Span<T>& bufferData, const BufferLayout& layout, const BufferFlagGPU flag)
        {
            const Span<const Byte> bytes = AsBytes(bufferData);
            return AcquireUniqueUniformBuffer(bytes, layout, flag);
        }
        template<typename T>
        std::pair<Hash64, Ref<UniformBuffer>> AcquireUniqueUniformBuffer(const T& bufferData, const BufferLayout& layout, const BufferFlagGPU flag)
        {
            const Span<const Byte> bytes( &bufferData, sizeof(T) );
            return AcquireUniqueUniformBuffer(bytes, layout, flag);
        }
        std::pair<Hash64, Ref<UniformBuffer>> AcquireUniqueUniformBuffer(const Span<const Byte>& bufferData, const BufferLayout& layout, BufferFlagGPU flag);
        Ref<UniformBuffer> AcquireUniqueUniformBufferHash(Hash64 hash);

        std::pair<Hash64, Ref<StorageBuffer>> AcquireUniqueStorageBuffer(const Span<const Byte>& bufferData, const BufferLayout& layout, BufferFlagGPU flag);
        Ref<StorageBuffer> AcquireUniqueStorageBufferHash(Hash64 hash);

        std::pair<Hash64, Ref<IndexBuffer>> AcquireUniqueIndexBuffer32(const Span<uint32_t>& bufferData, BufferFlagGPU flag);
        Ref<IndexBuffer> AcquireUniqueIndexBufferHash32(Hash64 hash);

        std::pair<Hash64, Ref<IndexBuffer>> AcquireUniqueIndexBuffer16(const Span<uint16_t>& bufferData, BufferFlagGPU flag);
        Ref<IndexBuffer> AcquireUniqueIndexBufferHash16(Hash64 hash);


        std::pair<Hash64, Ref<Texture>> GetUniqueTextureHash(const Span<Byte>& bufferData, const TextureSpecification& specification);
        Ref<Texture> AcquireUniqueTextureHash(Hash64 hash);


        template<typename T>
        std::pair<Hash64, Ref<VertexBuffer>> AcquireUniqueVertexBuffer(const Span<T>& bufferData, const BufferLayout& layout, const BufferFlagGPU flag)
        {
            const Span<const Byte> bytes = AsBytes(bufferData);
            return AcquireUniqueVertexBuffer(bytes, layout, flag);
        }
        std::pair<Hash64, Ref<VertexBuffer>> AcquireUniqueVertexBuffer(const Span<const Byte>& bufferData, const BufferLayout& layout, BufferFlagGPU flag);
        Ref<VertexBuffer> AcquireUniqueVertexBufferHash(Hash64 hash);


        std::pair<Hash64, Ref<VertexArray>> AcquireUniqueVertexArray(const Ref<IndexBuffer>& indexBuffer, std::initializer_list<VertexArray::VertexElements> vertexBufferList);
        std::pair<Hash64, Ref<VertexArray>> AcquireUniqueVertexArray(const Ref<IndexBuffer>& indexBuffer, Span<const VertexArray::VertexElements> vertexBufferVec);
        Ref<VertexArray> AcquireUniqueVertexArrayHash(Hash64 hash);



        void SetFrame(uint64_t frame);
        void RemoveBufferGarbage();

        void ReservedRegisterGpuMaxGpuMemoryUsage(uint64_t gpuMemoryUsage);
        void Clear();
        bool IsOverBudget();

        uint64_t GetRegisterGpuMemoryUnused() const;
        uint64_t GetUnusedGpuBudged() const;
        uint64_t GetRegisterGpuMemoryUsage() const;
        uint64_t GetReservedRegisterGpuMemory() const;

    // public member variable -------------------------------------------------------------------------------------------------
    private:
    // private member methode -------------------------------------------------------------------------------------------------
        template<typename Register>
        typename Register::Buffer AcquireStored(Register& registerStored, const typename Register::Key& key);

        template<typename Register>
        std::pair<typename Register::Key, typename Register::Buffer> AcquireUnique(Register& registerUnique, const typename Register::ConstructData & data);

        /**
         * checks for space in budget can destroy old GPu resources if needed.
         * @param requiredByteSize
         * @return { @true memory could not be reserved. @false not enough budget left }
         */
        bool EnsureBudget(uint64_t requiredByteSize);
        uint64_t FreeGpuMemory(uint64_t requiredByteSize);
        uint64_t TrimAllUnused(uint64_t maxFrame);
    // private static methode -------------------------------------------------------------------------------------------------
        static Hash64 HashVertexArray(const Ref<IndexBuffer>& indexBuffer, const Span<const VertexArray::VertexElements>& vertexBufferVec);
    // private alias ----------------------------------------------------------------------------------------------------------
        using StoredStorageRegister = StoredBufferRegister<StoredKey, Ref<StorageBuffer>, StoredBufferController<StorageBuffer>>;
        using StoredVertexRegister = StoredBufferRegister<StoredKey, Ref<VertexBuffer>, StoredBufferController<VertexBuffer>>;
        using StoredUniformRegister =  StoredBufferRegister<StoredKey, Ref<UniformBuffer>, StoredBufferController<UniformBuffer>>;
        // using StoredTextureRegister = StoredBufferRegister<StoredTextureKey, Ref<Texture>, StoredTextureController>;

        using UniqueUniformRegister = UniqueBufferRegister<Hash64, Ref<UniformBuffer>, UniqueBufferController<UniformBuffer, Byte>>;
        using UniqueIndexRegister32 = UniqueBufferRegister<Hash64, Ref<IndexBuffer>, UniqueBufferController<IndexBuffer, uint32_t>>;
        using UniqueIndexRegister16 = UniqueBufferRegister<Hash64, Ref<IndexBuffer>, UniqueBufferController<IndexBuffer, uint16_t>>;
        using UniqueStorageRegister = UniqueBufferRegister<Hash64, Ref<StorageBuffer>, UniqueBufferController<StorageBuffer, Byte>>;
        // using UniqueTextureRegister = UniqueBufferRegister<Hash64, Ref<Texture>, UniqueBufferController<Texture, Byte>>;
        using UniqueVertexRegister = UniqueBufferRegister<Hash64, Ref<VertexBuffer>, UniqueBufferController<VertexBuffer, Byte>>;

        using UniqueVertexArrayRegister = UniqueBufferRegister<Hash64, Ref<VertexArray>, UniqueVertexObjectArrayController>;

        enum RegisterFlagBits : uint32_t
        {
            RegisterFlag_None = 0u,
            RegisterFlag_OverBudgetLogged = BIT(0)
        };

    // private member variable ------------------------------------------------------------------------------------------------

        StoredVertexRegister m_StoredVertex;
        StoredUniformRegister m_StoredUniform;
        StoredStorageRegister m_StoredStorage;

        UniqueVertexRegister m_UniqueVertex;
        UniqueIndexRegister32 m_UniqueIndex32;
        UniqueIndexRegister16 m_UniqueIndex16;
        UniqueUniformRegister m_UniqueUniform;
        UniqueStorageRegister m_UniqueStorage;
        // UniqueTextureRegister m_UniqueTexture;

        UniqueVertexArrayRegister m_UniqueVertexArray;

        uint64_t m_ReservedGpuMemory;
        uint64_t m_Frame;
        uint64_t m_LastGarbageFrame;
        uint64_t m_GarbageInterval;
        uint64_t m_MaxUnusedFrames;
        uint32_t m_Flags;
    };

    template <typename  Buffer>
    Ref<Buffer> CachedBufferRegister::AcquireBuffer(const Span<const Byte>& bufferData, BufferFlagGPU flag)
    {
        static_assert(false, "No default implemented!");
        return Ref<Buffer>{nullptr};
    }

    template <typename Buffer>
    Ref<Buffer> CachedBufferRegister::AcquireUniqueBufferHash(Hash64 hash)
    {
        static_assert(false, "No default implemented!");
        return Ref<Buffer>{nullptr};
    }

    template <typename Buffer>
    std::pair<Hash64, Ref<Buffer>> CachedBufferRegister::AcquireUniqueBuffer(const Span<const Byte>& bufferData, const BufferLayout& layout, const BufferFlagGPU flag)
    {
        static_assert(false, "No default implemented!");
        return std::pair<Hash64, Ref<Buffer>>(0ull, Ref<Buffer>{nullptr});
    }

    template<>
    inline Ref<UniformBuffer> CachedBufferRegister::AcquireBuffer<UniformBuffer>(const Span<const Byte>& bufferData, BufferFlagGPU flag)
    {
        return AcquireUniformBuffer(bufferData, flag);
    }

    template<>
    inline Ref<UniformBuffer> CachedBufferRegister::AcquireUniqueBufferHash<UniformBuffer>(Hash64 hash)
    {
        return AcquireUniqueUniformBufferHash(hash);
    }

    template<>
    inline std::pair<Hash64, Ref<UniformBuffer>> CachedBufferRegister::AcquireUniqueBuffer<UniformBuffer>(const Span<const Byte>& bufferData, const BufferLayout& layout, const BufferFlagGPU flag)
    {
        return AcquireUniqueUniformBuffer(bufferData, layout, flag);
    }

    template<>
    inline Ref<VertexBuffer> CachedBufferRegister::AcquireBuffer<VertexBuffer>(const Span<const Byte>& bufferData, BufferFlagGPU flag)
    {
        return AcquireVertexBuffer(bufferData);
    }

    template<>
    inline Ref<VertexBuffer> CachedBufferRegister::AcquireUniqueBufferHash<VertexBuffer>(Hash64 hash)
    {
        return AcquireUniqueVertexBufferHash(hash);
    }

    template<>
    inline std::pair<Hash64, Ref<VertexBuffer>> CachedBufferRegister::AcquireUniqueBuffer<VertexBuffer>(const Span<const Byte>& bufferData, const BufferLayout& layout, const BufferFlagGPU flag)
    {
        return AcquireUniqueVertexBuffer(bufferData, layout, flag);
    }

    template<>
    inline Ref<StorageBuffer> CachedBufferRegister::AcquireBuffer<StorageBuffer>(const Span<const Byte>& bufferData, BufferFlagGPU flag)
    {
        return AcquireStorageBuffer(bufferData);
    }

    template<>
    inline Ref<StorageBuffer> CachedBufferRegister::AcquireUniqueBufferHash<StorageBuffer>(Hash64 hash)
    {
        return AcquireUniqueStorageBufferHash(hash);
    }

    template<>
    inline std::pair<Hash64, Ref<StorageBuffer>> CachedBufferRegister::AcquireUniqueBuffer<StorageBuffer>(const Span<const Byte>& bufferData, const BufferLayout& layout, const BufferFlagGPU flag)
    {
        return AcquireUniqueStorageBuffer(bufferData, layout, flag);
    }

    template<typename Register>
    typename Register::Buffer CachedBufferRegister::AcquireStored(Register& registerStored, const typename Register::Key& key)
    {
        using Buffer = typename Register::Buffer;

        Buffer* found = registerStored.TryAcquireBuffer(key);
        if (nullptr != found)
        {
            return *found;
        }

        const uint64_t byteSize = key.GetByteSize();
        if(!EnsureBudget(byteSize))
        {
            RY_CORE_ERROR("the acquired byteSize {0} is too large! empty (buffer) return", byteSize);
            return Buffer{ nullptr };
        }

        // EnsureBudget may have changed the register, so the buffer is created afterwards.
        Buffer& created = registerStored.CreateBuffer(key);
        return created;
    }



    template<typename Register>
    std::pair<typename Register::Key, typename Register::Buffer> CachedBufferRegister::AcquireUnique(Register& registerUnique, const typename Register::ConstructData& data)
    {
        using Buffer = typename Register::Buffer;
        using ConstructData = typename Register::ConstructData;
        using Key = typename Register::Key;
        Key key = data.GetHash();

        const uint64_t byteSize = data.m_Data.size_bytes();
        if(!EnsureBudget(byteSize))
        {
            RY_CORE_ERROR("the acquired byteSize {0} is too large! empty (hash, buffer) return", byteSize);
            return std::make_pair(0, nullptr);
        }

        Buffer& buffer = registerUnique.Create(key, data);
        return std::make_pair(key, buffer);
    }


}

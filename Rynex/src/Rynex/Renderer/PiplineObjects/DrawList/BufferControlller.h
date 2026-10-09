#pragma once
// UniqueController created in Project Rynex-Rendering on 06/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/PiplineObjects/DrawList/UniqueKey.h>
#include <Rynex/Renderer/PiplineObjects/DrawList/StoredKey.h>

namespace Rynex {



    template<typename T, typename N>
    class UniqueBufferController
    {
    public:
        struct ConstructData
        {
            const Span<const N> m_Data;
            const BufferLayout&  m_Layout;
            const BufferFlagGPU m_Flag;
            const BufferType m_Type;

            ConstructData(const Span<const N> byteData, const BufferLayout& layout, const BufferFlagGPU flag, const BufferType type)
                : m_Data(byteData)
                , m_Layout(layout)
                , m_Flag(flag)
                , m_Type(type)
            {
            }

            Hash64 GetHash() const
            {
                const uint64_t byteSizeData = m_Data.size_bytes();
                const N* data = m_Data.data();
                const Hash64 hashData = robin_hood::hash_bytes(data, byteSizeData);
                const Hash64 hashArray[4]
                {
                    hashData,
                    m_Layout.GetHash(),
                    static_cast<Hash64>(m_Flag),
                    static_cast<Hash64>(m_Type)
                };
                constexpr Hash64 byteSizeHash = sizeof(hashArray);
                const Hash64 hash = robin_hood::hash_bytes(hashArray, byteSizeHash);
                return hash;
            }
        };
    // public static methode ----------------------------------------------------------------------------------------------
        static Ref<T> Create(const ConstructData& constructData);
        static uint64_t GetByteSize(const Ref<T>& buffer);


        static bool IsReferencedElsewhere(const Ref<T>& buffer)
        {
            return 1l < buffer.use_count();
        }
    };


    template<typename T>
    class StoredBufferController
    {
    public:
        // public static methode ----------------------------------------------------------------------------------------------
        static Ref<T> Create(const StoredKey& key);

        static uint64_t GetByteSize(const StoredKey& key)
        {
            const uint64_t byteSize = key.GetByteSize();
            return byteSize;
        }

        static bool IsReferencedElsewhere(const Ref<T>& buffer)
        {
            return 1l < buffer.use_count();
        }
    };

    template <typename T>
    Ref<T> StoredBufferController<T>::Create(const StoredKey& key)
    {
        static_assert(false, "no default implementation!");
        return Ref<T>(nullptr);
    }

    template<>
    inline Ref<VertexBuffer> StoredBufferController<VertexBuffer>::Create(const StoredKey& key)
    {
        const uint32_t byteSize = static_cast<uint32_t>(key.GetByteSize());
        return VertexBuffer::Create(byteSize);
    }

    template<>
    inline Ref<StorageBuffer> StoredBufferController<StorageBuffer>::Create(const StoredKey& key)
    {
        const uint32_t byteSize = static_cast<uint32_t>(key.GetByteSize());
        const void* data = nullptr;
        constexpr BufferFlagGPU flag = BufferFlag::None;
        return StorageBuffer::Create(data, byteSize, flag);
    }

    template<>
    inline Ref<UniformBuffer> StoredBufferController<UniformBuffer>::Create(const StoredKey& key)
    {
        const uint32_t byteSize = static_cast<uint32_t>(key.GetByteSize());
        const void* data = nullptr;
        return UniformBuffer::Create(data, byteSize);
    }




    template <typename T, typename N>
    Ref<T> UniqueBufferController<T, N>::Create(const ConstructData& data)
    {
        static_assert(false, "no default implementation!");
        return Ref<T>(nullptr);
    }

    template<>
   inline Ref<UniformBuffer> UniqueBufferController<UniformBuffer, Byte>::Create(const ConstructData& data)
    {
        return UniformBuffer::Create(data.m_Data.data(), data.m_Data.size_bytes(), data.m_Layout,data.m_Flag);
    }

    template<>
    inline Ref<VertexBuffer> UniqueBufferController<VertexBuffer, Byte>::Create(const ConstructData& data)
    {
        return VertexBuffer::Create(data.m_Data.data(), data.m_Data.size_bytes(), data.m_Flag , data.m_Layout);
    }

    template<>
    inline Ref<StorageBuffer> UniqueBufferController<StorageBuffer, Byte>::Create(const ConstructData& data)
    {
        if (BufferType::None == data.m_Type)
            return StorageBuffer::Create(data.m_Data.data(), data.m_Data.size_bytes(), data.m_Flag);

        return StorageBuffer::Create(data.m_Data.data(), data.m_Data.size_bytes(), data.m_Type, data.m_Flag);
    }

    template<>
    inline Ref<IndexBuffer> UniqueBufferController<IndexBuffer, uint32_t>::Create(const ConstructData& data)
    {
        return IndexBuffer::Create(data.m_Data.data(), data.m_Data.size(), data.m_Flag);
    }

    template<>
    inline Ref<IndexBuffer> UniqueBufferController<IndexBuffer, uint16_t>::Create(const ConstructData& data)
    {
        return IndexBuffer::Create(data.m_Data.data(), data.m_Data.size(), data.m_Flag);
    }

    template <typename T, typename N>
    uint64_t UniqueBufferController<T, N>::GetByteSize(const Ref<T>& buffer)
    {
        RY_CORE_ASSERT(nullptr == buffer, "missing vertex buffer!");
        return buffer->GetByteSize();
    }


    template <>
    inline uint64_t UniqueBufferController<VertexArray, Byte>::GetByteSize(const Ref<VertexArray>& vertexObjectArray)
    {
        uint64_t bytesSize = 0u;
        RY_CORE_ASSERT(nullptr == vertexObjectArray, "missing vertex object array!");
        for (const VertexArray::VertexElements& vertexElements : vertexObjectArray->GetVertexBuffers())
        {
            const Ref<VertexBuffer> vertexBuffer = vertexElements.m_Buffer;
            RY_CORE_ASSERT(nullptr == vertexBuffer, "missing vertex buffer in vertex object array!");
            const uint32_t byteSize = vertexBuffer->GetByteSize();
            bytesSize += static_cast<uint64_t>(byteSize);
        }

        if (const Ref<IndexBuffer>& indexBuffer = vertexObjectArray->GetIndexBuffer())
        {
            const uint32_t byteSize = indexBuffer->GetByteSize();
            bytesSize += static_cast<uint64_t>(byteSize);
        }
        return bytesSize;
    }
}



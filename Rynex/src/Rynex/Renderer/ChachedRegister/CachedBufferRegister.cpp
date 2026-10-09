// CachedBufferRegister created in Project Rynex-Rendering on 05/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/ChachedRegister/CachedBufferRegister.h>


namespace Rynex {
    CachedBufferRegister::CachedBufferRegister()
        : m_ReservedGpuMemory(0)
        ,  m_Frame(0)
        , m_LastGarbageFrame(0)
        , m_GarbageInterval(0)
        , m_MaxUnusedFrames(0)
        , m_Flags(0)
    {
    }

    CachedBufferRegister::~CachedBufferRegister()
    {
    }

    Ref<StorageBuffer> CachedBufferRegister::AcquireStorageBuffer(const Span<const Byte>& bufferData)
    {
        const uint64_t byteSize = bufferData.size();
        const StoredKey key(byteSize);
        Ref<StorageBuffer> buffer = AcquireStored(m_StoredStorage, key);
        buffer->SetData(bufferData.data(), static_cast<uint32_t>(byteSize));
        return buffer;
    }

    Ref<VertexBuffer> CachedBufferRegister::AcquireVertexBuffer(const Span<const Byte>& bufferData)
    {
        const uint64_t byteSize = bufferData.size();
        const StoredKey key(byteSize);
        Ref<VertexBuffer> buffer = AcquireStored(m_StoredVertex, key);
        buffer->SetData(bufferData.data(), static_cast<uint32_t>(byteSize));
        return buffer;
    }

    Ref<UniformBuffer> CachedBufferRegister::AcquireUniformBuffer(const Span<const Byte>& bufferData, BufferFlagGPU flag)
    {
        const uint64_t byteSize = bufferData.size();
        const StoredKey key(byteSize);
        Ref<UniformBuffer> buffer = AcquireStored(m_StoredUniform, key);
        buffer->SetData(bufferData.data(), static_cast<uint32_t>(byteSize));
        return buffer;
    }



    Ref<UniformBuffer> CachedBufferRegister::AcquireUniqueUniformBufferHash(Hash64 hash)
    {
        Ref<UniformBuffer>* uniformBufferPtr = m_UniqueUniform.TryAcquire(hash);
        if (nullptr == uniformBufferPtr)
            return Ref<UniformBuffer>(nullptr);
        Ref<UniformBuffer> uniformBuffer(*uniformBufferPtr);
        return uniformBuffer;
    }



    std::pair<Hash64, Ref<UniformBuffer>> CachedBufferRegister::AcquireUniqueUniformBuffer(const Span<const Byte>& bufferData, const BufferLayout& layout, const BufferFlagGPU flag)
    {
        UniqueUniformRegister::ConstructData constructData(bufferData, layout, flag, BufferType::None);
        return AcquireUnique(m_UniqueUniform, constructData);;
    }



    std::pair<Hash64, Ref<IndexBuffer>> CachedBufferRegister::AcquireUniqueIndexBuffer32(const Span<uint32_t>& bufferData, BufferFlagGPU flag)
    {
        UniqueIndexRegister32::ConstructData constructData(bufferData, BufferLayout(), flag, BufferType::None);
        return AcquireUnique(m_UniqueIndex32, constructData);;
    }


    Ref<IndexBuffer> CachedBufferRegister::AcquireUniqueIndexBufferHash32(Hash64 hash)
    {
        Ref<IndexBuffer>* indexBufferPtr = m_UniqueIndex16.TryAcquire(hash);
        if (nullptr == indexBufferPtr)
            return Ref<IndexBuffer>(nullptr);
        Ref<IndexBuffer> indexBuffer(*indexBufferPtr);
        return indexBuffer;
    }

    std::pair<Hash64, Ref<IndexBuffer>> CachedBufferRegister::AcquireUniqueIndexBuffer16(const Span<uint16_t>& bufferData, BufferFlagGPU flag)
    {
        UniqueIndexRegister16::ConstructData constructData(bufferData, BufferLayout(), flag, BufferType::None);
        return AcquireUnique(m_UniqueIndex16, constructData);
    }

    Ref<IndexBuffer> CachedBufferRegister::AcquireUniqueIndexBufferHash16(Hash64 hash)
    {
        Ref<IndexBuffer>* indexBufferPtr = m_UniqueIndex16.TryAcquire(hash);
        if (nullptr == indexBufferPtr)
            return Ref<IndexBuffer>(nullptr);
        Ref<IndexBuffer> indexBuffer(*indexBufferPtr);
        return indexBuffer;
    }



    Ref<VertexBuffer> CachedBufferRegister::AcquireUniqueVertexBufferHash(const Hash64 hash)
    {
        Ref<VertexBuffer>* vertexBufferPtr = m_UniqueVertex.TryAcquire(hash);
        if (nullptr == vertexBufferPtr)
            return Ref<VertexBuffer>(nullptr);
        Ref<VertexBuffer> vertexBuffer(*vertexBufferPtr);
        return vertexBuffer;
    }

    std::pair<Hash64, Ref<VertexArray>> CachedBufferRegister::AcquireUniqueVertexArray(const Ref<IndexBuffer>& indexBuffer, std::initializer_list<VertexArray::VertexElements> vertexBufferList)
    {
        const VertexArray::VertexElements* listBegin = vertexBufferList.begin();
        const size_t listSize = vertexBufferList.size();
        const Span<const VertexArray::VertexElements> vertexBufferVec(listBegin, listSize);
        return AcquireUniqueVertexArray(indexBuffer, vertexBufferVec);
    }

    std::pair<Hash64, Ref<VertexArray>> CachedBufferRegister::AcquireUniqueVertexArray(const Ref<IndexBuffer>& indexBuffer, const Span<const VertexArray::VertexElements> vertexBufferVec)
    {
        using ConstructData =  UniqueVertexArrayRegister::ConstructData;
        using Key = Hash64;
        const Key key = HashVertexArray(indexBuffer, vertexBufferVec);
        const ConstructData data{ indexBuffer, vertexBufferVec };
        Ref<VertexArray>& vertexObjectArray = m_UniqueVertexArray.Create(key, data);
        return std::make_pair(key, vertexObjectArray);
    }

    void CachedBufferRegister::SetFrame(const uint64_t frame)
    {
        RY_CORE_ASSERT(m_Frame <= frame, "The frame are expedite to go forward or stay equal!");
        m_Frame = frame;

        const bool isNewFrame = m_Frame != frame;
        m_Frame = frame;

        m_StoredStorage.SetFrame(frame);
        m_StoredVertex.SetFrame(frame);
        m_StoredUniform.SetFrame(frame);
#ifndef RY_TEXTURE_REGISTER
        m_StoredTexture.SetFrame(frame);
#endif

        m_UniqueUniform.SetFrame(frame);
        m_UniqueIndex16.SetFrame(frame);
        m_UniqueIndex32.SetFrame(frame);
#ifndef RY_TEXTURE_REGISTER
        m_UniqueTexture.SetFrame(frame);
#endif
        m_UniqueVertex.SetFrame(frame);
#ifndef RY_VERTEX_BUFFER_REGISTER
        m_UniqueVertexArray.SetFrame(frame);
#endif

        if (!isNewFrame)
            return;


        m_Flags &= ~static_cast<uint32_t>(RegisterFlag_OverBudgetLogged);

        const uint64_t elapsedFrames = m_Frame - m_LastGarbageFrame;
        if (m_GarbageInterval <= elapsedFrames)
            RemoveBufferGarbage();

    }

    void CachedBufferRegister::RemoveBufferGarbage()
    {
        uint64_t freedBytes = 0ull;
        if (m_MaxUnusedFrames <= m_Frame)
        {
            const uint64_t maxFrame = m_Frame - m_MaxUnusedFrames;
            freedBytes += TrimAllUnused(maxFrame);
        }

        freedBytes += m_StoredStorage.TrimToWindowPeak();
        freedBytes += m_StoredVertex.TrimToWindowPeak();
        freedBytes += m_StoredUniform.TrimToWindowPeak();
#ifndef RY_TEXTURE_REGISTER
        freedBytes += m_StoredTexture.TrimToWindowPeak();
#endif
        m_LastGarbageFrame = m_Frame;

        RY_CORE_TRACE_IF(0ull < freedBytes, "CachedBufferRegister garbage collection freed {} bytes", freedBytes);
    }

    void CachedBufferRegister::ReservedRegisterGpuMaxGpuMemoryUsage(uint64_t gpuMemoryUsage)
    {
        m_ReservedGpuMemory = gpuMemoryUsage;
        if (0ull == m_ReservedGpuMemory)
            return;


        // A smaller budget takes effect right away.
        const uint64_t usage = GetRegisterGpuMemoryUsage();
        if (usage <= m_ReservedGpuMemory)
            return;


        const uint64_t overshoot = usage - m_ReservedGpuMemory;
        FreeGpuMemory(overshoot);
    }

    void CachedBufferRegister::Clear()
    {
        m_StoredStorage.Clear();
        m_StoredVertex.Clear();
        m_StoredUniform.Clear();

        m_UniqueUniform.Clear();
        m_UniqueIndex16.Clear();
        m_UniqueIndex32.Clear();
#ifndef RY_TEXTURE_REGISTER
        m_UniqueTexture.Clear();
        m_StoredTexture.Clear();
#endif
        m_UniqueVertex.Clear();
#ifndef RY_VERTEX_BUFFER_REGISTER
        m_UniqueVertexArray.Clear();
#endif
    }

    bool CachedBufferRegister::IsOverBudget()
    {
        if (0ull == m_ReservedGpuMemory)
        {
            return false;
        }

        const uint64_t usage = GetRegisterGpuMemoryUsage();
        return m_ReservedGpuMemory < usage;
    }

    uint64_t CachedBufferRegister::GetRegisterGpuMemoryUnused() const
    {
        uint64_t unusedBytes = 0ull;
        unusedBytes += m_StoredStorage.GetIdleByteSize();
        unusedBytes += m_StoredVertex.GetIdleByteSize();
        unusedBytes += m_StoredUniform.GetIdleByteSize();
#ifndef RY_TEXTURE_REGISTER
        unusedBytes += m_StoredTexture.GetIdleByteSize();
#endif
        unusedBytes += m_UniqueUniform.GetIdleByteSize();
        unusedBytes += m_UniqueIndex16.GetIdleByteSize();
        unusedBytes += m_UniqueIndex32.GetIdleByteSize();
#ifndef RY_TEXTURE_REGISTER
        unusedBytes += m_UniqueTexture.GetIdleByteSize();
#endif
        unusedBytes += m_UniqueVertex.GetIdleByteSize();
        return unusedBytes;
    }

    uint64_t CachedBufferRegister::GetRegisterGpuMemoryUsage() const
    {
        uint64_t usageBytes = 0ull;
        usageBytes += m_StoredStorage.GetRegisterBufferByteSize();
        usageBytes += m_StoredVertex.GetRegisterBufferByteSize();
        usageBytes += m_StoredUniform.GetRegisterBufferByteSize();
#ifndef RY_TEXTURE_REGISTER
        usageBytes += m_StoredTexture.GetRegisterBufferByteSize();
#endif
        usageBytes += m_UniqueUniform.GetRegisterBufferByteSize();
        usageBytes += m_UniqueIndex16.GetRegisterBufferByteSize();
        usageBytes += m_UniqueIndex32.GetRegisterBufferByteSize();
#ifndef RY_TEXTURE_REGISTER
        usageBytes += m_UniqueTexture.GetRegisterBufferByteSize();
#endif
        usageBytes += m_UniqueVertex.GetRegisterBufferByteSize();
        return usageBytes;
    }

    uint64_t CachedBufferRegister::GetReservedRegisterGpuMemory() const
    {
        return m_ReservedGpuMemory;
    }

    std::pair<Hash64, Ref<VertexBuffer>> CachedBufferRegister::AcquireUniqueVertexBuffer(const Span<const Byte>& bufferData, const BufferLayout& layout, BufferFlagGPU flag)
    {
        UniqueVertexRegister::ConstructData constructData(bufferData, layout, flag, BufferType::None);
        return AcquireUnique<UniqueVertexRegister>(m_UniqueVertex, constructData);
    }



    void CachedBufferRegister::EnsureBudget(uint64_t requiredByteSize)
    {
        if (0ull == m_ReservedGpuMemory)
            return;

        const uint64_t usage = GetRegisterGpuMemoryUsage();
        const uint64_t projected = usage + requiredByteSize;
        if (projected <= m_ReservedGpuMemory)
            return;

        const uint64_t missing = projected - m_ReservedGpuMemory;
        FreeGpuMemory(missing);

        const uint64_t usageAfter = GetRegisterGpuMemoryUsage();
        const uint64_t projectedAfter = usageAfter + requiredByteSize;
        const bool stillOver = m_ReservedGpuMemory < projectedAfter;
        const bool alreadyLogged = 0u != (m_Flags & RegisterFlag_OverBudgetLogged);
        if (stillOver && !alreadyLogged)
        {
            m_Flags |= RegisterFlag_OverBudgetLogged;
            RY_CORE_WARN("CachedBufferRegister is over its GPU memory budget: {} of {} bytes in use, {} bytes requested", usageAfter, m_ReservedGpuMemory, requiredByteSize);
        }
    }

    uint64_t CachedBufferRegister::FreeGpuMemory(uint64_t requiredByteSize)
    {
        uint64_t freedBytes = 0ull;

        // Cached vertex arrays of earlier frames are cheap to rebuild and they pin buffers, so they go first.
        if (0ull < m_Frame)
        {
            const uint64_t lastFrame = m_Frame - 1ull;
#ifndef RY_VERTEX_BUFFER_REGISTER
            freedBytes += m_UniqueVertexArray.TrimUnused(lastFrame);
#endif

        }

        // Oldest first: remove everything unused for at least this many frames, then a younger group, and so on.
        static constexpr uint64_t s_EvictionAges[] = { 600ull, 120ull, 30ull, 8ull, 1ull, 0ull };
        for (const uint64_t age : s_EvictionAges)
        {
            if (requiredByteSize <= freedBytes)
                break;

            if (m_Frame < age)
                continue;


            const uint64_t maxFrame = m_Frame - age;
            freedBytes += TrimAllUnused(maxFrame);
        }
        return freedBytes;
    }

    uint64_t CachedBufferRegister::TrimAllUnused(uint64_t maxFrame)
    {
        uint64_t freedBytes = 0ull;
#ifndef RY_VERTEX_BUFFER_REGISTER
        freedBytes += m_UniqueVertexArray.TrimUnused(maxFrame);
#endif
#ifndef RY_TEXTURE_REGISTER
        freedBytes += m_UniqueTexture.TrimUnused(maxFrame);
#endif

        freedBytes += m_UniqueUniform.TrimUnused(maxFrame);
        freedBytes += m_UniqueIndex32.TrimUnused(maxFrame);
        freedBytes += m_UniqueIndex16.TrimUnused(maxFrame);

        freedBytes += m_UniqueVertex.TrimUnused(maxFrame);
        freedBytes += m_StoredStorage.TrimUnused(maxFrame);
        freedBytes += m_StoredVertex.TrimUnused(maxFrame);
        freedBytes += m_StoredUniform.TrimUnused(maxFrame);
#ifndef RY_TEXTURE_REGISTER
        freedBytes += m_StoredTexture.TrimUnused(maxFrame);
#endif
        return freedBytes;
    }

    Hash64 CachedBufferRegister::HashVertexArray(const Ref<IndexBuffer>& indexBuffer, const Span<const VertexArray::VertexElements>& vertexBufferVec)
    {
        std::vector<Hash64> hashCombineVec;
        const uint64_t hashCombineCount = 1ull + (vertexBufferVec.size() * 2ull);
        hashCombineVec.reserve(hashCombineCount);

        const Hash64 indexBufferAddress = reinterpret_cast<Hash64>(indexBuffer.get());
        hashCombineVec.emplace_back(indexBufferAddress);
        for (const VertexArray::VertexElements& vertexElements : vertexBufferVec)
        {
            const Ref<VertexBuffer>& vertexBuffer = vertexElements.m_Buffer;
            if (nullptr == vertexBuffer)
                continue;

            const Hash64 vertexBufferAddress = reinterpret_cast<Hash64>(vertexBuffer.get());
            const BufferLayout& layout = vertexElements.m_UseLayout;
            const Hash64 vertexBufferLayout = layout.GetHash();

            hashCombineVec.emplace_back(vertexBufferAddress);
            hashCombineVec.emplace_back(vertexBufferLayout);
        }
        const Hash64 hash = robin_hood::hash_bytes(hashCombineVec.data(), hashCombineVec.size());
        return hash;
    }
}

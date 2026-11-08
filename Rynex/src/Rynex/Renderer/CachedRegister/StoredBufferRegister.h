#pragma once
// RegisterBuffer created in Project Rynex-Rendering on 05/10/2026.
#include <Rynex/Renderer/CachedRegister/StoredKey.h>

namespace Rynex {


    template<typename K, typename T, typename Controller>
    class StoredBufferRegister
    {
        struct StoredBuffer
        {
            std::vector<T> m_StoredBufferVec;
            uint64_t m_Frame;           // last frame a buffer of this key was handed out
            int64_t m_Index;            // buffer handed out last, -1 = nothing handed out since the last

            uint64_t m_WindowPeak;      // most buffers handed out in one frame since the last trim
            uint64_t m_WindowPeakFrame;
            uint64_t m_PrevCount;       // buffers handed out in the previous frame

            StoredBuffer()
                : m_StoredBufferVec()
                , m_Frame(0ull)
                , m_Index(-1)
                , m_WindowPeak(0ull)
                , m_WindowPeakFrame(0ull)
                , m_PrevCount(0ull)
            {
            }

            uint64_t GetBufferCount() const
            {
                return m_StoredBufferVec.size();
            }

            uint64_t GetHandedOutCount() const
            {
                const int64_t count = m_Index + 1ll;
                return static_cast<uint64_t>(count);
            }

            bool HasIdleBuffer() const
            {
                return GetHandedOutCount() < GetBufferCount();
            }

            T* TryNextIdleBuffer(const uint64_t frame)
            {
                if (!HasIdleBuffer())
                {
                    return nullptr;
                }

                ++m_Index;
                m_Frame = frame;
                return &m_StoredBufferVec[static_cast<size_t>(m_Index)];
            }
#if 0
            T& GetNextBuffer(const K& key, const uint64_t frame)
            {
                m_Index++;
                if (m_StoredBufferVec.size() == m_Index)
                {
                    T value = Controller::Create(key);
                    m_StoredBufferVec.emplace_back(value);
                }
                return GetBuffer(frame);
            }
#endif

            T& AddBuffer(const K& key, const uint64_t frame)
            {
                RY_CORE_ASSERT(!HasIdleBuffer(), "Only create a buffer when no idle buffer is left");

                T buffer = Controller::Create(key);
                RY_CORE_ASSERT(nullptr != buffer, "Buffer controller returned no buffer");
                m_StoredBufferVec.emplace_back(buffer);

                ++m_Index;
                m_Frame = frame;
                return m_StoredBufferVec.back();
            }

            T& GetBuffer(const uint64_t frame)
            {
                m_Frame = frame;
                RY_CORE_ASSERT(0 <= m_Index && m_Index < m_StoredBufferVec.size(), "Index out of bounds");
                return m_StoredBufferVec[m_Index];
            }

            void NewFrame()
            {
                m_PrevCount = GetHandedOutCount();
                if (m_WindowPeak < m_PrevCount)
                {
                    m_WindowPeakFrame = m_Frame;
                    m_WindowPeak = m_PrevCount;
                }
                m_Index = -1ll;
            }

            /**
             * count alle buffers behind the @keepCount that can be removed.
             * @param keepCount
             * @return
             */
            uint64_t CountRemovableAfter(const uint64_t keepCount) const
            {
                const uint64_t handedOut = GetHandedOutCount();
                const uint64_t keep = std::max(keepCount, handedOut);
                uint64_t index = GetBufferCount();
                uint64_t removable = 0ull;
                while (keep < index)
                {
                    --index;
                    const T& buffer = m_StoredBufferVec[index];
                    RY_CORE_ERROR_IF(Controller::IsReferencedElseWhere(buffer), "References are still is found else where!");

                    ++removable;
                }
                return removable;
            }

            /**
             * Remove on range all old buffer (old > 600 frames with out using).
             * @param keepCount how many buffer we keep alive.
             * @return
             */
            uint64_t RemoveIdleAfter(const uint64_t keepCount)
            {

                const uint64_t removable = CountRemovableAfter(keepCount);
                const uint64_t count = GetBufferCount();
                const uint64_t newCount = count - removable;

                const auto begin = m_StoredBufferVec.begin();
                const auto itErase = begin + static_cast<int64_t>(newCount);
                const auto end = m_StoredBufferVec.end();
                m_StoredBufferVec.erase(itErase, end);
                return removable;
            }

            T* NextIdleBuffer(const uint64_t frame)
            {
                if (!HasIdleBuffer())
                    return nullptr;


                ++m_Index;
                m_Frame = frame;
                return &m_StoredBufferVec.at(m_Index);
            }

            void Clear()
            {
                m_StoredBufferVec.clear();
                IndexOnStart();
            }

            void IndexOnStart()
            {
                m_Index = -1;
            }
#if 0
            uint64_t RemoveBufferAfterIndex()
            {
                uint64_t nextBufferIndex = m_Index + 1;
                const uint64_t count = m_StoredBufferVec.size();
                const uint64_t removeCount = count - nextBufferIndex;

                const auto it = m_StoredBufferVec.begin();
                const auto itErase = it + nextBufferIndex;
                const auto end = m_StoredBufferVec.end();

                m_StoredBufferVec.erase(itErase, end);
                return removeCount;
            }
#endif
        };
    public:
        using Key = K;
        using Unit = uint32_t;
        using Value = StoredBuffer;
        using Buffer = T;
        using Container = HashMapFlat<Key, Value>;
        using Iterator = typename Container::iterator;
        using IteratorConst = typename Container::const_iterator;

    // public member methode ---------------------------------------------------------------------------------------------------
        StoredBufferRegister()
            : m_Register()
            , m_Frame(0ull)
            , m_ByteSize(0ull)
        {
        }
        
        ~StoredBufferRegister()
        {
        }


        T* TryAcquireBuffer(const Key& key)
        {
            Iterator it = m_Register.find(key);
            if (m_Register.end() == it)
            {
                return nullptr;
            }

            Value& value = it->second;
            return value.NextIdleBuffer(m_Frame);
        }

        T& CreateBuffer(const Key& key)
        {
            auto result = m_Register.emplace(key, Value());
            Value& value = result.first->second;
            T& buffer = value.AddBuffer(key, m_Frame);

            const uint64_t byteSize = Controller::GetByteSize(key);
            m_ByteSize += byteSize;
            return buffer;
        }

        // Convenience for users without a memory budget.
        T& AcquireBuffer(const Key& key)
        {
            T* buffer = TryAcquireBuffer(key);
            if (nullptr != buffer)
            {
                return *buffer;
            }
            return Controller::Create(key);
        }

        Unit GetKeyIndex(const Key& key)
        {
            Iterator it = m_Register.find(key);
            if(it == m_Register.end())
                return 0u;

            Value& value = it->second;
            return value.m_Index;
        }

        void Clear()
        {
            m_Register.clear();
            m_ByteSize = 0ull;
        }

        void ResetBuffer()
        {
            for (auto& [key, value] : m_Register)
            {
                value.IndexOnStart();
            }
        }

        void SetFrame(const uint64_t frame)
        {
            if (frame == m_Frame)
                return;
            m_Frame = frame;
            for (auto& [key, value] : m_Register)
            {
                value.NewFrame();
            }

        }

        // Removes idle buffers of keys that were not handed out since maxFrame (inclusive). Returns the freed bytes.
        uint64_t TrimUnused(const uint64_t maxFrame)
        {
            uint64_t freedBytes = 0ull;
            Iterator it = m_Register.begin();
            while (m_Register.end() != it)
            {
                Value& value = it->second;
                if (maxFrame < value.m_Frame)
                {
                    ++it;
                    continue;
                }
                constexpr uint32_t keepCount = 0u; // how many Buffer we need to keep at max!
                const uint64_t removed = value.RemoveIdleAfter(keepCount);
                const uint64_t keyByteSize = Controller::GetByteSize(it->first);
                const uint64_t removedBytes = removed * keyByteSize;
                freedBytes += removedBytes;

                if (value.m_StoredBufferVec.empty())
                {
                    it = m_Register.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            m_ByteSize -= freedBytes;
            return freedBytes;
        }

        // Garbage collection: shrinks every key to the most buffers it needed in one frame since the last call.windowPeakRelevance
        uint64_t TrimToWindowPeak(uint64_t frame, uint64_t minFrameDifference = 600ull)
        {
            uint64_t freedBytes = 0ull;
            Iterator it = m_Register.begin();
            while (m_Register.end() != it)
            {
                Value& value = it->second;
                const uint64_t peakWindowFrameDifference = frame - value.m_WindowPeakFrame;
                uint64_t windowPeakRelevance = value.m_WindowPeak;
                if (minFrameDifference < peakWindowFrameDifference)
                    windowPeakRelevance = 0ull == value.m_PrevCount ? 0ull : (value.m_PrevCount / value.m_WindowPeak);

                const uint64_t keep = std::max(windowPeakRelevance, value.m_PrevCount);
                const uint64_t removed = value.RemoveIdleAfter(keep);
                const uint64_t keyByteSize = Controller::GetByteSize(it->first);
                const uint64_t removedBytes = removed * keyByteSize;
                freedBytes += removedBytes;
                value.m_WindowPeak = keep;

                if (value.m_StoredBufferVec.empty())
                {
                    it = m_Register.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            m_ByteSize -= freedBytes;
            return freedBytes;
        }

        // Bytes that could be freed right now (idle buffers nobody else holds). Iterates the whole register.
        uint64_t GetIdleByteSize() const
        {
            uint64_t idleBytes = 0ull;
            for (const auto& entry : m_Register)
            {
                const Value& value = entry.second;
                const uint64_t removable = value.CountRemovableAfter(0ull);
                const uint64_t keyByteSize = Controller::GetByteSize(entry.first);
                const uint64_t removableBytes = removable * keyByteSize;
                idleBytes += removableBytes;
            }
            return idleBytes;
        }

        uint64_t GetKeyCount() const
        {
            return m_Register.size();
        }

        uint64_t GetBufferCount() const
        {
            uint64_t count = 0ull;
            for (const auto& entry : m_Register)
            {
                count += entry.second.GetBufferCount();
            }
            return count;
        }

        uint64_t GetRegisterBufferByteSize() const
        {
            return m_ByteSize;
        }
    // public static methode ---------------------------------------------------------------------------------------------------
    // public member operator --------------------------------------------------------------------------------------------------
    private:
    // private member variable -------------------------------------------------------------------------------------------------
        Container m_Register;
        uint64_t m_Frame;
        uint64_t m_ByteSize;
    };


}
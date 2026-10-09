#pragma once
// StoredBufferRegister created in Project Rynex-Rendering on 06/10/2026.
#include <Rynex/Renderer/PiplineObjects/DrawList/UniqueKey.h>



namespace Rynex {
    template<typename K, typename T, typename Controller>
    class UniqueBufferRegister
    {
    public:
        struct UniqueBuffer
        {
            T m_Buffer;
            uint64_t m_Frame;

            explicit UniqueBuffer(const T& uniqueBuffer)
                : m_Buffer(uniqueBuffer)
                , m_Frame(0ull)
            { }

            T& GetBuffer(const uint64_t frame)
            {
                m_Frame = frame;
                return m_Buffer;
            }

            T* GetBufferPtr(const uint64_t frame)
            {
                m_Frame = frame;
                return &m_Buffer;
            }

        };
    // public member alias ----------------------------------------------------------------------------------------------------
#ifdef RY_UNIQUE_KEYS_HASH
        using Key = K;
#else
        using Key = K;
        using ConstructData = typename Controller::ConstructData;
#endif
        using Buffer = T;
        using Container = HashMapFlat<Key, UniqueBuffer>;
        using Iterator = typename Container::iterator;
        using IteratorConst = typename Container::const_iterator;
        using Unit = uint32_t;

    // public member methode ---------------------------------------------------------------------------------------------------
        UniqueBufferRegister()
            : m_Register()
            , m_Frame(0ull)
            , m_ByteSize(0)
        {
        }

        ~UniqueBufferRegister()
        {
        }

#ifdef RY_UNIQUE_KEYS_HASH

        T* TryAcquire(const Key& key)
        {
            Iterator it = m_Register.find(key);
            if (m_Register.end() == it)
            {
                return nullptr;
            }

            Value& value = it->second;
            T& buffer = value.GetBuffer(m_Frame);
            return &buffer;
        }

        T& Create(const Key& key)
        {
            Iterator it = m_Register.find(key);
            if (m_Register.end() != m_Register.find(key))
            {
                RY_CORE_INFO("Unique resource already existed.");
                return it->second;
            }
            T buffer = Controller::Create(key);
            const uint64_t byteSize = Controller::GetByteSize(buffer);
            auto result = m_Register.emplace(key, Value(buffer, m_Frame, byteSize));
            RY_CORE_ASSERT(result.second, "Unique key is already registered!");

            m_ByteSize += byteSize;
            return result.first->second.m_Buffer;
        }

        Value& AcquireUnique(const Key& key)
        {
            T* buffer = TryAcquire(key);
            if (nullptr != buffer)
            {
                return *buffer;
            }
            return Controller::Create(key);
        }
#else

        T* TryAcquire(const Key& key)
        {
            Iterator it = m_Register.find(key);
            if (m_Register.end() == it)
            {
                return nullptr;
            }

            UniqueBuffer& value = it->second;
            T* bufferPtr = value.GetBufferPtr(m_Frame);
            return bufferPtr;
        }

        T& Create(const Key& key, const ConstructData& constructData)
        {
            Iterator it = m_Register.find(key);
            if(it != m_Register.end())
            {
                UniqueBuffer& uniqueBuffer = it->second;
                return uniqueBuffer.GetBuffer(m_Frame);
            }

            T buffer =  Controller::Create(constructData);
            std::pair<Iterator, bool> result = m_Register.emplace(key, UniqueBuffer(buffer));
            RY_CORE_ASSERT(result.second, "Unique key is already registered!");

            it = result.first; // resuing the iterator from find.
            UniqueBuffer& uniqueBuffer = it->second;
            T* bufferPtr = uniqueBuffer.GetBufferPtr(m_Frame);

            m_ByteSize += Controller::GetByteSize(*bufferPtr);
            return *bufferPtr;
        }

        T& AcquireUnique(const Key& key, const ConstructData& fallbackData)
        {
            T* buffer = TryAcquire(key);
            if (nullptr != buffer)
            {
                return *buffer;
            }
            return Create(fallbackData);
        }

#endif

        void Clear()
        {
            m_Register.clear();
        }

        void SetFrame(const uint64_t frame)
        {
            m_Frame = frame;
        }

        uint64_t GetRegisterBufferByteSize() const
        {
            return m_ByteSize;
        }

        uint64_t TrimUnused(const uint64_t maxFrame)
        {
            uint64_t freedBytes = 0ull;
            Iterator it = m_Register.begin();
            while (m_Register.end() != it)
            {
                UniqueBuffer& value = it->second;
                const bool isOld = value.m_Frame <= maxFrame;
                if (isOld && !Controller::IsReferencedElsewhere(value.m_Buffer))
                {
                    freedBytes += Controller::GetByteSize(value.m_Buffer);
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

        // Bytes that could be freed right now (resources nobody else holds). Iterates the whole register.
        uint64_t GetIdleByteSize() const
        {
            uint64_t idleBytes = 0ull;
            for (const auto& entry : m_Register)
            {
                const UniqueBuffer& value = entry.second;
                if (!Controller::IsReferencedElsewhere(value.m_Buffer))
                    idleBytes +=  Controller::GetByteSize(value.m_Buffer);

            }
            return idleBytes;
        }

    // public static methode ---------------------------------------------------------------------------------------------------
    // public member operator --------------------------------------------------------------------------------------------------
    private:
    // private static methode --------------------------------------------------------------------------------------------------
    // private member methode --------------------------------------------------------------------------------------------------
    // private member variable -------------------------------------------------------------------------------------------------
        Container m_Register;
        uint64_t m_Frame;
        uint64_t m_ByteSize;
    };


}
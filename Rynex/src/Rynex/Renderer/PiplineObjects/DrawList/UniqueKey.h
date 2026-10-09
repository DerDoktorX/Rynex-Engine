#pragma once
// UniqueKey created in Project Rynex-Rendering on 06/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/API/Buffer.h>

// #define RY_UNIQUE_KEYS_HASH
namespace Rynex {

    class UniqueKey
    {
    public:
        explicit UniqueKey(const Span<Byte>& data, BufferLayout* layoutPtr)
            : m_Data(data)
            , m_LayoutPtr(layoutPtr)
        {
#ifdef RY_UNIQUE_KEYS_HASH
            Hash64 hash = robin_hood::hash_bytes(data.data(), m_Data.size());
            const Hash64 hashArray[2]
            {
                hash,
                m_LayoutPtr->GetHash()
            };
            constexpr size_t byteSize = sizeof(hashArray);
            robin_hood::hash_bytes(&hashArray, byteSize);
#endif
        }

#ifdef RY_UNIQUE_KEYS_HASH
        UniqueKey(const UniqueKey& key)
            : m_Data(nullptr, 0u)
            , m_LayoutPtr(nullptr)
            , m_Hash(key.m_Hash)
        {
        }
#else
        UniqueKey(const UniqueKey& key)
            : m_Data(nullptr, 0u)
            , m_LayoutPtr(nullptr)
        {
        }
#endif

#ifdef RY_UNIQUE_KEYS_HASH
        explicit UniqueKey(const Hash64 hash)
            : m_Data(nullptr, 0)
            , m_LayoutPtr(nullptr)
            , m_Hash(hash)
        {
        }
#endif

        UniqueKey& operator=(const UniqueKey& key)
        {
#ifdef RY_UNIQUE_KEYS_HASH
            m_Hash = key.m_Hash;
#endif
            m_LayoutPtr = nullptr;
            m_Data = Span<Byte>{nullptr, 0 };

            return *this;
        }

        ~UniqueKey()
        {
        }

#ifdef RY_UNIQUE_KEYS_HASH
        Hash64 GetHash() const
        {
            return m_Hash;
        }
#endif
        BufferLayout* GetLayoutPtr() const
        {
            return m_LayoutPtr;
        }

        const Span<Byte>& GetData() const
        {
            return m_Data;
        }

        void ClearData()
        {
            m_Data = Span<Byte>{nullptr,0 };
            m_LayoutPtr = nullptr;
        }

    private:
    // private static methode ------------------------------------------------------------------------------------------------
    // private member methode ------------------------------------------------------------------------------------------------
    // private member variable -----------------------------------------------------------------------------------------------

        Span<Byte> m_Data;
        BufferLayout* m_LayoutPtr;
#ifdef RY_UNIQUE_KEYS_HASH
        Hash64 m_Hash;
#endif
    };
#ifdef RY_UNIQUE_KEYS_HASH
    inline bool operator==(const UniqueKey& a, const UniqueKey& b)
    {
        return a.GetHash() == b.GetHash();
    }

}

namespace robin_hood {
    template<>
    struct hash<Rynex::UniqueKey>
    {
        size_t operator()(const Rynex::UniqueKey& key) const
        {
            return key.GetHash();
        }
    };


}

#else

}
#endif


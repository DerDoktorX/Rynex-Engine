#pragma once
// StoredKey created in Project Rynex-Rendering on 06/10/2026.
#include <rypch.h>


namespace Rynex {

    class StoredKey 
    {
    public:
        explicit StoredKey(const uint64_t byteSize)
        {
            constexpr int32_t maxCount = 2000;
            constexpr uint64_t posBlockSizeBit = 10;

            uint64_t temp = 0;
            uint64_t bitValue = 0;
            int32_t i = 0;

            while (byteSize < bitValue && i < maxCount)
            {
                if (i < posBlockSizeBit)
                {
                   bitValue = BIT(i);
                }
                else
                {
                    const uint64_t byteGroup = ++temp << posBlockSizeBit;
                    bitValue = byteGroup;
                }
                i++;
            }
            RY_CORE_ASSERT(i < maxCount, "max count overflowed!");
            m_ByteGroupSize = byteSize;
        }

        Hash64 GetHash() const
        {
            return m_ByteGroupSize;
        }

        uint64_t GetByteSize() const
        {
            return m_ByteGroupSize;
        }
    private:
    // private static methode ------------------------------------------------------------------------------------------------
    // private member methode ------------------------------------------------------------------------------------------------
    // private member variable -----------------------------------------------------------------------------------------------
        uint64_t m_ByteGroupSize;
    };

    inline bool operator==(const StoredKey& a, const StoredKey& b)
    {
        return a.GetHash() == b.GetHash();
    }
}

namespace robin_hood {
    template<>
    struct hash<Rynex::StoredKey>
    {
        size_t operator()(const Rynex::StoredKey& key) const
        {
            return key.GetHash();
        }
    };
}



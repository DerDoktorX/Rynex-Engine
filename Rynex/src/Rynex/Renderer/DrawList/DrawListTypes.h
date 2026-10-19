#pragma once
// DrawListTypes created in Project Rynex-Rendering on 04/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/ChachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/Rendering/ShaderDrawList.h>

namespace Rynex {



    // Instance fields an instance record can store (InstanceControl::s_Fields)
    // or that a MaterielControl fills (MaterielControl::s_InstanceFields).
    enum InstanceFieldBits : uint32_t
    {
        InstanceField_None           = 0u,
        InstanceField_MaterialIndex  = BIT(0),
        InstanceField_ParameterIndex = BIT(1),
        InstanceField_TextureIndex   = BIT(2),
        InstanceField_All            = InstanceField_MaterialIndex | InstanceField_ParameterIndex | InstanceField_TextureIndex
    };

    enum class DrawSplit
    {
        None = 0,
        Same,
        NewCommand,
        NewDraw
    };

    enum ShaderFeatureBits
    {
        ShaderFeatureBits_None = 0,
        ShaderFeatureBits_BaseInstance = BIT(0),
        ShaderFeatureBits_DrawID = BIT(1),
        ShaderFeatureBits_Bindless = BIT(2),
        ShaderFeatureBits_CountDraw = BIT(3),
        ShaderFeatureBits_MultiViewLayer = BIT(4)
    };

    // Everything an InstanceControl may read from the caller for one instance. New field = new member, Add() keeps its shape.
    struct InstanceSource
    {
        const glm::mat4& m_Model;
        uint32_t m_EntityID;
    };

    // One draw call worth of instances. Always inside ONE group, so Bind() of the controls has exactly one state.
    struct DrawChunk
    {
        uint32_t m_Section;
        uint32_t m_FirstInstance;
        uint32_t m_InstanceCount;
    };

    struct NoIndices{ };

    // Fixed bindings of the template shader draw (camera, ...). All masks are uint64_t: g_UniformBindArrayCount is 36.
    struct NoTemplateSlots
    {
        static constexpr uint64_t s_AttributeLocationMask = 0ull;
        static constexpr uint64_t s_VertexBufferMask = 0ull;
        static constexpr uint64_t s_UniformSlotMask = 0ull;
        static constexpr uint64_t s_StorageSlotMask = 0ull;
        static constexpr uint64_t s_TextureSlotMask = 0ull;
    };

    // Measurable per frame counters (FEATURE.md R1).
    struct DrawListStats
    {
        uint32_t m_InstanceCount = 0u;
        uint32_t m_GroupCount = 0u;
        uint32_t m_ChunkCount = 0u;
        uint32_t m_ByteSize = 0u;
    };

    // What one geometry contributes to a command. The GeometryControl fills it, the CommandControl builds its DrawObject from it.
    // Meaning of m_VertexElementCount (vertices or VertexElements) is open, FEATURE.md Q16.
    struct GeometryDrawInfo
    {
        uint32_t m_IndexCount = 0u;
        uint32_t m_VertexElementCount = 0u;
        uint32_t m_FirstIndex = 0u;
        int32_t m_BaseVertex = 0;
    };

    // True when no bit is set in more than one of the four masks.
    constexpr bool AreMasksDisjoint(const uint64_t first, const uint64_t second, const uint64_t third, const uint64_t fourth, const uint64_t five)
    {
        const uint64_t firstSecond = first & second;
        const uint64_t firstThird = first & third;
        const uint64_t firstFourth = first & fourth;
        const uint64_t firstFive = first & five;
        const uint64_t secondThird = second & third;
        const uint64_t secondFourth = second & fourth;
        const uint64_t secondFive = second & five;
        const uint64_t thirdFourth = third & fourth;
        const uint64_t thirdFive = third & five;
        const uint64_t fourthFive = fourth & five;


        uint64_t overlap = firstSecond | firstThird;
        overlap |= firstFourth;
        overlap |= firstFive;
        overlap |= secondThird;
        overlap |= secondFourth;
        overlap |= secondFive;
        overlap |= thirdFourth;
        overlap |= thirdFive;
        overlap |= fourthFive;
        return 0ull == overlap;
    }

    constexpr uint32_t CountBits(const uint64_t slotMask)
    {
        uint32_t count = 0u;
        for (int i = 0; i < 64; i++)
        {
            constexpr uint64_t bitValue = BIT(i);
            if (0 != (slotMask & bitValue))
                count++;
        }
        return count;
    }

    constexpr bool GapeBetweenBufferSlots(const uint64_t slotMask)
    {
        constexpr int countBit = sizeof(uint64_t) * 8;
        constexpr int lastBitPos = countBit - 1;
        constexpr int bitCount = lastBitPos - 1;
        for (int i = bitCount; 0 <= i; i--)
        {
            constexpr uint64_t bitValue = BIT(i);
            if (0 != (slotMask & bitValue))                             // { 0111                   | 0101 }
            {
                constexpr int indexBevor = i + 1;
                constexpr uint64_t bitValueBevor = BIT(indexBevor);     // 1000
                constexpr uint64_t onlyOneBitExpected = slotMask + 1;   // { 0111 + 1 = 1000        | 0101 + 1 = 0110 }
                return bitValueBevor != onlyOneBitExpected;             // { 1000 != 1000 = false   | 1000 != 0110 = true }
            }
        }
        // no bit set so there is no gape between bits
        return false;
    }
}
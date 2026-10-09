#pragma once
// DrawListTypes created in Project Rynex-Rendering on 04/10/2026.
#include <rypch.h>
#include <Rynex/Renderer/ChachedRegister/CachedBufferRegister.h>

namespace Rynex {



    // Instance fields an instance record can store (InstanceControl::s_Fields)
    // or that a MaterielControl fills (MaterielControl::s_InstanceFields).
    enum InstanceFieldBits : uint32_t
    {
        InstanceField_None           = 0u,
        InstanceField_MaterialIndex  = 1u << 0u,
        InstanceField_ParameterIndex = 1u << 1u,
        InstanceField_TextureIndex   = 1u << 2u,
        InstanceField_All            = InstanceField_MaterialIndex | InstanceField_ParameterIndex | InstanceField_TextureIndex
    };



    // One draw call worth of instances. Always inside ONE group, so Bind() of the controls has exactly one state.
    struct DrawChunk
    {
        uint32_t m_Group;
        uint32_t m_FirstInstance;
        uint32_t m_InstanceCount;
    };

    template<size_t VertexCount, size_t IndexCount>
    struct BufferOutPut
    {

        std::array<VertexArray::VertexElements, VertexCount> m_VertexBufferArray;
        std::array<Ref<StorageBuffer>, IndexCount> m_StorageBufferArray;
    };

    template<typename T, typename N>
    typename std::array<VertexArray::VertexElements, (T::VertexCount + N::InstanceCount)> AcquirerVertexBufferArrayFromBufferOutPut(const T& outPutGeometry, const N& outPutInstance)
    {
        constexpr size_t VertexCount = T::VertexCount + N::InstanceCount;
        std::array<VertexArray::VertexElements, VertexCount> vertexBufferArray;
        int32_t i = 0;
        for (const VertexArray::VertexElements& vertexElements : outPutGeometry.m_VertexBufferArray)
        {
            vertexBufferArray[i] =vertexElements;
            i++;
        }
        for (const VertexArray::VertexElements& vertexElements : outPutInstance.m_VertexBufferArray)
        {
            vertexBufferArray[i] = vertexElements;
            i++;
        }
        return vertexBufferArray;


    }

}
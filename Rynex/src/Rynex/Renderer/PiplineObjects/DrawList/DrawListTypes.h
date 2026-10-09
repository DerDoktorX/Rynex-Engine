#pragma once
// DrawListTypes created in Project Rynex-Rendering on 04/10/2026.
#include <rypch.h>

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

    // MaterielControl::Indices for controls without per instance indices.
    struct NoIndices
    {
    };

    // One draw call worth of instances. Always inside ONE group, so Bind() of the controls has exactly one state.
    struct DrawChunk
    {
        uint32_t m_Group;
        uint32_t m_FirstInstance;
        uint32_t m_InstanceCount;
    };

}
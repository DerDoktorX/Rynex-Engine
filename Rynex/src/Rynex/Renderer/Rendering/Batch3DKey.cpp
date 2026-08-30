#include "rypch.h"
#include "Batch3DKey.h"

namespace Rynex {

    Batch3DKey::Batch3DKey()
        : m_Materiel(nullptr)
        , m_Mesh(nullptr)
        , m_Shader(nullptr)
        , m_DrawSpec(0)
        , m_RenderMode(0)
        , m_LOD_Level(0)
    {
    }

    Batch3DKey::Batch3DKey(const Ref<Material>& materiel, const Ref<MeshSingle>& mesh, const BufferLayout& outPut, int lodTier)
        : m_Mesh(mesh)
        , m_Materiel(materiel)
        , m_LOD_Level(lodTier)
        
    {

        const Ref<Shader>& shader = m_Materiel->GetShaderForDraw(outPut, m_LOD_Level);
        int drawSpec = m_Materiel->GetDrawSpecification(outPut, m_LOD_Level);
        int renderMode = m_Materiel->GetRenderMode(outPut, m_LOD_Level);

        m_Shader = shader;
        m_DrawSpec = drawSpec;
        m_RenderMode = renderMode;
    }

    Batch3DKey::operator uint64_t() const
    {
        uint64_t hash = this->GetHash();
        return hash;
    }

    uint64_t Batch3DKey::GetHash() const
    {
#ifdef RY_BATCH_KEY_HASH_FUNC        

        constexpr uint64_t lockDrawSpecBitsOne = BIT(DrawSpecification::s_TextureModeBitMove + 1) - 1;
        constexpr uint64_t lockDrawSpecBitsinvert = ~lockDrawSpecBitsOne;

        uint64_t hash = m_DrawSpec;
        constexpr int geomtryBitDrawSpec = DrawSpecification::ResurceType::Geometry;
        constexpr int materielBitDrawSpec = DrawSpecification::ResurceType::MaterielPram | DrawSpecification::ResurceType::MaterielTex;

        constexpr uint32_t shaderBitCount = 15;
        constexpr uint32_t meshBitsCount = 15;
        constexpr uint32_t materielBitsCount = 15;

        constexpr uint64_t shaderBitsOne = BIT(shaderBitCount) - 1;
        constexpr uint64_t meshBitsOne = BIT(meshBitsCount) - 1;
        constexpr uint64_t materielBitsOne = BIT(materielBitsCount) - 1;
        uint32_t bitsUsed = DrawSpecification::s_DrawSpecificationCount;

        if (m_DrawSpec != 0)
        {
            uint64_t shaderPtrNumber = reinterpret_cast<uint64_t>(m_Shader.get());
            uint64_t shaderBits = shaderBitsOne << bitsUsed;
            shaderBits ^= shaderBits & shaderPtrNumber;
            hash ^= shaderBits;

            bitsUsed += shaderBitCount;
        }
       
        

        if ((m_DrawSpec & geomtryBitDrawSpec) == 0)
        {
            uint64_t meshPtrNumber = reinterpret_cast<uint64_t>(m_Mesh.get());
#if 1
            uint64_t meshBits = meshBitsOne << bitsUsed;
            hash ^= meshPtrNumber & meshBits;
            bitsUsed += meshBitsCount;
#else
            hash ^= lockDrawSpecBitsinvert & materielBits;
#endif
        }
        if ((m_DrawSpec & materielBitDrawSpec) == 0)
        {
            uint64_t materielPtrNumber = reinterpret_cast<uint64_t>(m_Materiel.get());
#if 1
            uint64_t materielBits = materielBitsOne << bitsUsed;
            hash ^= materielBits & materielBits;
            bitsUsed += materielBitsCount;
#else
            hash ^= lockDrawSpecBitsinvert & materielBits;
#endif
        }
#else
        uint64_t hash = m_DrawSpec;

        constexpr int geomtryBitDrawSpec = DrawSpecification::ResurceType::Geometry;
        constexpr int materielBitDrawSpec = DrawSpecification::ResurceType::MaterielPram | DrawSpecification::ResurceType::MaterielTex;
        uint64_t bitsUsed = 0ull;
        if (0 != m_DrawSpec)
            hash = GetShaderHash(hash, bitsUsed, m_Shader);
        if (0 == (m_DrawSpec & geomtryBitDrawSpec))
            hash = GetMeshHash(hash, bitsUsed, m_Mesh);
        if (0 == (m_DrawSpec & materielBitDrawSpec))
            hash = GetMaterielHash(hash, bitsUsed, m_Materiel);

#endif
        return hash;
    }

    uint64_t Batch3DKey::Mix64(uint64_t value)
    {
        value ^= value >> 30;
        value *= 0xbf58476d1ce4e5b9;
        value ^= value >> 27;
        value *= 0x94d049bb133111eb;
        value ^= value >> 31;
        return value;
    }

    uint64_t Batch3DKey::GetMaterielHash(uint64_t hash, uint64_t& bitsUsed, const Ref<Material>& materiel)
    {
#ifdef RY_BATCH_KEY_HASH
        constexpr uint32_t materielBitsCount = 15;
        constexpr uint64_t materielBitsOne = BIT(materielBitsCount) - 1;

        uint64_t materielPtrNumber = reinterpret_cast<uint64_t>(materiel.get());
        uint64_t materielBits = materielBitsOne << bitsUsed;
        hash ^= materielBits & materielBits;
        bitsUsed += materielBitsCount;
#else
        uint64_t hashPtr = robin_hood::hash<void*>{}(materiel.get());
        uint64_t hashPtrCombineValue = hash ^ hashPtr;
        hash = Mix64(hashPtrCombineValue);
#endif
        return hash;
    }

    uint64_t Batch3DKey::GetMeshHash(uint64_t hash, uint64_t& bitsUsed, const Ref<MeshSingle>& mesh)
    {
#ifdef RY_BATCH_KEY_HASH        
        constexpr uint32_t meshBitsCount = 15;
        constexpr uint64_t meshBitsOne = BIT(meshBitsCount) - 1;

        uint64_t meshPtrNumber = reinterpret_cast<uint64_t>(mesh.get());
#if 1
        uint64_t meshBits = meshBitsOne << bitsUsed;
        hash ^= meshPtrNumber & meshBits;
        bitsUsed += meshBitsCount;
#else
        hash ^= lockDrawSpecBitsinvert & materielBits;
#endif

#else
        uint64_t hashPtr = robin_hood::hash<void*>{}(mesh.get());
        uint64_t hashPtrCombineValue = hash ^ hashPtr;
        hash = Mix64(hashPtrCombineValue);
#endif


        return hash;
    }

    uint64_t Batch3DKey::GetShaderHash(uint64_t hash, uint64_t& bitsUsed, const Ref<Shader>& shader)
    {

#ifdef RY_BATCH_KEY_HASH        
        constexpr uint32_t shaderBitCount = 15;
        constexpr uint64_t shaderBitsOne = BIT(shaderBitCount) - 1;
        uint64_t shaderPtrNumber = reinterpret_cast<uint64_t>(shader.get());

        uint64_t shaderBits = shaderBitsOne << bitsUsed;
        shaderBits ^= shaderBits & shaderPtrNumber;
        hash ^= shaderBits;

        bitsUsed += shaderBitCount;
#else
        uint64_t hashPtr = robin_hood::hash<void*>{}(shader.get());
        uint64_t hashPtrCombineValue = hash ^ hashPtr;
        hash = Mix64(hashPtrCombineValue);
#endif
        return hash;

    }
}
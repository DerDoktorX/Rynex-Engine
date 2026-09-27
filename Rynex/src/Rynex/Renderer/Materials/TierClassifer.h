#pragma once
#include <Rynex/Renderer/Materials/Material.h>

#define RY_TIER_GRANULARITY_PER_BATCH

// #define RY_TIER_GRANULARITY_PER_MATERIAL

#if defined(RY_TIER_GRANULARITY_PER_BATCH) && defined(RY_TIER_GRANULARITY_PER_MATERIAL)
    #error "Nur eine Granularitaets-Strategie darf aktiv sein."
#endif

namespace Rynex {
    struct RenderMeshBatch;

    class TierClassiefer
    {
#ifdef RY_TIER_GRANULARITY_PER_BATCH
        // a decision for the hole (Mesh-) group.
        // Conflict rule to difference Material-Preference: open,
        // static BatchPresets::BatchProfile ClassifyGroup(/* MeshBatchGroup& group */);
        static BatchPresets::BatchProfile ClassifyGroup(RenderMeshBatch& group);
#endif

#ifdef RY_TIER_GRANULARITY_PER_MATERIAL
        // Every (Mesh,Material)-Pair get his own Tier;
        // grouping start right after withe the key (Mesh, BatchProfile).
        static BatchPresets::BatchProfile ClassifyObject(/* const Ref<Material>& material, const RenderProxy& proxy */);
#endif
    };
}

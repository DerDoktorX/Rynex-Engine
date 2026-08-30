#pragma once
#pragma once
// ============================================================================
//  SceneRendererTypes.h  –  Rynex Engine
//
//  Enumerations, flag types, and small shared constants used across the entire
//  SceneRenderer subsystem.  Every other SceneRenderer header depends on this
//  one; it must remain dependency-free (only Base.h allowed).
// ============================================================================

#include <rypch.h>

namespace Rynex {

    // -- System-wide limit -------------------------------------------------------
    // Defined here so SceneRenderResources and SceneLightEnvironment share the
    // same value instead of maintaining two independent hard-coded constants.
    static constexpr uint32_t kMaxShadowMaps = 8u;

    // -- Rendering technique for an entire pipeline -----------------------------
    enum class SceneRenderMode : uint8_t
    {
        Forward = 0,    // All lights evaluated per fragment (simple)
        ForwardPlus = 1,    // Tile-based light culling (compute) + forward shading
        Custom = 2,    // User-defined pipeline
    };

    // -- Identifies the role / ordering semantics of a RenderPass ---------------
    enum class RenderPassType : uint8_t
    {
        None = 0,
        DepthPrePass = 1,       // Depth-only pre-pass; required for Forward+
        ShadowDepth = 2,        // Shadow map generation (one pass per caster)
        LightCulling = 3,       // Forward+ compute culling pass
        Opaque = 4,             // Opaque 3-D geometry
        Transparent = 5,        // Alpha-blended geometry (back-to-front sorted)
        EntityPicking = 6,      // Integer entity-ID render for mouse picking
        EntityOutline = 7,      // Coloured outline / glow on selected entities
        Sprite2D = 8,           // Batched 2-D quads / sprites
        Text = 9,               // SDF text rendering
        PostProcess = 10,       // Full-screen or compute post-processing
        EditorOverlay = 11,     // Editor-only visuals (icons, grid, debug)
        Custom = 12,
    };

    // -- Shadow filtering technique ----------------------------------------------
    enum class ShadowTechnique : uint8_t
    {
        None = 0,
        Hard = 1,
        PCF = 2,    // Percentage-Closer Filtering
        PCSS = 3,   // Percentage-Closer Soft Shadows
    };

    // -- CPU-side frustum culling strategy --------------------------------------
    enum class FrustumCullMode : uint8_t
    {
        None = 0,
        CPU = 1,   // BoundingVolume vs. camera frustum on the CPU
        GPU = 2,   // Compute-shader culling (future extension)
    };

    // -- Per-pass GPU state flags ------------------------------------------------
    using RenderPassFlags = uint16_t;

    namespace RenderPassFlag
    {
        enum : RenderPassFlags
        {
            None = 0,
            WriteEntityID     = BIT(0),         // Emit entity ID to a RED_INTEGER attachment
            EnableDepthTest   = BIT(1),
            EnableDepthWrite  = BIT(2),
            EnableBlending    = BIT(3),
            FrustumCull       = BIT(4),         // Cull commands using camera frustum
            ClearColor        = BIT(5),
            ClearDepth        = BIT(6),
            ClearStencil      = BIT(7),
            IsCompute         = BIT(8),         // Pass dispatches compute work
            UseIndirectDraw   = BIT(9),         // GPU-driven multi-draw indirect
            WireFrame         = BIT(10),        // Debug polygon fill override
        };

        // -- Preset combinations -------------------------------------------------
        static constexpr RenderPassFlags Default = EnableDepthTest | EnableDepthWrite | ClearColor | ClearDepth | FrustumCull;

        static constexpr RenderPassFlags Default2D = EnableBlending | ClearColor;

        static constexpr RenderPassFlags ShadowDefault = EnableDepthTest | EnableDepthWrite | ClearDepth;

        static constexpr RenderPassFlags PickingDefault = EnableDepthTest | EnableDepthWrite | ClearColor | ClearDepth;

    } // namespace RenderPassFlag


    // -- Renderer statistics – standalone so users can forward-declare it --------
    // (Originally nested inside SceneRenderer; moved here so subsystems can
    //  reference it without pulling in the full renderer header.)
    struct RYNEX_API SceneRendererStats
    {
        // Draw calls
        uint32_t drawCalls = 0u;
        uint32_t dispatchCalls = 0u;   // compute dispatches

        // Geometry
        uint32_t triangleCount = 0u;
        uint32_t vertexCount = 0u;
        uint32_t indexCount = 0u;

        // Entities
        uint32_t entitiesSubmitted = 0u;
        uint32_t entitiesRendered = 0u;
        uint32_t entitiesCulled = 0u;

        // Lights
        uint32_t directionalLights = 0u;
        uint32_t pointLights = 0u;
        uint32_t spotLights = 0u;
        uint32_t shadowCasters = 0u;

        // Batching / instancing
        uint32_t batchCount = 0u;
        uint32_t instanceCount = 0u;

        // Timing (microseconds)
        int64_t  sceneSubmitTime = 0;
        int64_t  shadowPassTime = 0;
        int64_t  geometryPassTime = 0;
        int64_t  lightCullingTime = 0;
        int64_t  totalRenderTime = 0;
    };

} // namespace Rynex
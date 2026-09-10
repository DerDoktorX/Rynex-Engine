#pragma once
// ============================================================================
//  SceneLightData.h  –  Rynex Engine
//
//  All CPU-side and GPU-side light structs, the runtime SceneLightEnvironment,
//  and the separate ForwardPlusConfig.
//
//  Key changes from original:
//   - ForwardPlusConfig is a new struct extracted from SceneLightEnvironment.
//     Tile size and max lights-per-tile are a *rendering technique* concern, not
//     a lighting concern.  SceneLightEnvironment is now technique-agnostic.
//   - SceneLightEnvironment::MaxShadowCasters now uses kMaxShadowMaps (from
//     SceneRendererTypes.h) as its default so both resource limits stay in sync.
//   - GPU structs are in the same file because they map 1:1 to the CPU structs
//     and keeping them adjacent makes it easy to verify std140 packing.
// ============================================================================

#include <rypch.h>
#include <Rynex/Renderer/SceneRender/SceneRendererTypes.h>

#include <entt/entt.hpp>

namespace Rynex {

    // ============================================================================
    //  CPU-SIDE PER-LIGHT STRUCTS
    //  Gathered from the ECS each frame.  EntityHandle allows passes to associate
    //  a render artifact (shadow map, outline) with a specific entity.
    // ============================================================================

    struct RYNEX_API DirectionalLightData
    {
        glm::vec3       direction = glm::vec3(0.0f, -1.0f, 0.0f);
        glm::vec3       color = glm::vec3(1.0f);
        float           intensity = 1.0f;
        bool            castShadow = false;
        uint32_t        shadowMapIndex = UINT32_MAX;   // UINT32_MAX = no shadow map assigned
        entt::entity    entity = entt::null;
    };

    struct RYNEX_API PointLightData
    {
        glm::vec3       position = glm::vec3(0.0f);
        glm::vec3       color = glm::vec3(1.0f);
        float           intensity = 1.0f;
        float           range = 10.0f;
        float           constant = 1.0f;
        float           linear = 0.022f;
        float           quadratic = 0.0019f;
        bool            castShadow = false;
        uint32_t        shadowMapIndex = UINT32_MAX;
        entt::entity    entity = entt::null;
    };

    struct RYNEX_API SpotLightData
    {
        glm::vec3       position = glm::vec3(0.0f);
        glm::vec3       direction = glm::vec3(0.0f, -1.0f, 0.0f);
        glm::vec3       color = glm::vec3(1.0f);
        float           intensity = 1.0f;
        float           Range = 30.0f;
        float           InnerConeAngle = 0.95f;    // cos of inner half-angle
        float           OuterConeAngle = 0.90f;    // cos of outer half-angle
        bool            CastShadow = false;
        uint32_t        ShadowMapIndex = UINT32_MAX;
        entt::entity    Entity = entt::null;
    };


    // ============================================================================
    //  RUNTIME LIGHT ENVIRONMENT
    //  Built from ECS data before each pipeline execution.  The renderer does not
    //  query the ECS directly – the caller fills this via SceneRenderer::Submit*
    //  or SceneRenderer::SubmitLightsFromScene().
    //
    //  No Forward+ configuration here: that belongs in ForwardPlusConfig (§C)
    //  and LightCullingPass::Config.
    // ========================================================================================================================

    struct RYNEX_API SceneLightEnvironment
    {
        std::vector<DirectionalLightData>   directionalLights;
        std::vector<PointLightData>         pointLights;
        std::vector<SpotLightData>          spotLights;

        // -- Shadow settings shared across all shadow depth passes -----------------------------------------------------------
        // MaxShadowCasters defaults to the system limit (kMaxShadowMaps) so the
        // two constants are always in sync; callers may lower it to save memory.
        uint32_t        maxShadowCasters = kMaxShadowMaps;
        ShadowTechnique shadowTech = ShadowTechnique::PCF;
        uint32_t        sampleCountPCF = 4u;

        // -- Ambient ---------------------------------------------------------------------------------------------------------
        glm::vec3       ambientColor = glm::vec3(0.1f);
        float           ambientIntensity = 1.0f;

        // -- Helpers ---------------------------------------------------------------------------------------------------------
        void     Clear();
        bool     Empty()            const;
        uint32_t TotalLightCount()  const;  // dir + point + spot
        uint32_t ShadowCasterCount()const;  // lights with CastShadow == true
    };


    // ========================================================================================================================
    //  FORWARD+ TILE CONFIGURATION
    //  Extracted from the original SceneLightEnvironment because tile dimensions
    //  are a property of the rendering technique, not of the lights themselves.
    //  Stored in SceneRenderContext and consumed by LightCullingPass::Config.
    // ============================================================================

    struct RYNEX_API ForwardPlusConfig
    {
        glm::uvec2  tTileSize = { 16u, 16u };
        uint32_t    maxLightsPerTile = 256u;

        // Derived helpers (bodies in SceneRenderer.cpp)
        glm::uvec2 TileCount(glm::uvec2 viewportSize) const;
        uint32_t   TotalLightIndexCount(glm::uvec2 viewportSize) const;
    };


    // ============================================================================
    //  GPU-SIDE LIGHT STRUCTS  (std140 / std430 compatible)
    //  Packed to match GLSL layout declarations exactly.
    //  Each vec3 field is padded to vec4 as required by std140.
    // ============================================================================

    // Must match:  layout(std430) readonly buffer DirectionalLights { GPU_DirectionalLight lights[]; };
    struct RYNEX_API GPU_DirectionalLight
    {
        glm::vec4   directionIntensity; // xyz = direction (world),  w = intensity
        glm::vec4   colorShadow;        // xyz = color,              w = cast shadow (0/1)
        glm::mat4   lightSpaceMatrix;   // light view-projection
    };
    static_assert(sizeof(GPU_DirectionalLight) == 2 * sizeof(glm::vec4) + sizeof(glm::mat4), "GPU_DirectionalLight layout mismatch – check std140 packing");

    // Must match:  layout(std430) readonly buffer PointLights { GPU_PointLight lights[]; };
    struct RYNEX_API GPU_PointLight
    {
        glm::vec4   positionRange;      // xyz = world position,     w = range
        glm::vec4   colorIntensity;     // xyz = color,              w = intensity
        glm::vec4   attenuationShadow;  // x = const, y = lin, z = quad, w = shadow index (-1 = none)
    };
    static_assert(sizeof(GPU_PointLight) == 3 * sizeof(glm::vec4), "GPU_PointLight layout mismatch");

    // Must match:  layout(std430) readonly buffer SpotLights { GPU_SpotLight lights[]; };
    struct RYNEX_API GPU_SpotLight
    {
        glm::vec4   positionRange;      // xyz = world position,     w = range
        glm::vec4   directionIntensity; // xyz = direction (world),  w = intensity
        glm::vec4   colorShadow;        // xyz = color,              w = cast shadow (0/1)
        glm::vec4   coneAngles;         // x = inner cos, y = outer cos, z-w = unused
    };
    static_assert(sizeof(GPU_SpotLight) == 4 * sizeof(glm::vec4), "GPU_SpotLight layout mismatch");

    // CPU → GPU conversion helpers (bodies in SceneRenderer.cpp)
    RYNEX_API GPU_DirectionalLight ToGPU(const DirectionalLightData& light, const glm::mat4& lightSpaceMatrix = glm::mat4(1.0f));
    RYNEX_API GPU_PointLight       ToGPU(const PointLightData& light);
    RYNEX_API GPU_SpotLight        ToGPU(const SpotLightData& light);

} // namespace Rynex
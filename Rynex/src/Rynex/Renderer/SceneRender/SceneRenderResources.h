#pragma once
// ============================================================================
//  SceneRenderResources.h  –  Rynex Engine
//
//  GPU-side buffers owned by the SceneRenderer and shared – by reference –
//  across all RenderPass instances.  Avoids redundant uploads and ensures
//  every pass reads identical per-frame data.
//
//  Lifecycle: Init(w,h) once, Resize(w,h) on viewport change, Shutdown() on
//  destruction.  All Ref<> members are null-initialized; Init() allocates
//  them.
// ============================================================================

#include <Rynex/Core/Base.h>
#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/SceneRender/SceneRendererTypes.h>  // kMaxShadowMaps

#include <array>
#include <cstdint>

namespace Rynex {

    struct RYNEX_API SceneRenderResources
    {
        // -- Camera ----------------------------------------------------------------
        // std140 UBO layout: View, Projection, ViewProjection (3×mat4),
        // Inverse variants (3×mat4), Position (vec4), NearFarFovAspect (vec4).
        Ref<UniformBuffer>  cameraUBO;

        // -- Lighting -------------------------------------------------------------
        Ref<UniformBuffer>  lightCountsUBO;     // uvec4 { dirCount, pointCount, spotCount, pad }
        Ref<StorageBuffer>  dirLightSSBO;       // GPU_DirectionalLight[]
        Ref<StorageBuffer>  pointLightSSBO;     // GPU_PointLight[]
        Ref<StorageBuffer>  spotLightSSBO;      // GPU_SpotLight[]
        Ref<UniformBuffer>  ambientUBO;         // vec4 { r, g, b, intensity }

        // -- Shadow maps -----------------------------------------------------------
        // kMaxShadowMaps is the hard system limit; SceneLightEnvironment::MaxShadowCasters
        // is the user-configurable soft limit (≤ kMaxShadowMaps).
        static constexpr uint32_t MaxShadowMaps = kMaxShadowMaps;

        std::array<Ref<Texture>, MaxShadowMaps> whadowMaps;
        std::array<Ref<Framebuffer>, MaxShadowMaps> whadowFramebuffers;
        Ref<UniformBuffer>  ShadowMatrixUBO;    // mat4 LightSpaceMatrix[MaxShadowMaps]
        Ref<Shader>         ShadowDepthShader;  // shared depth-only shader

        // -- Forward+ resources ----------------------------------------------------
        // Populated only when a LightCullingPass is in the pipeline; null otherwise.
        Ref<Texture>        depthPrepassTexture;         // output of DepthPrePass
        Ref<StorageBuffer>  visibleLightIndicesSSBO;     // flat per-tile index lists
        Ref<UniformBuffer>  tileGridUBO;                 // uvec4 { tileCount.xy, tileSize.xy }
        Ref<Shader>         lightCullingShader;

        Ref<Framebuffer>    pickingFramebuffer;
        Ref<Shader>         pickingShader;

        // -- Lifecycle -------------------------------------------------------------
        void Init(uint32_t width, uint32_t height);
        void Resize(uint32_t width, uint32_t height);
        void Shutdown();
    };

} // namespace Rynex
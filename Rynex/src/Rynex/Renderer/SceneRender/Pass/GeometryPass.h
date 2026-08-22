#pragma once
// ============================================================================
//  GeometryPass.h  –  Rynex Engine
//
//  Primary shading pass for opaque 3-D meshes.
//  Optionally writes integer entity IDs to a RED_INTEGER attachment for
//  pixel-exact picking, and optionally reads tile-based light lists for Forward+.
// ============================================================================
#include <rypch.h>
#include <Rynex/Renderer/SceneRender/Pass/RenderPass.h>

namespace Rynex {

    class RYNEX_API GeometryPass final : public BaseRenderPass
    {
    public:
        struct Config
        {
            // Emit (int32_t)entity to a RED_INTEGER attachment on the target FB.
            // Requires the framebuffer to have a RED_INTEGER colour attachment.
            bool writeEntityID = false;

            FrustumCullMode culling = FrustumCullMode::CPU;

            // Extension: multi-draw indirect / GPU-driven draw.
            bool uUseIndirectDraw = false;

            // When true, reads SceneRenderResources::VisibleLightIndicesSSBO;
            // requires a LightCullingPass at lower priority.
            bool useForwardPlus = false;
        };

    // --- public member funktions --------------------------------------------------------------------------------------------
        GeometryPass() = default;
        GeometryPass(const RenderPassSpec& spec, const Config& config = {});

        void OnInit(Ref<SceneRenderResources> resources) override;
        void OnBegin(const SceneRenderCameraData& camera,
            const SceneLightEnvironment& lights) override;
        void OnExecute(const DrawCommandQueue& queue) override;
        void OnEnd()     override;
        void OnResize(uint32_t width, uint32_t height) override;
        void OnShutdown() override;

        Config& GetConfig();
        const Config& GetConfig() const;

        static Ref<GeometryPass> Create(const RenderPassSpec& spec, const Config& config = {});

    private:
        Config                      m_Config;
    };

} // namespace Rynex
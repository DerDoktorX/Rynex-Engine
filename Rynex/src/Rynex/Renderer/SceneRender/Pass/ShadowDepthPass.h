#pragma once
// ============================================================================
//  ShadowDepthPass.h  –  Rynex Engine
//
//  Renders scene geometry from a light's point of view to produce a shadow map.
//  One ShadowDepthPass per shadow-casting light; lights are discovered at
//  runtime by SceneRenderer::BuildShadowCameras().
//
//  Key change from original:
//   LightCamera and ShadowMapIndex have moved from RenderPassSpec (where they
//   wasted ~600 bytes in every non-shadow pass spec) into this pass's own
//   Config struct.
// ============================================================================
#include <rypch.h>
#include <Rynex/Renderer/SceneRender/Pass/RenderPass.h>
#include <Rynex/Renderer/SceneRender/SceneRenderCamera.h>

namespace Rynex {

    class RYNEX_API ShadowDepthPass final : public BaseRenderPass
    {
    public:
        struct Config
        {
            uint32_t        shadowMapResolution = 2048u;
            ShadowTechnique technique = ShadowTechnique::PCF;
            uint32_t        sampleCountPCF = 4u;
            // StabiliseCSM: snap the shadow frustum to texel-grid boundaries to
            // reduce shadow shimmering as the camera moves.
            bool            stabiliseCSM = true;

            // -- Fields moved here from RenderPassSpec -----------------------------
            // Previously, RenderPassSpec held ShadowCamera and ShadowMapIndex even
            // for passes that are not shadow depth passes, wasting ~600 bytes each.
            SceneRenderCameraData lightCamera;              // light's view-projection
            uint32_t              shadowMapIndex = 0u;     // which slot in ShadowMaps[]
        };

    public:
        ShadowDepthPass() = default;
        ShadowDepthPass(const RenderPassSpec& spec, const Config& config = {});

        void OnInit(Ref<SceneRenderResources> resources) override;
        void OnBegin(const SceneRenderCameraData& camera, const SceneLightEnvironment& lights) override;
        void OnExecute(const DrawCommandQueue& queue) override;
        void OnEnd()     override;
        void OnResize(uint32_t width, uint32_t height) override;
        void OnShutdown() override;

        // -- Runtime light camera update --------------------------------------------
        // Called by SceneRenderer::BuildShadowCameras() each frame.
        void SetLightCamera(const SceneRenderCameraData& cam);
        void SetShadowMapIndex(uint32_t index);

        const Ref<Framebuffer>& GetShadowMapFramebuffer() const;
        const Ref<Texture>& GetShadowMapTexture()     const;
        const Config& GetConfig()               const;

        static Ref<ShadowDepthPass> Create(const RenderPassSpec& spec, const Config& config = {});

    private:
        Config              m_Config;
        Ref<Framebuffer>    m_ShadowFramebuffer;  // depth-only FB for this light
    };

} // namespace Rynex
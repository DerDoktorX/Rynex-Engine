#pragma once
// Renders depth only (no colour output).
// Required before LightCullingPass in a Forward+ pipeline; also enables
// early-z rejection on hardware that supports it.
// Result is written into SceneRenderResources::DepthPrepassTexture.
#include <rypch.h>
#include <Rynex/Renderer/SceneRender/Pass/RenderPass.h>
#include <Rynex/Renderer/SceneRender/DrawCommands.h>

namespace Rynex {

    class RYNEX_API DepthPrePass final : public BaseRenderPass
    {
    public:
        DepthPrePass() = default;
        explicit DepthPrePass(const RenderPassSpec& spec);

        void OnInit(Ref<SceneRenderResources> resources) override;
        void OnBegin(const SceneRenderCameraData& camera,  const SceneLightEnvironment& lights) override;
        void OnExecute(const DrawCommandQueue& queue) override;
        void OnEnd()     override;
        void OnResize(uint32_t width, uint32_t height) override;
        void OnShutdown() override;

        static Ref<DepthPrePass> Create(const RenderPassSpec& spec);
    };

} // namespace Rynex
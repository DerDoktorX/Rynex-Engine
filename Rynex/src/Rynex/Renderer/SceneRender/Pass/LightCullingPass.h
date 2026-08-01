#pragma once
// ============================================================================
//  LightCullingPass.h  –  Rynex Engine
//
//  Dispatches a compute shader that, for each screen tile, determines which
//  point/spot lights are visible given the tile's depth range (read from the
//  DepthPrepassTexture written by DepthPrePass).
//  Writes results into SceneRenderResources::VisibleLightIndicesSSBO.
//
//  Requires DepthPrePass to execute before this pass (lower Priority value).
// ============================================================================
#include <rypch.h>
#include <Rynex/Renderer/SceneRender/Pass/RenderPass.h>
#include <Rynex/Renderer/SceneRender/SceneLightData.h>  // ForwardPlusConfig

namespace Rynex {

    class RYNEX_API LightCullingPass final : public BaseRenderPass
    {
    public:
        // Config re-exports ForwardPlusConfig so callers have a single place to look.
        using Config = ForwardPlusConfig;

    // --- public member funktions --------------------------------------------------------------------------------------------
        LightCullingPass() = default;
        LightCullingPass(const RenderPassSpec& spec, const Config& config = {});

        void OnInit(Ref<SceneRenderResources> resources) override;
        void OnBegin(const SceneRenderCameraData& camera,
            const SceneLightEnvironment& lights) override;
        void OnExecute(const DrawCommandQueue& queue) override;
        void OnEnd()     override;
        void OnResize(uint32_t width, uint32_t height) override;
        void OnShutdown() override;

        void       SetTileSize(glm::uvec2 tileSize);
        glm::uvec2 GetTileSize()  const;
        // TileCount() depends on the current viewport size (tracked via OnResize).
        glm::uvec2 GetTileCount() const;
        const Config& GetConfig() const;

        static Ref<LightCullingPass> Create(const RenderPassSpec& spec, const Config& config = {});

    private:
        Config                      m_Config;
        glm::uvec2                  m_ViewportSize = { 0u, 0u };
    };

} // namespace Rynex
#pragma once
// ============================================================================
//  EntityOutlinePass.h  –  Rynex Engine
//
//  Renders a coloured outline around selected entities.
//  Two strategies are available at construction time:
//    Stencil – fast; good for sharp, 1-pixel outlines.
//    JFA     – jump-flood algorithm; supports variable width and soft glow.
//
//  The selected entity list is updated by SceneRenderer::UpdateOutlinePass()
//  each frame, so callers only need to call SceneRenderer::SetSelectedEntity().
// ============================================================================

#include <rypch.h>

#include <Rynex/Renderer/SceneRender/Pass/RenderPass.h>


namespace Rynex {

    class RYNEX_API EntityOutlinePass final : public BaseRenderPass
    {
    public:
        struct Config
        {
            glm::vec4   OutlineColor = { 1.0f, 0.5f, 0.0f, 1.0f };
            float       OutlineWidth = 2.0f;     // pixels
            bool        UseJFA = false;    // false = stencil approach
        };

    // --- public member funktions --------------------------------------------------------------------------------------------
        EntityOutlinePass() = default;
        EntityOutlinePass(const RenderPassSpec& spec, const Config& config = {});

        void OnInit(Ref<SceneRenderResources> resources) override;
        void OnBegin(const SceneRenderCameraData& camera, const SceneLightEnvironment& lights) override;
        void OnExecute(const DrawCommandQueue& queue) override;
        void OnEnd() override;
        void OnResize(uint32_t width, uint32_t height) override;
        void OnShutdown() override;

        // -- Selection management – called by SceneRenderer::UpdateOutlinePass() ---
        void SetSelectedEntities(const std::vector<entt::entity>& entities);
        void SetSelectedEntity(entt::entity entity);     // replaces entire selection
        void ClearSelectedEntities();

        // -- Visual tweaks ---------------------------------------------------------
        void SetOutlineColor(const glm::vec4& color);
        void SetOutlineWidth(float width);

        const Config& GetConfig() const;

        static Ref<EntityOutlinePass> Create(const RenderPassSpec& spec, const Config& config = {});

    private:
        Config                      m_Config;
        std::vector<entt::entity>   m_SelectedEntities;
    };

} // namespace Rynex
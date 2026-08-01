#pragma once
#pragma once
// ============================================================================
//  EntityPickingPass.h  –  Rynex Engine
//
//  Renders the scene into an off-screen RED_INTEGER framebuffer where each
//  pixel stores the entt::entity value of the mesh covering that pixel.
//  ReadEntityIDAt(x, y) reads back the CPU-side value synchronously.
//
//  The pass writes to SceneRenderResources::PickingFramebuffer using
//  SceneRenderResources::PickingShader so that the main output FB is never
//  touched.
// ============================================================================
#include <rypch.h>

#include <Rynex/Renderer/SceneRender/Pass/RenderPass.h>

namespace Rynex {

    class RYNEX_API EntityPickingPass final : public BaseRenderPass
    {
    public:
        EntityPickingPass() = default;
        explicit EntityPickingPass(const RenderPassSpec& spec);

        void OnInit(Ref<SceneRenderResources> resources) override;
        void OnBegin(const SceneRenderCameraData& camera,
            const SceneLightEnvironment& lights) override;
        void OnExecute(const DrawCommandQueue& queue) override;
        void OnEnd()     override;
        void OnResize(uint32_t width, uint32_t height) override;
        void OnShutdown() override;

        // -- Picking query ---------------------------------------------------------
        // Returns -1 when no entity was rendered at (x, y).
        // Performs a synchronous glReadPixels; call once per frame at most.
        int32_t      ReadEntityIDAt(int32_t x, int32_t y) const;
        entt::entity GetEntityAt(int32_t x, int32_t y)    const;

        static Ref<EntityPickingPass> Create(const RenderPassSpec& spec);
    };

} // namespace Rynex
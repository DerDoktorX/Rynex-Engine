#pragma once
// ============================================================================
//  RenderPass.h  –  Rynex Engine
//
//  Contains:
//   -> RenderPassSpec  – plain-data configuration for a single pass
//   -> RenderPass      – abstract base class; all concrete passes derive from it
//   -> BaseRenderPass  – non-virtual helper that stores m_Spec / m_Resources
//                       and implements GetSpec().  Concrete passes derive
//                       BaseRenderPass to eliminate 4–5 lines of copy-paste
//                       per class (was repeated in all 11 original passes).
//
//  Key change from original:
//   RenderPassSpec no longer carries ShadowCamera / ShadowMapIndex.  Those
//   fields were present in *every* pass spec but were only read by
//   ShadowDepthPass.  At ~600 bytes of wasted space per non-shadow pass,
//   they now live in ShadowDepthPass::Config where they belong.
// ============================================================================


#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Renderer/SceneRender/SceneRendererTypes.h>
#include <Rynex/Renderer/SceneRender/SceneRenderCamera.h>   // SceneRenderCameraData
#include <Rynex/Renderer/SceneRender/SceneLightData.h>      // SceneLightEnvironment
#include <Rynex/Renderer/SceneRender/SceneRenderResources.h>
#include <Rynex/Renderer/SceneRender/DrawCommands.h>



namespace Rynex {

    // ============================================================================
    //  RenderPassSpec – plain-data configuration
    //  Intentionally minimal: only generic pass properties go here.
    //  Technique-specific data (shadow cameras, tile sizes, …) lives in each
    //  concrete pass's inner Config struct.
    // ============================================================================

    struct RYNEX_API RenderPassSpec
    {
        std::string         name;
        RenderPassType      type = RenderPassType::Opaque;
        RenderPassFlags     flags = RenderPassFlag::Default;

        // nullptr → use the scene renderer's current output framebuffer.
        Ref<Framebuffer>    targetFramebuffer = nullptr;
        glm::vec4           clearColorValue = { 0.1f, 0.1f, 0.1f, 1.0f };
        float               clearDepthValue = 1.0f;
        uint32_t            clearStencilValue = 0u;

        // Optional full override: if non-null, replaces every draw call's own shader.
        Ref<Shader>         overrideShader = nullptr;
        // -1 = use each command's own RenderMode; any other value overrides it.
        int                 overrideRenderMode = -1;

        // Execution order: lower value runs earlier.  Sorting is stable so
        // equal-priority passes execute in insertion order.
        uint32_t            priority = 100u;
        bool                enabled = true;
    };


    // ============================================================================
    //  RenderPass  –  Abstract Base Class
    // ============================================================================

    class RYNEX_API RenderPass
    {
    public:
        virtual ~RenderPass() = default;

        // -- Lifecycle -------------------------------------------------------------
        // OnInit    – called once during pipeline initialisation.
        // OnBegin   – called at the start of each pass, before commands execute.
        // OnExecute – core: render the commands relevant to this pass.
        // OnEnd     – cleanup, memory barriers, etc.
        // OnResize  – viewport size change.
        // OnShutdown– pipeline destruction.
        virtual void OnInit(Ref<SceneRenderResources> resources) = 0;
        virtual void OnBegin(const SceneRenderCameraData& camera, const SceneLightEnvironment& lights) = 0;
        virtual void OnExecute(const DrawCommandQueue& queue) = 0;
        virtual void OnEnd() = 0;
        virtual void OnResize(uint32_t width, uint32_t height) = 0;
        virtual void OnShutdown() = 0;

        // -- Spec access (pure virtual so BaseRenderPass can return its stored spec) -
        virtual       RenderPassSpec& GetSpec() = 0;
        virtual const RenderPassSpec& GetSpec() const = 0;

        // -- Non-virtual convenience forwarders (implemented via GetSpec()) ---------
        const std::string& GetName() const;
        RenderPassType GetType() const;
        bool IsEnabled() const;
        void SetEnabled(bool enabled);
        uint32_t GetPriority() const;
        void SetPriority(uint32_t priority);

        const Ref<Framebuffer>& GetFramebuffer() const;
        void SetFramebuffer(const Ref<Framebuffer>& fb);
        void SetOverrideShader(const Ref<Shader>& shader);

        // Factory – creates a generic pass that executes commands using material shaders.
        static Ref<RenderPass> Create(const RenderPassSpec& spec);
    };


    // ========================================================================================================================
    //  BaseRenderPass  –  Non-virtual implementation helper
    //
    //  Why: every concrete pass in the original header declared identical private
    //  members { RenderPassSpec m_Spec; Ref<SceneRenderResources> m_Resources; }
    //  and implemented the same two-liner GetSpec() overrides.  Eleven classes ×
    //  ~5 lines each = ~55 lines of boilerplate that now live here once.
    //
    //  Concrete passes inherit BaseRenderPass instead of RenderPass directly.
    //  They still override the five lifecycle virtuals (OnInit … OnShutdown) and
    //  may add pass-specific private members freely.
    // ========================================================================================================================

    class RYNEX_API BaseRenderPass : public RenderPass
    {
    public:
        // Default-constructible for two-step init patterns (construct, then Init()).
        BaseRenderPass() = default;

        explicit BaseRenderPass(const RenderPassSpec& spec)
            : m_Spec(spec)
            , m_Resources(nullptr)
        {
        }

        // GetSpec() implemented here once; concrete passes do not repeat this.
        RenderPassSpec& GetSpec()       override { return m_Spec; }
        const RenderPassSpec& GetSpec() const override { return m_Spec; }

    protected:
        RenderPassSpec              m_Spec;
        Ref<SceneRenderResources>   m_Resources;    // assigned in OnInit()
    };

} // namespace Rynex
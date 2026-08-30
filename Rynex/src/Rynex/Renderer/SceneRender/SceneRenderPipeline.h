#pragma once

#include <rypch.h>
#include <Rynex/Renderer/SceneRender/SceneLightData.h>
#include <Rynex/Renderer/SceneRender/Pass/RenderPass.h>


namespace Rynex {

    // ============================================================================
    //  SCENE RENDER PIPELINE
    //  An ordered collection of RenderPass instances.
    //  Passes execute in ascending Priority order.
    // ============================================================================

    class RYNEX_API SceneRenderPipeline
    {
    public:
        SceneRenderPipeline() = default;
        ~SceneRenderPipeline() = default;

        // -- Pass management -------------------------------------------------------
        void AddPass(Ref<RenderPass> pass);
        void InsertPass(Ref<RenderPass> pass, uint32_t atIndex);
        bool RemovePass(const std::string& name);
        bool HasPass(const std::string& name) const;

        // -- Pass access -----------------------------------------------------------
        Ref<RenderPass> GetPass(const std::string& name) const;
        Ref<RenderPass> GetPass(uint32_t index) const;
        Ref<RenderPass>& GetPassRef(const std::string& name);

        // Typed access – caller is responsible for verifying the correct type.
        // Bodies provided in SceneRenderer.inl (template definition required in header).
        template<typename PassType>
        Ref<PassType> GetPassAs(const std::string& name) const;

        // -- Pass control ----------------------------------------------------------
        void EnablePass(const std::string& name, bool enabled);
        void SetPassPriority(const std::string& name, uint32_t priority);
        void SortPasses();

        const std::vector<Ref<RenderPass>>& GetPasses()    const;
        uint32_t                            GetPassCount()  const;

        // -- Lifecycle -------------------------------------------------------------
        void Init(Ref<SceneRenderResources> resources);
        void Shutdown();
        void OnResize(uint32_t width, uint32_t height);

        // -- Standard pipeline factories -------------------------------------------
        // Build a ready-to-use pipeline for common scenarios.

        // Simple forward: shadow + opaque + transparent + 2D + text
        static Ref<SceneRenderPipeline> CreateForwardPipeline(const Ref<Framebuffer>& output);

        // Forward+: depth pre-pass + light culling + shadow + opaque (FP) + transparent + 2D + text
        static Ref<SceneRenderPipeline> CreateForwardPlusPipeline(const Ref<Framebuffer>& output,glm::uvec2 tileSize = { 16u, 16u });

        // Editor: forward + entity-picking + outline + editor overlay
        static Ref<SceneRenderPipeline> CreateEditorPipeline( const Ref<Framebuffer>& output);

        // Game: chooses Forward or ForwardPlus based on the mode argument.
        static Ref<SceneRenderPipeline> CreateGamePipeline(const Ref<Framebuffer>& output, SceneRenderMode mode = SceneRenderMode::Forward);

    private:
        std::vector<Ref<RenderPass>>            m_Passes;
        HashMapFlat<std::string, uint32_t>      m_NameIndexMap;
        bool                                    m_Dirty = false;
    };


    // -- Template body -------------------------------------------------------------
    // Must be visible at instantiation; placed here rather than in a .inl file
    // for simplicity since it is a single-liner.
    template<typename PassType>
    Ref<PassType> SceneRenderPipeline::GetPassAs(const std::string& name) const
    {
        return std::dynamic_pointer_cast<PassType>(GetPass(name));
    }

}
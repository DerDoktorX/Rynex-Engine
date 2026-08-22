#pragma once
// ============================================================================
//  DrawCommands.h  –  Rynex Engine
//
//  Lightweight, POD descriptions of a single draw call (3-D mesh, 2-D quad,
//  or text) plus the DrawCommandQueue that accumulates them per frame.
//
//  Key changes from original:
//   -> DrawCommand3D gains an IsTransparent flag so DrawCommandQueue::Push()
//     can route automatically to the opaque or transparent bucket.
//     Previously there was no routing cue; callers had to split themselves.
//   -> Shadow commands are auto-populated inside Push() when CastShadow == true.
//     Callers no longer need to maintain a separate shadow submission path.
//   -> SortKey / DistanceSq are set by the queue's Sort*() methods, so they
//     are declared private-writable via a struct trick – callers should treat
//     them as read-only on retrieved commands.
// ============================================================================

#include <rypch.h>
#include <Rynex/Renderer/API/VertexArray.h>    // Ref<VertexArray>, Ref<IndexBuffer>
#include <Rynex/Renderer/API/Texture.h>        // Ref<Texture>
#include <Rynex/Renderer/Objects/BoundingVolume.h>

#include <entt.hpp>

namespace Rynex {

    class Material;
    class Font;

    // ============================================================================
    //  3-D Mesh Draw Command
    // ============================================================================

    struct RYNEX_API DrawCommand3D
    {
        entt::entity        entity = entt::null;

        // -- Geometry + material --------------------------------------------------
        Ref<VertexArray>    geometry = nullptr;
        Ref<Material>       materialRef = nullptr;
        uint32_t            indexCount = 0u;
        int                 renderMode = 0;

        // -- Transform ------------------------------------------------------------
        glm::mat4           transform = glm::mat4(1.0f);

        // -- Routing flags ---------------------------------------------------------
        // IsTransparent: tells DrawCommandQueue to place this in the transparent
        // bucket (back-to-front) rather than the opaque bucket (front-to-back).
        bool                isTransparent = false;
        bool                castShadow = true;
        bool                receiveShadow = true;

        // -- Bounds – used by frustum culling -------------------------------------
        BoundingVolume      worldBounds;

        // -- Batching / instancing extension --------------------------------------
        // BatchID == 0  →  no batching; pass renders immediately.
        // BatchID  > 0  →  pass may group commands with the same BatchID.
        uint32_t            batchID = 0u;
        uint32_t            instanceCount = 1u;
        uint32_t            baseInstance = 0u;

        // -- LOD ------------------------------------------------------------------
        int                 lodTier = 0;   // 0 = highest detail

        // -- Sorting (set by DrawCommandQueue::Sort*(), treat as read-only) -------
        uint64_t            sortKey = 0ull;
        float               distanceSq = 0.0f;
    };


    // ============================================================================
    //  2-D Quad / Sprite Draw Command
    // ============================================================================

    struct RYNEX_API DrawCommand2D
    {
        entt::entity    entity = entt::null;

        glm::mat4       transform = glm::mat4(1.0f);
        glm::vec4       color = glm::vec4(1.0f);
        Ref<Texture>    texture = nullptr;
        float           tilingFactor = 1.0f;

        // 2-D layer ordering: higher ZOrder renders on top.
        float           zOrder = 0.0f;

        // Sorting key set by DrawCommandQueue::Sort2D()
        uint64_t        sortKey = 0ull;
    };


    // ============================================================================
    //  Text Draw Command
    // ============================================================================

    struct RYNEX_API DrawCommandText
    {
        entt::entity    entity = entt::null;

        glm::mat4       transform = glm::mat4(1.0f);
        std::string     text;
        Ref<Font>       fontAsset = nullptr;
        glm::vec4       color = glm::vec4(1.0f);
        float           kerning = 0.0f;
        float           LineSpacing = 0.0f;

        float           zOrder = 0.0f;
    };


    // ============================================================================
    //  DrawCommandQueue
    //  Accumulates all submitted commands for one frame.  Passes pull the commands
    //  they care about from the typed accessors.
    //
    //  Routing rules for DrawCommand3D (handled inside Push()):
    //    IsTransparent == false  →  m_OpaqueCommands
    //    IsTransparent == true   →  m_TransparentCommands
    //    CastShadow    == true   →  also added to m_ShadowCommands (either case)
    // ============================================================================

    class RYNEX_API DrawCommandQueue
    {
    public:
        DrawCommandQueue() = default;
        ~DrawCommandQueue() = default;

        // -- Submission ------------------------------------------------------------
        // Push() routes to the correct bucket automatically; callers should not
        // use the mutable GetXxxCommands() accessors for submission.
        void Push(const DrawCommand3D& cmd);
        void Push(DrawCommand3D&& cmd);
        void Push(const DrawCommand2D& cmd);
        void Push(const DrawCommandText& cmd);

        // -- Sorting ---------------------------------------------------------------
        // Must be called after all submissions and before pipeline execution.
        void SortOpaque();                                  // front-to-back by sort key / material hash
        void SortTransparent(const glm::vec3& cameraPos);   // back-to-front by distance
        void Sort2D();                                      // by ZOrder, then texture
        void SortAll(const glm::vec3& cameraPos);           // convenience: calls all three

        // -- Const accessors (for passes reading commands) -------------------------
        const std::vector<DrawCommand3D>& GetOpaqueCommands() const;
        const std::vector<DrawCommand3D>& GetTransparentCommands() const;
        // Shadow commands are a *view* derived from opaque + transparent where
        // CastShadow == true; they share the same Geometry/Material pointers.
        const std::vector<DrawCommand3D>& GetShadowCommands() const;
        const std::vector<DrawCommand2D>& GetSprite2DCommands() const;
        const std::vector<DrawCommandText>& GetTextCommands() const;

        // -- Mutable accessors (for passes that cull commands in-place) ------------
        // Use only for culling / marking; do not add new commands here.
        std::vector<DrawCommand3D>& GetOpaqueCommands();
        std::vector<DrawCommand3D>& GetTransparentCommands();
        std::vector<DrawCommand3D>& GetShadowCommands();

        // -- Info ------------------------------------------------------------------
        uint32_t TotalCount() const;
        bool     IsEmpty() const;
        void     Clear();

        // -- Filtering helpers -----------------------------------------------------
        // Returns only opaque commands whose Entity is in the provided set.
        // Used by EntityOutlinePass to render only the selected entities.
        std::vector<DrawCommand3D> FilterOpaque3D(const std::vector<entt::entity>& mask) const;

        // Visitor variant to avoid the copy overhead of FilterOpaque3D.
        void ForEachOpaque3D(const std::vector<entt::entity>& mask,
            const std::function<void(const DrawCommand3D&)>& visitor) const;

    private:
        // Push helper that updates all relevant buckets and sets initial SortKey.
        void PushImpl(DrawCommand3D cmd);

    private:
        std::vector<DrawCommand3D>   m_OpaqueCommands;
        std::vector<DrawCommand3D>   m_TransparentCommands;
        // Shadow commands reference the same geometry/material as the above
        // vectors.  Maintained as copies (not pointers) to allow independent
        // culling per shadow-casting light without affecting main camera rendering.
        std::vector<DrawCommand3D>   m_ShadowCommands;
        std::vector<DrawCommand2D>   m_Sprite2DCommands;
        std::vector<DrawCommandText> m_TextCommands;
    };


    // -- Sort-key utility (body in SceneRenderer.cpp) ----------------------------
    // Encodes material, render-mode, and distance into a single 64-bit key that
    // allows opaque commands to be sorted front-to-back by material group with
    // a single integer comparison.
    RYNEX_API uint64_t ComputeDrawSortKey(const Ref<Material>& material,  int renderMode, float distanceSq);

} // namespace Rynex
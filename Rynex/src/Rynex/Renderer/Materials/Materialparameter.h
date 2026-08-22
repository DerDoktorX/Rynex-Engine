#pragma once
#include <Rynex/Renderer/Materials/MaterialTypes.h>
#include <Rynex/Renderer/Materials/MaterialBlueprint.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Asset/Base/Asset.h>

// ============================================================
//  MaterialParameter.h
//  High-level, user-space material parameterization.
//
//  Responsibilities
//  ----------------
//  -> Holds a reference to one MaterialBlueprint (the "template").
//  -> Stores only the parameter overrides that differ from the
//    blueprint's defaults — not a full copy.
//  -> Supports a parent chain: an override not found here is
//    resolved by walking up to the parent, then to the blueprint.
//    This enables lightweight material "inheritance" without
//    recompilation (e.g. a wet variant of a stone material).
//  -> Resolves the final packed param block on demand and uploads
//    it into a per-instance UniformBuffer.
//  -> Supports per-pass texture overrides (e.g. a different
//    emissive map for the entity-picking pass).
//
//  What it does NOT do
//  -------------------
//  -> Compile shaders or manage shader variants → that is MaterialBlueprint.
//  -> Know how many instances exist in the scene.
//  -> Upload data to the GPU unprompted (lazy / dirty pattern).
// ============================================================

namespace Rynex {

    class MaterialParameter : public Asset
    {
    public:
        // -- Factory -------------------------------------------------
        // All scalar/vector params are initialised from the blueprint's
        // defaults; no CPU-side copy is made until the first override.
        static Ref<MaterialParameter> Create(const Ref<MaterialBlueprint>& blueprint);

        // Create as a child of an existing parameter set.
        // Child inherits all of parent's overrides + its own on top.
        static Ref<MaterialParameter> CreateChild(const Ref<MaterialParameter>& parent);

        virtual ~MaterialParameter() = default;

        // -- Blueprint & parent linkage --------------------------------
        const Ref<MaterialBlueprint>& GetBlueprint()    const;

        // Re-point to a different blueprint.
        // All overrides whose names no longer exist in the new blueprint
        // become "orphaned" — they are kept but never resolved.

        void SetBlueprint(const Ref<MaterialBlueprint>& blueprint);

        // Optional parent for the override-resolution chain.
        void                        SetParent(const Ref<MaterialParameter>& parent);
        Ref<MaterialParameter>      GetParent() const;   // may be nullptr
        bool                        HasParent() const;

        // -- Scalar / vector overrides ----------------------------------
        // Only stores the value if it differs from what would be resolved
        // without this call (blueprint default or parent value).

        void SetFloat(const std::string& name, float value);
        void SetFloat2(const std::string& name, const glm::vec2& value);
        void SetFloat3(const std::string& name, const glm::vec3& value);
        void SetFloat4(const std::string& name, const glm::vec4& value);

        void SetInt(const std::string& name, int32_t value);
        void SetInt2(const std::string& name, const glm::ivec2& value);
        void SetInt3(const std::string& name, const glm::ivec3& value);
        void SetInt4(const std::string& name, const glm::ivec4& value);

        void SetBool(const std::string& name, bool value);


        // Generic setter — type must match what the blueprint declared.
        void SetParam(const std::string& name, const MaterialParamValue& value);


        // Remove a local override; resolution falls back to parent / blueprint.
        void ClearOverride(const std::string& name);
        void ClearAllOverrides();


        // Returns true if this instance (not parent/blueprint) has a local entry.
        bool HasLocalOverride(const std::string& name) const;

        // Resolve a parameter by walking the chain: this → parent → blueprint default.
        // Returns std::nullopt if the name is unknown in the blueprint.
        std::optional<MaterialParamValue> ResolveParam(const std::string& name) const;


        // -- Texture overrides ------------------------------------------

        // Global texture override (applies to all passes unless a per-pass
        // override is also set).
        void SetTexture(const std::string& slotName, const Ref<Texture>& texture);
        void ClearTextureOverride(const std::string& slotName);
        bool HasTextureOverride(const std::string& slotName) const;


        // Per-pass texture override.
        // Takes priority over the global texture override for that slot.
        // Useful for e.g. a flat colour in the EntityPicking pass.
        void SetPassTexture(RenderPassTag pass, const std::string& slotName, const Ref<Texture>& texture);
        void ClearPassTextureOverride(RenderPassTag pass, const std::string& slotName);
        bool HasPassTextureOverride(RenderPassTag pass, const std::string& slotName) const;

        // Resolve the effective texture for a slot + pass.
        // Chain: per-pass override → global override → parent → blueprint default.
        Ref<Texture> ResolveTexture(const std::string& slotName, RenderPassTag pass = RenderPassTag::None) const;



        // -- GPU upload / UBO management ----------------------------

        // Write the resolved param block into outBuffer (caller owns memory).
        // outByteSize must be >= blueprint->GetParamBlockByteSize().
        void BuildParamData(void* outBuffer, uint32_t outByteSize) const;

        // Return (and lazily create) a per-instance UniformBuffer holding
        // the resolved param block.  Re-uploads only when IsDirty().
        Ref<UniformBuffer> GetOrBuildParamUBO();

        // Convenience: bind the UBO at the given slot.
        void BindParamUBO(uint32_t slot = 0);


        // -- Pass-variant forwarding ----------------------------
        // Delegates to the blueprint; provided here so RenderTarget code
        // can query everything from the parameter object alone.

        const MaterialBlueprint::ShaderVariant& GetVariantForPass(RenderPassTag pass, MaterialQuality quality = MaterialQuality::High) const;
        bool HasVariantForPass(RenderPassTag pass, MaterialQuality quality = MaterialQuality::High) const;


        // -- Dirty tracking ----------------------------
        bool IsDirty()     const;
        void MarkDirty();
        void MarkClean()   const;   // called after GPU upload


        // -- Asset type-------------------------------------------------
        // NOTE: AssetType::MaterialParameter must be added to your AssetType enum.
        static  AssetType GetStaticType() { return AssetType::MaterialParameter; }
        AssetType         GetType() const override { return GetStaticType(); }
    private:
        // -- Blueprint reference ----------------------------
        Ref<MaterialBlueprint>  m_Blueprint;

        // -- Parent chain --------------------------------------------
        // Stored as Weak to avoid ownership cycles (parent outlives child).
        Weak<MaterialParameter> m_Parent;

        // -- Scalar / vector overrides --------------------------
        // Only entries that diverge from parent/blueprint are stored.
        // Key: parameter name (matches MaterialParamDescriptor::name).
        HashMapFlat<std::string, MaterialParamValue> m_ParamOverrides;

        // -- Texture overrides ---------------------------------------
        // Global (pass-agnostic) texture overrides.
        HashMapFlat<std::string, Ref<Texture>>  m_TextureOverrides;

        // Per-pass texture overrides.
        // Outer key: RenderPassTag cast to uint32_t.
        // Inner key: slot name.
        HashMapFlat<uint32_t, HashMapFlat<std::string, Ref<Texture>>> m_PassTextureOverrides;

        // -- GPU cache -----------------------------------------------
        // Lazily created; nullptr until GetOrBuildParamUBO() is first called.
        Ref<UniformBuffer>      m_ParamUBO;

        // -- State -------------------------------------------------
        mutable bool m_IsDirty = true;
    };

}
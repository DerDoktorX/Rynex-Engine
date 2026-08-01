#pragma once
#include <Rynex/Renderer/Materials/MaterialTypes.h>
#include <Rynex/Renderer/API/Shader.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Asset/Base/Asset.h>


// ============================================================
//  MaterialBlueprint.h
//  Low-level, procedurally generated representation of a material
//  for the render hardware.
//
//  Responsibilities
//  -----------------
//  -> Owns the map  RenderPassTag → ShaderVariant  (compiled GPU program
//    + its render state + batch config + parameter-block layout).
//  -> Declares the canonical parameter table that MaterialParameter
//    instances reference for overrides.
//  -> Manages one UniformBuffer cache slot per quality tier
//    (populated with default values at compile time).
//  -> Exposes compile-time #define injection so the same source
//    template can produce variants for Forward-Plus, deferred,
//    entity-picking, etc. without separate source files.
//
//  What it does NOT do
//  --------------------
//  -> Store per-instance values  → that is MaterialParameter's job.
//  -> Know about scene entities or render passes beyond its slot map.
// ============================================================


namespace Rynex {

    class MaterialBlueprint : public Asset
    {
    public:
        // ----------------------------------------------------------
        //  ShaderVariant
        //  One fully compiled GPU program for a specific pass + quality.
        //  This is the atom that RenderTarget / PiplineRenderBase
        //  picks up at draw time.
        // ----------------------------------------------------------
        struct ShaderVariant
        {
            Ref<Shader>                     shader;

            // Layout of the parameter uniform-block this variant expects.
            // Must match the blueprint's canonical m_ParamDescriptors byte-layout.
            BufferLayout                    paramBlockLayout;

            // GPU fixed-function state required by this variant.
            MaterialStateFlag               stateFlags = MaterialStateFlags::None;

            // How instances / material params are batched for this pass.
            DrawSpecification::BatchConfig  batchConfig = DrawSpecification::None;

            // Render-mode integer forwarded to RendererAPI::SetMode.
            int                             renderMode = 0;

            bool IsValid() const { return shader != nullptr; }
        };

        // ----------------------------------------------------------
        //  QualityCache
        //  Per-quality-tier CPU-side + GPU-side cache of default values.
        //  Invalidated when a #define or source changes (m_IsDirty).
        // ----------------------------------------------------------
        struct QualityCache
        {
            // GPU buffer filled with m_DefaultParamData at compile time.
            // MaterialParameter patches this at bind time if it has overrides.
            Ref<UniformBuffer>              defaultParamUBO;

            // Per-pass compiled variants at this quality level.
            // Index: static_cast<uint32_t>(RenderPassTag)
            std::array<ShaderVariant, g_RenderPassTagCount> variantMap;

            bool IsCompiled() const { return defaultParamUBO != nullptr; }
        };


        // -- Factory -----------------------------------------------
        //   blueprintSourceName: logical name used for shader-library lookup.
        static Ref<MaterialBlueprint> Create(const std::string& blueprintSourceName);
        static Ref<MaterialBlueprint> CreateAndCompile(const std::string& blueprintSourceName, MaterialQuality quality = MaterialQuality::High);


        virtual ~MaterialBlueprint() = default;


        // -- Shader-variant registration (called during procedural build) --

        // Register or overwrite a variant for a specific pass + quality.
        void SetVariant(RenderPassTag pass, MaterialQuality quality, const ShaderVariant& variant);
        const ShaderVariant& GetVariant(RenderPassTag pass, MaterialQuality quality = MaterialQuality::High) const;

        bool HasVariant(RenderPassTagpass pass, MaterialQuality quality = MaterialQuality::High) const;


        // -- Parameter-table API ------------------------------------
        // Called once during blueprint construction; not per-frame.

        // Append a scalar/vector slot to the canonical parameter table.
        void DeclareParam(const MaterialParamDescriptor& descriptor);

        // Append a texture slot.
        void DeclareTextureSlot(const TextureSlotDescriptor& slot);

        // Write default value bytes for a named param.
        // data must match descriptor.byteSize.
        void SetDefaultParamData(const std::string& paramName, const void* data, uint32_t byteSize);

        // Set a default texture for a named slot (used when MaterialParameter
        // has no override for this slot).
        void SetDefaultTexture(const std::string& slotName, const Ref<Texture>& texture);

        // Read-only access to the canonical descriptor table.
        const std::vector<MaterialParamDescriptor>& GetParamDescriptors() const;
        const std::vector<TextureSlotDescriptor>& GetTextureSlots()     const;


        // Total byte size of one packed parameter block (all scalar/vector params).
        uint32_t GetParamBlockByteSize() const;

        // Raw pointer to the default parameter data (CPU-side, packed).
        const void* GetDefaultParamDataPtr()  const;
        uint32_t GetDefaultParamDataSize() const;

        // Index lookup — returns UINT32_MAX if not found.
        uint32_t FindParamIndex(const std::string& name) const;
        uint32_t FindTextureSlotIndex(const std::string& name) const;


        // -- Compile-time define injection ----------------------
        // #defines are injected into every shader source before
        // compilation; Recompile() must be called afterwards.

        void SetDefine(const std::string& name, const std::string& value = "1");
        void RemoveDefine(const std::string& name);
        bool HasDefine(const std::string& name) const;
        const std::vector<std::pair<std::string, std::string>>& GetDefines() const;




        // -- Compilation --------------------------------------------
        // Procedurally assembles all ShaderVariants for the given quality,
        // injects defines, links shader stages, allocates the default UBO.
        // Thread-safety: call from render thread only.
        void Compile(MaterialQuality quality = MaterialQuality::High);

        // Recompile all quality tiers that have previously been compiled.
        void RecompileAll();

        bool IsCompiled(MaterialQuality quality = MaterialQuality::High) const;
        bool IsDirty()  const;

        // Force-invalidate (e.g. after a shader source hot-reload).
        void MarkDirty();

        // -- Default GPU buffer ------------------------------------
        // Returns the pre-filled UniformBuffer for this quality tier.
        // Used as a fast-path when no MaterialParameter overrides exist.
        const Ref<UniformBuffer>& GetDefaultParamUBO(MaterialQuality quality = MaterialQuality::High) const;

        // -- Asset type ----------------------------------------------
        // NOTE: AssetType::MaterialBlueprint must be added to your AssetType enum.
        static  AssetType GetStaticType() { return AssetType::MaterialBlueprint; }
        AssetType         GetType() const override { return GetStaticType(); }

        const std::string& GetSourceName() const;


    private:
        // Logical name → shader library key
        std::string m_SourceName;

        // -- Parameter table ----------------------------------------
        // Canonical list of scalar/vector parameter descriptors.
        std::vector<MaterialParamDescriptor>    m_ParamDescriptors;

        // Canonical list of texture slot descriptors.
        std::vector<TextureSlotDescriptor>      m_TextureSlots;

        // Packed default scalar/vector values (CPU-side, matches UBO layout).
        std::vector<uint8_t>                    m_DefaultParamData;

        // Default textures, indexed by slot index (parallel to m_TextureSlots).
        std::vector<Ref<Texture>>               m_DefaultTextures;

        // -- Quality cache ------------------------------------------
        // One cache entry per quality tier.
        std::array<QualityCache, g_MaterialQualityCount> m_QualityCache;

        // -- Define list -------------------------------------------
        std::vector<std::pair<std::string, std::string>> m_Defines;

        // -- State -------------------------------------------------
        bool m_IsDirty = true;


    };
}
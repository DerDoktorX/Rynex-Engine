#pragma once
#include<rypch.h>
#include<Rynex/Renderer/API/Texture.h>
#include<Rynex/Renderer/API/Buffer.h>

// ============================================================
//  MaterialTypes.h
//  Shared vocabulary types for MaterialBlueprint & MaterialParameter.
//  No implementation — only declarations, enums, and memory layout.
// ============================================================

namespace Rynex {

    // ----------------------------------------------------------
    //  RenderPassTag
    //  Identifies a logical render pass slot.
    //  The blueprint keeps one compiled ShaderVariant per tag.
    //  Add custom passes by extending the enum or using Custom + a
    //  separate string registry (not shown here).
    // ----------------------------------------------------------
    enum class RenderPassTag : uint32_t
    {
        None = 0,

                            // Geometry passes
        Depth,              // Early-Z / shadow receiver depth
        Shadow,             // Shadow-map generation
        GBuffer,            // Deferred G-buffer fill
        ForwardOpaque,      // Forward / Forward-Plus opaque
        ForwardAlpha,       // Forward transparent (sorted back-to-front)

        // Utility passes
        EntityPicking,      // Renders integer entity-ID into RED_INTEGER attachment
        Outline,            // Silhouette / selection highlight (isolated RenderTarget)
        PostProcess,        // Screen-space effects (compute or fullscreen-tri)

        // Reserved for user-defined passes
        Custom0, Custom1, Custom2, Custom3,

        COUNT
    };
    static constexpr uint32_t g_RenderPassTagCount = static_cast<uint32_t>(RenderPassTag::COUNT);


    // ----------------------------------------------------------
    //  MaterialQuality
    //  Controls the compiled complexity of a blueprint
    //  (e.g. PCF shadow samples, normal-map usage, BRDF terms).
    // ---------------------------------------------------------
    enum class MaterialQuality : uint32_t
    {
        Low = 0,
        Medium = 1,
        High = 2,
        Ultra = 3,
        COUNT = 4
    };
    static constexpr uint8_t g_MaterialQualityCount = static_cast<uint8_t>(MaterialQuality::COUNT);

    // ----------------------------------------------------------
    //  TextureTypes
    //  Semanticer slot Name for Texturs.
    // ----------------------------------------------------------
    enum class TextureTypes
    {
        None = 0,
        AlbedoMap,
        NormaleMap,
        SpecularMap,
        BumbMap,
        HigthMap,
        RougnessMap,
        MetalicMap,
        DiplacmentMap,
        InviremtnMap,

        ReflectionMap_0,
        ReflectionMap_1,
        ReflectionMap_2,
        ReflectionMap_3,

        ShadowMap_0,
        ShadowMap_1,
        ShadowMap_2,
        ShadowMap_3,
        ShadowMap_4,
        ShadowMap_5,
        ShadowMap_6,
        ShadowMap_7
    };


    // ----------------------------------------------------------
    //  MaterialParamDescriptor
    //  One entry in the blueprint's canonical parameter table.
    //  Defines name, type, byte offset inside the packed GPU
    //  buffer, and byte size.  Default values are stored
    //  separately as raw bytes (see MaterialBlueprint).
    // ----------------------------------------------------------
    struct MaterialParamDescriptor
    {
        std::string         name;                           // GLSL-side uniform / slot name
        ShaderDataType      type = ShaderDataType::None;
        uint32_t            byteOffset  = 0;                // offset inside the packed param block
        uint32_t            byteSize    = 0;                // sizeof the value
        uint32_t            arrayCount  = 1;                // >1 for small inline arrays

        // Texture-specific: which logical slot index (0..15)
        uint8_t             textureSlotIndex = 0xFF;
    };


    // ----------------------------------------------------------
    //  MaterialParamValue
    //  Runtime typed union used by MaterialParameter overrides.
    //  Scalar/vector variants occupy the first part;
    //  texture references the last.
    // ----------------------------------------------------------
    using MaterialParamValue = std::variant<
        float,
        glm::vec2,
        glm::vec3,
        glm::vec4,
        int32_t,
        glm::ivec2,
        glm::ivec3,
        glm::ivec4,
        uint32_t,
        glm::uvec2,
        glm::uvec3,
        glm::uvec4,
        bool,
        Ref<Texture>        // texture override; nullptr = use blueprint default
    >;

    // ----------------------------------------------------------
    //  TextureSlotDescriptor
    //  Declares one texture slot in the blueprint.
    //  name must match the GLSL sampler uniform.
    // ----------------------------------------------------------
    struct TextureSlotDescriptor
    {
        std::string  name;              // sampler name in shader
        TextureTypes semanticType;      // albedo / normal / roughness …
        uint8_t      bindingSlot;       // explicit GPU binding point
        bool         optional = true;   // false → blueprint compilation fails without it
    };




    // ----------------------------------------------------------
    //  MaterialStateFlags
    //  GPU fixed-function state that a pass variant may require.
    //  Stored alongside each ShaderVariant.
    // ----------------------------------------------------------
    namespace MaterialStateFlags
    {
        enum Bits : uint32_t
        {
            None = 0,
            DepthTestEnable     = BIT(0),
            DepthWriteEnable    = BIT(1),
            AlphaBlend          = BIT(2),
            DoubleSided         = BIT(3),
            WireFrame           = BIT(4),
            StencilEnable       = BIT(5),
            PolygonOffset       = BIT(6)    // maps to SetBiasGPU
        };
    }
    using MaterialStateFlag = uint32_t;

}
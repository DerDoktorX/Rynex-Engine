#pragma once
#if RY_ENABLE_WEBGPU_PILINE
namespace Rynex {

	using ColorWriteFlags = int;
	using StencilValue = int;
	using DepthBias = int;
	using Size32 = int;
	using SampleMask = int;
	using ShaderLocation = int;
	using Index32 = int;
	using Size64 = uint64_t;

	enum class VertexStepMode
	{
		None = 0
		, Vertex
		, Instance
	};

	enum class VertexFormat
	{
		None = 0
		, Unint32
		, Int32
		, Float32
	};

	enum class PrimitiveTopology
	{
		None = 0
		, Point_list
		, Line_list
		, Line_strip
		, Triangle_list
		, Triangle_strip
	};

	enum class TextureFormat : uint32_t 
	{
		None = 0
		, R8unorm
		, R8snorm
		, R8uint
		, R8sint
		, R16uint
		, R16sint
		, R16float
		, Rg8unorm
		, Rg8snorm
		, Rg8uint
		, Rg8sint
		, R32uint
		, R32sint
		, R32float
		, Rg16uint
		, Rg16sint
		, Rg16float
		, Rgba8unorm
		, Rgba8unorm_srgb
		, Rgba8snorm
		, Rgba8uint
		, Rgba8sint
		, Bgra8unorm
		, Bgra8unorm_srgb
		, Rgb9e5ufloat
		, Rgb10a2unorm
		, Rg11b10ufloat
		, Rg32uint
		, Rg32sint
		, Rg32float
		, Rgba16uint
		, Rgba16sint
		, Rgba16float
		, Rgba32uint
		, Rgba32sint
		, Rgba32float
		, Stencil8
		, Depth16unorm
		, Depth24plus
		, Depth24plus_stencil8
		, Depth32float
		, Depth32float_stencil8
		, Bc1_rgba_unorm
		, Bc1_rgba_unorm_srgb
		, Bc2_rgba_unorm
		, Bc2_rgba_unorm_srgb
		, Bc3_rgba_unorm
		, Bc3_rgba_unorm_srgb
		, Bc4_r_unorm
		, Bc4_r_snorm
		, Bc5_rg_unorm
		, Bc5_rg_snorm
		, Bc6h_rgb_ufloat
		, Bc6h_rgb_float
		, Bc7_rgba_unorm
		, Bc7_rgba_unorm_srgb
		, Etc2_rgb8unorm
		, Etc2_rgb8unorm_srgb
		, Etc2_rgb8a1unorm		
		, Etc2_rgb8a1unorm_srgb	
		, Etc2_rgba8unorm			
		, Etc2_rgba8unorm_srgb	
		, Eac_r11unorm			
		, Eac_r11snorm			
		, Eac_rg11unorm			
		, Eac_rg11snorm			
		, Astc_4x4_unorm			
		, Astc_4x4_unorm_srgb		
		, Astc_5x4_unorm			
		, Astc_5x4_unorm_srgb		
		, Astc_5x5_unorm			
		, Astc_5x5_unorm_srgb		
		, Astc_6x5_unorm			
		, Astc_6x5_unorm_srgb
		, Astc_6x6_unorm
		, Astc_6x6_unorm_srgb
		, Astc_8x5_unorm
		, Astc_8x5_unorm_srgb
		, Astc_8x6_unorm
		, Astc_8x6_unorm_srgb
		, Astc_8x8_unorm
		, Astc_8x8_unorm_srgb
		, Astc_10x5_unorm
		, Astc_10x5_unorm_srgb
		, Astc_10x6_unorm
		, Astc_10x6_unorm_srgb
		, Astc_10x8_unorm
		, Astc_10x8_unorm_srgb
		, Astc_10x10_unorm
		, Astc_10x10_unorm_srgb
		, Astc_12x10_unorm
		, Astc_12x10_unorm_srgb
		, Astc_12x12_unorm
		, Astc_12x12_unorm_srgb
	};

	enum class BlendOperation
	{
		None = 0
		, Add 
		, Subtrac 
		, Reverse 
		, Min 
		, Max
	};

	enum class BlendFactor
	{
		None = 0
		, Zero
		, One
		, Src
		, One_minus_src
		, Src_alpha
		, One_minus_src_alpha
		, Dst
		, One_minus_dst
		, Dst_alpha
		, One_minus_dst_alpha
		, Src_alpha_saturated
		, Constant
		, One_minus_constant
	};

	enum class IndexFormat
	{
		None = 0
		, Unint16
		, Unint32
	};

	enum class FrontFace
	{
		None = 0
		, ccw
		, cw
	};

	enum class CullMode
	{
		None = 0
		, Front
		, Back
	};

	enum class CompareFunction
	{
		None = 0
		, Never
		, Less
		, Equal
		, Less_equal
		, Greater
		, Not_equal
		, Greater_equal
		, Always
	};

	enum class StencilOperation
	{
		None = 0
		, Sero 
		, Keep 
		, Replace 
		, Invert 
		, Increment_clamp 
		, Decrement_clamp 
		, Increment_wrap 
		, Decrement_wrap
	};
#if 0
	enum class BindingResource
	{
		None = 0
		, GPUSampler
		, GPUTextureView
		, GPUBufferBinding
		, GPUExternalTexture
	};
#endif
	struct VertexAttribute
	{
		VertexFormat Format;
		uint32_t Offset;
		Index32 ShaderLocation;
	};

	struct VertexBufferLayout
	{
		uint32_t ArrayStride;
		VertexStepMode StepMode;
		std::vector<VertexAttribute> Attributes;
	};

	struct PrimitiveState
	{
		PrimitiveTopology Topology;// Not Rady
		IndexFormat StripIndexFormat;
		FrontFace Face;
		CullMode Cull;
		bool UnclippedDepth;

	};

	struct StencilFaceState
	{
		CompareFunction Compare;
		StencilOperation FailOp;
		StencilOperation DepthFailOp;
		StencilOperation PassOp;
	};

	struct BlendComponent
	{
		BlendOperation Operation;
		BlendFactor SrcFactor;
		BlendFactor DstFactor;
	};

	struct BlendState
	{
		BlendComponent Color;
		BlendComponent Alpha;
	};

	struct ColorTargetState
	{
		TextureFormat Format;
		BlendState Blend;
		ColorWriteFlags WriteMask;
	};

	struct FragmentState
	{
		std::vector<ColorTargetState> Targets;
	};

	struct DepthStencilState
	{
		TextureFormat Format;
		bool DepthWriteEnabled;
		CompareFunction DepthCompare;
		StencilFaceState StencilFront;
		StencilFaceState StencilBack;		
		StencilValue StencilReadMask;
		StencilValue StencilWriteMask;
		DepthBias DepthBias;
		int DepthBiasSlopeScale;
		int DepthBiasClamp;
	};

	struct MultisampleState
	{
		Size32 Count;
		SampleMask Mask;
		bool AlphaToCoverageEnabled;
	};

	struct VertexState
	{
		std::vector<VertexBufferLayout> Buffers;
	};

	struct Buffer
	{

	};

	// CreateDiscription

	struct BindingResource {};

	struct BufferBinding : BindingResource
	{
		Buffer Buffer;
		Size64 Offset;
		Size64 Size;
	};

	struct Sampler : BindingResource 
	{
	};

	struct TextureView : BindingResource
	{
	};

	struct ExternalTexture : BindingResource
	{
	};



	struct BindGroupEntry
	{
		Index32 Binding;
		BindingResource Resource;
	};

	struct BindGroupLayout
	{

	};

	struct RenderPipeline
	{

	};

	struct BindGroup
	{

	};

	// Descriptions
	struct RenderPipelineDescriptor
	{
		std::vector <VertexState> Vertex;
		FragmentState Fragment;
		PrimitiveState Primitive;
		DepthStencilState DepthStencil;
		MultisampleState Multisample;

	};

	struct BindGroupDescriptor
	{
		BindGroupLayout Layout;
		std::vector<BindGroupEntry> Entries;
	};

	


	class Device
	{
	public:
		static RenderPipeline CreateRenderPipeline(RenderPipelineDescriptor descriptor);
		static BindGroup CreateBindGroup(BindGroupDescriptor descriptor);

	};

}
#endif


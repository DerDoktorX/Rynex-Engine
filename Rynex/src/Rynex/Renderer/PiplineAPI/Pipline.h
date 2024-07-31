#pragma once

namespace Rynex {

	enum class VertexStepMode
	{
		Nonoe = 0,
		Vertex,
		Instance,
	};

	enum class VertexFormat
	{
		Nonoe = 0,
		Unint32,
		Int32,
		Float32,
	};

	enum class PrimitiveTopology
	{
		Nonoe = 0,
		
	};

	struct VertexAttribute
	{
		VertexFormat Format;
		uint32_t Offset;
	};

	struct VertexBufferLayout
	{
		uint32_t ArrayStride;
		VertexStepMode StepMode;
		VertexAttribute Attributes;
	};

	struct PrimitiveState
	{
		PrimitiveTopology Topology;// Not Rady
	};

	struct FragmentState
	{

	};

	struct DepthStencilState
	{

	};

	struct MultisampleState
	{

	};

	struct RenderPipelineDescriptor
	{
		VertexBufferLayout* Vertex = nullptr; // Array
		FragmentState Fragment;
		PrimitiveState Primitive;
		DepthStencilState DepthStencil;
		MultisampleState Multisample;
	};

	class Pipline
	{
	public:
		virtual void CreateRenderPipeline(RenderPipelineDescriptor descriptor) = 0;
	};

}



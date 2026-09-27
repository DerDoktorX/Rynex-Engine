#pragma once
#include <rypch.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/PipelineRenderImpl.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplinePoliciesDepth.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplinePoliciesShade.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplinePoliciesShape.h>

namespace Rynex {

#if 1


#ifdef RY_INSTANCE_MESH_PIPLINE_RENDER_SHADE_TEMPLATE
    using InstanceMeshPiplineRenderShade = PipelineRenderImpl<Shade::InstanceLayout, Shade::Resources, Shade::Geometry>;
#endif

#ifdef RY_INSTANCE_MESH_PIPLINE_RENDER_DEPTH_TEMPLATE
    using InstanceMeshPiplineRenderDepth = PipelineRenderImpl<Depth::InstanceLayout, Depth::Resources,  Depth::Geometry>;
#endif

#ifdef RY_INSTANCE_MESH_PIPLINE_RENDER_SHAPE_TEMPLATE
    using InstanceMeshPiplineRenderShape = PipelineRenderImpl<Shade::InstanceLayout, Shape::Resources, Shade::Geometry>;
#endif


#endif
}

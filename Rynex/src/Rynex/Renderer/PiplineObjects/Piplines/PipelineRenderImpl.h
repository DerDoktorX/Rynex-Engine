#pragma once


#include <Rynex/Renderer/PiplineObjects/Piplines/InstanceLayout.h>
#include <Rynex/Renderer/RenderCommand.h>

namespace Rynex {
#ifdef RY_INSTANCE_MESH_PIPLINE_RENDER_TEMPLATE
    template<typename InstanceLayout, typename Resources, typename Geometry>
    class PipelineRenderImpl : public InstanceMeshPiplineRenderBase
    {
    public:
        using RenderObject = typename InstanceLayout::RenderObject;

        PiplineResultState SubmitEntityMeshObject(const SingleMeshObject& singleMesh, const Ref<Shader>& shader, const glm::mat4& model, uint32_t& storeIndex, int entityID) override
        {
            // CheckSubmitMeshObject
            int result = CheckSubmitMeshObject(shader, singleMesh);           // shared, unchanged
            if (State_MaxEntityRender <= m_InstanceCount)
                return static_cast<PiplineResultState>(result | Result_NoRenderObjectSpaceLeft);

            if (BIT_EQUAL(result, Result_AllNotAllowed | Result_AllNoSpaceLeft) == 0)
            {
                if (m_InstanceCount != storeIndex)
                    storeIndex = m_InstanceCount;

                bool newResources = SubmitResources(shader, singleMesh);      // shared
                SetObject(storeIndex, InstanceLayout::Make(model, entityID));  // shared growth logic
                m_InstanceCount++;
                m_DrawsAfterLastUpdate = 0u;
                result = Result_Success;
            }

            return static_cast<PiplineResultState>(result);
        }

        void DrawNow(int flags) override
        {
            BeforeDrawCall();     // shared: grows/uploads m_ModelBufferVAO using InstanceLayout::GetVertexLayout()
            RenderCommand::SetMode(flags);
            BindResources();      // base binds shader+camera, then m_Resources.Bind()
            Mesh::PerDrawObject drawElement = Geometry::DrawObject(m_SingleMeshObject.m_MeshSingle);
            drawElement.m_InstanceCount = m_InstanceCount;
            RenderCommand::DrawElement(m_VertexArray, drawElement);
        }

    protected:
        int CheckSubmitMeshObject(const Ref<Shader>&shader, const SingleMeshObject & singleMesh);
        bool SubmitResources(const Ref<Shader>&shader, const SingleMeshObject & singleMesh);
    private:
        std::vector<RenderObject> m_RenderObjectsVec;
        Resources                 m_Resources;
    };


    template <typename InstanceLayout, typename Resources, typename Geometry>
    int PipelineRenderImpl<InstanceLayout, Resources, Geometry>::CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
    {
		static_assert(false, "Default implemtion not seported!");
		return Result_Error;
	}

    template<typename InstanceLayout, typename Resources, typename Geometry>
    bool PipelineRenderImpl<InstanceLayout, Resources, Geometry>::SubmitResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
    {
        static_assert(false, "Default implemtion not seported!");
        return false;
    }

    using InstanceMeshPiplineRenderShade = PipelineRenderImpl<ShadeInstanceLayout, ShadeResources, ShadeGeometry>;
    using InstanceMeshPiplineRenderDepth = PipelineRenderImpl<DepthInstanceLayout, DepthResources, DepthGeometry>;
    // using InstanceMeshPiplineRenderShape = PipelineRenderImpl<DepthInstanceLayout, ShapeResources, DepthGeometry>;

    template <>
    inline int PipelineRenderImpl<ShadeInstanceLayout, ShadeResources, ShadeGeometry>::CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
    {
        int result = Result_None;

        CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
        CheckObject(m_SingleMeshObject.m_MeshSingle, singleMesh.m_MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);
        const Ref<Material>& materielTest = singleMesh.m_Material;
        const Ref<Material>& materielThis = m_SingleMeshObject.m_Material;
        if (nullptr == materielTest)
            return (result | Result_NotAllowedShadeDefinition);
        if (nullptr == materielThis)
            return result;

        if(materielThis->GetDepthRenderMode() != materielTest->GetDepthRenderMode())
            result |= Result_NotAllowedShadeDefinition | Result_NoShadeDefinitionSpaceLeft;


        return result;
    }

    template <>
    inline int PipelineRenderImpl<DepthInstanceLayout, DepthResources, DepthGeometry>::CheckSubmitMeshObject(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
    {
        int result = Result_None;

        CheckObject(m_Shader, shader, result, Result_NotAllowedShader, Result_NoShaderSpaceLeft);
        CheckObject(m_SingleMeshObject.m_MeshSingle, singleMesh.m_MeshSingle, result, Result_NotAllowedRenderShape, Result_NoRenderShapeSpaceLeft);
        const Ref<Material>& materielTest = singleMesh.m_Material;
        const Ref<Material>& materielThis = m_SingleMeshObject.m_Material;
        if (nullptr == materielTest)
            return (result | Result_NotAllowedShadeDefinition);
        if (nullptr == materielThis)
            return result;

        if(materielThis->GetDepthRenderMode() != materielTest->GetDepthRenderMode())
            result |= Result_NotAllowedShadeDefinition | Result_NoShadeDefinitionSpaceLeft;

        return result;
    }


    template <>
    inline bool PipelineRenderImpl<ShadeInstanceLayout, ShadeResources, ShadeGeometry>::SubmitResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
    {
        m_Shader = shader;
        m_SingleMeshObject = singleMesh;

        const Ref<Material>& materiel = m_SingleMeshObject.m_Material;

        m_Resources.m_Albedo = materiel->GetAlbedoTextures();
        m_RenderMode = materiel->GetShadeRenderMode();

        m_Resources.m_MaterielUB = materiel->GetMaterielUniformBuffer();

        Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
        return CheckVAOFromMeshSingleShade(m_VertexArray, meshSingle);
    }

    template <>
    inline bool PipelineRenderImpl<DepthInstanceLayout, DepthResources, DepthGeometry>::SubmitResources(const Ref<Shader>& shader, const SingleMeshObject& singleMesh)
    {
        m_Shader = shader;
        m_SingleMeshObject = singleMesh;
        const Ref<Material>& materiel = m_SingleMeshObject.m_Material;

        m_RenderMode = materiel->GetShadeRenderMode();

        Ref<MeshSingle>& meshSingle = m_SingleMeshObject.m_MeshSingle;
        return CheckVAOFromMeshSingleDepth(m_VertexArray, meshSingle);
    }




#endif


}




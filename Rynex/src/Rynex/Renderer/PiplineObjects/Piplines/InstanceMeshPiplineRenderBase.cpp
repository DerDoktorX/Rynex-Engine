#include "rypch.h"
#include "InstanceMeshPiplineRenderBase.h"
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/API/Shader.h>

namespace Rynex {

    InstanceMeshPiplineRenderBase::InstanceMeshPiplineRenderBase()
        : m_SingleMeshObject()
        , m_InstanceCount(0u)
        , m_DrawsAfterLastUpdate(0u)
        , m_ManagingMode(PiplineManagingState::Managing_None)
        , m_Shader(nullptr)
        , m_CameraBuffer(nullptr)
        , m_RenderMode(0)
    {
    }

    InstanceMeshPiplineRenderBase::~InstanceMeshPiplineRenderBase()
    {
        RY_DESTROY_REF(m_ModelBufferVAO);
        RY_DESTROY_REF(m_VertexArray);
    }

    BufferLayout InstanceMeshPiplineRenderBase::GetExpectedOutput() const
    {
        return m_OutPut;
    }

    void InstanceMeshPiplineRenderBase::SetExpectedOutput(const BufferLayout& output)
    {
        m_OutPut = output;
    }

    void InstanceMeshPiplineRenderBase::SetCameraUniformBuffer(Ref<UniformBuffer> camerbuffer)
    {
        m_CameraBuffer = camerbuffer;

    }

    void InstanceMeshPiplineRenderBase::SetDisplayUniformBuffer(Ref<UniformBuffer> dispalaybuffer)
    {
    }



    void InstanceMeshPiplineRenderBase::SetDataMangingFlags(PiplineManagingState flags)
    {
        RY_CORE_WARN("This PiplineManagingState is changing nothing on this is a one Render Object Call");
        m_ManagingMode = flags;
    }

    void InstanceMeshPiplineRenderBase::DrawNow()
    {
        DrawNow(m_RenderMode);
    }

    bool InstanceMeshPiplineRenderBase::IsExpectedOutPut(const Ref<Shader>& shader) const
    {
        RY_REMBER_FUNC_CHANGE("Implemnt function check if the out put layout matches the out put layout from shader!");
        const BufferLayout& layout = shader->GetOutPut();
        return layout == m_OutPut;
    }

    void InstanceMeshPiplineRenderBase::BindResources()
    {
        m_Shader->Bind();
        m_CameraBuffer->Bind(UniformBinding_MainCamera);
    }

    void InstanceMeshPiplineRenderBase::UnbindResources()
    {
        m_Shader->UnBind();
        m_CameraBuffer->UnBind(UniformBinding_MainCamera);
    }

    void InstanceMeshPiplineRenderBase::Clear()
    {
        m_InstanceCount = 0u;
        m_RenderMode = 0;
        RY_DESTROY_REF(m_Shader);
        RY_DESTROY_REF(m_CameraBuffer);
    }

    void InstanceMeshPiplineRenderBase::ClearRenderObjects()
    {
        m_InstanceCount = 0u;
    }


} // Rynex
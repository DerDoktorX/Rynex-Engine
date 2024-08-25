#include "rypch.h"
#include "Renderer3D.h"

#include "Rynex/Renderer/RenderCommand.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rynex {

    struct RendererData
    {
        Camera Camera;
        glm::mat4 CameraView;
        glm::mat4 CameraProj;
        glm::mat4 CameraViewProj;
        glm::vec3 CameraPos;
        int RenderChacheAktive = 0;
        int RenderDefaultAktive = 0;
        Renderer3D::Statistics Stats;
    };


    static RendererData s_Data;


    void Renderer3D::Init()
    {
        RY_PROFILE_FUNCTION();
        RY_CORE_INFO("Renderer3D::Init");
    }

    void Renderer3D::Shutdown()
    {
        RY_PROFILE_FUNCTION();
        RY_CORE_INFO("Renderer3D::Shutdown");
    }

    void Renderer3D::BeginScene(const Ref<EditorCamera>& camera)
    {
        RY_PROFILE_FUNCTION();
        s_Data.CameraViewProj = camera->GetViewProjection();
        s_Data.CameraView = camera->GetViewMatrix();
        s_Data.CameraProj = camera->GetProjektion();
        s_Data.CameraPos = camera->GetPosition();
    }

    void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transformCamera)
    {
        RY_PROFILE_FUNCTION();
        s_Data.Camera = camera;
        s_Data.CameraView = transformCamera;
        s_Data.CameraViewProj = camera.GetProjektion() * transformCamera;
        s_Data.CameraProj = camera.GetProjektion();
        
    }

    void Renderer3D::EndScene()
    {
        RY_PROFILE_FUNCTION();
    }



    void Renderer3D::BeforDrawEntity(const MaterialComponent& material, const glm::mat4& model)
    {
        RY_PROFILE_FUNCTION();
        const Ref<Shader>& shader = material.Shader;
        const std::vector<UniformElement>& unifomrsLayoute = material.UniformLayoute;
        shader->Bind();
        
        // Vertex
        // shader->SetMat4("u_ViewProj", s_Data.ViewProj);
        // shader->SetMat4("u_Model", model);
       
        // Fragment
        // shader->SetFloat3("u_Color", material.Color);
        for (const UniformElement& uniform : unifomrsLayoute)
        {
            if (uniform.GloblelResurce && uniform.UUID == 0)
            {
                switch (uniform.ShaderResourceType)
                {
                    case ShaderResourceType::LocalModel:
                    {
                        shader->SetMat4(uniform.Name, model);
                        break;
                    }
                    case ShaderResourceType::LocalColor:
                    {
                        shader->SetFloat3(uniform.Name, material.Color);
                        break;
                    }
                    case ShaderResourceType::MainCamerPos:
                    {
                        shader->SetFloat3(uniform.Name, s_Data.CameraPos);
                        break;
                    }
                    case ShaderResourceType::MainCameraViewProjectionMatrix:
                    {
                        shader->SetMat4(uniform.Name, s_Data.CameraViewProj);
                        break;
                    }
                    case ShaderResourceType::MainCameraViewMatrix:
                    {
                        shader->SetMat4(uniform.Name, s_Data.CameraView);
                        break;
                    }
                    case ShaderResourceType::MainCamerProjectionMatrix:
                    {
                        shader->SetMat4(uniform.Name, s_Data.CameraProj);
                        break;
                    }
                    default:
                        break;
                }
            }
            else
            {
                // RY_CORE_ASSERT(false, "Not Rady!");
                if (uniform.Type == ShaderDataType::Texture)
                {

                }
                else 
                {
                    shader->SetUniformValue(uniform.Name, uniform.LocalResurce, uniform.Type);
                }
            }
            
        }

        if (material.Texture != nullptr)
             material.Texture->Bind();
    }

    void Renderer3D::AfterDrawEntity(const MaterialComponent& material)
    { 
        RY_PROFILE_FUNCTION();
        material.Shader->UnBind();
    }

    void Renderer3D::AktivePolyGunMode(bool active)
    {
        RenderCommand::AktivePolyGunMode(active);
    }

    void Renderer3D::SetDethTest(bool active)
    {
        RenderCommand::SetDethTest(active);
    }

    void Renderer3D::SetFace(CallFace callFace)
    {
        RenderCommand::SetFace(callFace);
    }

    void Renderer3D::DrawMesh(const Ref<VertexArray>& vertexArray)
    {
        RY_PROFILE_FUNCTION();
        //vertexArray->Bind();
        RenderCommand::DrawIndexedMesh(vertexArray);
        //RenderCommand::DrawIndexedLineLoop(vertexArray);
        //vertexArray->UnBind();
    }

    void Renderer3D::DrawMeshStrips(const Ref<VertexArray>& vertexArray)
    {
        RenderCommand::DrawStripsMesh(vertexArray);
    }

    void Renderer3D::DrawLine(const Ref<VertexArray>& vertexArray)
    {
        RY_PROFILE_FUNCTION();
        //vertexArray->Bind();
        RenderCommand::DrawIndexedLine(vertexArray);
        //vertexArray->UnBind(); 
    }

    void Renderer3D::DrawLineLoop(const Ref<VertexArray>& vertexArray)
    {
        RenderCommand::DrawIndexedLineLoop(vertexArray);
    }

    void Renderer3D::DrawPoints(const Ref<VertexArray>& vertexArray)
    {
        //RY_CORE_ASSERT(false, "DrawPoints not implemntet jet!");
        RenderCommand::DrawIndexedPoints(vertexArray);
    }

    void Renderer3D::DrawPatches(const Ref<VertexArray>& vertexArray)
    {
        RenderCommand::DrawPatches(vertexArray);
    }

    void Renderer3D::DrawError()
    {
        RenderCommand::DrawError();
    }
}
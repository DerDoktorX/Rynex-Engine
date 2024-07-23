#include "rypch.h"
#include "Renderer3D.h"

#include "Rynex/Renderer/RenderCommand.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rynex {

    struct Renderer
    {
        Camera Camera;
        glm::mat4 TransformCamera;
        glm::mat4 ViewProj;

        Renderer3D::Statistics Stats;
    };


    static Renderer s_Data;

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

    void Renderer3D::BeginScene(const EditorCamera& camera)
    {
        RY_PROFILE_FUNCTION();
        s_Data.ViewProj = camera.GetViewProjection();
    }

    void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transformCamera)
    {
        RY_PROFILE_FUNCTION();
        s_Data.Camera = camera;
        s_Data.TransformCamera = transformCamera;
        s_Data.ViewProj = camera.GetProjektion() * glm::inverse(transformCamera);
    }

    void Renderer3D::EndScene()
    {
        RY_PROFILE_FUNCTION();
    }

    void Renderer3D::BeforDrawEntity(const MaterialComponent& material, const glm::mat4& model)
    {
        RY_PROFILE_FUNCTION();
        auto& shader = material.Shader;
        shader->Bind();

        // Vertex
        shader->SetMat4("u_ViewProj", s_Data.ViewProj);
        shader->SetMat4("u_Model", model);

        // Fragment
        shader->SetFloat3("u_Color", material.Color);

        if (material.Texture != nullptr)
             material.Texture->Bind();
    }

    void Renderer3D::AfterDrawEntity(const MaterialComponent& material)
    { 
        RY_PROFILE_FUNCTION();
        //material.Shader->UnBind();
    }

    void Renderer3D::DrawMesh(const Ref<VertexArray>& vertexArray)
    {
        RY_PROFILE_FUNCTION();
        //vertexArray->Bind();
        RenderCommand::DrawIndexedMesh(vertexArray);
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
}
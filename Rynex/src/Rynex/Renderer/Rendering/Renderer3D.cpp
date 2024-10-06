#include "rypch.h"
#include "Renderer3D.h"

#include "Rynex/Renderer/RenderCommand.h"
#include "Rynex/Asset/Base/AssetManager.h"

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
        glm::vec3 CameraPos = {0.0,-1,1.5};
        int RenderChacheAktive = 0;
        int RenderDefaultAktive = 0;
        Renderer3D::Statistics Stats;
        Ref<Shader> Default3DShader = nullptr;
    };


    static RendererData s_Data;

   

    void Renderer3D::Init()
    {
        RY_PROFILE_FUNCTION();
        RY_CORE_INFO("Renderer3D::Init");
        s_Data.Default3DShader = AssetManager::GetAsset<Shader>("Assets/shaders/Default_Rynex-Editor_Shader.glsl");
    }

    void Renderer3D::Shutdown()
    {
        RY_CORE_INFO("Renderer3D::Shutdown");
    }

    void Renderer3D::BindDefault3DShader(const Ref<Texture>& bindeTexure, const glm::mat4& modelMatrix, int entityID, const glm::vec3& objectColor)
    { 
        Ref<Shader>& shader = s_Data.Default3DShader;
        if (shader == nullptr) 
            return;
      
        shader->Bind();
        shader->SetInt("u_EntityID", entityID);
        shader->SetMat4("u_Model", modelMatrix);
        shader->SetMat4("u_ViewProj", s_Data.CameraViewProj);
        shader->SetFloat3("u_Color", objectColor);
        shader->SetFloat3("u_CamerPos", s_Data.CameraPos);
        shader->SetInt("u_Texture_Diffuse1", 0);  
        if (bindeTexure != nullptr)
            bindeTexure->Bind(0);
    }

    void Renderer3D::BeginScene(const Ref<EditorCamera>& camera)
    {
        s_Data.CameraViewProj = camera->GetViewProjection();
        s_Data.CameraView = camera->GetViewMatrix();
        s_Data.CameraProj = camera->GetProjektion();
        s_Data.CameraPos = camera->GetPosition();
        s_Data.Stats.StartTimePoint = std::chrono::high_resolution_clock::now();
    }

    void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transformCamera)
    {
        s_Data.Camera = camera;
        s_Data.CameraView = transformCamera;
        s_Data.CameraViewProj = camera.GetProjektion() * transformCamera;
        s_Data.CameraProj = camera.GetProjektion();
        glm::vec3 camerPos = glm::vec3(transformCamera[3]);
        s_Data.CameraPos = camerPos;
        s_Data.Stats.StartTimePoint = std::chrono::high_resolution_clock::now();
    }

    void Renderer3D::EndScene()
    {
        s_Data.Stats.EndTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.Stats.DrawTime.push_back(
            std::chrono::time_point_cast<std::chrono::microseconds>(s_Data.Stats.EndTimePoint).time_since_epoch().count() 
            - std::chrono::time_point_cast<std::chrono::microseconds>(s_Data.Stats.StartTimePoint).time_since_epoch().count()
        );

    }

    void Renderer3D::DrawObjectRender3D(const Ref<VertexArray>& vertexArray)
    {
        switch (vertexArray->GetPrimitv())
        {
        case VertexArray::Primitv::Traingle:
        {
            DrawMesh(vertexArray);
            return;
        }
        case VertexArray::Primitv::TraingleStrips:
        {
            DrawMeshStrips(vertexArray);

            return;
        }
        case VertexArray::Primitv::TraingleFan:
        {
            RY_CORE_ASSERT(false, "Primitv TraingleFan is not Implemantent jet!");
            //DrawMeshStrips(geomtry.Geometry);
            return;
        }
        case VertexArray::Primitv::Line:
        {
            DrawLine(vertexArray);
            return;
        }
        case VertexArray::Primitv::LineLoop:
        {
            //RY_CORE_ASSERT(false, "Primitv LineLoop is not Implemantent jet!");
            DrawLineLoop(vertexArray);
            return;
        }
        case VertexArray::Primitv::LineStrips:
        {
            RY_CORE_ASSERT(false, "Primitv LineStrips is not Implemantent jet!");
            return;
        }
        case VertexArray::Primitv::Points:
        {
             DrawPoints(vertexArray);
            return;
        }
        case VertexArray::Primitv::Patches:
        {
             DrawPatches(vertexArray);
            return;
        }
        default:
            RY_CORE_ASSERT(false, "Primitv type is Unknown!");
            break;
        }
    };

    void Renderer3D::BeforDrawEntity(const MaterialComponent& material, const glm::mat4& model, int entityID)
    {
        const Ref<Shader>& shader = material.Shader;
        if (shader == nullptr)
        {
            BindDefault3DShader(material.Texture, model, entityID, material.Color);
            return;
        }
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
                    case ShaderResourceType::EnitiyID:
                    {
                        shader->SetInt(uniform.Name, entityID);
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
                    shader->SetUniformValue(uniform.Name, (void*)uniform.LocalResurce.data(), uniform.Type);
                }
            }
            
        }

        if (material.Texture != nullptr)
             material.Texture->Bind();
    }

    void Renderer3D::DrawModdelMesh(const MaterialComponent& material, const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, int entityID)
    {
        s_Data.Stats.DrawCalls++;
        const Ref<Shader>& shader = material.Shader;
        const Ref<VertexArray>& vertexArray = mesh->GetVertexArray();
        const std::vector<MeshTexture>& textures = mesh->GetTextures();

        if (shader == nullptr)
        {
           
            uint32_t diffuseNr = 1;
            uint32_t specularNr = 1;
            std::string number;
            Ref<Texture> texture = nullptr;
            for (uint32_t i = 0; i < textures.size(); i++)
            {
                const std::string& name = textures[i].Type;
                if (name == "u_Texture_Diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "u_Texture_Speculare")
                    number = std::to_string(specularNr++);


                texture = textures[i].TextureResur;
            } 
            BindDefault3DShader(texture, modelMatrix, entityID, material.Color);
            vertexArray->Bind();
            RenderCommand::DrawIndexedMesh(vertexArray);
            return;
        }

        const std::vector<UniformElement>& unifomrsLayoute = material.UniformLayoute;

        shader->Bind();

        for (const UniformElement& uniform : unifomrsLayoute)
        {
            if (uniform.GloblelResurce && uniform.UUID == 0)
            {
                switch (uniform.ShaderResourceType)
                {
                case ShaderResourceType::LocalModel:
                {
                    shader->SetMat4(uniform.Name, modelMatrix);
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
                case ShaderResourceType::EnitiyID:
                {
                    shader->SetInt(uniform.Name, entityID);
                    break;
                }
                default:
                    break;
                }
            }
            else
            {
                if (uniform.Type == ShaderDataType::Texture)
                {

                }
                else
                {
                    shader->SetUniformValue(uniform.Name, (void*)uniform.LocalResurce.data(), uniform.Type);
                }
            }

        }
        
        uint32_t diffuseNr = 1;
        uint32_t specularNr = 1;
        std::string number = "";
        for (uint32_t i = 0; i < textures.size(); i++)
        {
            const std::string& name = textures[i].Type;
            if (name == "u_Texture_Diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "u_Texture_Speculare")
                number = std::to_string(specularNr++);
            

            shader->SetInt(("material." + name + number).c_str(), i);
            textures[i].TextureResur->Bind(i);
        }

        vertexArray->Bind();
        RenderCommand::DrawIndexedMesh(vertexArray);
        vertexArray->UnBind();
        shader->UnBind();
    }

    void Renderer3D::DrawModdel(const MaterialComponent& material, const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID)
    {
        DrawModdelMesh(material, modelMatrix, model.MeshR, entityID);
    }

    void Renderer3D::DrawModdel(const MaterialComponent& material, const glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID)
    {

        const std::vector<Ref<Mesh>> &meshes = model.ModelR->GetMeshes();
        for (auto& mesh : meshes)
        {
            DrawModdelMesh(material, modelMatrix, mesh, entityID);
        }
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
        s_Data.Stats.DrawCalls++;
        //vertexArray->Bind();
        RenderCommand::DrawIndexedMesh(vertexArray);
        //RenderCommand::DrawIndexedLineLoop(vertexArray);
        //vertexArray->UnBind();
    }

    void Renderer3D::DrawMeshStrips(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawStripsMesh(vertexArray);
    }

    void Renderer3D::DrawLine(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        //vertexArray->Bind();
        RenderCommand::DrawIndexedLine(vertexArray);
        //vertexArray->UnBind(); 
    }

    void Renderer3D::DrawLineLoop(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawIndexedLineLoop(vertexArray);
    }

    void Renderer3D::DrawPoints(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        //RY_CORE_ASSERT(false, "DrawPoints not implemntet jet!");
        RenderCommand::DrawIndexedPoints(vertexArray);
    }

    void Renderer3D::DrawPatches(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawPatches(vertexArray);
    }

    void Renderer3D::DrawError()
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawError();
    }

    void Renderer3D::ResetStats()
    {
        s_Data.Stats.DrawTime.clear();
        s_Data.Stats.DrawCalls = 0;
        s_Data.Stats.FrameCount++;
    }

    Renderer3D::Statistics Renderer3D::GetStats()
    {
        return s_Data.Stats;
    }
}
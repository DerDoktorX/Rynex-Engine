#include "rypch.h"
#include "Renderer3D.h"

#include "Rynex/Renderer/RenderCommand.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Renderer/Objects/BoxAABB.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rynex {

    struct AmbientData
    {
        float Color[3] = { 0,0,0 };
        float Intensitie= 0;
        AmbientData() = default;
        AmbientData(glm::vec3& color, float intensitie)
        {
            Color[0] = color.x;
            Color[1] = color.y;
            Color[2] = color.z;
            Intensitie = intensitie;
        }
    };

    struct DrirectionLigtheData
    {
        float Color[4] = {0,0,0,-1 };
        float Dirction[3] = { 0,0,0 };
        float Intensitie = 0;

        DrirectionLigtheData() = default;
        DrirectionLigtheData(glm::vec3& color, glm::vec3& dirction, float intensitie)
        {
            Color[0] = color.x;
            Color[1] = color.y;
            Color[2] = color.z;

            Dirction[0] = dirction.x;
            Dirction[1] = dirction.y;
            Dirction[2] = dirction.z;

            Intensitie = intensitie;
        }
    };

    struct DrirectionData
    {
        int Aktive[4] = { 0,-1,-1,-1 };

        DrirectionLigtheData Drirecion[8];
    };

    struct PointLigtheData
    {
        float Color[3] = { 0,0,0 };
        float Distence = 0;
        float Postion[3] = { 0,0,0 };
        float Intensitie =  0;
        

        PointLigtheData() = default;
        PointLigtheData(glm::vec3& color, glm::vec3& postion, float intensitie, float distence)
        {
            Color[0] = color.x;
            Color[1] = color.y;
            Color[2] = color.z;

            Postion[0] = postion.x;
            Postion[1] = postion.y;
            Postion[2] = postion.z;

            Intensitie = intensitie;
            Distence = distence;
        }
    };

    struct PointData
    {
        int Aktive[4] = { 0,-1,-1,-1 };
        PointLigtheData Point[32];
    };

    struct SpotLigtheData
    {
        float Color[3] = { 0,0,0 };
        float Intensitie = 0;
        float Postion[3] = { 0,0,0 };
        float Distence = 0;
        float Direction[3] = { 0,0,0 };
        float Inner = 0;
        float Outer = 0;
        float Empty[2] = { -1, -1 };
        SpotLigtheData() = default;
        SpotLigtheData(glm::vec3& color, glm::vec3& postion, glm::vec3& direction,  float intensitie, float distence, float inner, float outer)
            : Intensitie(intensitie), Distence(distence), Inner(inner), Outer(outer) 
        {
            Color[0] = color.x;
            Color[1] = color.y;
            Color[2] = color.z;

            Postion[0] = postion.x;
            Postion[1] = postion.y;
            Postion[2] = postion.z;

            Direction[0] = direction.x;
            Direction[1] = direction.y;
            Direction[2] = direction.z;
        }
    };

    struct SpotData
    {
        int Aktive[4] = { 0,-1,-1,-1 };
        SpotLigtheData Spot[32];
    };

    struct RendererData
    {
        Camera Camera;
        glm::mat4 CameraView;
        glm::mat4 CameraProj;
        glm::mat4 CameraViewProj;
        glm::vec3 CameraPos = { 0.0,-1,1.5 };
        int RenderChacheAktive = 0;
        int RenderDefaultAktive = 0;
        int RendererDefaultModeFlags;
        Renderer3D::Statistics Stats;
        Ref<Shader> Default3DShader = nullptr;

        Ref<UniformBuffer> AmbientLigthBuffer;
        Ref<UniformBuffer> DirectionelLigthBuffer;
        Ref<UniformBuffer> SpotLigthBuffer;
        Ref<UniformBuffer> PointLigthBuffer;
        DrirectionData BufferDrirectionData;

        Ref<UniformBuffer> CameraProjAndViewBuffer;
        Ref<UniformBuffer> CameraViewProjBuffer;
        // AABB
        Ref<Shader> BoxAABBShader = nullptr;
        Ref<VertexArray> BoxAABBVertexArray = nullptr;
        Ref<VertexBuffer> BoxAABBVertexBuffer = nullptr;
        glm::vec3 BoxAABB[8] = {
            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3( 1.0f, -1.0f,  1.0f),
            glm::vec3( 1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),

            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3( 1.0f, -1.0f, -1.0f),
            glm::vec3( 1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),
        };
    };


    static RendererData s_Data;



    void Renderer3D::Init()
    {
        RY_PROFILE_FUNCTION();
        RY_CORE_INFO("Renderer3D::Init");
        s_Data.Default3DShader = AssetManager::GetAsset<Shader>("Assets/shaders/Default_Rynex-Editor_Shader.glsl");
        {
            s_Data.BoxAABBShader = AssetManager::GetAsset<Shader>("Assets/shaders/BoxAABB.glsl");
            s_Data.BoxAABBVertexArray = VertexArray::Create();

            s_Data.BoxAABBVertexBuffer = VertexBuffer::Create(s_Data.BoxAABB, 8 * sizeof(glm::vec3), BufferDataUsage::DynamicDraw);

            s_Data.BoxAABBVertexBuffer->SetLayout({ { ShaderDataType::Float3,"a_Postion" } });

            uint32_t incies[] = {
                0, 1,
                1, 2,
                2, 3,
                3, 0,
                0, 4,
                4, 5,
                5, 6,
                6, 7,
                7, 4,
                7, 3,
                6, 2,
                5, 1
            };
            
            Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(incies, 24 * sizeof(uint32_t), BufferDataUsage::StaticDraw);
            s_Data.BoxAABBVertexArray->AddVertexBuffer(s_Data.BoxAABBVertexBuffer);
            s_Data.BoxAABBVertexArray->SetIndexBuffer(indexBuffer);
            s_Data.BoxAABBVertexArray->SetPrimitv(VertexArray::Primitv::Line);
        }

        {
            s_Data.CameraViewProjBuffer = UniformBuffer::Create(nullptr, sizeof(glm::mat4) * 1,
                {
                    {ShaderDataType::Float4x4, "Camera"},
                }, 0);

            s_Data.CameraProjAndViewBuffer = UniformBuffer::Create(nullptr, sizeof(glm::mat4) * 2,
                {
                         {ShaderDataType::Float4x4, "View"},
                         {ShaderDataType::Float4x4, "Projection"},
                }, 1);
        }
        
        {
            s_Data.AmbientLigthBuffer = UniformBuffer::Create(nullptr, 4 * 3 + 4 * 3 + 1 * 4,
                {
                    { ShaderDataType::Float3, "Color", 0 },
                    { ShaderDataType::Float, "Intensitie", 0 }
                }, 2);
        }

        {
            std::vector<BufferElement> element;
            element.resize((8 * 3));
            
            for (uint32_t i = 1; i < 9; i++)
            {
                element.emplace_back(ShaderDataType::Float3, "Color", i);
                element.emplace_back(ShaderDataType::Float3, "Dirction", i);
                element.emplace_back(ShaderDataType::Float, "Intensitie", i);
            }
           


            s_Data.DirectionelLigthBuffer = UniformBuffer::Create(&s_Data.BufferDrirectionData, (4 * 1) + ((4 * 4 + 4 * 4 + 1 * 4) * 8), element, 3);
        }

        {
            std::vector<BufferElement> element;
            element.resize((32 * 4)+1);
            element.emplace_back(ShaderDataType::Int, "Aktive", 0);
            for (uint32_t i = 1; i < 33; i++)
            {
                element.emplace_back(ShaderDataType::Float3, "Color", i);
                element.emplace_back(ShaderDataType::Float3, "Postion", i);
                element.emplace_back(ShaderDataType::Float, "Intensitie", i);
                element.emplace_back(ShaderDataType::Float, "Distence", i);
            }
            PointData pointBufferData;

            s_Data.PointLigthBuffer = UniformBuffer::Create(&pointBufferData, sizeof(PointData), element, 4);
        }

        {
            std::vector<BufferElement> element;
            element.resize((32 * 6)+1);
            element.emplace_back(ShaderDataType::Int, "Aktive", 0);
            for (uint32_t i = 1; i < 33; i++)
            {
                element.emplace_back(ShaderDataType::Float3, "Color", i);
                element.emplace_back(ShaderDataType::Float3, "Postion", i);
                element.emplace_back(ShaderDataType::Float, "Intensitie", i);
                element.emplace_back(ShaderDataType::Float, "Distence", i);
                element.emplace_back(ShaderDataType::Float, "Inner", i);
                element.emplace_back(ShaderDataType::Float, "Outer", i);
            }
            SpotData spotBufferData;
            s_Data.SpotLigthBuffer = UniformBuffer::Create(&spotBufferData, sizeof(SpotData), element, 5);
        }
    
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
        Renderer::SetMode(s_Data.RendererDefaultModeFlags);
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

    void Renderer3D::SetLigthUniform(AmbientLigthComponent& ambient, int index)
    {
        if (index !=  1)
            return;
        AmbientData ambientData(ambient.Color, ambient.Intensitie);
        s_Data.AmbientLigthBuffer->SetData(&ambientData, 0, sizeof(AmbientData));
    }

    void Renderer3D::SetLigthUniform(DrirektionleLigthComponent& drirektion, const glm::mat4& matrix, int index)
    {
        if (index <= 8)
        {
            DrirectionLigtheData drirectionLigtheData(drirektion.Color, glm::vec3(matrix[3]), drirektion.Intensitie);
            s_Data.DirectionelLigthBuffer->SetData(&index, 0, sizeof(int));
            s_Data.DirectionelLigthBuffer->SetData(&drirectionLigtheData, sizeof(int)*4 + sizeof(DrirectionLigtheData) * (index - 1), sizeof(DrirectionLigtheData));
        }
    }

    void Renderer3D::SetLigthUniform(SpotLigthComponent& spot, const glm::mat4& matrix, int index)
    {
        if (index <= 32)
        {
            glm::vec3 skew = {};
            glm::vec4 perspectiv = {};
            glm::qua qaurt = glm::quat(glm::vec3());
            glm::vec3 postion = {};
            glm::vec3 scale = {};
            glm::decompose(matrix, scale, qaurt, postion, skew, perspectiv);
            SpotLigtheData spotLigtheData(spot.Color, glm::vec3(matrix[3]), glm::eulerAngles(qaurt), spot.Intensitie, spot.Distence, spot.Inner, spot.Distence);
            s_Data.SpotLigthBuffer->SetData(&index, 0, sizeof(int));
            s_Data.SpotLigthBuffer->SetData(&spotLigtheData, sizeof(int)*4 + sizeof(SpotLigtheData) * (index - 1), sizeof(SpotLigtheData));
        }
    }

    void Renderer3D::SetLigthUniform(PointLigthComponent& point, const glm::mat4& matrix, int index)
    {
        if (index <= 32)
        {
            PointLigtheData pointLigtheData(point.Color, glm::vec3(matrix[3]), point.Intensitie, point.Distence);
            s_Data.PointLigthBuffer->SetData(&index, 0, sizeof(int));
            s_Data.PointLigthBuffer->SetData(&pointLigtheData, sizeof(int)*4 + sizeof(PointLigtheData) * (index - 1) , sizeof(PointLigtheData));
        }
    }


    void Renderer3D::BeginScene(const Ref<EditorCamera>& camera)
    {
        s_Data.CameraViewProj = camera->GetViewProjection();
        s_Data.CameraView = camera->GetViewMatrix();
        s_Data.CameraProj = camera->GetProjektion();
        s_Data.CameraPos = camera->GetPosition();
        s_Data.Stats.StartTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.RendererDefaultModeFlags = Renderer::GetMode();

        s_Data.CameraViewProjBuffer->SetData(&s_Data.CameraViewProj, sizeof(glm::mat4), { ShaderDataType::Float4x4, "Camera" });
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
        s_Data.RendererDefaultModeFlags = Renderer::GetMode();

        s_Data.CameraViewProjBuffer->SetData(&s_Data.CameraViewProj, sizeof(glm::mat4), { ShaderDataType::Float4x4, "Camera" });
    }

    void Renderer3D::EndScene()
    {
        s_Data.Stats.EndTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.Stats.DrawTime.push_back(
            std::chrono::time_point_cast<std::chrono::microseconds>(s_Data.Stats.EndTimePoint).time_since_epoch().count()
            - std::chrono::time_point_cast<std::chrono::microseconds>(s_Data.Stats.StartTimePoint).time_since_epoch().count()
        );
        Renderer::SetMode(s_Data.RendererDefaultModeFlags);
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
        Renderer::SetMode(material.AlgorithmFlags);
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
                switch (uniform.ShResourceType)
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
        Renderer::SetMode(material.AlgorithmFlags);
        const std::vector<UniformElement>& unifomrsLayoute = material.UniformLayoute;

            shader->Bind();

            for (const UniformElement& uniform : unifomrsLayoute)
            {
                if (uniform.GloblelResurce && uniform.UUID == 0)
                {
                    switch (uniform.ShResourceType)
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

        RenderCommand::DrawIndexedMesh(vertexArray);
        vertexArray->UnBind();
        shader->UnBind();

        
    }

    void Renderer3D::DrawLineBoxAABB(const BoxAABB& aabb, const glm::mat4& modelMatrix, int entityID)
    {
        s_Data.BoxAABBShader->Bind();
        s_Data.BoxAABBShader->SetMat4("u_ViewProj", s_Data.CameraViewProj);
        s_Data.BoxAABBShader->SetMat4("u_Model", modelMatrix);
        s_Data.BoxAABBShader->SetFloat3("u_GloblePostion", modelMatrix[3]);
        s_Data.BoxAABBShader->SetInt("u_EntityID", entityID);
        const glm::vec3& min = aabb.GetMin();
        const glm::vec3& max = aabb.GetMax();
        s_Data.BoxAABB[0] = glm::vec3(min.x, min.y, max.z);
        s_Data.BoxAABB[1] = glm::vec3(max.x, min.y, max.z);
        s_Data.BoxAABB[2] = max;
        s_Data.BoxAABB[3] = glm::vec3(min.x, max.y, max.z);

        s_Data.BoxAABB[4] = min;
        s_Data.BoxAABB[5] = glm::vec3(max.x, min.y, min.z);
        s_Data.BoxAABB[6] = glm::vec3(max.x, max.y, min.z);
        s_Data.BoxAABB[7] = glm::vec3(min.x, max.y, min.z);

        s_Data.BoxAABBVertexBuffer->SetData(s_Data.BoxAABB, 8 * sizeof(glm::vec3));
        DrawLine(s_Data.BoxAABBVertexArray);
        s_Data.BoxAABBVertexArray->UnBind();
        s_Data.BoxAABBShader->UnBind();

    }

    void Renderer3D::SetMaterial(const MaterialComponent& material, const glm::mat4& modelMatrix, int entityID)
    {
        Renderer::SetMode(material.AlgorithmFlags);
        const Ref<Shader>& shader = material.Shader;
        if (shader == nullptr)
        {
            uint32_t diffuseNr = 1;
            uint32_t specularNr = 1;
            std::string number;
            return;
        }
        shader->Bind();

        for (const UniformElement& uniform : material.UniformLayoute)
        {
            if (uniform.GloblelResurce && uniform.UUID == 0)
            {
                switch (uniform.ShResourceType)
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
    }

    void Renderer3D::DrawModdel(const MaterialComponent& material, const glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID)
    {
        DrawModdelMesh(material, modelMatrix, model.MeshR, entityID);
    }

    void Renderer3D::DrawModdel(const MaterialComponent& material, const glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID)
    {

        const std::vector<Ref<Mesh>>& meshes = model.ModelR->GetMeshes();
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
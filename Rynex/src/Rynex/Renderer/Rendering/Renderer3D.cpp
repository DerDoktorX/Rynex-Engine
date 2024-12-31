#include "rypch.h"
#include "Renderer3D.h"

#include "Rynex/Renderer/RenderCommand.h"
#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Renderer/Objects/BoxAABB.h"
#include "Rynex/DataStructs/HashPtrTable.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Rynex {


    struct AmbientData
    {
        
        float Color[3] = {-1,-2,-3};
        float Intensitie= -4;
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
        glm::mat4 ShadowMatrix = {
            1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1
        };
        float Color[4] = {-10,-11,-12,-13 };
        glm::vec3 Dirction = { -14, -15, -16 };
        float Intensitie = -17;
       
        DrirectionLigtheData() = default;
        DrirectionLigtheData(glm::vec3& color, float intensitie, const glm::mat4& matrix)
        {
            Color[0] = color.x;
            Color[1] = color.y;
            Color[2] = color.z;

            Dirction = glm::vec3(matrix[3]);
            Intensitie = intensitie;
            ShadowMatrix = matrix;
        }
    };

#define DIRCTION_SIZE sizeof(DrirectionData)

    struct DrirectionData
    {
        int Aktive[4] = { 0, - 1, -2, -3 };

        // int Aktive =  0 ;

        // int Aktive[3] = { 0, -1, -2 };

        // float CameraPositionGlobel[3] = { -1, -2, -3 };

        DrirectionLigtheData Drirecion[8];

        DrirectionData() = default;
        DrirectionData(int aktive,const glm::vec3& camerPos)
           // : Aktive(aktive)
        {
            Aktive[0] = aktive;
            // CameraPositionGlobel[0] = camerPos.x;
            // CameraPositionGlobel[1] = camerPos.y;
            // CameraPositionGlobel[2] = camerPos.z;
        }
    };

    struct PointLigtheData
    {
        float Color[3] = { -10,-11,-12 };
        float Distence = -13;
        float Postion[3] = { -14,-15,-16 };
        float Intensitie =  -17;
        

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

#define DIRCTION_SIZE 4 * 4 + (8 * (16+12+4))

    struct PointData
    {
        int Aktive[4] = { 0,-2,-3,-4 };
        PointLigtheData Point[32];
    };

    struct SpotLigtheData
    {
        float Color[3] = { -10,-11,-12 };
        float Intensitie = -13;
        float Postion[3] = { -14,-15,-16 };
        float Distence = -17;
        float Direction[3] = { -18,-19,-20 };
        float Inner = -21;
        float Outer = -22;
        float Empty[3] = { -23, -24,-25 };

        SpotLigtheData() = default;
        SpotLigtheData(const glm::vec3& color, const  glm::vec3& postion, const glm::vec3& direction,  float intensitie, float distence, float inner, float outer)
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
        int Aktive[4] = { 0,-1,-2,-3 };
        SpotLigtheData Spot[32];
    };

    struct CameraData
    {
        glm::mat4 ViewProjectionMatrix;
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
        glm::vec3 CamerPosition;
        int Empty = -1;
    };

    struct RendererData
    {
        Camera Camera;
        CameraData CameraData;

        int RenderChacheAktive = 0;
        int RenderDefaultAktive = 0;
        int RendererDefaultModeFlags;
        
        Ref<Shader> Default3DShader = nullptr;
        Ref<Texture> WhiteTexture = nullptr;
        Ref<UniformBuffer> AmbientLigthBuffer = nullptr;
        Ref<UniformBuffer> DirectionelLigthBuffer = nullptr;
        Ref<UniformBuffer> SpotLigthBuffer = nullptr;
        Ref<UniformBuffer> PointLigthBuffer = nullptr;
        glm::ivec4 AkitiveLigthsIndexes = { 0,0,0,0 };

        DrirectionData BufferDrirectionData;
        AmbientData BufferAmbientData;
        PointData BufferPointData;
        SpotData BufferSpotData;

        Ref<UniformBuffer> CameraViewProjBuffer = nullptr;

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
        uint32_t InstencCountAABB = 0;
#if RY_INSTECING
        HashPtrTable InstenceMesh;
#endif
        // Max Ligths: 8 directionel + 32 Pointligths + 32 Spotligths
        std::array<Ref<Texture>, 72> ShadowTextures;
        uint32_t MaxShadowsLigthsCount = 0;

        Renderer3D::Statistics Stats;
    };



    static RendererData s_Data;

    // Layout binding slots
    // 0: Camera(ViewProjectionMatrix, ViewMatrix, ProjectionMatrix, CamerPosition)
    // 1: Model ...  (ModelMatrix, Color, Alpha, ...)
    // 
    // 2: AmbientLigth(Color, Intensitie)
    // 3: DrirectionLigth(Aktive, (Color, Position, Intensitie)[8])
    // 4: PointLigth(Aktive, (Color, Distence, Position, Intensitie)[32])
    // 5: SpotLigthe(Aktive, (Color, Intensitie, Position, Distence, Direction, Inner, Outer)[32])
    
    void Renderer3D::Init()
    {
        RY_PROFILE_FUNCTION();
        RY_CORE_INFO("Renderer3D::Init");
        s_Data.Default3DShader = AssetManager::GetAsset<Shader>("../Rynex-Editor/Editor-Assets/shaders/Default_Rynex-Editor_Shader.glsl");
        {
            s_Data.BoxAABBShader = AssetManager::GetAsset<Shader>("../Rynex-Editor/Editor-Assets/shaders/BoxAABB.glsl");
            s_Data.BoxAABBVertexArray = VertexArray::Create();

            s_Data.BoxAABBVertexBuffer = VertexBuffer::Create(s_Data.BoxAABB, 8 * sizeof(glm::vec3), BufferDataUsage::DynamicDraw);

            s_Data.BoxAABBVertexBuffer->SetLayout({ { ShaderDataType::Float3, "a_Postion" } });

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
            s_Data.CameraViewProjBuffer = UniformBuffer::Create(&s_Data.CameraData, sizeof(CameraData),
                {
                     { ShaderDataType::Float4x4, "ViewProjectionMatrix" },
                     { ShaderDataType::Float4x4, "ViewMatrix" },
                     { ShaderDataType::Float4x4, "ProjectionMatrix" },
                     { ShaderDataType::Float3,   "CamerPosition" },
                     { ShaderDataType::Int,      "Empty"}
                }, 0);

           
        }
        
        {
            s_Data.AmbientLigthBuffer = UniformBuffer::Create(&s_Data.BufferAmbientData, sizeof(AmbientData),
                {      
                    { ShaderDataType::Float3, "Color" },
                    { ShaderDataType::Float, "Intensitie" },
                }, 2);
        }

        {
            std::vector<BufferElement> element;
            element.resize((8 * 4)+2);
            element.emplace_back(ShaderDataType::Int, "Aktiv" );
            element.emplace_back(ShaderDataType::Float3, "CameraPositionGlobel");
            for (uint32_t i = 1; i < 9; i++)
            {
                element.emplace_back(ShaderDataType::Float4x4, "ShadowMatrix", i);
                element.emplace_back(ShaderDataType::Float4, "Color", i);
                element.emplace_back(ShaderDataType::Float3, "Dirction", i);
                element.emplace_back(ShaderDataType::Float, "Intensitie", i);
            }
           

            s_Data.DirectionelLigthBuffer = UniformBuffer::Create(&s_Data.BufferDrirectionData, sizeof(DrirectionData), element, 3);
        }

        {
            std::vector<BufferElement> element;
            element.resize((32 * 4)+1);
            element.emplace_back(ShaderDataType::Int4, "Aktiv" );
            for (uint32_t i = 1; i < 33; i++)
            {
                element.emplace_back(ShaderDataType::Float3, "Color", i);
                element.emplace_back(ShaderDataType::Float, "Distence", i);
                element.emplace_back(ShaderDataType::Float3, "Postion", i);
                element.emplace_back(ShaderDataType::Float, "Intensitie", i);
            }
            

            s_Data.PointLigthBuffer = UniformBuffer::Create(&s_Data.BufferPointData, sizeof(PointData), element, 4);
        }

        {
            std::vector<BufferElement> element;
             element.resize((32 * 6)+1);
            // element.resize((32 * 6));
             element.emplace_back(ShaderDataType::Int4, "Aktiv");
            for (uint32_t i = 1; i < 33; i++)
            {
                element.emplace_back(ShaderDataType::Float3, "Color", i);
            
                element.emplace_back(ShaderDataType::Float, "Intensitie", i);
                element.emplace_back(ShaderDataType::Float, "Distence", i);
                element.emplace_back(ShaderDataType::Float3, "Postion", i);
                element.emplace_back(ShaderDataType::Float, "Inner", i);
                element.emplace_back(ShaderDataType::Float, "Outer", i);
                element.emplace_back(ShaderDataType::Float2, "Empty", i);
            }

            s_Data.SpotLigthBuffer = UniformBuffer::Create(&s_Data.BufferSpotData, sizeof(SpotData), element, 5);
        }
    
        
#if RY_INSTECING
        {
            s_Data.InstenceMesh = HashPtrTable(512);
        }
#endif
        s_Data.WhiteTexture = Texture::Default();


        
        {
            // Get Shadow maps and counter in Stats for posible Displaying
            s_Data.Stats.MaxShadowsLigthsCount = &s_Data.MaxShadowsLigthsCount;
            s_Data.Stats.ShadowsTex = &s_Data.ShadowTextures;
        }
    }

    void Renderer3D::Shutdown()
    {
        RY_CORE_INFO("Renderer3D::Shutdown");
        s_Data.AmbientLigthBuffer.reset();
        s_Data.BoxAABBShader.reset();
        s_Data.BoxAABBVertexArray.reset();
        s_Data.BoxAABBVertexBuffer.reset();
        s_Data.CameraViewProjBuffer.reset();
        s_Data.Default3DShader.reset();
        s_Data.DirectionelLigthBuffer.reset();
        s_Data.PointLigthBuffer.reset();
        for (auto& tex : s_Data.ShadowTextures)
            tex.reset();
        s_Data.SpotLigthBuffer.reset();
        s_Data.WhiteTexture.reset();
    }


#pragma region UniformLigths

    void Renderer3D::SetLigthUniform(AmbientLigthComponent* ambient, int ambientActive, int directionActive, int spotActive, int pointActive)
    {
        if (ambientActive !=  1 || ambient == nullptr)
        {
            AmbientData ambientData(glm::vec3(0.0,0.0,0.0), 0.0f);
            s_Data.BufferAmbientData = ambientData;
            s_Data.AmbientLigthBuffer->SetData(&s_Data.BufferAmbientData, 0, sizeof(AmbientData));
        }
        else
        {
            AmbientData ambientData(ambient->Color, ambient->Intensitie);
            s_Data.BufferAmbientData = ambientData;
            s_Data.AmbientLigthBuffer->SetData(&s_Data.BufferAmbientData, 0, sizeof(AmbientData));
        }
            
        s_Data.BufferDrirectionData.Aktive[0] = directionActive;
        s_Data.DirectionelLigthBuffer->SetData(&s_Data.BufferDrirectionData, 0, sizeof(int) * 4 + sizeof(DrirectionLigtheData) * spotActive);
        // s_Data.BufferDrirectionData.CameraPositionGlobel[0] = s_Data.CameraPos.x;
        // s_Data.BufferDrirectionData.CameraPositionGlobel[1] = s_Data.CameraPos.y;
        // s_Data.BufferDrirectionData.CameraPositionGlobel[2] = s_Data.CameraPos.z;

        s_Data.BufferSpotData.Aktive[0] = spotActive;
        s_Data.SpotLigthBuffer->SetData(&s_Data.BufferSpotData, 0, sizeof(int) * 4 + sizeof(SpotLigtheData)* spotActive);

        s_Data.BufferPointData.Aktive[0] = pointActive;
        s_Data.PointLigthBuffer->SetData(&s_Data.BufferPointData, 0, sizeof(int) * 4 + sizeof(PointLigtheData) * pointActive);

        s_Data.AmbientLigthBuffer->Bind(2);
        s_Data.DirectionelLigthBuffer->Bind(3);
        s_Data.SpotLigthBuffer->Bind(4);
        s_Data.PointLigthBuffer->Bind(5);
    }

    void Renderer3D::SetLigthUniform(DrirektionleLigthComponent& drirektion, const glm::mat4& matrix, int index)
    {
        if (index < 8)
        {
            DrirectionLigtheData drirectionLigtheData(drirektion.Color, drirektion.Intensitie, matrix);
            s_Data.BufferDrirectionData.Drirecion[index] = drirectionLigtheData;
        }
    }

    void Renderer3D::SetLigthUniform(SpotLigthComponent& spot, const glm::mat4& matrix, int index)
    {
        if (index < 32)
        {
            glm::vec3 skew = {};
            glm::vec4 perspectiv = {};
            glm::qua qaurt = glm::quat(glm::vec3());
            glm::vec3 postion = {};
            glm::vec3 scale = {};
            glm::decompose(matrix, scale, qaurt, postion, skew, perspectiv);
            SpotLigtheData spotLigtheData(spot.Color, glm::vec3(matrix[3]), glm::eulerAngles(qaurt), spot.Intensitie, spot.Distence, spot.Inner, spot.Distence);
            s_Data.BufferSpotData.Aktive[0]++;
            s_Data.BufferSpotData.Spot[index] = spotLigtheData;
        }
    }

    void Renderer3D::SetLigthUniform(PointLigthComponent& point, const glm::mat4& matrix, int index)
    {
        if (index < 32)
        {
            PointLigtheData pointLigtheData(point.Color, glm::vec3(matrix[3]), point.Intensitie, point.Distence);
            s_Data.BufferPointData.Point[index] = pointLigtheData;
        }
    }

    void Renderer3D::SetShadowsUniform()
    {
    }


   
#pragma endregion

#pragma region Frame_Start_Ende

    void Renderer3D::BeginFrame()
    {   
        for (auto& tex : s_Data.ShadowTextures)
            tex = nullptr;
        s_Data.MaxShadowsLigthsCount = 0;
        s_Data.InstencCountAABB = 0;
    }

   

    void Renderer3D::EndFrame()
    {
     
    }

#pragma endregion

    

   


#pragma region SceneShadows

    void Renderer3D::BeginSceneShadow(const Camera& camera, const glm::mat4& transform)
    {
        s_Data.Camera = camera;
        s_Data.CameraData.ViewMatrix = transform;
        s_Data.CameraData.ViewProjectionMatrix = camera.GetProjektion() * transform;
        s_Data.CameraData.ProjectionMatrix = camera.GetProjektion();

        s_Data.CameraViewProjBuffer->SetData(&s_Data.CameraData, sizeof(CameraData));
        s_Data.CameraViewProjBuffer->Bind(0);
    }

    void Renderer3D::EndSceneShadow(const Ref<Texture>& deathTextur, int index)
    {
        s_Data.ShadowTextures[index] = deathTextur;
        s_Data.MaxShadowsLigthsCount++;
    }

    void Renderer3D::DrawModdelMeshShadow(const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, int entityID)
    {
        const Ref<Material> material = mesh->GetMateriel();
        const Ref<Shader>& shader = material->GetShader();
        const Ref<VertexArray>& vertexArray = mesh->GetVertexArray();
        // const std::vector<MeshTexture>& textures = mesh->GetTextures();

        if (shader == nullptr || material == nullptr)
        {
            RY_CORE_FATAL("Some Shader don't exist!");
            return;
        }

        material->SetMatrix(modelMatrix);
        material->BindShadow();
        RenderCommand::DrawIndexedMesh(vertexArray);
        s_Data.Stats.DrawCalls++;
        vertexArray->UnBind();
        shader->UnBind();
    }

    void Renderer3D::DrawModdelShadow(glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID)
    {
        const std::vector<Ref<Mesh>>& meshes = model.ModelR->GetMeshes();
        for (const Ref<Mesh>& mesh : meshes)
        {
            if(mesh->IsRady())
                DrawModdelMeshShadow(modelMatrix, mesh, entityID);
        }
    }

#pragma endregion

#pragma region SceneRender
    
    void Renderer3D::BeginScene(const Ref<EditorCamera>& camera)
    {
        s_Data.CameraData.ViewProjectionMatrix = camera->GetViewProjection();
        s_Data.CameraData.ViewMatrix = camera->GetViewMatrix();
        s_Data.CameraData.ProjectionMatrix = camera->GetProjektion();
        s_Data.CameraData.CamerPosition = camera->GetPosition();
        s_Data.Stats.StartTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.RendererDefaultModeFlags = Renderer::GetMode();

        s_Data.CameraViewProjBuffer->SetData(&s_Data.CameraData, sizeof(CameraData));
        s_Data.CameraViewProjBuffer->Bind();
        if(s_Data.ShadowTextures[0])
            s_Data.ShadowTextures[0]->Bind(7);
    }

    void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transformCamera)
    {
        s_Data.Camera = camera;
        s_Data.CameraData.ViewProjectionMatrix = camera.GetProjektion() * transformCamera;
        s_Data.CameraData.ViewMatrix = transformCamera;
        s_Data.CameraData.ProjectionMatrix = camera.GetProjektion();
          glm::vec3 camerPos = glm::vec3(transformCamera[3]);
        s_Data.CameraData.CamerPosition = camerPos;


        s_Data.Stats.StartTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.RendererDefaultModeFlags = Renderer::GetMode();

        s_Data.CameraViewProjBuffer->SetData( &s_Data.CameraData, sizeof(CameraData));
        s_Data.CameraViewProjBuffer->Bind();
        if(s_Data.ShadowTextures[0])
            s_Data.ShadowTextures[0]->Bind(7);
    }


    void Renderer3D::EndScene()
    {
#if RY_INSTECING  
        for ( auto&[mesh, matrix, count] : s_Data.InstenceMesh)
        {
            if(Ref<Mesh> meshRef = mesh.lock())
                DrawModdelMesh(meshRef, matrix, count);
            count.clear();
            matrix.clear();
        }
#endif

        s_Data.Stats.EndTimePoint = std::chrono::high_resolution_clock::now();
        s_Data.Stats.DrawTime.emplace_back(
            std::chrono::time_point_cast<std::chrono::microseconds>(s_Data.Stats.EndTimePoint).time_since_epoch().count()
            - std::chrono::time_point_cast<std::chrono::microseconds>(s_Data.Stats.StartTimePoint).time_since_epoch().count()
        );

        Renderer::SetMode(s_Data.RendererDefaultModeFlags);
    }

#pragma endregion

 

// We try to seport this funktion But there not a huge part of the Render pipline!
#pragma region PrimtvFunktionsDraw

    void Renderer3D::BindDefault3DShader(const Ref<Texture>& bindeTexure, const glm::mat4& modelMatrix, int entityID, const glm::vec3& objectColor)
    {
        Ref<Shader>& shader = s_Data.Default3DShader;
        if (shader == nullptr)
            return;
        Renderer::SetMode(s_Data.RendererDefaultModeFlags);
        shader->Bind();
        shader->SetInt("u_EntityID", entityID);
        shader->SetMat4("u_Model", modelMatrix);
        shader->SetMat4("u_ViewProj", s_Data.CameraData.ViewProjectionMatrix);
        shader->SetFloat3("u_Color", objectColor);
        shader->SetFloat3("u_CamerPos", s_Data.CameraData.CamerPosition);
        shader->SetInt("u_Texture_Diffuse1", 0);
        if (bindeTexure != nullptr)
            bindeTexure->Bind(0);
    }

    void Renderer3D::BeforDrawEntity(const MaterialComponent& material, const glm::mat4& model, int entityID)
    {

        if (material.Materiel == nullptr)
        {
            BindDefault3DShader(Texture::Default(), model, entityID, material.Materiel->GetColor());
            return;
        }
        material.Materiel->SetMatrix(model);
        material.Materiel->Bind(&entityID);

    } 
    
    void Renderer3D::AfterDrawEntity(const MaterialComponent& material)
    {
        RY_PROFILE_FUNCTION();
        material.Materiel->UnBind();
    }

    void Renderer3D::SetMaterial(const MaterialComponent& material, const glm::mat4& modelMatrix, int entityID)
    {
        material.Materiel->SetMatrix(modelMatrix);
        material.Materiel->Bind(&entityID);
    }

    void Renderer3D::DrawModdel(glm::mat4& modelMatrix, const DynamicMeshComponent& model, int entityID)
    {
        if (model.MeshR->IsRady())
            DrawModdelMesh(modelMatrix, model.MeshR, entityID);
    }    
    
    void Renderer3D::DrawModdel( glm::mat4& modelMatrix, const StaticMeshComponent& model, int entityID)
    {

        const std::vector<Ref<Mesh>>& meshes = model.ModelR->GetMeshes();
        for (const Ref<Mesh>& mesh : meshes)
        {
#if RY_INSTECING
           
            MeshInstences& meshInst = s_Data.InstenceMesh.GetItem(mesh);
            meshInst.Mesh = mesh;
            meshInst.EntityIDs.emplace_back(entityID);
            meshInst.ModelMatrix.emplace_back(modelMatrix);
#else
            if(mesh->IsRady())
                DrawModdelMesh( modelMatrix, mesh, entityID);
#endif
        }
    }

    void Renderer3D::DrawMesh(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawIndexedMesh(vertexArray);
    }
    
    void Renderer3D::DrawMeshStrips(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawStripsMesh(vertexArray);
    }

    void Renderer3D::DrawLine(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawIndexedLine(vertexArray);
    }

    void Renderer3D::DrawLineLoop(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
        RenderCommand::DrawIndexedLineLoop(vertexArray);
    }

    void Renderer3D::DrawPoints(const Ref<VertexArray>& vertexArray)
    {
        s_Data.Stats.DrawCalls++;
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

#pragma endregion

    

    // Experimantle Funktion Instecing
    void Renderer3D::DrawModdelMesh(const Ref<Mesh>& mesh, std::vector<glm::mat4>& modelMatricies, std::vector<int>& entityIDs)
    {
        uint32_t countInstence = modelMatricies.size();
        if (countInstence != entityIDs.size())
            return;
        
        s_Data.Stats.DrawCalls++;
        
        const Ref<VertexArray>& vertexArray = mesh->GetVertexArray();
        const std::vector<MeshTexture>& textures = mesh->GetTextures();

        Ref<Shader> shader = nullptr;
        
        RY_CORE_ASSERT(shader != nullptr);

        {
            shader->Bind();
            shader->SetIntArray("u_EntityID", (int32_t*)entityIDs.data(), entityIDs.size());
            shader->SetMat4Array("u_ModelMatrix", (float*)modelMatricies.data(), modelMatricies.size());

        }

        uint32_t diffuseNr = 1;
        uint32_t specularNr = 1;
        std::string number = "";
        for (uint32_t i = 0; i < textures.size(); i++)
        {
            const MeshTexture& tex = textures[i];
            const std::string& name = tex.Type;
            if (name == "u_Texture_Diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "u_Texture_Speculare")
                number = std::to_string(specularNr++);


            shader->SetInt(("material." + name + number).c_str(), i);
            if (tex.TextureResur != nullptr)
                tex.TextureResur->Bind(i);
            else
                s_Data.WhiteTexture->Bind(i);
        }

        RenderCommand::DrawIndexedMeshInstecing(countInstence, vertexArray);
        vertexArray->UnBind();
        shader->UnBind();
    }



    // Force to make this Funktion default for all Normle Object Draw-Calls
    void Renderer3D::DrawModdelMesh( const glm::mat4& modelMatrix, const Ref<Mesh>& mesh, int entityID)
    {
       
        const Ref<Material> material = mesh->GetMateriel();
        const Ref<Shader>& shader = material->GetShader();
        const Ref<VertexArray>& vertexArray = mesh->GetVertexArray();
        // const std::vector<MeshTexture>& textures = mesh->GetTextures();
        
        if (shader == nullptr || material == nullptr)
        {
            RY_CORE_FATAL("Some Shader don't exist!");
            return;
        }
#if 0
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
            auto& tex = textures[i];
            

            const std::string& name = tex.Type;
            if (name == "u_Texture_Diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "u_Texture_Speculare")
                number = std::to_string(specularNr++);

                
            shader->SetInt(("material." + name + number).c_str(), i);
            if(tex.TextureResur != nullptr)
                tex.TextureResur->Bind(i);
            else
                s_Data.WhiteTexture->Bind(i);
        }
#else
        material->SetMatrix(modelMatrix);
        material->Bind(&entityID);
#endif
        RenderCommand::DrawIndexedMesh(vertexArray);
        s_Data.Stats.DrawCalls++;
        vertexArray->UnBind();
        shader->UnBind();
        
    }


    // TODO: Update the Funktion for current main Pipline
    // - UniformBuffer
    // - Batching
    void Renderer3D::DrawLineBoxAABB(const BoxAABB& aabb, const glm::mat4& modelMatrix, int entityID)
    {
        s_Data.BoxAABBShader->Bind();
        s_Data.BoxAABBShader->SetMat4("u_ViewProj", s_Data.CameraData.ViewProjectionMatrix);
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
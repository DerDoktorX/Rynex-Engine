#include <rypch.h>
#include "ViewPortPannel.h"
#include "EditorLayer.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Core/Input.h>
#include <Rynex/Math/Math.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/Rendering/Renderer2D.h>
#include <Rynex/Renderer/Rendering/Renderer3D.h>
#include <Rynex/Renderer/Materials/ShaderMaterial.h>

#include <imgui/imgui.h>
#include <ImGuizmo.h>
#include <imgui/imgui_internal.h>

namespace Rynex {
#define RY_MULTY_VIEWPORTS 1
#define RY_RENDERER_SHOADOWS_SECOUNDARY 1

#pragma region ViewPortPannel

    ViewPortPannel::ViewPortPannel()
        : m_SceneState(SceneState::Edit)
    {
    }

    ViewPortPannel::~ViewPortPannel()
    {
    }

    void ViewPortPannel::OnAttache(const std::string& name, EditorLayer* editorLayer)
    {
        m_WindowName = name;
        m_EditorLayer = editorLayer;
        m_Camera = CreateRef<EditorCamera>(30.0f, 1.778f, 0.1, 1000.0f);
       
        FramebufferSpecification fbSpec, fbSpec2;
        fbSpec.Attachments = {
            {
                TexFrom::RGBA32F, 1,
                { TexWarp::ClampEdge,  TexWarp::ClampEdge, TexWarp::ClampEdge, },
                TexFilter::Nearest
            },
            {
                TexFrom::RED_INTEGER, 1,
                { TexWarp::ClampEdge,  TexWarp::ClampEdge, TexWarp::ClampEdge, },
                TexFilter::Nearest
            },
            {
                TexFrom::Depth24Stencil8, 1,
                { TexWarp::ClampEdge,  TexWarp::ClampEdge, TexWarp::ClampEdge, },
                TexFilter::Nearest,
            }
        };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);
        fbSpec2.Attachments = {
            {
                TexFrom::RGBA32F, 1,
                { TexWarp::ClampEdge,  TexWarp::ClampEdge, TexWarp::ClampEdge, },
                TexFilter::Nearest
            },
            { 
                TexFrom::Depth24Stencil8, 1,
                { TexWarp::ClampEdge,  TexWarp::ClampEdge, TexWarp::ClampEdge, },
                TexFilter::Nearest
            }
        };
        fbSpec2.Width = 1280;
        fbSpec2.Height = 720;

        m_SelectedFramebuffer = Framebuffer::Create(fbSpec2);
        //m_CallFace = CallFace::None;

        m_Filtering = AssetManager::GetAsset<Shader>("../Rynex-Editor/Editor-Assets/shaders/Compute.glsl");

        m_Image = Texture::Create({
            1280, 720,
            TexTar::Texture2D, TexFrom::RGBA32F,
            1, TexFilter::Nearest
        });
        m_AktiveScene = m_EditorLayer->GetAktivScene();
        m_GizmoType = m_EditorLayer->GetPtrGizmoType();
        
        {
            m_ViewPorts.push_back(CreateRef<ViewPort>());
           // m_ViewPorts.push_back(CreateRef<ViewPort>());
           
           
            m_ViewPorts[0]->OnAttache(m_Framebuffer,
                m_SelectedFramebuffer, 
                m_Camera, 
                m_Filtering, 
                m_EditorLayer, 
                &m_SceneState, 
                this, 
                &m_HoveredEntity, 
                0, 
                0, 
                m_WindowName + " Attachment: " + std::to_string(0)
            );
             
            // m_ViewPorts[1]->OnAttache(m_Framebuffer,
            //     m_SelectedFramebuffer,
            //     m_Camera,
            //     m_Filtering,
            //     m_EditorLayer,
            //     &m_SceneState,
            //     this,
            //     &m_HoveredEntity,
            //     0,
            //     0,
            //     m_WindowName + " Attachment: " + std::to_string(1)
            // );
            
        }
        ShaderMaterialDefaultNames names(
            { SDT::Float3,   "Color"},
            { SDT::Float,    "Alpha"},
            { SDT::Float4x4, "ModelMatrix" }
        );

        m_MaterialC.Materiel = Material::CreateShader(names );
        m_MaterialC.Materiel->SetUniformBuffer(UniformBuffer::Create(
            nullptr, 
            sizeof(glm::mat4),  {
                { SDT::Float4x4,   "ModelMatrix"},
            }, 1));
        m_MaterialC.Materiel->SetFlage(Renderer::A_Buffer | Renderer::Death_Buffer | Renderer::CallFace_Back);
        m_MaterialC.Materiel->SetColor( { 1.0f,1.0f,1.0f });
        m_MaterialC.Materiel->SetShader(AssetManager::GetAsset<Shader>("../Rynex-Editor/Editor-Assets/shaders/ViewPort-Shader.glsl"));
    }

    void ViewPortPannel::OnUpdate()
    {
        auto [mx, my] = ImGui::GetMousePos();
        for (auto& viewPort : m_ViewPorts)
        {
            viewPort->OnUpdate({ mx ,my });
        }

    }

    void ViewPortPannel::OnRenderEditor(TimeStep ts)
    {
        m_SceneState = SceneState::Edit;
        if (m_WindowFocused)
        {
            m_Camera->OnUpdate(ts);
        }
        m_AktiveScene->OnRenderEditor(m_Framebuffer, m_Camera);
        RenderSelectedEntity();
        for (auto& viewPort : m_ViewPorts)
        {
            viewPort->OnRenderEditorFilter();
        }


    }

    void ViewPortPannel::OnRenderRuntime(int camerCount)
    {
        m_SceneState = SceneState::Play;
        m_AktiveScene->OnRenderRuntime(m_Framebuffer, camerCount);
    }

    void ViewPortPannel::OnRenderSimultion()
    {
        m_SceneState = SceneState::Simulate;
        m_AktiveScene->OnRenderSimulation(m_Framebuffer, m_Camera);
    } 

    void ViewPortPannel::OnDetache()
    { 
        m_EditorLayer = nullptr;
       
        for (auto& viewPort : m_ViewPorts)
        {
            viewPort->OnDetache();
            viewPort = nullptr;
        }
        m_ViewPorts.clear();

    }

    bool ViewPortPannel::OnImGuiRender()
    {
        bool isFoverdFocuse = false;
        bool isSomeHoverd = false;
        bool isSomeFocuse = false;
        bool isGizmoFocuse = false;
        for (auto& viewPort : m_ViewPorts)
        {
            isFoverdFocuse = viewPort->OnImGuiRender() || isFoverdFocuse;
            isSomeFocuse = viewPort->IsWindowFocused() || isSomeFocuse;
            isSomeHoverd = viewPort->IsWindowHoverd() || isSomeHoverd;
            isGizmoFocuse = viewPort->IsGizmoHoverd() || isGizmoFocuse;
        }
        m_WindowHoverd = isSomeHoverd;
        m_WindowFocused = isSomeFocuse;
        bool result = m_WindowHoverd && (!isGizmoFocuse);
        m_AktiveScene->SetHoverViewPort(result);

        return result;
    }

    void ViewPortPannel::SetEventBlocker(bool blockEvents)
    {
        Application::Get().GetImGuiLayer()->BlockEvents(blockEvents);
    }

    void ViewPortPannel::OnEventCamera(Event& e)
    {
        m_Camera->OnEvent(e);
    }

    bool ViewPortPannel::OnMousPressed(MouseButtenPressedEvent& e)
    {
        if (m_WindowHoverd)
        {
            if (e.GetMouseButton() == Mouse::ButtonLeft)
            {
                if (m_AktiveScene->GetEntityCount() == 0)
                    OpenScene();
                else if (!ImGuizmo::IsOver())
                    m_EditorLayer->SetSelectedEntity(m_HoveredEntity);
            }
        }
        return false;
    }

    void ViewPortPannel::HoveredEntity()
    {

    }

    void ViewPortPannel::SetNewAktiveSecen(const Ref<Scene>& scene)
    {
        m_AktiveScene = scene;
        for (auto& viewPort : m_ViewPorts)
            viewPort->SetNewAktiveSecen(scene); 
    }

    void ViewPortPannel::WindowResize(const glm::vec2& windowSize)
    {

    }

    void ViewPortPannel::DragAndDrop()
    {

    }

    void ViewPortPannel::OpenScene(AssetHandle handle)
    {

    }

    void ViewPortPannel::OpenScene()
    {

    }

    void ViewPortPannel::CalculateMausePos(const glm::vec2& mausOffset)
    {

    }


    void ViewPortPannel::CalculateGizmo()
    {

    }

    void ViewPortPannel::RenderSelectedEntity()
    {
#if RY_OLD_RENDER_SYSTEM
        Entity slelcted = m_EditorLayer->GetSelectedEntity();
       
        m_SelectedFramebuffer->Bind();
        RenderCommand::Clear(); 
        RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
        
        if (!slelcted)
        {
            m_SelectedFramebuffer->Unbind();
            return;
        }

        Camera& mainCamera = (Camera)m_Camera->GetProjektion();
        glm::mat4 viewMatrix = m_Camera->GetViewMatrix();

        if ((slelcted != Entity() || slelcted != 0) && slelcted.HasComponent<ModelMatrixComponent>())
        {
            ModelMatrixComponent& modelC = slelcted.GetComponent<ModelMatrixComponent>();
            if (slelcted.HasComponent<SpriteRendererComponent>())
            {
                Renderer2D::BeginSceneQuade(mainCamera, viewMatrix);
                SpriteRendererComponent& spriteC = slelcted.GetComponent<SpriteRendererComponent>();
                Renderer2D::DrawSprite(modelC.Globle, spriteC, slelcted.GetEntityHandle());
                Renderer2D::EndSceneQuade();
            }

            if (slelcted.HasComponent<CameraComponent>())
            {
                Renderer2D::BeginSceneIcon(mainCamera, viewMatrix, m_SelectedFramebuffer->GetFrambufferSize());
                Renderer2D::DrawCameraIcon(modelC.Globle, slelcted.GetEntityHandle());
                Renderer2D::EndSceneIcon();
                CameraComponent& camerC = slelcted.GetComponent<CameraComponent>();
               
                

                
            }
            if (slelcted.HasComponent<Rynex::DrirektionleLigthComponent>())
            {
                Renderer2D::BeginSceneIcon(mainCamera, viewMatrix, m_SelectedFramebuffer->GetFrambufferSize());
                Renderer2D::DrawLigthDirctionelIcon(modelC.Globle, slelcted.GetEntityHandle());
                Renderer2D::EndSceneIcon();

                
                

            }
            if (slelcted.HasComponent<Rynex::SpotLigthComponent>())
            {
                Renderer2D::BeginSceneIcon(mainCamera, viewMatrix, m_SelectedFramebuffer->GetFrambufferSize());
                Renderer2D::DrawLigthSpotIcon(modelC.Globle, slelcted.GetEntityHandle());
                Renderer2D::EndSceneIcon();



               
            }
            if (slelcted.HasComponent<Rynex::PointLigthComponent>())
            {
                Renderer2D::BeginSceneIcon(mainCamera, viewMatrix, m_SelectedFramebuffer->GetFrambufferSize());
                Renderer2D::DrawLigthPointIcon(modelC.Globle, slelcted.GetEntityHandle());
                Renderer2D::EndSceneIcon();

                
                
            }
#if 0
            else  if (slelcted.HasComponent<GeomtryComponent>() )
            {
                Renderer3D::BeginScene(mainCamera, viewMatrix);

                if (geomtryC.Geometry != nullptr)
                {
                    
                    Renderer3D::BeforDrawEntity(m_MaterialC, mat4C.Globle, slelcted.GetEntityHandle());
                    Renderer3D::DrawObjectRender3D(geomtryC.Geometry);
                    Renderer3D::AfterDrawEntity(m_MaterialC);
                }
                Renderer3D::EndScene();
            }
#endif
           if (slelcted.HasComponent<StaticMeshComponent>() )
            {
                Renderer3D::BeginScene(mainCamera, viewMatrix);
                StaticMeshComponent& staticMeshC = slelcted.GetComponent<StaticMeshComponent>();
                

                if ( staticMeshC.ModelR != nullptr)
                {
#if RY_MODEL_NODE

                    Renderer3D::RenderComponet(modelC.Globle, staticMeshC, m_MaterialC.Materiel, slelcted.GetEntityHandle(), Renderer3D::DrawMesh3D);

#else
                    const std::vector<Ref<Mesh>>& meshes = staticMeshC.ModelR->GetMeshes();
#if 0
                    const std::vector<glm::mat4>& globels = staticMeshC.GlobleMeshMatrix;
                    for (uint32_t i = 0, length = globels.size(); i < length; i++)
                    {
                        Renderer3D::DrawModdelSeclection(globels[i], meshes[i], m_MaterialC.Materiel, slelcted.GetEntityHandle());
                    }
#elif 0
                    for (const Ref<Mesh>& mesh : meshes)
                    {
                        Renderer3D::DrawModdel(mat4C.Globle, staticMeshC, slelcted.GetEntityHandle());
                    }
#else
                    for(const Ref<Mesh>& mesh : meshes)
                    {
                        Renderer3D::DrawModdelSeclection(mat4C.Globle, mesh, m_MaterialC.Materiel, slelcted.GetEntityHandle());
                    }

#endif


#endif
                }
                Renderer3D::EndScene();
            }
           if (slelcted.HasComponent<DynamicMeshComponent>())
           {
                DynamicMeshComponent& dynamicMeshC = slelcted.GetComponent<DynamicMeshComponent>();
                UUID parent = slelcted.GetComponent<RealtionShipComponent>().ParentID;
                Renderer3D::BeginScene(mainCamera, viewMatrix);
               
               
#if RY_MODEL_NODE
                Renderer3D::RenderComponet(modelC.Globle, dynamicMeshC, m_MaterialC.Materiel, slelcted.GetEntityHandle(), Renderer3D::DrawMesh3D);
#else
                    if ( dynamicMeshC.MeshR != nullptr)
                    {
                        m_MaterialC.Materiel->SetMatrix(mat4C.Globle);
                        Renderer3D::DrawModdelSeclection( mat4C.Globle, dynamicMeshC.MeshR, m_MaterialC.Materiel,slelcted.GetEntityHandle());
                        
                    }
#endif
                
                Renderer3D::EndScene();
            }

        }

        m_SelectedFramebuffer->Unbind();
#else
#endif
    }

#pragma endregion


#pragma region ViewPort

    ViewPort::ViewPort()
    {
    }

    ViewPort::~ViewPort()
    {
    }

    void ViewPort::OnAttache(const Ref<Framebuffer>& frambuffer, const Ref<Framebuffer>& selectedFrameBuffer, const Ref<EditorCamera>& editorCamera, const Ref<Shader>& shader, EditorLayer* editorLayer, SceneState* sceneState, ViewPortPannel* viewPortPannel, Entity* hoveredEntity, int renderOnAtachment, int runtimCameraIndex, const std::string& name)
    {
        m_Framebuffer = frambuffer;
        m_SelectedFramebuffer = selectedFrameBuffer;
        m_Camera = editorCamera;
        m_EditorLayer = editorLayer;
        m_SceneState = sceneState;
        m_HoveredEntity = hoveredEntity;
        m_RenderOnAtachment = renderOnAtachment;
        m_RuntimCameraIndex = runtimCameraIndex;
        m_WindowName = name;
        m_Filtering = shader;

        m_AktiveScene = m_EditorLayer->GetAktivScene();
        m_GizmoType = m_EditorLayer->GetPtrGizmoType();
        m_Image = Texture::Create({
            1280, 720,
            TextureTarget::Texture2D,
            TextureFormat::RGBA32F,
            1,
            TextureFilteringMode::Nearest
        });

        
    }

    void ViewPort::OnDetache()
    {
        m_Framebuffer = nullptr;
        m_SelectedFramebuffer = nullptr;
        m_Camera = nullptr;
        m_EditorLayer = nullptr;
        m_SceneState = nullptr;
        m_HoveredEntity = nullptr;
        m_Filtering = nullptr;
        m_AktiveScene = nullptr;
        m_GizmoType = nullptr;
        m_Image = nullptr;
    }

    void ViewPort::OnUpdate(const glm::vec2& mousePos)
    {
        HoveredEntity(mousePos);
    }

    bool ViewPort::OnImGuiRender()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0 ,0 });
        ImGui::Begin(m_WindowName.c_str());
        // GetMousePos
        ImVec2 viewportOffset = ImGui::GetCursorPos();

        // View Port   Hoverd + Focuse -> block Events
        m_WindowFocused = ImGui::IsWindowFocused();
        m_WindowHoverd = ImGui::IsWindowHovered();

        // ViewPort get Size + Resize Image + SetImage in ViewPort
        ImVec2 viewportPannelSize = ImGui::GetContentRegionAvail();
        WindowResize({ viewportPannelSize.x , viewportPannelSize.y });
        m_WindowMoving = IsCurrentWindowMoving();
        uint32_t textureID = 0;
        switch (*m_SceneState)
        {
        case SceneState::Edit:
        {
            textureID = m_Image->GetRenderID();
            break;
        }
        case SceneState::Play:
        {
            textureID = m_Framebuffer->GetColorAttachmentRendererID(m_RenderOnAtachment);
            break;
        }
        case SceneState::Simulate:
        {
            textureID = m_Image->GetRenderID();
            break;
        }
        default:
        {
            textureID = m_Image->GetRenderID();
            break;
        }
        }
        // Renderer3D::Statistics stats = Renderer3D::GetStats();
        // if(stats.ShadowsTex->at(0) != nullptr)
        //    textureID = stats.ShadowsTex->at(0)->GetRenderID();
        // textureID = m_SelectedFramebuffer->GetDeathAttachmentRendererID();
        // 
        // textureID = m_Framebuffer->GetDeathAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ viewportPannelSize.x, viewportPannelSize.y }, ImVec2(0, 1), ImVec2(1, 0));

        // Drag and drop conten Broser
        DragAndDrop();


        // Get View port AABB In Mointore Scren Space / Not App Scren Space!
        if (m_WindowHoverd)
            CalculateMausePos({ viewportOffset.x, viewportOffset.y });

        // Gizmos
        CalculateGizmo();

        ImGui::End();
        ImGui::PopStyleVar();

        
        Renderer3D::Statistics stats = Renderer3D::GetStats();
        if (stats.ShadowsTex != nullptr)
        {
            ImGui::Begin("Shadow Map");
            uint32_t shadowID = stats.ShadowsTex->GetRenderID(); 
            ImVec2 viewportPannelShadowSize = ImGui::GetContentRegionAvail();
            ImGui::Image(reinterpret_cast<void*>(shadowID), ImVec2{ (stats.ShadowsTex->GetWidth() / stats.ShadowsTex->GetHeight()) * viewportPannelShadowSize.y,  viewportPannelShadowSize.y }, ImVec2(0, 1), ImVec2(1, 0));

            ImGui::End();
        }

        return (m_WindowFocused && m_WindowHoverd);
    }

    void ViewPort::OnRenderEditorFilter()
    {
        m_Filtering->Bind();
        m_Filtering->SetUniformValue("u_MauseInViewPixelPos", &m_MauseInViewPixelPos, ShaderDataType::Float2);
        m_SelectedFramebuffer->BindColorAttachmentImage(Acces::Read, 0, 0);
        m_Framebuffer->BindColorAttachmentImage(Acces::Read, m_RenderOnAtachment, 1);
        m_Image->BindImage(Acces::Write, 2);

        RenderCommand::DispatcheCompute({ (m_WindowSize.x / 32)  , (m_WindowSize.y / 16)  , 1 });
    }

    void ViewPort::SetNewAktiveSecen(const Ref<Scene>& scene)
    {
        m_AktiveScene = scene;
        m_AktiveScene->OnViewportResize((uint32_t)m_WindowSize.x, (uint32_t)m_WindowSize.y);
    }


    bool ViewPort::IsCurrentWindowMoving()
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (!window)
            return false;

        return (window->MoveId == ImGui::GetActiveID());
    }

    void ViewPort::CalculateMausePos(const glm::vec2& mausOffset)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImGuiDockNode* node = window->DockNode;
        bool isTabBarVisible = node ? node->WantHiddenTabBarToggle : false;

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 minBound = ImGui::GetWindowPos();
        minBound.x += mausOffset.x;
        minBound.y += mausOffset.y;

        ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
        m_WindowBounds[0] = { minBound.x, isTabBarVisible ? minBound.y : minBound.y-25 };
        m_WindowBounds[1] = { maxBound.x, isTabBarVisible ? maxBound.y : maxBound.y-25 };
    }

    void ViewPort::WindowResize(const glm::vec2& windowSize)
    {
        bool result = m_WindowSize != windowSize;
        m_AktiveScene->SetWindowResize(result);

        if (result)
        {
            m_Framebuffer->Resize(windowSize.x, windowSize.y);
            m_SelectedFramebuffer->Resize(windowSize.x, windowSize.y);
            m_Image->Resize(windowSize.x, windowSize.y);
            m_WindowSize = windowSize;
            m_Camera->SetViewportSize(windowSize.x, windowSize.y);

            m_AktiveScene->OnViewportResize(windowSize.x, windowSize.y);
        }
    }

    void ViewPort::DragAndDrop()
    {
        if (ImGui::BeginDragDropTarget())
        {

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Asset::GetAssetTypeDragAndDropName(AssetType::Scene).c_str()))
            {
                AssetHandle handle = *(AssetHandle*)payload->Data;
                OpenScene(handle);
            }
            ImGui::EndDragDropTarget();
        }
    }

    void ViewPort::OpenScene(AssetHandle handle)
    {
        m_EditorLayer->OpenScene(handle);
    }

    void ViewPort::OpenScene()
    {
        m_EditorLayer->OpenScene();
    }

    void ViewPort::CalculateGizmo()
    {
        Entity selectedEntity = m_EditorLayer->GetSelectedEntity();
        m_ImGizmoHovered = false;
        if (selectedEntity && *m_GizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            float windowWidth = (float)m_WindowSize.x;
            float windowHeight = (float)m_WindowSize.y;

            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

            glm::mat4 camerProj, camerView;

            switch (m_EditorLayer->GetSceneState())
            {
            case SceneState::Edit:
            {
                camerProj = m_Camera->GetProjektion();
                camerView = m_Camera->GetViewMatrix();
                break;
            }
            case SceneState::Play:
            {
                auto camerEntt = m_AktiveScene->GetEntityPrimaryCamera();
                const auto& camera = camerEntt.GetComponent<CameraComponent>().Camera;
                const glm::mat4& camerProj = camera.GetProjektion();
                camerView = glm::inverse(camerEntt.GetComponent<ModelMatrixComponent>().Globle);
                break;
            }
            default:
            {
                camerProj = m_Camera->GetProjektion();
                camerView = m_Camera->GetViewMatrix();
                break;
            }
            }

            if (!selectedEntity.HasComponent<TransformComponent>()) 
                return;

            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();


            bool snap = Input::IsKeyPressed(Key::LeftControl);
            float snapeValue = *m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;
            float snapeValues[3] = { snapeValue , snapeValue , snapeValue };

            ImGuizmo::Manipulate(glm::value_ptr(camerView), glm::value_ptr(camerProj),
                (ImGuizmo::OPERATION)*m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                nullptr, snap ? snapeValues : nullptr);
            m_ImGizmoHovered = ImGuizmo::IsUsing();
            if (m_ImGizmoHovered)
            {
                glm::vec3 transation, rotation, scale;
                Math::DecomposeTransform(transform, transation, rotation, scale);

                glm::vec3 dealteRotation = rotation - tc.Rotation;
                tc.Transaltion = transation;
                tc.Rotation += dealteRotation;
                tc.Scale = scale;

                ModelMatrixComponent& selecE_Mat4 = selectedEntity.GetComponent<ModelMatrixComponent>();
                selecE_Mat4.Locale = tc.GetTransform();
                UUID parent = selectedEntity.GetComponent<RealtionShipComponent>().ParentID;
                if(parent)
                {
                    Entity parentE = m_AktiveScene->GetEntitiyByUUID(parent);
                    selecE_Mat4.Globle = parentE.GetComponent<ModelMatrixComponent>().Globle * selecE_Mat4.Locale;
                }
                else 
                {
                    selecE_Mat4.Globle = selecE_Mat4.Locale;
                }
                
            }
            
        }
        
    }

    void ViewPort::RenderSelectedEntity()
    {
#if 0
        Entity slelcted = m_EditorLayer->GetSelectedEntity();
        m_SelectedFramebuffer->Bind();
        RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
        RenderCommand::Clear();
        Camera& mainCamera = (Camera)m_Camera->GetProjektion();
        glm::mat4 viewMatrix = m_Camera->GetViewMatrix();
        if ((slelcted != Entity() || slelcted != 0) && slelcted.HasComponent<TransformComponent>())
        {
            TransformComponent transformC = slelcted.GetComponent<TransformComponent>();
            if (slelcted.HasComponent<SpriteRendererComponent>())
            {
                Renderer2D::BeginScene(mainCamera, viewMatrix);
                SpriteRendererComponent spriteC = slelcted.GetComponent<SpriteRendererComponent>();
                Renderer2D::DrawSprite(transformC.GetTransform(), spriteC, slelcted.GetEntityHandle());
                Renderer2D::EndScene();
            }
            else  if (slelcted.HasComponent<GeomtryComponent>() && slelcted.HasComponent<MaterialComponent>())
            {
                Renderer3D::BeginScene(mainCamera, viewMatrix);
                GeomtryComponent geomtryC = slelcted.GetComponent<GeomtryComponent>();
                MaterialComponent materialC = slelcted.GetComponent<MaterialComponent>();
                Ref<VertexArray> vertexArray = geomtryC.Geometry;
                if ( vertexArray != nullptr)
                {
                    Renderer3D::BeforDrawEntity(materialC, transformC.GetTransform(), slelcted.GetEntityHandle());
                    Renderer3D::DrawObjectRender3D(vertexArray);
                    Renderer3D::AfterDrawEntity(materialC);
                }
                Renderer3D::EndScene();
            }
            else  if (slelcted.HasComponent<StaticMeshComponent>() && slelcted.HasComponent<MaterialComponent>())
            {
                Renderer3D::BeginScene(mainCamera, viewMatrix);
                StaticMeshComponent staticMeshC = slelcted.GetComponent<StaticMeshComponent>();
                MaterialComponent materialC = slelcted.GetComponent<MaterialComponent>();
                
                if ( staticMeshC.ModelR != nullptr)
                {
                    Renderer3D::DrawModdel(materialC, transformC.GetTransform(), staticMeshC, slelcted.GetEntityHandle());
                }
                Renderer3D::EndScene();
            }
            else  if (slelcted.HasComponent<DynamicMeshComponent>())
            {
                DynamicMeshComponent dynamicMeshC = slelcted.GetComponent<DynamicMeshComponent>();
                int parent = slelcted.GetComponent<RealtionShipComponent>().Parent;
                Renderer3D::BeginScene(mainCamera, viewMatrix);
                if(parent!=-1)
                {
                    MaterialComponent materialC = Entity((entt::entity)parent, m_AktiveScene.get()).GetComponent<MaterialComponent>();
                    if (dynamicMeshC.MeshR != nullptr)
                    {
                        Renderer3D::DrawModdel(materialC, transformC.GetTransform(), dynamicMeshC, slelcted.GetEntityHandle());
                    }
                }
                Renderer3D::EndScene();
            }

        }

        m_SelectedFramebuffer->Unbind();
#endif
    }

    void ViewPort::HoveredEntity(const glm::vec2& mousePos)
    {
        m_MauseInViewPixelPos = { -0.0f, -0.0f };
        if (m_WindowHoverd && m_WindowFocused && !m_WindowMoving)
        {
            float mx = mousePos.x;
            float my = mousePos.y;
            mx -= m_WindowBounds[0].x;
            my -= m_WindowBounds[0].y;
            glm::vec2 viewPortSize = m_WindowBounds[1] - m_WindowBounds[0];
            my = viewPortSize.y - my;
            int mauseX = (int)mx;
            int mauseY = (int)my;
            m_MauseInViewPixelPos = { mx, my };
            if (mauseX >= 0 && mauseY >= 0 && mauseX < (int)viewPortSize.x && mauseY < (int)viewPortSize.y)
            {
                m_Framebuffer->Bind();
                m_AktiveScene->SetMousPixelPos({ mauseX, mauseY });
                int pixeldata = m_Framebuffer->ReadPixel(1, mauseX, mauseY);
#if 0
                if (pixeldata <= -1  || m_AktiveScene->GetEntityCount() < pixeldata)
#else
                if (pixeldata <= -1)
#endif
                {
                    *m_HoveredEntity = Entity();
                }
                else
                {
                    *m_HoveredEntity = pixeldata == -1 || pixeldata == -2 ? Entity() : Entity((entt::entity)pixeldata, m_AktiveScene.get());
                }
                m_Framebuffer->Unbind();
            }
        }
    }

#pragma endregion
}
#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Rynex/Scene/SceneSerializer.h"

#include "ImGuizmo.h"

#include "Rynex/Math/Math.h"
#include "Rynex/Scripting/ScriptingEngine.h"

namespace Rynex {

    extern const std::filesystem::path g_AssetsPath;

    EditorLayer::EditorLayer()
        : Layer("Rynex-Editor")
        , m_CameraController((1280.0f / 720.0f), true)  
    {
    }


    void EditorLayer::OnAttach()
    {
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = { 
            FramebufferTextureFormat::RGBA8, 
            FramebufferTextureFormat::RGBA8, 
            FramebufferTextureFormat::RED_INTEGER, 
            FramebufferTextureFormat::Depth 
        };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_AktiveScene = CreateRef<Scene>();
        m_Scene_HPanel.SetContext(m_AktiveScene);
        m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 100.0f);
       
#if 1
        m_AktiveScene->CreateEntityWitheUUID(UUID(89234786376786), "3D_RendererTestEntity");
        auto& entiy = m_AktiveScene->GetEntitiyByUUID(89234786376786);

        if(!entiy.HasComponent<TagComponent>())
            entiy.AddComponent<TagComponent>("3D_RendererTestEntity");

        if (!entiy.HasComponent<TransformComponent>())
            entiy.AddComponent<TransformComponent>();

        if (!entiy.HasComponent<GeomtryComponent>()) 
        {
            entiy.AddComponent<GeomtryComponent>();
            auto& geometry = entiy.GetComponent<GeomtryComponent>();

            geometry.Geometry = VertexArray::Create();
            geometry.Buffer = VertexBuffer::Create(256);

            Geomtrys::GetCubeVertex(geometry.Geometry, geometry.Buffer);
            Geomtrys::SetCubeIndex(geometry.Geometry);
            
            
            
        }

        if (!entiy.HasComponent<MaterialComponent>())
        {
            entiy.AddComponent<MaterialComponent>();
            auto& material = entiy.GetComponent<MaterialComponent>();
            material.Shader = Shader::Create("Assets/shaders/3DTest.glsl");
        }

#endif
    }

    void EditorLayer::OnDetach()
    {
    }

    void EditorLayer::OnUpdate(TimeStep ts)
    {
        if (FramebufferSpecification spec = m_Framebuffer->GetFramebufferSpecification(); m_ViewPortSize.x > 0.0f && m_ViewPortSize.y > 0.0f && (spec.Width != m_ViewPortSize.x || spec.Height != m_ViewPortSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
            m_CameraController.OnResize(m_ViewPortSize.x, m_ViewPortSize.y);
            m_EditorCamera.SetViewportSize(m_ViewPortSize.y, m_ViewPortSize.x);
            m_AktiveScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
        }
        m_PasTime += ts;

#if CHECK_FOR_ERRORS
        if (m_PasTime > 10.0f)
        {
            m_PasTime = 0.0f;
            m_Scene_HPanel.SetCheckErrors();
        }
#endif

        Renderer2D::ResetStats();
        
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        RenderCommand::Clear();

        m_Framebuffer->ClearAttachment(2, -1);
        
        switch (m_SceneState)
        {
            case SceneState::Edit:
            { 
                if (m_ViewPortFocused)
                {
                    m_CameraController.OnUpdate(ts);
                    m_EditorCamera.OnUpdate(ts);
                }

                m_EditorCamera.OnUpdate(ts);
                m_AktiveScene->OnUpdateEditor(ts, m_EditorCamera);
                break;
            }
            case SceneState::Simulate:
            {
                m_EditorCamera.OnUpdate(ts);

                break;
            }
            case SceneState::Play:
            {
                m_AktiveScene->OnUpdateRuntime(ts);
                break;
            }
        }

        ////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////
        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewPortSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewPortSize.y - my;
        int mauseX = (int)mx;
        int mauseY = (int)my;
   
        if ( mauseX >= 0  && mauseY >= 0  &&  mauseX < (int)viewPortSize.x  &&  mauseY < (int)viewPortSize.y )
        {
            int pixeldata = m_Framebuffer->ReadPixel(2, mauseX, mauseY);
            RY_CORE_ASSERT(pixeldata > -2, "Minus Entity!");
            m_HoveredEntity = pixeldata == -1 || pixeldata == -2 ? Entity() : Entity((entt::entity)pixeldata, m_AktiveScene.get());
        }
        m_Framebuffer->Unbind();
    }


    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEnvent(e);
        m_EditorCamera.OnEvent(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(RY_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtenPressedEvent>(RY_BIND_EVENT_FN(EditorLayer::OnMousePressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        if (e.GetRepeatCount() > 0)
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        int key = e.GetKeyCode();
        switch (key)
        {

        case Key::N:
        {
            if (control)
                NewScene();

            break;
        }
        case Key::Delete:
        {
            if (m_ViewPortFocused)
                NewScene();

            break;
        }
        case Key::O:
        {
            if (control)
                OpenScene();

            break;
        }
        case Key::S:
        {
            if (control)
                SaveSceneAs();

            break;
        }

        //Gizmos
        case Key::Q:
            m_GizmoType = -1;
            break;
        
        case Key::W:
            //if (control)
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            //else

            break;
        
        case Key::E:
            //if (control)
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
            //else

            break;
        case Key::R:
            //if (control)
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
            //else

            break;

        default:
            break;
        }

    }

    bool EditorLayer::OnMousePressed(MouseButtenPressedEvent& e)
    {
        if (m_ViewPortHoverd)
        {
            if (e.GetMouseButton() == Mouse::ButtonLeft)
            {
                if (m_AktiveScene->GetEntityCount() == 0)
                    OpenScene();
                else if (!ImGuizmo::IsOver())
                    m_Scene_HPanel.SetSelectedEntity(m_HoveredEntity);
            }
        }
        return false;
    }


    void EditorLayer::NewScene()
    {
        m_AktiveScene = CreateRef<Scene>();
        m_AktiveScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
        m_Scene_HPanel.SetContext(m_AktiveScene);
    }

    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialoges::OpenFile("Rynex Scene (*.rynex)\0*.rynex\0");
        if (!filepath.empty())
        {
            OpenScene(filepath);
        }
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {  
        m_AktiveScene = CreateRef<Scene>();
        m_AktiveScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
        m_Scene_HPanel.SetContext(m_AktiveScene);

        SceneSerializer serialzer(m_AktiveScene);
        serialzer.Deserialize(path.string());
    }

    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialoges::SaveFile("Rynex Scene (*.rynexscene)\0*.rynexscene\0");
        if (!filepath.empty())
        {
            SceneSerializer serialzer(m_AktiveScene);
            serialzer.Serialize(filepath);
        }
    }

    
    /////////////////////////////////////////////////////////////////
    //// ImGui //////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    
    void EditorLayer::OnImGuiRender()
    {
        static bool dokingEnabled   = true;
        static bool assetsEnabled   = false;
        static bool sceneEnabled    = true;

        if (dokingEnabled) {

            static bool dokingSpaceOpen = true;
            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                const ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }
            

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
            // and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", &dokingSpaceOpen, window_flags);
            ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }

            //////////////////////////////////////////////////
            ///// Top Taskbar ////////////////////////////////
            //////////////////////////////////////////////////

            ImGuiTopTaskBar();
            
            //////////////////////////////////////////////////
            ///// Windoe Layoute /////////////////////////////
            //////////////////////////////////////////////////
            if(sceneEnabled)    ImGuiScenePannel();
            ImGuiSettings();
            ImGuiViewPort();
            
            

            ImGui::End();
        }
        else
        {   
            if(sceneEnabled)    ImGuiScenePannel();
            ImGuiSettings();
            ImGuiViewPort();
           

            ImGui::End();
        }


    }


    //--- Layoute ----------------

    void EditorLayer::ImGuiViewPort()
    { 
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0 ,0 });
        ImGui::Begin("Viewport");
        // GetMousePos
        ImVec2 viewportOffset = ImGui::GetCursorPos();

        // View Port   Hoverd + Focuse -> block Events
        m_ViewPortFocused = ImGui::IsWindowFocused();
        m_ViewPortHoverd = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused && !m_ViewPortHoverd);
        
        // ViewPort get Size + Resize Image + SetImage in ViewPort
        ImVec2 viewportPannelSize = ImGui::GetContentRegionAvail();
        ImGuiViewPortResize(viewportPannelSize);
        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0);
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ viewportPannelSize.x, viewportPannelSize.y }, ImVec2(0,1), ImVec2(1,0)); 

        // Drag and drop conten Broser
        ImGuiContentBrowserViewPort();

        // Set New ViewPort for Debuging or secen other stuffe
        ImGuiSecundaryViewPort(m_Framebuffer, 1, viewportPannelSize.x / 3, viewportPannelSize.y / 3);
       
        // Get View port AABB In Mointore Scren Space / Not App Scren Space!
        ImGuiSetMausPosInViewPort(viewportOffset);
       
        // Gizmos
        ImGizmoInViewPort();

        ImGui::End();
        ImGui::PopStyleVar();
    }

   

    void EditorLayer::ImGuiSecundaryViewPort(const Ref<Framebuffer>& framebuffer, uint32_t id, float width , float height)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0 ,0 });
        ImGui::Begin("Secundary Viewport");

        uint32_t textureID = framebuffer->GetColorAttachmentRendererID(id);
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ width, height }, ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::ImGuiSettings()
    {
        ImGui::Begin("Settings");
        
        std::string name = "None";
        uint64_t uuid = 0;
        if (m_HoveredEntity)
        {
            name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
            uuid = m_HoveredEntity.GetUUID();
        }
       
        ImGui::Text("Hovered Entity : %s", name.c_str());
        ImGui::Text("UUID                    : %i", uuid);
        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quad   : %d", stats.QuadCount);
        ImGui::Text("Vertex : %d", stats.GetTotalVertexCount());
        ImGui::Text("Indexs : %d", stats.GetTotalIndexCount());


        std::string sceneState;
        switch (m_SceneState)
        {
        case SceneState::Edit:
        {
            sceneState = "Edit";
            break;
        }
        case SceneState::Play:
        {
            sceneState = "Play";
            break;
        }
        case SceneState::Simulate:
        {
            sceneState = "Simulate";
            break;
        }
       
        }
        ImGui::Text("Curent Scene State: %s", sceneState.c_str());
        bool hasPlayButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play;
        bool hasSimulateButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate;
        bool hasPauseButton = m_SceneState != SceneState::Edit;
        if (hasPlayButton)
        {
            if (ImGui::Button("Play", ImVec2(50, 0)))
            {
                if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
                {
                    m_SceneState = SceneState::Play;
                    m_AktiveScene->OnRuntimStart();
                }
                else if(m_SceneState == SceneState::Play)
                    m_SceneState = SceneState::Edit;

            }
        }
        if (hasSimulateButton)
        {
            if (hasPlayButton)
                ImGui::SameLine();
#if 0
            if (ImGui::Button("Simulation", ImVec2(50, 0)))
            {

                if (m_Runtime)
                    m_AktiveScene->OnRuntimStop();
                else
                    m_AktiveScene->OnRuntimStart();
                
                m_Runtime != m_Runtime;

            }
#endif
        }
        if (hasPauseButton)
        {
            if (ImGui::Button("Pause", ImVec2(50, 0)))
            {
                m_SceneState = SceneState::Edit;
            }
        }

        ImGui::End();
    }

    void EditorLayer::ImGuiScenePannel()
    {
        m_Scene_HPanel.OnImGuiRender();
        m_Content_BPannel.OnImGuiRender();
    }

    void EditorLayer::ImGuiContentBrowserViewPort()
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const wchar_t* path = (const wchar_t*)payload->Data;
                OpenScene(std::filesystem::path(g_AssetsPath) / path);
            }
            ImGui::EndDragDropTarget();
        }
    }

    void EditorLayer::ImGuiSetMausPosInViewPort( ImVec2 vpOffset)
    {
        auto windowSize = ImGui::GetWindowSize();
        ImVec2 minBound = ImGui::GetWindowPos();
        minBound.x += vpOffset.x;
        minBound.y += vpOffset.y;

        ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
        m_ViewportBounds[0] = { minBound.x, minBound.y };
        m_ViewportBounds[1] = { maxBound.x, maxBound.y };
    }

    void EditorLayer::ImGizmoInViewPort()
    {
        Entity selectedEntity = m_Scene_HPanel.GetSelectedEntity();

        if (selectedEntity && m_GizmoType != -1)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            float windowWidth = (float)ImGui::GetWindowWidth();
            float windowHeight = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

            //Camera
#if 0
            auto camerEntt = m_AktiveScene->GetPrimaryCameraEntity();
            const auto& camera = camerEntt.GetComponent<CameraComponent>().Camera;
            const glm::mat4& camerProj = camera.GetProjektion();
            glm::mat4 camerView = glm::inverse(camerEntt.GetComponent<TransformComponent>().GetTransform());
#else

            const glm::mat4& camerProj = m_EditorCamera.GetProjektion();
            glm::mat4 camerView = m_EditorCamera.GetViewMatrix();
#endif

            // Entity transform
            auto& tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetTransform();


            bool snap = Input::IsKeyPressed(Key::LeftControl);
            float snapeValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f;
            float snapeValues[3] = { snapeValue , snapeValue , snapeValue };

            ImGuizmo::Manipulate(glm::value_ptr(camerView), glm::value_ptr(camerProj),
                (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                nullptr, snap ? snapeValues : nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 transation, rotation, scale;
                Math::DecomposeTransform(transform, transation, rotation, scale);

                glm::vec3 dealteRotation = rotation - tc.Rotation;
                tc.Transaltion = transation;
                tc.Rotation += dealteRotation;
                tc.Scale = scale;
            }

            }
    }

    void EditorLayer::ImGuiViewPortResize( ImVec2 vPSize)
    {
        if (m_ViewPortSize != *((glm::vec2*)&vPSize))
        {
            m_Framebuffer->Resize((uint32_t)vPSize.x, (uint32_t)vPSize.y);
            m_ViewPortSize = { vPSize.x, vPSize.y };
            m_EditorCamera.SetViewportSize(vPSize.x, vPSize.y);
            m_CameraController.OnResize((float)vPSize.x, (float)vPSize.y);

            m_AktiveScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
        }
    }



    //--- Top Taskbar -------------

    void EditorLayer::ImGuiTopTaskBar()
    {
        if (ImGui::BeginMenuBar())
        {
            ImGuiFile();
            ImGuiScript();
            ImGuiEdit();
            ImGuiView();
            ImGuiHelp();
            ImGui::EndMenuBar();
        }
    }

    void EditorLayer::ImGuiFile()
    {
        if (ImGui::BeginMenu("File"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.

            if (ImGui::MenuItem("New", NULL, false))
                NewScene();

            if (ImGui::MenuItem("Open...", "Crtl+O"))
                OpenScene();

            if (ImGui::MenuItem("Save As...", "Crtl+Shift+S"))
                SaveSceneAs();

            if (ImGui::MenuItem("Exit", NULL, false)) 
                Application::Get().Close();
                
            ImGui::EndMenu();
         }


    }

    void EditorLayer::ImGuiScript()
    {
        if (ImGui::BeginMenu("Script"))
        {
            if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
                ScriptingEngine::ReloadAssambly();

            ImGui::EndMenu();
        }
    }



    void EditorLayer::ImGuiEdit()
    { 
        if (ImGui::BeginMenu("Edite"))
        {
        // Disabling fullscreen would allow the window to be moved to the front of other windows,
        // which we can't undo at the moment without finer window depth/z control.

            if (ImGui::MenuItem("Exit", NULL, false)) Application::Get().Close();
                ImGui::EndMenu();
        }
    }

    void EditorLayer::ImGuiView()
    {

        if (ImGui::BeginMenu("View"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.


            if (ImGui::MenuItem("Exit", NULL, false)) Application::Get().Close();
            ImGui::EndMenu();
        }
    }

    void EditorLayer::ImGuiHelp()
    {
        if (ImGui::BeginMenu("Help"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.


            if (ImGui::MenuItem("Exit", NULL, false)) Application::Get().Close();
            ImGui::EndMenu();
        }
    }

}

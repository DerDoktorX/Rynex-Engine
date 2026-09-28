#include <rypch.h>
#include "EditorLayer.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Core/Input.h>
#include <Rynex/Project/Project.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Utils/PlatformUtils.h>
#include <Rynex/Math/Math.h>

#include <Rynex/Serializers/SceneSerializer.h>
#ifdef RY_SCRIPT_ENGINE
    #include <Rynex/Scripting/Mono/ScriptingEngine.h>
#endif

#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/Rendering/Renderer.h>

#include <Rynex/Asset/Import/ShaderImporter.h>

#include <imgui.h>

#ifdef RY_IM_GUIZMO
#include <ImGuizmo.h>
#endif // IM_GUIZMO

#include <Rynex/Renderer/Mesh/MeshSource.h>
#include <Rynex/Renderer/Rendering/Render3D/IndirectDrawMap.h>
#include <Rynex/Renderer/Mesh/MeshStatic.h>

#include <Rynex/Serializers/StaticMeshSerializer.h>

#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>

#include <Rynex/Asset/Import/TextureImporter.h>




namespace Rynex {

#define RY_EDITOR_TEST_ENITITY 1
#define RY_ENABLE_VIEWPORT 1
#define TEST_SHADER_OUTPUT 0

#define RY_KEY_CASE(key, func) \
case key:\
    func(); \
    break



#define RY_INTERNEL_IF_KEY_COMB_CASE(key, is, action) \
case key: \
    if(is) \
        action; \
    break

#define RY_INTERNEL_IF_OR_ELSE_KEY_COMB_CASE(key, is, action1, action2) \
case key: \
    if(is) \
        action1; \
    else \
        action2; \
    break

#define RY_INTERNEL_IF_OR_IF_ELSE_KEY_COMB_CASE(key, is1, is2, action1, action2) \
case key: \
    if(is1) \
        action1; \
    else if(is2) \
        action2; \
    break

#define RY_INTERNEL_KEY_COMB_CASE_GET_MACRO_NAME(key, is, is_Or_Action, action1_Or_2, action2_Or_3, marco, ...) marco

#define RY_INTERNEL_KEY_COMB_CASE_GET_MACRO(...) RY_EXPAND_MOAKRO( RY_INTERNEL_KEY_COMB_CASE_GET_MACRO_NAME(__VA_ARGS__, RY_INTERNEL_IF_OR_IF_ELSE_KEY_COMB_CASE, RY_INTERNEL_IF_OR_ELSE_KEY_COMB_CASE, RY_INTERNEL_IF_KEY_COMB_CASE) )
#define RY_KEY_COMB_CASE(key, ...) RY_EXPAND_MOAKRO( RY_INTERNEL_KEY_COMB_CASE_GET_MACRO(key, __VA_ARGS__)(key, __VA_ARGS__) )

#define TEST_SCENE_STATE_00 0
#define TEST_SCENE_STATE_01 0
#define TEST_SCENE_STATE_02 0
#define TEST_SCENE_STATE_03 0
#define TEST_SCENE_STATE_04 0
#define TEST_SCENE_STATE_05 0
#define TEST_SCENE_STATE_06 0
#define TEST_SCENE_STATE_07 0
#define TEST_SCENE_STATE_08 0
#define TEST_SCENE_STATE_09 0
#define TEST_SCENE_STATE_10 0

#define TEST_BINDING_MANGER_SYSTEM 0

#define TEST_RENDER_PIPLINE_SYSTEME 0
#define TEST_MESH_STATIC_SERILAZTION 0
#define TEST_LOADING_ASSETS_ASYNC 0
#define TEST_PTR_RNEDERPIPLINE 0


    namespace Utils {

        static int RandomRange(int max)
        {
            return rand() % max;
        }

        static float RandomFloatRange(int max, int min, float pos = 100.0f)
        {
            int v = RandomRange((max - min) * pos) + min * pos;
            return v / pos;
        }

    }

    
    EditorLayer::EditorLayer()
        : Layer("Rynex-Editor")
        , m_CameraController((1280.0f / 720.0f), true)
        , m_ProjectPannel("Project")
        , m_Content_BPannel()
        , m_MenuBarPannel()
        , m_RendererPannel("Renderer")
        , m_ViewPortRenderTime(1ull)
        , m_ViewPortUpdateTime(1ull)
        , m_CallFace(CallFace::None)
        , m_ViewportBounds()
    {
    }


    void EditorLayer::OnAttach()
    {

        RY_CORE_INFO("EditorLayer::OnAttach Start!");
        RY_PROFILE_FUNCTION();

        m_AktiveScene = CreateRef<Scene>();
        m_EditorScene = CreateRef<Scene>();

        auto cLA = Application::Get().GetSpecification().CommandLineArgs;
        
        if (cLA.Count > 1)
        {
            auto projFilePath = cLA[1];
            OpenProject(projFilePath);
        }
        else
        {
            if(!OpenProject())
                Application::Get().Close();

        }

       
        m_Scene_HPanel.SetContext(m_AktiveScene);

        m_Content_BPannel.OnAttache();
       
        m_EditorCamera = CreateRef<EditorCamera>(30.0f, 1.778f, 0.1, 1000.0f);
        // m_EditorCamera->SetDistance(5.5f);
        // m_EditorCamera->SetYaw(0.02f);
        // m_EditorCamera->SetPitch(-0.05f);

        m_ViewPortPannel = ViewPortPannel();
        m_ViewPortPannel.OnAttache("Main ViewPort", this);

        RY_CORE_INFO("Init Test Hard Coded Enttiy");

        uint32_t count = 20;
        for (uint32_t i = 0; i < count; i++)
        {
            Entity entiy = m_AktiveScene->CreateEntity("Test " + std::to_string(i));
            entiy.AddComponent<SpriteRendererComponent>();
            TransformComponent& trasC = entiy.GetComponent<TransformComponent>();

            trasC.m_Transform = glm::vec3(
                Utils::RandomFloatRange(25, -25),
                Utils::RandomFloatRange(25, -25),
                Utils::RandomFloatRange(25, -25)
            );

            entiy.UpdateMatrix();
        }
        

        {
#if 0
            Ref<MeshStatic> ship;
            Ref<MeshStatic> cv;

            Ref<MeshStatic> cube;
            Ref<MeshStatic> cube2;

            Ref<MeshStatic> sponzer;
            Ref<MeshStatic> sponzer2;
#if TEST_BINDING_MANGER_SYSTEM 
            TestBindMangerSystem();
#endif

#if TEST_RENDER_PIPLINE_SYSTEME
            TestRenderPiplineMesh();
#endif

#if TEST_MESH_STATIC_SERILAZTION
            TestSerliceMesh();
            
#endif

#if TEST_LOADING_ASSETS_ASYNC
            TestLoadingAssetAsync();
#endif
#if TEST_PTR_RNEDERPIPLINE
            TestElementPtrFunc();
#endif

#if TEST_SCENE_STATE_00 || TEST_SCENE_STATE_04
            ship = AssetManager::GetAsset<MeshStatic>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/Yamto-Model/scene.rystmesh"));

            cv = AssetManager::GetAsset<MeshStatic>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/CV-Model/scene.rystmesh"));

            cube = AssetManager::GetAsset<MeshStatic>("Assets/Models/Cube.rystmesh");
            cube2 = AssetManager::GetAsset<MeshStatic>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/Cube2.rystmesh"));
#elif TEST_SCENE_STATE_01|| TEST_SCENE_STATE_08
            sponzer = AssetManager::GetAsset<MeshStatic>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/pkg_a_curtains/pkg_a_curtains/NewSponza_Curtains_glTF.rystmesh"));
#elif TEST_SCENE_STATE_02 || TEST_SCENE_STATE_07
            sponzer2 = AssetManager::GetAsset<MeshStatic>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/main_sponza/main_sponza/NewSponza_Main_glTF_003.rystmesh"));
#elif TEST_SCENE_STATE_03 || TEST_SCENE_STATE_09
            sponzer2 = AssetManager::GetAsset<MeshStatic>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/pkg_a_curtains/pkg_a_curtains/NewSponza_Curtains_glTF.rystmesh"));
            sponzer = AssetManager::GetAsset<MeshStatic>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/main_sponza/main_sponza/NewSponza_Main_glTF_003.rystmesh"));
#elif TEST_SCENE_STATE_05 || TEST_SCENE_STATE_10
            cube = AssetManager::GetAsset<MeshStatic>("Assets/Models/Cube.rystmesh");
#elif TEST_SCENE_STATE_6
            cube2 = AssetManager::GetAsset<MeshStatic>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/Cube2.rystmesh"));
#endif
            Ref<Shader> shader = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShader.glsl"));
            
            TestProfileRenderShaderMapSubmit(cube, cube2, ship, cv, sponzer, sponzer2, m_AktiveScene);

            glm::mat4 matrix = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            );
#endif

            m_RendererPannel.OnAttache(this);
        }
        
        
        m_MenuBarPannel.OnAttache(this);
        m_ProjectPannel.OnAttache(this);
        m_MeshPannel.OnAttache(this);


        m_Scene_HPanel.TestSubmitStaticProxyLocal();
        RY_CORE_INFO("EditorLayer::Sucese Finished!");



    }

    void EditorLayer::OnDetach()
    {
        RY_CORE_WARN("OnDetach Activ!");
        RY_PROFILE_FUNCTION();

        RY_DESTROY_REF(m_AktiveScene);
        RY_DESTROY_REF(m_NextScene);
        RY_DESTROY_REF(m_EditorCamera);
        RY_DESTROY_REF(m_EditorScene);
        RY_DESTROY_REF(m_AssetManger);
        RY_DESTROY_REF(m_Project);
       

        m_RendererPannel.OnDetache();
        m_MenuBarPannel.OnDetache();
        m_ProjectPannel.OnDetache();
        m_ViewPortPannel.OnDetache();
        m_Content_BPannel.OnDetache();
        m_Scene_HPanel.OnDetache();  
        m_MeshPannel.OnDetache();
#if defined(RY_SCRIPT_ENGINE)
        if (Renderer::IsInit())
            Renderer::Init();
#endif
        if (Renderer::IsEditorInit())
            Renderer::ShutdownEditor();
        if (Renderer::IsInit())
            Renderer::Shutdown();


        Project::Shutdown();

        RY_CORE_WARN("OnDetach Done!");
    }

    void EditorLayer::OnUpdate(TimeStep ts)
    {   
        m_PasTime += ts;

        if (m_NextScene != nullptr)
        {
            if (m_SceneState != SceneState::Edit)
            {
                RY_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate, "Error Futer Funktion: EditorLayer::OnSceneStop()");

                if (m_SceneState == SceneState::Play)
                    m_AktiveScene->OnRuntimeStop();
                else if (m_SceneState == SceneState::Simulate)
                    m_AktiveScene->OnRuntimeStop();

                m_SceneState = SceneState::Edit;
            }

            Ref<Scene> newScene = Scene::Copy(m_NextScene);

            m_EditorScene = newScene;
            m_AktiveScene = newScene;

            m_Scene_HPanel.SetContext(m_AktiveScene);
            m_EditorScenePath = Project::GetActive()->GetEditorAssetManger()->GetMetadata(m_NextScene->m_Handle).m_FilePath;
            m_ViewPortPannel.SetNewAktiveSecen(m_AktiveScene);

            m_NextScene = nullptr;
        }

#if TEST_SCENE_STATE_00
        static bool s_NotUpdated = true;
#endif

        switch (m_SceneState)
        {
            case SceneState::Edit:
            { 
#if TEST_SCENE_STATE_00
                s_NotUpdated = true;
#endif
                {
                    RY_SCOPE_TIMER(m_ViewPortUpdateTime);
                    m_AktiveScene->OnUpdateEditor(ts);
                }
                m_ViewPortPannel.OnRenderEditor(ts);
                break;
            }
            case SceneState::Simulate:
            {
                
                m_EditorCamera->OnUpdate(ts);
                
                {
                    RY_SCOPE_TIMER(m_ViewPortUpdateTime);
                    m_AktiveScene->OnUpdateSimulation(ts);
                }
                m_ViewPortPannel.OnRenderSimultion();;
                break;
            }
            case SceneState::Play:
            {
                {
                    m_AktiveScene->OnUpdateRuntime(ts);
                    RY_SCOPE_TIMER(m_ViewPortUpdateTime);
                }
                m_ViewPortPannel.OnRenderRuntime(0);

                
#if TEST_SCENE_STATE_00
                if(s_NotUpdated)
                { 
#if 1
                    TestProfileRenderShaderMapRemove();
#else
                    Ref<Shader> shader = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShader.glsl"));
                    Ref<MeshStatic> cube = Mesh::CreateStaticMesh("Assets/Models/Cube.gltf");
                    glm::mat4 matrix2 = glm::mat4(
                        1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 1.25f, 0.0f, 1.0f
                    );
                    Renderer3D::UpdateMeshObject(cube, shader, glm::translate(matrix2, glm::vec3(0.0f, 2.5f, 0.0f)), 2);
#endif
                    s_NotUpdated = false;
                }
#endif

                break;
            }
        }
        m_ViewPortRenderTime = m_ViewPortPannel.GetSceneRenderTime();

        ////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////
        m_RendererPannel.OnUpdate(ts);
        m_ViewPortPannel.OnUpdate();
        m_MeshPannel.OnUpdate();
    }
    
    void EditorLayer::OnImGuiRender()
    {
        static bool dokingEnabled   = true;
        static bool assetsEnabled   = true;
        static bool sceneEnabled    = true;
        static bool viewPortEnabled = false;
        static bool settingsEnabled = true;
        static bool renderPannnel = false;
        static bool meshPannnel = false;

        static bool menuBarPannnel = false;

        if (dokingEnabled) 
        {
            RY_PROFILE_SCOPE("ImGui Window - Editor");
            static bool dokingSpaceOpen = true;
            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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
            

            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("Ryenex-Editor-Gui", &dokingSpaceOpen, window_flags);
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

            if(menuBarPannnel)
            {
                RY_PROFILE_SCOPE("ImGui Pannel - Renderer");
                ImGuiTopTaskBar();
            }
            else
            {
                m_MenuBarPannel.OnImGuiRender();
            }
            if (sceneEnabled)
            {
                RY_PROFILE_SCOPE("ImGui Pannel - Scene Pannels");
                ImGuiPannels();
            }
            if(settingsEnabled)
            {
                RY_PROFILE_SCOPE("ImGui Pannel - View Settings");
                ImGuiSettings(renderPannnel);
            }

            if (!viewPortEnabled)
            {
                RY_PROFILE_SCOPE("ImGui Pannel - View Port");
                ViewPortPannel::SetEventBlocker(!m_ViewPortPannel.OnImGuiRender());
            }
            m_ProjectPannel.OnImGuiRender();
            if (!renderPannnel)
            {
                RY_PROFILE_SCOPE("ImGui Pannel - Renderer");
                m_RendererPannel.OnImGuiRender();
            }

            if (!meshPannnel)
            {
                RY_PROFILE_SCOPE("ImGui Pannel - Mesh");
                m_MeshPannel.OnImGuiRender();
            }
            ImGui::End();
        }
        else
        {   
            if(sceneEnabled)    
                ImGuiPannels();

            if(settingsEnabled) 
                ImGuiSettings(renderPannnel);

            if(!renderPannnel)
                m_RendererPannel.OnImGuiRender();
           
            ImGui::End();
        }


    }

#pragma endregion

#pragma region Events

    void EditorLayer::OnEvent(Event& e)
    {
        m_RendererPannel.OnEvent(e);
        m_MenuBarPannel.OnEvent(e);
        m_ProjectPannel.OnEvent(e);
        m_ViewPortPannel.OnEventCamera(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(RY_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtenPressedEvent>(RY_BIND_EVENT_FN(EditorLayer::OnMousePressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {

        if (e.GetRepeatCount() > 0)
            return false;
        bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);
        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
        int key = e.GetKeyCode();
        switch (key)
        {
            RY_KEY_COMB_CASE(Key::N, control, NewScene());
            RY_KEY_COMB_CASE(Key::Delete, m_ViewPortFocused, NewScene());
            RY_KEY_COMB_CASE(Key::O, control, OpenScene());
            RY_KEY_COMB_CASE(Key::S, control && shift, control, NewScene(), SaveCurentScene());
#ifdef IM_GIZMO
            RY_KEY_COMB_CASE(Key::Q, control, m_GizmoType = -1);
            RY_KEY_COMB_CASE(Key::W, control, m_GizmoType = ImGuizmo::OPERATION::TRANSLATE);
            RY_KEY_COMB_CASE(Key::E, control, m_GizmoType = ImGuizmo::OPERATION::ROTATE);
            RY_KEY_COMB_CASE(Key::R, control, m_GizmoType = ImGuizmo::OPERATION::SCALE);
#endif

            
#if 0
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
            

            if (control && shift)
                SaveSceneAs();
            else if (control)
                SaveCurentScene();
            break;
        }

        //Gizmos
        case Key::Q:
            if (control)
                m_GizmoType = -1;
            break;
        
        case Key::W:
            if (control)
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            //else

            break;
        
        case Key::E:
            if (control)
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
            //else

            break;
        case Key::R:
            if (control)
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
            //else

            break;
#endif
        default:
            break;
        }
        return false;

    }

    bool EditorLayer::OnMousePressed(MouseButtenPressedEvent& e)
    {
        return m_ViewPortPannel.OnMousPressed(e);
    }

#pragma endregion


#pragma region OpenPannel

    void EditorLayer::OpenRenderPannel()
    {
        m_RendererPannel.OpenWindow();
    }

    void EditorLayer::OpenAssetPannel()
    {
        m_Content_BPannel.OpenAssetPannel();
    }

    void EditorLayer::OpenRegestriyPannel()
    {
        m_Content_BPannel.OpenRegestriyPannel();
    }

    void EditorLayer::OpenSceneHierachyPannel()
    {
        m_Scene_HPanel.OpenSceneHierarchy();
    }

    void EditorLayer::OpenPropertiesPannel()
    {
        m_Scene_HPanel.OpenProperties();
    }

    void EditorLayer::OpenProjectPannel()
    {
        m_ProjectPannel.OpenWindow();
    }

    void EditorLayer::OpenViewPortTexture()
    {
        m_ViewPortPannel.OpenTextureWindows();
    }

    void EditorLayer::OpenMeshPannel()
    {
        m_MeshPannel.OpenWindow();
    }

#pragma endregion

#pragma region ProjectFile

    void EditorLayer::NewProject()
    {
        Project::New();
    }

    bool EditorLayer::OpenProject()
    {
        std::string filepath = FileDialoges::OpenFile("Rynex Project (*.ryproj)\0*.ryproj\0");
        if (filepath.empty())
            return false;

        OpenProject(filepath);
        return true;
    }

    void EditorLayer::OpenProject(const std::filesystem::path& path)
    {
        if (Project::Load(path))
        {
#if defined(RY_SCRIPT_ENGINE)
            if(!ScriptingEngine::IsInit())
                ScriptingEngine::Init(true);
#endif

            if (!Renderer::IsInit())
            {
                
                Renderer::Init();
                Renderer::InitEditor();
            }
            m_Project = Project::GetActive();
            m_AssetManger = m_Project->GetEditorAssetManger();
            const ProjectConfig& config = m_Project->GetConfig();
            const std::filesystem::path& startScene = config.m_StartScene;
            if (!startScene.empty())
            {
                const FileSystem::Path scenePath(startScene);
                m_AssetManger->GetAssetHandle(scenePath);
                OpenScene();
                // OpenScene(startScene);
            }
            
            m_Content_BPannel = ContentBrowserPanel();
            
        }
    }

    void EditorLayer::SaveProject()
    {
        if(m_Project && m_Project->GetConfig().m_ProjectRady)
        {
            m_Project->SaveActive(m_Project->GetConfig().m_ProjectPath / (m_Project->GetConfig().m_Name + ".ryproj"));
        }
    }

#pragma endregion

#pragma region SceneFile

    void EditorLayer::NewScene()
    {
        m_AktiveScene = CreateRef<Scene>(); 
        m_ViewPortPannel.SetNewAktiveSecen(m_AktiveScene);
        m_Scene_HPanel.SetContext(m_AktiveScene);
        m_AktiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewPortSize.x), static_cast<uint32_t>(m_ViewPortSize.y));
    }

    void EditorLayer::OpenScene()
    {
        RY_PROFILE_FUNCTION();
        std::string filepath = FileDialoges::OpenFile("Rynex Scene (*.rynexscene)\0*.rynexscene\0");

       
        if (!filepath.empty())
        {
            OpenScene(filepath);

        }
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {  
        m_AktiveScene = CreateRef<Scene>();
        m_ViewPortPannel.SetNewAktiveSecen(m_AktiveScene);
        SceneSerializer serialzer(m_AktiveScene);     
        serialzer.Deserialize(FileSystem::Path(path));

        
        m_Scene_HPanel.SetContext(m_AktiveScene);
      
    }

    void EditorLayer::OpenScene(AssetHandle handle)
    {
        RY_CORE_ASSERT(handle, "Error: EditorLayer::OpenScene(AssetHandle handle)");        
        AssetManager::GetAssetAsync<Scene>(handle, &m_NextScene);
    }

    void EditorLayer::OpenSceneAsync(AssetHandle handle)
    {
    }

    void EditorLayer::SaveSceneAs()
    {
        RY_PROFILE_FUNCTION();
        std::string filepath = FileDialoges::SaveFile("Rynex Scene (*.rynexscene)\0*.rynexscene\0");
        if (!filepath.empty())
        {
            SceneSerializer serialzer(m_AktiveScene);
            FileSystem::Path path(filepath);
            serialzer.Serialize(path);
        }
    }

    void EditorLayer::SaveCurentScene()
    {
        AssetHandle handle = m_AktiveScene->m_Handle;
        if (m_AssetManger->IsAssetHandleValid(handle))
        {
            const AssetMetadata metadata = m_AssetManger->GetMetadata(handle);
            SceneSerializer serialzer(m_AktiveScene);
            serialzer.Serialize(metadata.m_Path);
        }
        else
        {
            SaveSceneAs();
        }
    }

    void EditorLayer::SaveImagViewPort()
    {
        std::string filepath = FileDialoges::SaveFile("png (*.png)\0*.png\0");
        if (!filepath.empty())
        {
            Ref<Texture> textureViewPort = m_ViewPortPannel.GetFinaleImage();
            TextureImporter::SaveTexture(textureViewPort, filepath);
        }
    }

#pragma endregion

#pragma region Pannels

    

    int* EditorLayer::GetPtrGizmoType()
    {
        return &m_GizmoType;
    }

    void EditorLayer::ImGuiSettings(bool renderPannnel)
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_None;
        flags |= ImGuiWindowFlags_NoCollapse;
        flags |= ImGuiWindowFlags_NoTitleBar;

        if(ImGui::Begin("Settings", &renderPannnel, flags))
        {

            if (renderPannnel)
            {
                ImGuiRenderInfo();
            }

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
            ImGui::Text("Current Scene State: %s", sceneState.c_str());
            ImGui::SameLine(500.0f, 1.0f);
            ImGuiPlayButten();
        }
        ImGui::End();
    }

    void EditorLayer::ImGuiPlayButten()
    {  
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
                    m_AktiveScene->OnRuntimeStart();
                }
                else if(m_SceneState == SceneState::Play)
                {
                    m_SceneState = SceneState::Edit;
                    m_AktiveScene->OnRuntimeStop();
                }

            }
        }
        if (hasSimulateButton)
        {
            if (hasPlayButton)
                ImGui::SameLine();
        }
        if (hasPauseButton)
        {
            if (ImGui::Button("Pause", ImVec2(100, 0)))
            {
                m_SceneState = SceneState::Edit;
                m_AktiveScene->OnRuntimeStop();
            }
        }
    }

    void EditorLayer::ImGuiRenderInfo()
    { 
    }

   

    void EditorLayer::ImGuiPannels()
    {
        m_Scene_HPanel.OnImGuiRender();
        m_Content_BPannel.OnImGuiRender();
    }


#pragma endregion


    //--- Top Taskbar -------------
#pragma region TopTaskBar

    void EditorLayer::ImGuiTopTaskBar()
    {
        if (ImGui::BeginMenuBar())
        {
            ImGuiFile();
            ImGuiProject();
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
            if (ImGui::MenuItem("Create Project", NULL, false))
                NewProject();

            if (ImGui::MenuItem("Open Project", "Crtl+O"))
                OpenProject();

            if (ImGui::MenuItem("Save Project...", "Crtl+S"))
                SaveProject();
            
            ImGui::EndMenu();
        }
    }

    void EditorLayer::ImGuiProject()
    {
        if (ImGui::BeginMenu("Project"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.

            if (ImGui::MenuItem("New Scene", NULL, false))
                NewScene();

            if (ImGui::MenuItem("Open Scene...", "Crtl+O"))
                OpenScene();

            if (ImGui::MenuItem("SaveAs Scene...", "Crtl+S"))
                SaveSceneAs();

            //if (ImGui::MenuItem("Exit", NULL, false)) 
                //Application::Get().Close();
                
            ImGui::EndMenu();
         }
    }

    void EditorLayer::ImGuiScript()
    {
#if defined(RY_SCRIPT_ENGINE)
        if (ImGui::BeginMenu("Script"))
        {

            if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
                ScriptingEngine::ReloadAssambly();

            ImGui::EndMenu();
        }
#endif
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

#pragma endregion
   
}

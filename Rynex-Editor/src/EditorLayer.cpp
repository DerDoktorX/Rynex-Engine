#include <rypch.h>
#include "EditorLayer.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Core/Input.h>
#include <Rynex/Project/Project.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Utils/PlatformUtils.h>
#include <Rynex/Math/Math.h>

#include <Rynex/Serializers/SceneSerializer.h>
#include <Rynex/Serializers/TextureSerialiazer.h>
#include <Rynex/Serializers/VertexArraySerialzer.h>

#include <Rynex/Scripting/ScriptingEngine.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/Rendering/Renderer2D.h>
#include <Rynex/Renderer/Rendering/Renderer3D.h>

#include <Rynex/Asset/Import/ShaderImporter.h>
#include <Rynex/Renderer/Objects/Model.h>

#include <imgui/imgui.h>
#include <ImGuizmo.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



namespace Rynex {

#define RY_EDITOR_TEST_ENITITY 0
#define RY_ENABLE_VIEWPORT 1

    extern const std::filesystem::path g_AssetsPath;

    EditorLayer::EditorLayer()
        : Layer("Rynex-Editor")
        , m_CameraController((1280.0f / 720.0f), true)
    {
    }


    void EditorLayer::OnAttach()
    {
        RY_CORE_INFO("EditorLayer::OnAttach Start!");
        RY_PROFILE_FUNCTION();
        

        m_AktiveScene = CreateRef<Scene>();
        m_Scene_HPanel.SetContext(m_AktiveScene);

        //m_Content_BPannel = Project::GetActive();

        m_EditorScene = CreateRef<Scene>();
        m_AktiveScene = m_EditorScene;


        auto cLA = Application::Get().GetSpecification().CommandLineArgs;
        if (cLA.Count > 1)
        {
            auto projFilePath = cLA[1];
            OpenProject(projFilePath);
        }
        else
        {
            //if(!OpenProject())
                //Application::Get().Close();
            RY_CORE_INFO("Open a Project!");
            OpenProject("Sandbox.rproj");

        }
        
        m_Content_BPannel.OnAtache();
       
        m_EditorCamera = CreateRef<EditorCamera>(30.0f, 1.778f, 0.1, 1000.0f);

        m_Project = Project::GetActive();
        m_AssetManger = m_Project->GetEditorAssetManger(); 
        
        Renderer::Init();
        FramebufferSpecification fbSpec, fbSpec2;
        fbSpec.Attachments =
        {
            FramebufferTextureFormat::RGBA8,
            FramebufferTextureFormat::RGBA8,
            FramebufferTextureFormat::RED_INTEGER,
            FramebufferTextureFormat::DEPTH24STENCIL8
        };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);
        fbSpec2.Attachments =
        {
            FramebufferTextureFormat::RGBA8,
            FramebufferTextureFormat::DEPTH24STENCIL8
        };
        fbSpec2.Width = 1280;
        fbSpec2.Height = 720;
        m_SelectedFramebuffer = Framebuffer::Create(fbSpec2);
        m_CallFace = CallFace::None;

        m_Filtering = AssetManager::GetAsset<Shader>("Assets/shaders/Compute.glsl");
        m_FinaleImage = Texture::Create({
            ImageFormat::RGBA8, TextureTarget::Texture2D,
            1280, 720,
            TextureFilteringMode::Nearest,
        });

#if RY_ENABLE_VIEWPORT
        m_ViewPortPannel.push_back( CreateRef<ViewPortPannel>());
        for(auto& viewportpannel: m_ViewPortPannel)
        {
            viewportpannel->OnAttache("Main ViewPort", this);
        }
       

#endif

#if RY_EDITOR_TEST_ENITITY

#if 1
        {
            
        }
#endif
#if 0
        {
            m_AktiveScene->CreateEntityWitheUUID(UUID(2), "MaiViewPort");
            auto& entiy = m_AktiveScene->GetEntitiyByUUID(2);
            entiy.AddComponent<MainViewPortComponent>();
            auto& mainViewPort = entiy.GetComponent<MainViewPortComponent>();
            mainViewPort.FrameBuffer = m_Framebuffer;
            mainViewPort.EditorCamera = m_EditorCamera;
        }
#endif
        RY_CORE_INFO("Init Test Hard Coded Enttiy");
#if 1
        {
            m_AktiveScene->CreateEntityWitheUUID(UUID(8976786), "3D_RendererTestEntity");
            auto& entiy = m_AktiveScene->GetEntitiyByUUID(8976786);

            if (!entiy.HasComponent<TagComponent>())
                entiy.AddComponent<TagComponent>("3D_RendererTestEntity");

            if (!entiy.HasComponent<TransformComponent>())
                entiy.AddComponent<TransformComponent>();

#if 0
            if (!entiy.HasComponent<GeomtryComponent>())
            {
                entiy.AddComponent<GeomtryComponent>();
                auto& geometry = entiy.GetComponent<GeomtryComponent>();

                geometry.Geometry = VertexArray::Create();
                geometry.Buffer = VertexBuffer::Create(6 * sizeof(float)); // Cube: 20 * (4 * 3 + 4* 2 + 4 * 3 )
                geometry.Buffer->SetLayout(STANDERD_3DLAYOUTE);
                //geometry.Geometry->SetPrimitv(VertexArray::Primitv::Traingle);
                //Geomtrys::SetCubeVertex(geometry.Geometry, geometry.Buffer);
                //Geomtrys::SetCubeIndex(geometry.Geometry);
            }
#endif
            if (!entiy.HasComponent<MaterialComponent>())
            {
                entiy.AddComponent<MaterialComponent>();
                auto& material = entiy.GetComponent<MaterialComponent>();
#if RY_PATH_IN_LINE
                //material.Shader = AssetManager::GetAsset<Shader>("Assets/shaders/3DTestTess.glsl");
                material.Shader = AssetManager::GetAsset<Shader>("Assets/shaders/3DLigthe.glsl");
                material.UniformLayoute.clear();
               
                UniformElement ellement;
                {
                    ellement.Name = "u_Color";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("vec3");
                    ellement.ShaderResourceType = ShaderResourceType::LocalColor;
                    material.UniformLayoute.push_back(ellement);
                }
                
                {
                    ellement.Name = "u_Model";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("mat4");
                    ellement.ShaderResourceType = ShaderResourceType::LocalModel;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_ViewProj";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("mat4");
                    ellement.ShaderResourceType = ShaderResourceType::MainCameraViewProjectionMatrix;
                    material.UniformLayoute.push_back(ellement);

                }

                {
                    ellement.Name = "u_EntityID";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("int");
                    ellement.ShaderResourceType = ShaderResourceType::EnitiyID;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_CamerPos";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("vec3");
                    ellement.ShaderResourceType = ShaderResourceType::MainCamerPos;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_LigthPos";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("vec3");
                    ellement.ShaderResourceType = ShaderResourceType::None;
                    ellement.GloblelResurce = false;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_LigthColor";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("vec3");
                    ellement.ShaderResourceType = ShaderResourceType::None;
                    ellement.GloblelResurce = false;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_Shinines";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("float");
                    ellement.ShaderResourceType = ShaderResourceType::None;
                    ellement.GloblelResurce = false;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_Specular";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("float");
                    ellement.ShaderResourceType = ShaderResourceType::None;
                    ellement.GloblelResurce = false;
                    material.UniformLayoute.push_back(ellement);
                    
                }

                {
                    ellement.Name = "u_Ambient";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("float");
                    ellement.ShaderResourceType = ShaderResourceType::None;
                    // ellement.GloblelResurce = false;
                    material.UniformLayoute.push_back(ellement); 
                }
                {
                    for (auto& element : material.UniformLayoute)
                    {
                        if ((!element.GloblelResurce) && (!element.LocalResurce.size()))
                        {
                            element.LocalResurce.resize(ShaderDataTypeSize(element.Type));
                            switch (element.Type)
                            {
                            case ShaderDataType::Float:
                            {
                                float* value = (float*)element.LocalResurce.data();
                                *value = 0.0f;
                                if (element.Name == "u_Shinines")
                                    *value = 16.0f;
                                else if (element.Name == "u_Specular")
                                    *value = 0.5f;
                                else if (element.Name == "u_Ambient")
                                    *value = 0.1f;
                                break;
                            }
                            case ShaderDataType::Float2:
                            {
                                glm::vec<2, float>* value = (glm::vec<2, float>*)element.LocalResurce.data();
                                *value = { 0.0f, 0.0f};
                                break;
                            }
                            case ShaderDataType::Float3:
                            {
                                
                                glm::vec<3, float>* value = (glm::vec<3, float>*)element.LocalResurce.data();
                                *value = { 0.0f, 0.0f,0.0f };
                                if (element.Name == "u_LigthPos")
                                    *value = { 2.5f, 3.0f, 2.0f };
                                else if (element.Name == "u_LigthColor")
                                    *value = { 1.0f, 1.0f, 1.0f };
                                break;
                            }
                            case ShaderDataType::Float4:
                            {
                                
                                
, float>* value = (glm::vec<4, float>*)element.LocalResurce.data();
                                *value = { 0.0f, 0.0f,0.0f, 0.0f };
                                break;
                            }
                            case ShaderDataType::Float3x3:
                            {
                                glm::mat3* value = (glm::mat3*)element.LocalResurce.data();
                                *value = {
                                    1.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f
                                };
                                break;
                            }
                            case ShaderDataType::Float4x4:
                            {
                                glm::mat4* value = (glm::mat4*)element.LocalResurce.data();
                                *value = {
                                    1.0f, 0.0f, 0.0f, 0.0f,
                                    0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0.0f, 0.0f, 0.0f, 1.0f
                                };
                                break;
                            }
                            case ShaderDataType::Int:
                            {
                                int* value = (int*)element.LocalResurce.data();
                                *value = 0;
                                break;
                            }
                            case ShaderDataType::Uint:
                            {
                                uint32_t* value = (uint32_t*)element.LocalResurce.data();
                                *value = 0;
                                break;
                            }
                            default:
                                break;
                            }
                        }
                    }
                }
#endif
            }

            if (!entiy.HasComponent<ScriptComponent>())
            {
                entiy.AddComponent<ScriptComponent>();
                auto& script = entiy.GetComponent<ScriptComponent>();
                script.Name = "Sandbox.Player";
            }
#if 1
            if (!entiy.HasComponent<MeshComponent>())
            {
                entiy.AddComponent<MeshComponent>();
                MeshComponent& meshC = entiy.GetComponent<MeshComponent>();
                //CreateRef<Model>("Assets/Models/Cube.gltf");

            }
#endif
        }
#endif

#if 0
        {
            m_AktiveScene->CreateEntityWitheUUID(UUID(8976734286), "FrameBuffer_Test");
            auto& entiy = m_AktiveScene->GetEntitiyByUUID(8976734286);

            if (!entiy.HasComponent<TagComponent>())
                entiy.AddComponent<TagComponent>("FrameBuffer_Test");

            if (!entiy.HasComponent<TransformComponent>())
                entiy.AddComponent<TransformComponent>();


            if (!entiy.HasComponent<FrameBufferComponent>())
            {
                entiy.AddComponent<FrameBufferComponent>();
                auto& framebufferC = entiy.GetComponent<FrameBufferComponent>();


                framebufferC.FramebufferSpecification = fbSpec;
                framebufferC.FrameBuffer = Framebuffer::Create(fbSpec);
            }

            if (!entiy.HasComponent<CameraComponent>())
            {
                entiy.AddComponent<CameraComponent>();
                auto& camer = entiy.GetComponent<CameraComponent>();
                 
            }

            if (!entiy.HasComponent<ScriptComponent>())
            {
                entiy.AddComponent<ScriptComponent>();
                auto& script = entiy.GetComponent<ScriptComponent>();
                script.Name = "Sandbox.Player";
            }
        }
#endif
#if 0
        {
            Ref<Texture> testTextPip1 = Texture::Create({
                ImageFormat::RGBA8,
                TextureTarget::Texture2D,
                99, 99,
                TextureFilteringMode::Nearest,
                1
                });


            AssetMetadata metadata;
            metadata.Type = testTextPip1->GetType();
            metadata.FilePath = "Assets/CheckeBordNearest.rytex2d";
            m_AssetManger->CreateAsset(metadata.FilePath, (Ref<Asset>)testTextPip1, metadata);
            uint32_t size = 99 * 99;
            uint32_t imageData[99 * 99];

            for (uint32_t i = 0; i < size; i++)
                imageData[i] = (i % 2) ? 0x00000000ui32 : 0xffffffffui32;

            uint32_t whitheTexData = 0xffffffffui32;
            testTextPip1->SetData(imageData, sizeof(uint32_t) * size);

            Texture2DSerialiazer::Serlize(metadata.FilePath, testTextPip1);
        }
        {
            Ref<Texture> testTextPip2 = Texture::Create({
                ImageFormat::RGBA8,
                TextureTarget::Texture2D,
                7, 7,
                TextureFilteringMode::Linear,
                1
            });
            

            AssetMetadata metadata;
            metadata.Type = testTextPip2->GetType();
            metadata.FilePath = "Assets/CheckeBordLinear.rytex2d";
            m_AssetManger->CreateAsset(metadata.FilePath, (Ref<Asset>)testTextPip2, metadata);
            uint32_t size = 7 * 7;
            uint32_t imageData[7 * 7];

            for (uint32_t i = 0; i < size; i++)
                imageData[i] = (i % 2) ? 0xff0000000 : 0xffffffff;

            uint32_t whitheTexData = 0xffffffff;
            testTextPip2->SetData(imageData, sizeof(uint32_t) * size);

            Texture2DSerialiazer::Serlize(metadata.FilePath, testTextPip2);
        }
#endif //This Code Create a Texture, saved it on disc if the same code run 2 tims in a row you get errors because its not allowd to overide olardy existen Assets!
#if 0
        {
            UUID uuid = UUID();
            m_AktiveScene->CreateEntityWitheUUID(uuid, "Texture_Test");
            auto& entiy = m_AktiveScene->GetEntitiyByUUID(uuid);

            if (!entiy.HasComponent<TagComponent>())
                entiy.AddComponent<TagComponent>("Texture_Test");

            if (!entiy.HasComponent<TransformComponent>())
            {
                 entiy.AddComponent<TransformComponent>();
                

            }
            auto& transformC = entiy.GetComponent < TransformComponent>();
            transformC.Transaltion = { 1.25f, 0.5f, 2.75f };
            //transformC.Scale = { 5.f, 3.f, 10.f };


            if (!entiy.HasComponent<SpriteRendererComponent>())
            {
                entiy.AddComponent<SpriteRendererComponent>();
                auto& spriteC = entiy.GetComponent<SpriteRendererComponent>();


                //spriteC.Texture = AssetManager::GetAsset<Texture2D>();
                //spriteC.Color = { 1.,1.,1., 1. };
            }

        }
#endif
#endif
        m_RendererPannel.OnAttache(this);
        RY_CORE_INFO("EditorLayer::Sucese Finished!");
    }

    void EditorLayer::OnDetach()
    {
        RY_CORE_WARN("OnDetach Aktiv!");
        RY_PROFILE_FUNCTION();
        
        Renderer::Shutdown();
        ScriptingEngine::Shutdown();
        Project::ShutDown();
        
        m_RendererPannel.OnDetache();
        for (auto& viewportpannel : m_ViewPortPannel)
        {
            viewportpannel->OnDetache();
        }
        RY_CORE_WARN("OnDetach Done!");
    }

    void EditorLayer::OnUpdate(TimeStep ts)
    {   
        m_PasTime += ts;

        Renderer2D::ResetStats();
        switch (m_SceneState)
        {
            case SceneState::Edit:
            { 

                m_AktiveScene->OnUpdateEditor(ts);   
                for (auto& viewportpannel : m_ViewPortPannel)
                {
                    viewportpannel->OnRenderEditor(ts);
                }
                
                break;
            }
            case SceneState::Simulate:
            {
                m_EditorCamera->OnUpdate(ts);
                m_AktiveScene->OnUpdateSimulation(ts);
                
                for (auto& viewportpannel : m_ViewPortPannel)
                {
                    viewportpannel->OnRenderSimultion();;
                }

                break;
            }
            case SceneState::Play:
            {
                m_AktiveScene->OnUpdateRuntime(ts);
                for (auto& viewportpannel : m_ViewPortPannel)
                {
                    viewportpannel->OnRenderRuntime(0);
                }

                break;
            }
        }
        
        ////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////
        m_RendererPannel.OnUpdate(ts);
        for (auto& viewportpannel : m_ViewPortPannel)
        {
            viewportpannel->OnUpdate();
        }

        
    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_RendererPannel.OnEvent(e);
       
        for (auto& viewportpannel : m_ViewPortPannel)
        {
            viewportpannel->OnEventCamera(e);
        }

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
        for (auto& viewportpannel : m_ViewPortPannel)
        {
            viewportpannel->OnMousPressed(e);
        }
        return false;
    }


    void EditorLayer::NewProject()
    {
        Project::New();
    }

    bool EditorLayer::OpenProject()
    {
        std::string filepath = FileDialoges::OpenFile("Rynex Project (*.rproj)\0*.rproj\0");
        if (filepath.empty())
            return false;

        OpenProject(filepath);
        return true;
    }

    void EditorLayer::OpenProject(const std::filesystem::path& path)
    {
        if (Project::Load(path))
        {
            ScriptingEngine::Init();
            std::filesystem::path startScene = Project::GetActive()->GetConfig().StartScene;
            if (startScene.string() != "")
                OpenScene(startScene);
            m_Content_BPannel = ContentBrowserPannel();
        }
    }

    void EditorLayer::SaveProject()
    {
    }


    void EditorLayer::NewScene()
    {
        m_AktiveScene = CreateRef<Scene>(); 
        for (auto& viewPort : m_ViewPortPannel)
        {
            viewPort->SetNewAktiveSecen(m_AktiveScene);
        }
        m_AktiveScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
        m_Scene_HPanel.SetContext(m_AktiveScene);
       
    }

    void EditorLayer::OpenScene()
    {
        RY_PROFILE_FUNCTION();
        std::string filepath = FileDialoges::OpenFile("Rynex Scene (*.rynex)\0*.rynex\0");

       
        if (!filepath.empty())
        {
            OpenScene(filepath);
        }
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {  
        m_AktiveScene = CreateRef<Scene>();

        for (auto& viewPort : m_ViewPortPannel)
        {
            viewPort->SetNewAktiveSecen(m_AktiveScene);

        }
        SceneSerializer serialzer(m_AktiveScene);     
        serialzer.Deserialize(path.string());  

        
        m_Scene_HPanel.SetContext(m_AktiveScene);
      
    }

    void EditorLayer::OpenScene(AssetHandle handle)
    {
        RY_CORE_ASSERT(handle, "Error: EditorLayer::OpenScene(AssetHandle handle)");

        if (m_SceneState != SceneState::Edit)
        {
            RY_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate, "Error Futer Funktion: EditorLayer::OnSceneStop()");
           
            if (m_SceneState == SceneState::Play)
                m_AktiveScene->OnRuntimStop();
            else if (m_SceneState == SceneState::Simulate)
                m_AktiveScene->OnRuntimStop();

            m_SceneState = SceneState::Edit;

            m_AktiveScene = m_EditorScene;

            m_Scene_HPanel.SetContext(m_AktiveScene);
            

        }

        Ref<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(handle);
        Ref<Scene> newScene = Scene::Copy(readOnlyScene);

        m_EditorScene = newScene;
        m_Scene_HPanel.SetContext(m_EditorScene);

        m_AktiveScene = m_EditorScene;
        m_EditorScenePath = Project::GetActive()->GetEditorAssetManger()->GetMetadata(handle).FilePath;    

        for (auto& viewPort : m_ViewPortPannel)
        {
            viewPort->SetNewAktiveSecen(m_AktiveScene);
        }

    }

    void EditorLayer::SaveSceneAs()
    {
        RY_PROFILE_FUNCTION();
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
        static bool assetsEnabled   = true;
        static bool sceneEnabled    = true;
        static bool viewPortEnabled = false;
        static bool settingsEnabled = true;
        static bool renderPannnel = false;

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
            if(sceneEnabled)    ImGuiPannels();
            if(settingsEnabled) ImGuiSettings(renderPannnel);
            if(viewPortEnabled) 
            {
                ImGuiViewPort();
            }
            else
            {
                bool blockEvents = false;
                for (auto& viewportpannel : m_ViewPortPannel)
                {
                    blockEvents = viewportpannel->OnImGuiRender() || blockEvents;
                }
                ViewPortPannel::SetEventBlocker(!blockEvents);
                
            }
            if (!renderPannnel)
            {
                m_RendererPannel.OnImGuiRender();
            }
            

            ImGui::End();
        }
        else
        {   
            if(sceneEnabled)    ImGuiPannels();
            if(settingsEnabled) ImGuiSettings(renderPannnel);
            if(viewPortEnabled) ImGuiViewPort();
            if (!renderPannnel)
            {
                m_RendererPannel.OnImGuiRender();
            }
           

            ImGui::End();
        }


    }

    int* EditorLayer::GetPtrGizmoType()
    {
        return &m_GizmoType;
    }



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
#if 0
        uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0);
#else
        uint32_t textureID = m_FinaleImage->GetRenderID();
#endif
        //uint32_t textureID = m_Framebuffer->GetDeathAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void*>(textureID),  viewportPannelSize, ImVec2(0,1), ImVec2(1,0)); 

        // Drag and drop conten Broser
        ImGuiContentBrowserViewPort();

        // Set New ViewPort for Debuging or secen other stuffe
        ImGuiSecundaryViewPort(m_Framebuffer, 1, viewportPannelSize.x, viewportPannelSize.y);
       
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
        ImGui::Begin("Secundary Viewport", nullptr, ImGuiWindowFlags_DockNodeHost  );

        uint32_t textureID = framebuffer->GetColorAttachmentRendererID(id);
        ImVec2 size = ImGui::GetWindowSize();
        size.y = size.x / ((float)width / height);
        ImGui::Image((ImTextureID)textureID, size, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SetWindowSize(size);
      
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void EditorLayer::ImGuiSettings(bool renderPannnel)
    {
        ImGui::Begin("Settings");
        
        if(renderPannnel)
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
        ImGui::Text("Curent Scene State: %s", sceneState.c_str());
        ImGui::SameLine(500.0f, 1.0f);
        ImGuiPlayButten();

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
        }
        if (hasPauseButton)
        {
            if (ImGui::Button("Pause", ImVec2(50, 0)))
            {
                m_SceneState = SceneState::Edit;
            }
        }
    }

    void EditorLayer::ImGuiRenderInfo()
    { 
        
         
        ImGui::Text("Renderer:");

        if(ImGui::Checkbox("Wirframe mode:", &m_RendereWirframe))
            Renderer3D::AktivePolyGunMode(m_RendereWirframe);

        if(ImGui::Checkbox("Deaph Test mode:", &m_RendereDepthe))
            
        
        if (ImGui::RadioButton("None", (int*)&m_CallFace, (int)CallFace::None))
        {
                m_CallFace = CallFace::None;
                Renderer3D::SetFace(m_CallFace);
        }
        if (ImGui::RadioButton("Front", m_CallFace == CallFace::Front))
        {
            m_CallFace = CallFace::Front;
            Renderer3D::SetFace(m_CallFace);
        }
        if (ImGui::RadioButton("Back", m_CallFace == CallFace::Back))
        {
            m_CallFace = CallFace::Back;
            Renderer3D::SetFace(m_CallFace);
        }
        if (ImGui::RadioButton("FrontBacke", m_CallFace == CallFace::FrontBacke))
        {
            m_CallFace = CallFace::FrontBacke;
            Renderer3D::SetFace(m_CallFace);
        }
        
        ImGui::Text("Renderer2D Stats:");
        auto stats = Renderer2D::GetStats();
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quad   : %d", stats.QuadCount);
        ImGui::Text("Vertex : %d", stats.GetTotalVertexCount());
        ImGui::Text("Indexs : %d", stats.GetTotalIndexCount());
    }

    void EditorLayer::ImGuiPannels()
    {
        m_Scene_HPanel.OnImGuiRender();
        m_Content_BPannel.OnImGuiRender();
    }

    void EditorLayer::ImGuiContentBrowserViewPort()
    {
        if (ImGui::BeginDragDropTarget())
        {
            

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( GetAssetTypeDragAndDropName(AssetType::Scene).c_str() ))
            {
                AssetHandle handle = *(AssetHandle*)payload->Data;
                OpenScene(handle);
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

            // Camera

            glm::mat4 camerProj, camerView;

            switch (m_SceneState)
            {
                case SceneState::Edit:
                {
                    camerProj = m_EditorCamera->GetProjektion();
                    camerView = m_EditorCamera->GetViewMatrix();
                    break;
                }
                case SceneState::Play:
                {
                    auto camerEntt = m_AktiveScene->GetPrimaryCameraEntity();
                    const auto& camera = camerEntt.GetComponent<CameraComponent>().Camera;
                    const glm::mat4& camerProj = camera.GetProjektion();
                    camerView = glm::inverse(camerEntt.GetComponent<TransformComponent>().GetTransform());
                    break;
                }
                default:
                {
                    camerProj = m_EditorCamera->GetProjektion();
                    camerView = m_EditorCamera->GetViewMatrix();
                    break;
                }
            }
            

            // Entity transform
            if (!selectedEntity.HasComponent<TransformComponent>()) return;

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

    void EditorLayer::RenderSelectedEntity(Entity slelcted)
    {
        
        m_SelectedFramebuffer->Bind();
        RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
        RenderCommand::Clear();
        Camera& mainCamera = (Camera)m_EditorCamera->GetProjektion();
        glm::mat4 viewMatrix = m_EditorCamera->GetViewMatrix();
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
                if (materialC.Shader != nullptr && vertexArray != nullptr)   
                {
                    Renderer3D::BeforDrawEntity(materialC, transformC.GetTransform(),(int)slelcted);
                    Renderer3D::DrawObjectRender3D(vertexArray);
                    Renderer3D::AfterDrawEntity(materialC);
                }
                Renderer3D::EndScene();
            }
        }
      
        m_SelectedFramebuffer->Unbind();
    }

    void EditorLayer::RenderHoveredEntity(Entity hovered)
    {
        m_HoveredFramebuffer->Bind();
        RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
        RenderCommand::Clear();
        Camera& mainCamera = (Camera)m_EditorCamera->GetProjektion();
        glm::mat4 viewMatrix = m_EditorCamera->GetViewMatrix();
        if (hovered != Entity() && hovered.HasComponent<TransformComponent>())
        {
            TransformComponent transformC = hovered.GetComponent<TransformComponent>();
            if (hovered.HasComponent<SpriteRendererComponent>())
            {
                Renderer2D::BeginScene(mainCamera, viewMatrix);
                SpriteRendererComponent spriteC = hovered.GetComponent<SpriteRendererComponent>();
                Renderer2D::DrawSprite(transformC.GetTransform(), spriteC, hovered.GetEntityHandle());
                Renderer2D::EndScene();
            }
            else  if (hovered.HasComponent<GeomtryComponent>() && hovered.HasComponent<MaterialComponent>())
            {
                Renderer3D::BeginScene(mainCamera, viewMatrix);
                GeomtryComponent geomtryC = hovered.GetComponent<GeomtryComponent>();
                MaterialComponent materialC = hovered.GetComponent<MaterialComponent>();
                Ref<VertexArray> vertexArray = geomtryC.Geometry;
                if (vertexArray != nullptr)
                {
                    Renderer3D::BeforDrawEntity(materialC, transformC.GetTransform(), (int)hovered);
                    Renderer3D::DrawObjectRender3D(vertexArray);
                    Renderer3D::AfterDrawEntity(materialC);
                }
                Renderer3D::EndScene();
            }
        }

        m_HoveredFramebuffer->Unbind();
    }

    void EditorLayer::FinaleImgaeFilterEditor()
    {
        m_Filtering->Bind();
        m_SelectedFramebuffer->BindColorAttachmentImage(Acces::Read, 0, 0);
        m_Framebuffer->BindColorAttachmentImage(Acces::Read, 0, 1);
        m_FinaleImage->BindImage(Acces::Write, 2);
        RenderCommand::DispatcheCompute({ (m_ViewPortSize.x/32 )  , (m_ViewPortSize.y / 16)  , 1 });
    }

    void EditorLayer::ImGuiViewPortResize( ImVec2 vPSize)
    {
        if (m_ViewPortSize != *((glm::vec2*)&vPSize))
        {
            glm::vec<2, uint32_t> size = { vPSize.x , vPSize.y };
            m_Framebuffer->Resize(size.x, size.y);
            m_SelectedFramebuffer->Resize(size.x, size.y);
            m_FinaleImage->Resize(size.x, size.y);
            m_ViewPortSize = { vPSize.x, vPSize.y };
            m_EditorCamera->SetViewportSize(vPSize.x, vPSize.y);
            m_CameraController.OnResize((float)vPSize.x, (float)vPSize.y);

            m_AktiveScene->OnViewportResize(size.x, size.y);
        }
    }



    //--- Top Taskbar -------------

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

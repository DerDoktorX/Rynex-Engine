#include <rypch.h>
#include "ViewPortPannel.h"
#include "EditorLayer.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Core/Input.h>
#include <Rynex/Math/Math.h>
#include <Rynex/Asset/Base/AssetManager.h>

#include <Rynex/Renderer/Rendering/RenderTarget.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/Rendering/Render2D/Renderer2D.h>

#include <Rynex/Renderer/Materials/Material.h>

#include <imgui/imgui.h>
#ifdef RY_IM_GUIZMO
#include <ImGuizmo.h>
#endif
#include <imgui/imgui_internal.h>
#include <Rynex/Renderer/Rendering/Render3D/Renderer3D.h>


namespace Rynex {
#define RY_MULTY_VIEWPORTS 1
#define RY_RENDERER_SHOADOWS_SECOUNDARY 1
#define RY_CHECK_FORCHANGE_BEFORE_COMPUTING 1


#pragma region ViewPortPannel
    namespace Utils {
        
        static ImVec2 Convert_GLMvec2_In_ImVec2(const glm::vec2& vec)
        {
            return ImVec2{ vec.x, vec.y };
        }
    }
    ViewPortPannel::ViewPortPannel()
        : m_SceneState(SceneState::Edit)
    {
    }

    ViewPortPannel::~ViewPortPannel()
    {
        RY_CORE_ASSERT(m_ViewPorts.empty());
    }

    void ViewPortPannel::OnAttache(const std::string& name, EditorLayer* editorLayer)
    {
        m_WindowName = name;
        m_EditorLayer = editorLayer;
        m_Camera = editorLayer->GetEditorCamera();
       
        FramebufferSpecification fbSpec, fbSpec2;
        fbSpec.Attachments = {
            {
                TexFrom::RGBA32F, 1u,
                { TexWarp::ClampEdge,  TexWarp::ClampEdge, TexWarp::ClampEdge, },
                TexFilter::Nearest
            },
            {
                TexFrom::RED_INTEGER, 1u,
                { TexWarp::ClampEdge,  TexWarp::ClampEdge, TexWarp::ClampEdge, },
                TexFilter::Nearest
            },
            {
                TexFrom::DepthComp24, m_TextureSampleCount,
                { TexWarp::ClampEdge,  TexWarp::ClampEdge, TexWarp::ClampEdge, },
                TexFilter::Nearest,
            }
        };

        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        fbSpec2.Samples = m_TextureSampleCount;


        m_Framebuffer = Framebuffer::Create(fbSpec);
        fbSpec2.Attachments = {
            {
                TexFrom::RGBA32F, 1u,
                { TexWarp::ClampEdge,  TexWarp::ClampEdge, TexWarp::ClampEdge, },
                TexFilter::Nearest
            },
            { 
                TexFrom::DepthComp16, 1u,
                { TexWarp::ClampEdge,  TexWarp::ClampEdge, TexWarp::ClampEdge, },
                TexFilter::Nearest
            }
        };
        fbSpec2.Width = 1280;
        fbSpec2.Height = 720;
        fbSpec2.Samples = 1u;

        m_SelectedFramebuffer = Framebuffer::Create(fbSpec2);
        m_SelctedTarget.SetFramebuffer(m_SelectedFramebuffer);
        m_SelctedTarget.SetClearColorAttachment(0, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        m_Filtering = AssetManager::GetAsset<Shader>("../Rynex-Editor/Editor-Assets/shaders/Compute.glsl");

        m_AktiveScene = m_EditorLayer->GetAktivScene();
        m_GizmoType = m_EditorLayer->GetPtrGizmoType();
        
        {
            m_ViewPorts.push_back(CreateRef<ViewPort>());
           
           
            m_ViewPorts[0]->OnAttache(m_Framebuffer,
                m_SelectedFramebuffer, 
                m_Camera, 
                m_Filtering, 
                m_EditorLayer, 
                &m_SceneState, 
                this, 
                &m_HoveredEntity, 
                GetStaicResultionSizePtr(),
                0, 
                0, 
                m_WindowName + " Attachment: " + std::to_string(0)
            );
             
            
        }
        
    }

    void ViewPortPannel::OnUpdate()
    {
        RY_PROFILE_SCOPE("ViewPortPannel Update");

        glm::vec2 mousPos = Input::GetMousePosition();        
        for (auto& viewPort : m_ViewPorts)
        {
            RY_PROFILE_SCOPE("ViewPortPannel Update for loop Element");
#if 0
            viewPort->OnUpdate(mousPos);
#elif 0
            viewPort->OnUpdate({ mx, my });
            
#else
            viewPort->OnUpdate();
#endif
        }

    }

    void ViewPortPannel::OnDetache()
    { 
        RY_DESTROY_REF(m_Framebuffer);
        RY_DESTROY_REF(m_SelectedFramebuffer);

        RY_DESTROY_REF(m_Camera);
        RY_DESTROY_REF(m_Filtering);

        RY_DESTROY_REF(m_MaterialC.material);
        RY_DESTROY_REF(m_AktiveScene);
        m_SelctedTarget.SetFramebuffer(nullptr);
        m_GizmoType = nullptr;

        m_EditorLayer = nullptr;

        for (auto& viewPort : m_ViewPorts)
        {
            viewPort->OnDetache();
            RY_DESTROY_REF(viewPort);
        }
        m_ViewPorts.clear();
        m_ViewPorts.shrink_to_fit();

    }

    void ViewPortPannel::SetComputeInvocation(const glm::vec3& invoction)
    {
        for (auto& viewPort : m_ViewPorts)
        {
            viewPort->SetComputeInvocation(invoction);
        }
    }

    Ref<Texture> ViewPortPannel::GetFinaleImage() const
    {
        RY_CORE_ASSERT(!m_ViewPorts.empty());
        const auto& viewPort = m_ViewPorts.at(0);
        RY_CORE_ASSERT(nullptr != viewPort);
        return viewPort->GetFinalImag();;
    }

   

    void ViewPortPannel::OnRenderEditor(TimeStep ts)
    {
        m_SceneState = SceneState::Edit;
        if (m_WindowFocused)
        {
            m_Camera->OnUpdate(ts);
        }
        {
            RY_SCOPE_TIMER(m_SceneRenderUpdateTime);
            m_AktiveScene->OnRenderEditor(m_Framebuffer, m_Camera);
        }
        bool change = RenderSelectedEntity();

        if (change)
        {
            for (auto& viewPort : m_ViewPorts)
            {
                viewPort->OnRenderEditorFilter();
            }
        }
        else
        {
            for (auto& viewPort : m_ViewPorts)
            {
                viewPort->OnRenderEditorFilterOff();
            }
            
        }
    }

    void ViewPortPannel::OnRenderRuntime(int camerCount)
    {        
        RY_SCOPE_TIMER(m_SceneRenderUpdateTime);
        m_SceneState = SceneState::Play;
        m_AktiveScene->OnRenderRuntime(m_Framebuffer, camerCount);
    }

    void ViewPortPannel::OnRenderSimultion()
    {
        RY_SCOPE_TIMER(m_SceneRenderUpdateTime);
        m_SceneState = SceneState::Simulate;
        m_AktiveScene->OnRenderSimulation(m_Framebuffer, m_Camera);

    }


    void ViewPortPannel::OpenTextureWindows()
    {
        for (auto& viewPort : m_ViewPorts)
        {
            viewPort->OpenTextureWindows();
        }
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
#ifdef IM_GUIZMO
                else if (!ImGuizmo::IsOver())
                    m_EditorLayer->SetSelectedEntity(m_HoveredEntity);
#endif
            }
        }
        return false;
    }

    void ViewPortPannel::HoveredEntity()
    {

    }

    void ViewPortPannel::OnStaicResultion()
    {
        for (auto& viewPort : m_ViewPorts)
            viewPort->OnStaicResultion(m_StaticReslution);
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


    bool ViewPortPannel::RenderSelectedEntity()
    {
#ifdef RY_RENERER_DESIGN_CURENT_MAIN
        RY_REMBER_FUNC_CHANGE("Enable the Func if render System is func");
        return false;
#endif

#if 0
        return false;
#else
        Entity slelcted = m_EditorLayer->GetSelectedEntity();
        static bool skipFramBevor = true;
        if (!slelcted && skipFramBevor)
        {
            skipFramBevor = true;
            return false;
        }

        skipFramBevor = false;
    
     
#if 1
        if (slelcted.IsNotVaild())
            return false;
#endif
        const Ref<Framebuffer>& fb = m_SelctedTarget.GetFramebuffer();

        glm::uvec3 sizeUint = fb->GetFrambufferSize();
        glm::ivec2 size = glm::ivec2(sizeUint);

        glm::ivec4 viwportSize = glm::ivec4(size, 0, 0);
        m_SelctedTarget.ResizeView(viwportSize);
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
        const RenderTarget targetCopy = Renderer::DirecktMainRenderingTarget(m_SelctedTarget);


        
        int drawCount = 0;
        RenderEntityAndParentRecusiv(&drawCount, slelcted);
#if 1
        std::function<void(Entity e)> func = std::bind(&RenderEntityAndParentRecusiv, &drawCount, std::placeholders::_1);
        slelcted.ExecuteOnChildrens(func);
#endif
        Renderer::RenderSubmitSceneMainNow(targetCopy);
#else
        Renderer::SetOnMainCameraCurentCamera();
        int drawCount = 0;

#if 1
        std::function<void(Entity e)> func = std::bind(&RenderEntityAndParentRecusiv, &drawCount, std::placeholders::_1);
        slelcted.ExecuteOnChildrens(func);
#endif
        Renderer::RenderingPassCurent();
        Renderer::ClearCurentPiplines();
#endif 
        return drawCount != 0;
#endif
    } 
    
    void ViewPortPannel::RenderEntityAndParentRecusiv(int* drawCount, Entity e)
    {
#if 1
        if (e.IsNotVaild())
            return;
#endif

        if (e.HasComponent<ModelMatrixComponent>())
        {
            ModelMatrixComponent& modelC = e.GetComponent<ModelMatrixComponent>();
            if (e.HasComponent<SpriteRendererComponent>())
            {
                SpriteRendererComponent& spriteC = e.GetComponent<SpriteRendererComponent>();
                Renderer2D::SubmitSprite(modelC.Globle, spriteC, e.GetEntityHandle());
                (*drawCount)++;
            }
            if (e.HasComponent<TextComponent>())
            {

                TextComponent& textC = e.GetComponent<TextComponent>();
                Renderer2D::SubmitStringCom(modelC.Globle, textC, e.GetEntityHandle());
                (*drawCount)++;
            }

            if (e.HasComponent<CameraComponent>())
            {
                CameraComponent& cameraC = e.GetComponent<CameraComponent>();
                Renderer2D::SubmitCameraIcon(modelC.Globle, e.GetEntityHandle());
                (*drawCount)++;
            }
#if 0
            if (e.HasComponent<StaticSingleComponetsMeshComponent>())
            {
                StaticSingleComponetsMeshComponent& meshStaticC = e.GetComponent<StaticSingleComponetsMeshComponent>();
                Renderer3D::MeshCompontDirekt(modelC.Globle, meshStaticC, e.GetEntityHandle());
                (*drawCount)++;
            }
#else

            if (e.HasComponent<ModelMangerComponent>())
            {
                ModelMangerComponent& meshStaticC = e.GetComponent<ModelMangerComponent>();
                Renderer3D::MeshCompontDirekt(modelC.Globle, meshStaticC, e.GetEntityHandle());
                (*drawCount)++;
            }
#endif
        }
#if 0
        std::function<void(Entity e)> func = std::bind(&RenderEntityAndParentRecusiv, drawCount, std::placeholders::_1);
        e.ExecuteOnChildrens(func);
#endif
    }

#pragma endregion


#pragma region ViewPort

    ViewPort::ViewPort()
    {
        
    }

    ViewPort::~ViewPort()
    {
        RY_CORE_ASSERT(nullptr == m_Image);
        RY_CORE_ASSERT(nullptr == m_FinaleImage);
    }

    void ViewPort::OnAttache(const Ref<Framebuffer>& frambuffer, const Ref<Framebuffer>& selectedFrameBuffer, const Ref<EditorCamera>& editorCamera, const Ref<Shader>& shader
        , EditorLayer* editorLayer, SceneState* sceneState, ViewPortPannel* viewPortPannel, Entity* hoveredEntity, glm::ivec2* reslutionPtr
        , int renderOnAtachment, int runtimCameraIndex, const std::string& name)
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
        m_StaticGameReslutionPtr = reslutionPtr;
        m_AktiveScene = m_EditorLayer->GetAktivScene();
        m_GizmoType = m_EditorLayer->GetPtrGizmoType();
        m_Image = Texture::Create({
            1280u, 720u, 1u,
            TextureTarget::Texture2D,
            TextureFormat::RGBA32F,
            m_TextureSampleCount,
            TextureFilteringMode::Nearest
        });

        m_FontTex = Font::GetDefault()->GetAtlasTexture();

        
    }

    void ViewPort::OnDetache()
    {
        m_EditorLayer = nullptr;
        m_SceneState = nullptr;
        m_HoveredEntity = nullptr;
         
        m_GizmoType = nullptr;
        m_StaticGameReslutionPtr = nullptr;

        RY_DESTROY_REF(m_Image);
        RY_DESTROY_REF(m_FinaleImage);
        RY_DESTROY_REF(m_Filtering);
        RY_DESTROY_REF(m_Camera);
        RY_DESTROY_REF(m_Framebuffer);
        RY_DESTROY_REF(m_SelectedFramebuffer);
        RY_DESTROY_REF(m_AktiveScene);
    }

    void ViewPort::OnUpdate(const glm::vec2& mousePos)
    {
        m_MousePos = mousePos;
        OnUpdate();
    }

    void ViewPort::OnUpdate()
    {
        HoveredEntity();
    }


   

    glm::vec2 ViewPort::CalculateImageWindowSize(const Ref<Texture>& texture)
    {
        ImVec2 viewportPannelShadowSize = ImGui::GetContentRegionAvail();

        float witheTexure = texture->GetWidth();
        float heigthTexure = texture->GetHeight();
        float aspectRoation = witheTexure / heigthTexure;

        float viewPortX = viewportPannelShadowSize.x;
        float viewPortY = viewportPannelShadowSize.y;
        viewPortY -= 2;

        if (viewPortX < viewPortY)
        {
            viewPortX = viewPortX;
            viewPortY = viewPortX * aspectRoation;
        }
        else
        {
            viewPortX = viewPortY * aspectRoation;
            viewPortY = viewPortY;
        }

        return glm::vec2{ viewPortX , viewPortY };
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
        m_MousButtenLeftPressed = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

        ImVec2 mousPos = ImGui::GetMousePos();
        m_MousePos = { mousPos.x, mousPos.y };
        // ViewPort get Size + Resize Image + SetImage in ViewPort
        ImVec2 viewportPannelSize = ImGui::GetContentRegionAvail();
        WindowResize({ viewportPannelSize.x , viewportPannelSize.y });
        m_WindowMoving = IsCurrentWindowMoving();

        Ref<Texture> finelTex = GetFinalImag();
        uint32_t textureID = finelTex->GetRenderID();
        ImVec2 uv0{ 0, 1 };
        ImVec2 uv1{ 1, 0 };
       
        ImTextureID imguiTextureID = reinterpret_cast<ImTextureID>(&textureID);
        ImGui::Image(
            imguiTextureID,
            viewportPannelSize
        );

        // Drag and drop conten Broser
        DragAndDrop();


        // Get View port AABB In Mointore Scren Space / Not App Scren Space!
        if (m_WindowHoverd)
            CalculateMausePos({ viewportOffset.x, viewportOffset.y });

        // Gizmos
        CalculateGizmo();

        ImGui::End();
        ImGui::PopStyleVar();

#if 1 
        
        if (m_FontTex != nullptr)
        {
            if(m_WindowText)
            {
                if(ImGui::Begin("Font Map", &m_WindowText))
                {
                    uint32_t shadowID = m_FontTex->GetRenderID();

                    glm::vec2 textureDisplaySize = CalculateImageWindowSize(m_FontTex);
                    ImVec2 displaySize = Utils::Convert_GLMvec2_In_ImVec2(textureDisplaySize);
                    ImTextureID imguiShadowID = reinterpret_cast<ImTextureID>(&shadowID);
                    ImVec2 uv0{ 0, 1 };
                    ImVec2 uv1{ 1, 0 };
                    ImGui::Image(
                        imguiShadowID, 
                        displaySize
                       
                    );
                }
                ImGui::End();
            }

            
        }

#ifndef RY_RENERER_DESIGN_CURENT_MAIN
        uint32_t size = Renderer::GetPassViewSepertSize("Shadow");
#else
        uint32_t size = Renderer::GetCurentIndex();
#endif
        if (m_ShadowBuffersAttechements.size() != size + 1ull)
            m_ShadowBuffersAttechements.resize(size + 1ull);

#ifndef RY_RENERER_DESIGN_CURENT_MAIN
        for(uint32_t i = 0; i < size; i++)
        {  
#else // !RY_RENERER_DESIGN_CURENT_MAIN
        uint32_t count = 0u;
        Renderer::ForEchStoredPassedRenderPassIndex(
            [this](const RenderPass& renderPass, uint32_t indexPass)
            {
                if ("Shadow" != renderPass.Name)
                    return;

                const Ref<RenderTarget>& target = renderPass.Target;
                if (nullptr == target)
                    return;

                const Ref<Framebuffer>& fb = target->GetFramebuffer();
                if (nullptr == fb)
                    return;

                Ref<Texture> tex = fb->GetDepthTexture();
                if (nullptr == tex)
                    return;

                const Ref<UniformBuffer>& ub = renderPass.CameraDataPackBufferUB;
                if (nullptr == ub)
                    return;

#endif // RY_RENERER_DESIGN_CURENT_MAIN
                AttachmentsPannel& attachmentsPannel = m_ShadowBuffersAttechements.at(indexPass);
                std::string& namePannel = attachmentsPannel.Name;
                if (namePannel == "")
                    namePannel = "Shadow FB " + std::to_string(indexPass);

                if (!attachmentsPannel.WindowAktive)
                    return;

                bool result = ImGui::Begin(namePannel.c_str(), &attachmentsPannel.WindowAktive);
                if (!result)
                {
                    ImGui::End();
                    return;
                }
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
                Ref<Framebuffer> shadowFB = Renderer::GetPassViewFramebuffer("Shadow", i);
#endif // !RY_RENERER_DESIGN_CURENT_MAIN

                const uint32_t attachmentsCount = fb->GetAttachmentTexturesSize();
                const Ref<Texture>& shadowMap = fb->GetDepthTexture();

                const int texCount = nullptr != shadowMap ? attachmentsCount : attachmentsCount - 1;

                int& index = attachmentsPannel.Index;
                if (attachmentsCount != 0)
                {
                    ImGui::SliderInt("Show Output Tex", &index, 0, texCount, "%i");
                }
                if (index < 0 || index > texCount)
                    index = 0;




                if (attachmentsCount == index && shadowMap != nullptr)
                {
                    if (m_WindowShadow)
                    {
                        if (attachmentsCount != 0)
                        {
                            std::string name = "DepthMap";
                            ImGui::Text(name.c_str());
                        }
                        uint32_t shadowID = shadowMap->GetRenderID();

                        glm::vec2 textureDisplaySize = CalculateImageWindowSize(m_FontTex);
                        ImVec2 displaySize = Utils::Convert_GLMvec2_In_ImVec2(textureDisplaySize);

                        ImTextureID textureID = reinterpret_cast<ImTextureID>(&shadowID);
                        ImVec2 uv0{ 0, 1 };
                        ImVec2 uv1{ 1, 0 };
                        ImVec4 backgroundColor{ 1.0f, 1.0f, 1.0f, 1.0f };
                        ImVec4 color{ 0.0f, 0.0f, 0.0f, 0.0f };
                        ImGui::Image(textureID, displaySize);


                    }
                }
                else if (attachmentsCount != 0)
                {
                    const Ref<Texture>& attechmentMap = fb->GetAttachmentTexture(index);
                    if (m_WindowShadow && nullptr != attechmentMap)
                    {
                        std::string name = "AttechmentMap " + std::to_string(index);
                        ImGui::Text(name.c_str());
                        uint32_t attechmentID = attechmentMap->GetRenderID();
                        ImVec2 viewportPannelShadowSize = ImGui::GetContentRegionAvail();

                        float witheTexure = shadowMap->GetWidth();
                        float heigthTexure = shadowMap->GetHeight();
                        float aspectRoation = witheTexure / heigthTexure;

                        float viewPortX = viewportPannelShadowSize.x;
                        float viewPortY = viewportPannelShadowSize.y;
                        viewPortY -= 2;

                        if (viewPortX < viewPortY)
                        {
                            viewPortX = viewPortX;
                            viewPortY = viewPortX * aspectRoation;
                        }
                        else
                        {
                            viewPortX = viewPortY * aspectRoation;
                            viewPortY = viewPortY;
                        }
                        ImTextureID textureID = reinterpret_cast<ImTextureID>(&attechmentID);
                        ImVec2 uv0{ 0, 1 };
                        ImVec2 uv1{ 1, 0 };
                        ImVec4 backgroundColor{ 1.0f, 1.0f, 1.0f, 1.0f };
                        ImVec4 textureColor{ 0.0f, 0.0f, 0.5f, 1.0f };
                        ImVec2 textureSize{
                            (shadowMap->GetWidth() / shadowMap->GetHeight()) * viewportPannelShadowSize.y,
                            viewportPannelShadowSize.y
                        };
                        ImGui::Image(textureID, textureSize);



                    }
                }


                ImGui::End();
                
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
            }
#else
            });
#endif
       

#endif

        return (m_WindowFocused && m_WindowHoverd);
    }

    void ViewPort::OpenTextureWindows()
    {
        m_WindowShadow = true;
        m_WindowText = true;
        for (AttachmentsPannel& textureWindows : m_ShadowBuffersAttechements)
        {
            textureWindows.WindowAktive = true;
        }
    }

    void ViewPort::OnRenderEditorFilterOff()
    {
        Ref<Texture> tex = m_Framebuffer->GetAttachmentTexture(m_RenderOnAtachment);
        m_FinaleImage = tex;
    }

    void ViewPort::OnRenderEditorFilter()
    {
        m_Filtering->Bind();
        m_Filtering->SetUniformValue("u_MauseInViewPixelPos", &m_MauseInViewPixelPos, ShaderDataType::Float2);
        m_SelectedFramebuffer->BindColorAttachmentImage(Acces::Read, 0, 0);
        m_Framebuffer->BindColorAttachmentImage(Acces::Read, m_RenderOnAtachment, 1);
        m_Image->BindImage(Acces::Write, 2);

        RenderCommand::DispatcheCompute({ glm::ceil<uint32_t>(m_WindowSize.x / m_ComputeGrups.x)  , glm::ceil<uint32_t>(m_WindowSize.y / m_ComputeGrups.y)  , m_ComputeGrups.z });
        m_FinaleImage = m_Image;
    }

    void ViewPort::SetNewAktiveSecen(const Ref<Scene>& scene)
    {
        m_AktiveScene = scene;
        m_AktiveScene->OnViewportResize(static_cast<uint32_t>(m_WindowSize.x), static_cast<uint32_t>(m_WindowSize.y));
    }

    void ViewPort::SetComputeInvocation(const glm::vec3& invocation)
    {
        m_ComputeGrups = invocation;
    }


    void ViewPort::OnStaicResultion(bool state)
    {
        m_StaticReslution = state;
        if (m_StaticReslution)
        {
            ResizeGameResultion(*m_StaticGameReslutionPtr);
        }
        else
        {
            ResizeGameResultion(m_WindowSize);
        }
    }

    Ref<Texture> ViewPort::GetFinalImag() const
    {
        Ref<Texture> tex = nullptr;
        switch (*m_SceneState)
        {
        case SceneState::Edit:
        {
            tex = m_FinaleImage;
            break;
        }
        case SceneState::Play:
        {
            tex = m_Framebuffer->GetAttachmentTexture(m_RenderOnAtachment);
            break;
        }
        case SceneState::Simulate:
        {
            tex = m_FinaleImage;
            break;
        }
        default:
        {
            tex = m_FinaleImage;
            break;
        }
        }
        return tex;
    }

    void ViewPort::ResizeGameResultion(const glm::vec2& size)
    {
        glm::vec2 sizeMulty = size * m_TextureScale;
        m_Camera->SetViewportSize(sizeMulty.x, sizeMulty.y);
        m_AktiveScene->OnViewportResize(sizeMulty.x, sizeMulty.y);
        m_Image->Resize2D(sizeMulty.x, sizeMulty.y);
        m_Framebuffer->Resize2D(sizeMulty.x, sizeMulty.y);
        m_SelectedFramebuffer->Resize2D(sizeMulty.x, sizeMulty.y);
        
        m_StaticResultionScreenOfset = (m_WindowSize / glm::vec2(*m_StaticGameReslutionPtr)) * sizeMulty;
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
        //bool isTabBarVisible = node ? !node->WantHiddenTabBarToggle : true;

        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 minBound = ImGui::GetWindowPos();
        minBound.x += mausOffset.x;
        minBound.y += mausOffset.y;

        ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
        m_WindowBounds[0] = { minBound.x, minBound.y };
        m_WindowBounds[1] = { maxBound.x, maxBound.y };

    }

    void ViewPort::WindowResize(const glm::vec2& windowSize)
    {
        bool result = m_WindowSize != windowSize;
        m_AktiveScene->SetWindowResize(result);

        if (!result)
            return;

        m_WindowSize = windowSize;
        if(!m_StaticReslution)
        {
            ResizeGameResultion(m_WindowSize);
            m_StaticResultionScreenOfset = {1.0f ,1.0f};
        }
        else
        {
            m_StaticResultionScreenOfset = m_WindowSize / glm::vec2(*m_StaticGameReslutionPtr);
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
#ifdef IM_GUIZMO
        Entity selectedEntity = m_EditorLayer->GetSelectedEntity();
        m_ImGizmoHovered = false;
        if (!selectedEntity || *m_GizmoType == -1)
            return;


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
                Entity camerEntt = m_AktiveScene->GetEntityPrimaryCamera();
                const CameraComponent& cameraComp = camerEntt.GetComponent<CameraComponent>();
                const Camera& camera = cameraComp.Camera;
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

        if (!m_ImGizmoHovered)
            return;

        glm::vec3 transation, rotation, scale;
        Math::DecomposeTransform(transform, transation, rotation, scale);

        glm::vec3 dealteRotation = rotation - tc.Rotation;
        tc.Transaltion = transation;
        tc.Rotation += dealteRotation;
        tc.Scale = scale;
        selectedEntity.UpdateMatrix();
#endif
        
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
            else  if (slelcted.HasComponent<ModelMangerComponent>() && slelcted.HasComponent<MaterialComponent>())
            {
                Renderer3D::BeginScene(mainCamera, viewMatrix);
                ModelMangerComponent staticMeshC = slelcted.GetComponent<ModelMangerComponent>();
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
                int parent = slelcted.GetComponent<RealtionShipComponent>().parent;
                Renderer3D::BeginScene(mainCamera, viewMatrix);
                if(parent!=-1)
                {
                    MaterialComponent materialC = Entity((entt::entity)parent, m_AktiveScene.get()).GetComponent<MaterialComponent>();
                    if (dynamicMeshC.meshR != nullptr)
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

    void ViewPort::HoveredEntity()
    {
        m_MauseInViewPixelPos = { -1.5f, -1.5f };
        static bool mouseReult = false ;
        mouseReult = m_MousButtenLeftPressed || mouseReult;

        // if (!mouseReult || !m_WindowHoverd || !m_WindowFocused || m_WindowMoving || 1u < m_TextureSampleCount)
        //     return;
        if (!mouseReult || !m_WindowHoverd || !m_WindowFocused || m_WindowMoving)
            return;
        mouseReult = false;
        float mx = m_MousePos.x;
        float my = m_MousePos.y;
        mx -= m_WindowBounds[0].x;
        my -= m_WindowBounds[0].y;
        
        glm::vec2 viewPortSize = m_WindowBounds[1] - m_WindowBounds[0];
        const glm::uvec2& fbSize = m_Framebuffer->GetFrambufferSize();

        if (static_cast<uint32_t>(viewPortSize.x) != fbSize.x || static_cast<uint32_t>(viewPortSize.y) != fbSize.y)
        {
            RY_CORE_WARN("Need to warn HoveredEntity Calceulation Not same View Port Size!");
            RY_CORE_TRACE("Viewport: ({}, {})", viewPortSize.x, viewPortSize.y);
            RY_CORE_TRACE("Frambuffer: ({}, {})", fbSize.x, fbSize.y);
            float downSetPosOffset = static_cast<float>(fbSize.y)- viewPortSize.y;
            float nextMouseY = my - downSetPosOffset;
            RY_CORE_INFO("SetMousePose {0} in y direction down! {1} - {0} = {2}", downSetPosOffset, my, nextMouseY);
            my = nextMouseY;
        }
        
        my = viewPortSize.y - my;

        if(m_StaticReslution)
        {
            mx = mx / m_StaticResultionScreenOfset.x;
            my = my / m_StaticResultionScreenOfset.y;
        }
        int mauseX = (int)(mx );
        int mauseY = (int)(my );
        m_MauseInViewPixelPos = { mx, my };
        

        if (mauseX < 0 || mauseY < 0 || mauseX >= fbSize.x || mauseY >= fbSize.y)
            return;

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
        m_Framebuffer->UnBind();

    }

    void ViewPort::HoveredEntity(const glm::vec2& mousePos)
    {
        m_MousePos = mousePos;
        HoveredEntity();
    }

#pragma endregion
}
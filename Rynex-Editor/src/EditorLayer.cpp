#include <rypch.h>
#include "EditorLayer.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Core/Input.h>
#include <Rynex/Project/Project.h>
#include <Rynex/Asset/Base/AssetManager.h>
// #include <Rynex/Asset/EditorAssetManegerThreade.h>
#include <Rynex/Utils/PlatformUtils.h>
#include <Rynex/Math/Math.h>

#include <Rynex/Serializers/SceneSerializer.h>
#include <Rynex/Serializers/TextureSerialiazer.h>
#include <Rynex/Serializers/VertexArraySerialzer.h>
#if RY_SCRIPTING_HAZEL
   #include <Rynex/Scripting/HazelScripting/ScriptEngine.h> 
#else
    #include <Rynex/Scripting/ScriptingEngine.h>
#endif
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Renderer/Rendering/Renderer2D.h>
#include <Rynex/Renderer/Rendering/Renderer3D.h>
#include <Rynex/Renderer/Materials/BasicMaterial.h>


#include <Rynex/Asset/Import/ShaderImporter.h>
#include <Rynex/Renderer/Objects/Model.h>

#include <imgui/imgui.h>
#include <ImGuizmo.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Rynex/Renderer/Text/Font.h>



namespace Rynex {

#define RY_EDITOR_TEST_ENITITY 0
#define RY_ENABLE_VIEWPORT 1
#define TEST_SHADER_OUTPUT 0


#if TEST_SHADER_OUTPUT

    namespace ShaderSim {
         
        using namespace glm;

        struct InPut_FS
        {
            vec3 Position;	// 12
            vec3 Normal;	// 24
            vec2 Coords;	// 32
        };

        struct DrirectionLigthData_FS
        {
            vec4 Color;
            vec3 Position;
            float Intensitie;
        };

        struct PointLigthData_FS
        {
            vec3 Color;
            float Distence;
            vec3 Position;
            float Intensitie;
        };

        struct SpotLigthData_FS
        {
            vec3 Color;
            float Intensitie;
            vec3 Position;
            float Distence;
            vec3 Direction;
            float Inner;
            float Outer;
        };


        struct CameraData_FS
        {
            mat4 ViewProjectionMatrix;	// 16
            mat4 ViewMatrix;			// 32
            mat4 ProjectionMatrix;		// 48
            vec3 CamerPosition;			// 60
            int Empty;					// 64
        };

        struct ModelData_FS
        {
            vec3 Color;
            float Alpha;
            float Emission;
            float Matalness;
            float Roughness;
            float Shinines;
            float Specular;

            int EntityID;
            ivec2 Empty;
            mat4 ModelMatrix;
        };

        struct AmbientLigth_FS
        {
            vec3 Color;
            float Intensitie;
        };

        struct DrirectionLigth_FS
        {
            int Aktive;
            ivec3 Empty;
            DrirectionLigthData_FS Direction[8];
        };

        struct PointLigth_FS
        {
            int Aktive;
            ivec3 Empty;
            PointLigthData_FS Point[32];
        };

        struct SpotLigth_FS
        {
            int Aktive;
            ivec3 Empty;
            SpotLigthData_FS Spot[32];
        };

        static CameraData_FS Camera = {
            {
                {1.39734, 0.00f, 0.00f, 0.00f},
                { 0.00f, 3.73205f, 0.00f, 0.00f },
                { 0.00f, 0.00f, -1.0002f, 9.80198f },
                { 0.00f, 0.00f, -1.00f, 10.00f }
            },
            {
                { 1.00f, 0.00f, 0.00f, 0.00f},
                { 0.00f, 1.00f, 0.00f, 0.00f},
                { 0.00f, 0.00f, 1.00f, -10.00f },
                { 0.00f, 0.00f, 0.00f, 1.00f }
            },
            {
                { 1.39734, 0.00f, 0.00f, 0.00f},
                { 0.00f, 3.73205f, 0.00f, 0.00f},
                { 0.00f, 0.00f, -1.0002f, -0.20002f },
                { 0.00f, 0.00f, -1.00f, 0.00f }
            },
            {
                 0.00f, 0.00f, -10.00f
            },
            -1
        };

        static ModelData_FS Model = {
            {
                1.00f, 0.00f, 1.00f
            },
            1.0f,
            0.1f,
            0.2f,
            0.3f,
            0.4f,
            0.5f,
            -1,
            {-2, -3},
            {
                { 1.00f, 0.00f, 0.00f, 0.00f },
                { 0.00f, 1.00f, 0.00f, 0.00f },
                { 0.00f, 0.00f, 1.00f, 0.00f },
                { 0.00f, 0.00f, 0.00f, 1.00f }
            }
        };

        static AmbientLigth_FS AmbientL = {
            {
                1.00f, 1.00f, 1.00f
            },
            0.1f
           
        };

        static DrirectionLigth_FS DrirectionelL = {
            1,
            {-1.0f,-2.0f,-3.0f},
            {
                {
                    { 1.00f, 1.00f, 1.00f, -13.00f },
                    { 3.00f, 3.10f, 0.00f },
                    1.00
                }
            }

        };

        // output
        static vec4 Color; 

        static InPut_FS vData = {
            {0.00f,0.00f, -1.00 },
            {0.0f, 0.0f, -1.0f },
            {0.0f,0.0f}
        };

        static const float PI = 3.14159265359f;

        static float distrbutionGXX(float NdotH, float roughness)
        {
            float a = roughness * roughness;
            float a2 = a * a;
            float demom = NdotH * NdotH * (a2 - 1.0f);
            demom = PI * demom * demom;
            return a2 / std::max(demom, 0.00000001f);
        }

        static float geomtrySmith(float NdotV, float NdotL, float roughness)
        {
            float r = roughness - 1.0f;
            float k = (r * r) / 8.0f;
            float ggx1 = NdotV * (NdotV * (1.0f - k) + k);
            float ggx2 = NdotL * (NdotL * (1.0f - k) + k);
            return ggx1 * ggx2;
        }

        static vec3 frenselSchlick(float HdotV, vec3 baseReflectivity)
        {
            return baseReflectivity + ( vec3( 1.0f) - baseReflectivity) * vec3( pow( 1.0f - HdotV, 5.0f ));
        }

        static vec3 drirectionelLigthing(vec3 N, vec3 V, vec3 baseReflectivity, float metalic, vec3 albedo)
        {
            float roughness = Model.Roughness;
            vec3 result = vec3(0.0f);
            int Aktive = DrirectionelL.Aktive;
            vec3 worldPostion = vData.Position;
            
            RY_CORE_INFO("roughness: {}", roughness);
            RY_CORE_INFO("result: {}", glm::to_string(result));
            RY_CORE_INFO("Aktive: {}", Aktive);
            RY_CORE_INFO("worldPostion: {}", glm::to_string(worldPostion));

            for (int i = 0; i < Aktive; i++)
            {
                DrirectionLigthData_FS ligthData = DrirectionelL.Direction[i];

                vec3 ligtheOffset = ligthData.Position - worldPostion;
                vec3 L = normalize(ligtheOffset);
                vec3 H = normalize(V + L);
                float distence = length(ligtheOffset);
                float attenuation = 1.0f / (distence * distence);
                vec3 radiance = vec3( ligthData.Color ) * attenuation;

                float NdotV = max( dot(N, V) , 0.0000001f);
                float NdotL = max( dot(N, L) , 0.0000001f);
                float HdotV = max( dot(H, V) , 0.0f);
                float NdotH = max( dot(N, H) , 0.0f);

                float D = distrbutionGXX(NdotH, roughness);
                float G = geomtrySmith(NdotV, NdotL, roughness);
                vec3 F = frenselSchlick(HdotV, baseReflectivity);
                RY_CORE_INFO("D: {}", D);
                RY_CORE_INFO("G: {}", G);
                RY_CORE_INFO("F: {}", glm::to_string(F));
                vec3 specular = D * G * F;
                specular /= 4.0f * NdotV * NdotL;

                vec3 kD = vec3(1.0f) - F;
                kD *= 1.0f - metalic;
                // result += ( kD * albedo / PI + specular ) * radiance * NdotL;
                result += (kD * albedo / PI + specular) * radiance * NdotL;
            }
            return result;
        }
       

        static vec3 ambientLigthing(vec3 albedo)
        {
            return (AmbientL.Color * AmbientL.Intensitie) * albedo;
        }

        static vec3 tonemapping(vec3 color)
        {
            return color / (color + vec3( 1.0f ));
        }
       
        static vec3 gamarCorrect(vec3 color)
        {
            return pow(color, vec3(1.0f / 2.2f));
        }

        static vec3 Ligth()
        {
            vec3 N = normalize(vData.Normal);
            vec3 V = normalize(Camera.CamerPosition - vData.Position);
            vec3 albedo = Model.Color;
            float metalic = Model.Matalness;
            vec3 baseReflectivity = mix(vec3(0.04f), albedo, metalic);
            vec3 Lo = vec3(0.0f);

            RY_CORE_INFO("N: {}", glm::to_string(N));
            RY_CORE_INFO("V: {}", glm::to_string(V));
            RY_CORE_INFO("albedo: {}", glm::to_string(albedo));
            RY_CORE_INFO("metalic: {}", metalic);
            RY_CORE_INFO("baseReflectivity: {}", glm::to_string(baseReflectivity));
            RY_CORE_INFO("Lo: {}", glm::to_string(Lo));

            Lo += drirectionelLigthing(N, V, baseReflectivity, metalic, albedo);
            RY_CORE_INFO("Lo: {} after drirectionelLigthing", glm::to_string(Lo));
            Lo += ambientLigthing(albedo);
            RY_CORE_INFO("Lo: {} after ambientLigthing", glm::to_string(Lo));

            return Lo;
        }

        static void main_FS()
        {
            vec3 color = vec3(0.0f);
            color += Ligth();
            RY_CORE_INFO("color after Ligth {}", glm::to_string(color));
            color = tonemapping(color);
            RY_CORE_INFO("color after tonning {}", glm::to_string(color));
            color = gamarCorrect(color);
            RY_CORE_INFO("color after gamma {}", glm::to_string(color));
            Color = vec4(color, Model.Alpha);
            RY_CORE_INFO("output color {}", glm::to_string(Color));
        }
    }

#endif

    EditorLayer::EditorLayer()
        : Layer("Rynex-Editor")
        , m_CameraController((1280.0f / 720.0f), true)
        , m_ProjectPannel("Project")
        , m_Content_BPannel()
        , m_MenuBarPannel()
        , m_RendererPannel("Renderer")
    {
        // glm::mat4 scaleMat4Test = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
        // glm::mat4 tranltionsMat4Test = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    }

    void EditorLayer::OnAttach()
    {
#if TEST_SHADER_OUTPUT

           ShaderSim::main_FS();

#endif
        
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
#if 1
            // Project::CreatNewPorject();
            if(!OpenProject())
                Application::Get().Close();
#else
            RY_CORE_INFO("Open a Project!");
            OpenProject("Sandbox.rproj");
#endif
        }

        Renderer::InitEditor();

        m_Scene_HPanel.SetContext(m_AktiveScene);

        m_Content_BPannel.OnAtache();
       
        m_EditorCamera = CreateRef<EditorCamera>(30.0f, 1.778f, 0.1, 1000.0f);



#if 0
        FramebufferSpecification fbSpec, fbSpec2;
        fbSpec.Attachments =
        {
            FramebufferTextureFormat::RGBA8,
            FramebufferTextureFormat::RGBA8,
            FramebufferTextureFormat::RED_INTEGER,
            FramebufferTextureFormat::Depth24Stencil8
        };
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);
        fbSpec2.Attachments =
        {
            FramebufferTextureFormat::RGBA8,
            FramebufferTextureFormat::Depth24Stencil8
        };
        fbSpec2.Width = 1280;
        fbSpec2.Height = 720;
        m_SelectedFramebuffer = Framebuffer::Create(fbSpec2);
        m_CallFace = CallFace::None;

        m_Filtering = AssetManager::GetAsset<Shader>("../Rynex-Editor/Editor-Assets/shaders/Compute.glsl");
        m_FinaleImage = Texture::Create({
            ImageFormat::RGBA8, TextureTarget::Texture2D,
            1280, 720,
            TextureFilteringMode::Nearest,
        });
#endif

#if RY_ENABLE_VIEWPORT
        m_ViewPortPannel = ViewPortPannel();
        m_ViewPortPannel.OnAttache("Main ViewPort", this);
#endif

#if RY_EDITOR_TEST_ENITITY


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
#if 0
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
                //material.Shader = AssetManager::GetAsset<Shader>("../Rynex-Editor/Editor-Assets/shaders/3DTestTess.glsl");
                material.Shader = AssetManager::GetAsset<Shader>("../Rynex-Editor/Editor-Assets/shaders/3DLigthe.glsl");
                material.UniformLayoute.clear();
               
                UniformElement ellement;
                {
                    ellement.Name = "u_Color";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("vec3");
                    ellement.ShResourceType = ShaderResourceType::LocalColor;
                    material.UniformLayoute.push_back(ellement);
                }
                
                {
                    ellement.Name = "u_Model";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("mat4");
                    ellement.ShResourceType = ShaderResourceType::LocalModel;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_ViewProj";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("mat4");
                    ellement.ShResourceType = ShaderResourceType::MainCameraViewProjectionMatrix;
                    material.UniformLayoute.push_back(ellement);

                }

                {
                    ellement.Name = "u_EntityID";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("int");
                    ellement.ShResourceType = ShaderResourceType::EnitiyID;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_CamerPos";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("vec3");
                    ellement.ShResourceType = ShaderResourceType::MainCamerPos;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_LigthPos";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("vec3");
                    ellement.ShResourceType = ShaderResourceType::None;
                    ellement.GloblelResurce = false;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_LigthColor";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("vec3");
                    ellement.ShResourceType = ShaderResourceType::None;
                    ellement.GloblelResurce = false;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_Shinines";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("float");
                    ellement.ShResourceType = ShaderResourceType::None;
                    ellement.GloblelResurce = false;
                    material.UniformLayoute.push_back(ellement);
                }

                {
                    ellement.Name = "u_Specular";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("float");
                    ellement.ShResourceType = ShaderResourceType::None;
                    ellement.GloblelResurce = false;
                    material.UniformLayoute.push_back(ellement);
                    
                }

                {
                    ellement.Name = "u_Ambient";
                    ellement.Type = BufferAPI::GetShaderDataTypeFromString("float");
                    ellement.ShResourceType = ShaderResourceType::None;
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

#if 1
            if (!entiy.HasComponent<GeomtryComponent>())
            {
                entiy.AddComponent<GeomtryComponent>();
                GeomtryComponent& geomtry = entiy.GetComponent<GeomtryComponent>();
                geomtry.Geometry = VertexArray::Create();
                { 
                    Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(32 * 18);
                    Geomtrys::SetCubeVertex(geomtry.Geometry, vertexBuffer);
                    Geomtrys::SetCubeIndex(geomtry.Geometry);
                    
                }
                geomtry.Geometry->SetBoxAABB({ ShaderDataType::Float3, "a_Postion" });
            }
#endif
#if 0
            if (!entiy.HasComponent<ScriptComponent>())
            {
                entiy.AddComponent<ScriptComponent>();
                auto& script = entiy.GetComponent<ScriptComponent>();
                script.Name = "Sandbox.Player";
            }
#endif
#if 0
            if (!entiy.HasComponent<MeshComponent>())
            {
                entiy.AddComponent<MeshComponent>();
                MeshComponent& meshC = entiy.GetComponent<MeshComponent>();
                //CreateRef<Model>("../Rynex-Editor/Editor-Assets/Models/Cube.gltf");

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
            metadata.FilePath = "../Rynex-Editor/Editor-Assets/CheckeBordNearest.rytex2d";
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
            metadata.FilePath = "../Rynex-Editor/Editor-Assets/CheckeBordLinear.rytex2d";
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

#if 1
        {
            Entity ambinet = m_AktiveScene->CreateEntity("3D_TestAmbient");
            if (!ambinet.HasComponent<AmbientLigthComponent>())
                ambinet.AddComponent<AmbientLigthComponent>();
            AmbientLigthComponent& ambienTC = ambinet.GetComponent<AmbientLigthComponent>();
            ambienTC.Color = { 0.0,0.0,0.0 };
            ambienTC.Intensitie = 0.0;
        }

        {
            Entity directionE = m_AktiveScene->CreateEntity("DrirektionleLigth");
            if (!directionE.HasComponent<DrirektionleLigthComponent>())
                directionE.AddComponent<DrirektionleLigthComponent>();
            DrirektionleLigthComponent& directionC = directionE.GetComponent<DrirektionleLigthComponent>();
            directionC.Color = { 1.0,1.0,1.0 };
            directionC.Intensitie = 1.0;
#if 0
            directionC.ShadowFrameBuffer = Framebuffer::Create({
                512, 512,
                    {
                        {
                            TextureFormat::DepthComp24,
                            1,
                            {
                                TextureWrappingMode::ClampEdge,
                                TextureWrappingMode::ClampEdge,
                                TextureWrappingMode::None,
                            },
                            TextureFilteringMode::Nearest
                        }
                    },
                    1,
                    false
            });
#endif


            if (!directionE.HasComponent<ModelMatrixComponent>())
                directionE.AddComponent<ModelMatrixComponent>();
            ModelMatrixComponent& mat4C = directionE.GetComponent<ModelMatrixComponent>();
            glm::mat4 ligthView = glm::lookAt(1.0f * glm::vec3(50.0f, 00.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0f));
            mat4C.Globle = ligthView;
            mat4C.Locale = ligthView;

            if (!directionE.HasComponent<TransformComponent>())
                directionE.AddComponent<TransformComponent>();
            TransformComponent& tranC = directionE.GetComponent<TransformComponent>();
            tranC.SetTransform(ligthView);

            directionE.UpdateMatrix();
        }
#if 1

        {
            Entity modelE = m_AktiveScene->CreateEntity("Sreene");
            if (!modelE.HasComponent<MeshComponent>())
                modelE.AddComponent<MeshComponent>();
            MeshComponent& modelC = modelE.GetComponent<MeshComponent>();

            if (!modelE.HasComponent<StaticMeshComponent>())
                modelE.AddComponent<StaticMeshComponent>();
            StaticMeshComponent& modelStaicC = modelE.GetComponent<StaticMeshComponent>();
            std::vector<MeshTexture> meshTex;
            std::vector<uint32_t> meshIndex;
            {
                meshIndex.reserve(3);
                meshIndex.emplace_back(0);
                meshIndex.emplace_back(1);
                meshIndex.emplace_back(2);

                meshIndex.emplace_back(0);
                meshIndex.emplace_back(2);
                meshIndex.emplace_back(3);
            }
            
         
            std::vector<MeshVertex> meshVertex;
            {
                meshVertex.reserve(4);
                meshVertex.emplace_back(MeshVertex(glm::vec3(-1.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
                meshVertex.emplace_back(MeshVertex(glm::vec3(-1.0f, -1.0f,  0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
                meshVertex.emplace_back(MeshVertex(glm::vec3 (1.0f, -1.0f,  0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
                meshVertex.emplace_back(MeshVertex(glm::vec3( 1.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
            }
#if RY_MODEL_NODE
            std::vector<Ref<Mesh>> mesh;

            mesh.push_back(CreateRef<Mesh>(std::move(meshVertex), std::move(meshIndex), std::move(meshTex), false));
            std::vector<NodeData> nodeRoot;
            nodeRoot.reserve(1);
            NodeData nodeData = { mesh, glm::mat4(1.0f), "Sreene", std::vector<int>(), -1 };
            nodeRoot.emplace_back(nodeData);
#else
            modelStaicC.UseAlleMeshes = true;

            std::vector<Ref<Mesh>> mesh;
            std::vector<MeshRootData> meshRootData;
            meshRootData.reserve(1);
            meshRootData.emplace_back(MeshRootData(glm::mat4(1.0f), std::string("Sreene")));
            
            mesh.push_back(CreateRef<Mesh>(std::move(meshVertex), std::move(meshIndex), std::move(meshTex), false));


            modelStaicC.UseAlleMeshes = true;
            modelStaicC.ModelR = CreateRef<Model>(mesh, meshRootData);
            modelC.ModelR = modelStaicC.ModelR;
#endif
        }
#endif
#endif
#endif
        
        m_RendererPannel.OnAttache(this);
        m_MenuBarPannel.OnAttache(this);
        m_ProjectPannel.OnAttache(this);
        RY_CORE_INFO("EditorLayer::Sucese Finished!");
    }

    void EditorLayer::OnDetach()
    {
        RY_CORE_WARN("OnDetach Aktiv!");
        RY_PROFILE_FUNCTION();
        
       
        Renderer::ShutdownEditor();
        Renderer::Shutdown();
        ScriptingEngine::Shutdown();
        Project::ShutDown();
        m_RendererPannel.OnDetache();
        m_MenuBarPannel.OnDetache();
        m_ProjectPannel.OnDetache();
        m_ViewPortPannel.OnDetache();
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
                    m_AktiveScene->OnRuntimStop();
                else if (m_SceneState == SceneState::Simulate)
                    m_AktiveScene->OnRuntimStop();

                m_SceneState = SceneState::Edit;
            }
            Ref<Scene> newScene = Scene::Copy(m_NextScene);

           
            m_EditorScene = newScene;
            m_AktiveScene = newScene;

            m_Scene_HPanel.SetContext(m_AktiveScene);
            m_EditorScenePath = Project::GetActive()->GetEditorAssetManger()->GetMetadata(m_NextScene->Handle).FilePath;
            m_ViewPortPannel.SetNewAktiveSecen(m_AktiveScene);

            m_NextScene = nullptr;
        }

        Renderer2D::ResetQuadeStats();

        switch (m_SceneState)
        {
            case SceneState::Edit:
            { 

                m_AktiveScene->OnUpdateEditor(ts);   
                m_ViewPortPannel.OnRenderEditor(ts);
                break;
            }
            case SceneState::Simulate:
            {
                m_EditorCamera->OnUpdate(ts);
                m_AktiveScene->OnUpdateSimulation(ts);
                m_ViewPortPannel.OnRenderSimultion();;
                break;
            }
            case SceneState::Play:
            {
                m_AktiveScene->OnUpdateRuntime(ts);
                m_ViewPortPannel.OnRenderRuntime(0);
                break;
            }
        }
        
        ////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////
        m_RendererPannel.OnUpdate(ts);
        m_ViewPortPannel.OnUpdate();
    }

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

        default:
            break;
        }

    }

    bool EditorLayer::OnMousePressed(MouseButtenPressedEvent& e)
    {
        m_ViewPortPannel.OnMousPressed(e);
        return true;
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
        m_Scene_HPanel.OpenSceneHierachy();
    }

    void EditorLayer::OpenPropertiesPannel()
    {
        m_Scene_HPanel.OpenProperties();
    }

    void EditorLayer::OpenProjectPannel()
    {
        m_ProjectPannel.OpenWindow();
    }

#pragma endregion

#pragma region ProjectFile

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
#if RY_SCRIPTING_HAZEL
            ScriptEngine::Init();
#else
            if(!ScriptingEngine::IsInit())
                ScriptingEngine::Init(true);
            

#endif
            m_Project = Project::GetActive();
            m_AssetManger = m_Project->GetEditorAssetManger();
            Renderer::Init();
            std::filesystem::path startScene = Project::GetActive()->GetConfig().StartScene;
            if (startScene.string() != "")
            {
                OpenScene(m_AssetManger->GetAssetHandle(startScene));
                // OpenScene(startScene);
            }
            
            m_Content_BPannel = ContentBrowserPannel();
            
        }
    }

    void EditorLayer::SaveProject()
    {
        if(m_Project && m_Project->GetConfig().ProjectRady)
        {
            m_Project->SaveActive(m_Project->GetConfig().ProjectPath / (m_Project->GetConfig().Name + ".rproj"));
        }
    }

#pragma endregion

#pragma region SceneFile

    void EditorLayer::NewScene()
    {
        m_AktiveScene = CreateRef<Scene>(); 
        m_ViewPortPannel.SetNewAktiveSecen(m_AktiveScene);
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
        m_ViewPortPannel.SetNewAktiveSecen(m_AktiveScene);
        SceneSerializer serialzer(m_AktiveScene);     
        serialzer.Deserialize(path.string(), false);  

        
        m_Scene_HPanel.SetContext(m_AktiveScene);
      
    }

    void EditorLayer::OpenScene(AssetHandle handle)
    {
        RY_CORE_ASSERT(handle, "Error: EditorLayer::OpenScene(AssetHandle handle)");

#if RY_EDITOR_ASSETMANGER_THREADE ? 1 : 0
        
        AssetManager::GetAssetAsync<Scene>(handle, &m_NextScene);

#else
        if (m_SceneState != SceneState::Edit)
        {
            RY_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate, "Error Futer Funktion: EditorLayer::OnSceneStop()");
           
            if (m_SceneState == SceneState::Play)
                m_AktiveScene->OnRuntimStop();
            else if (m_SceneState == SceneState::Simulate)
                m_AktiveScene->OnRuntimStop();

            m_SceneState = SceneState::Edit;
        }

        Ref<Scene> readOnlyScene = AssetManager::GetAsset<Scene>(handle);
        Ref<Scene> newScene = Scene::Copy(readOnlyScene);
        m_EditorScene = newScene;
        m_AktiveScene = m_EditorScene;
        
        m_Scene_HPanel.SetContext(m_EditorScene);
        m_EditorScenePath = Project::GetActive()->GetEditorAssetManger()->GetMetadata(handle).FilePath;    
        m_ViewPortPannel.SetNewAktiveSecen(m_AktiveScene);
#endif

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
            serialzer.Serialize(filepath);
        }
    }

    void EditorLayer::SaveCurentScene()
    {
        AssetHandle handle = m_AktiveScene->Handle;
        if (m_AssetManger->IsAssetHandleValid(handle))
        {
#if RY_EDITOR_ASSETMANGER_THREADE
            const AssetMetadata metadata = m_AssetManger->GetMetadata(handle);
#else
            const AssetMetadata& metadata = m_AssetManger->GetMetadataConst(handle);
#endif
            SceneSerializer serialzer(m_AktiveScene);
            serialzer.Serialize(metadata.FilePath.string());
        }
        else
        {
            SaveSceneAs();
        }
    }

#pragma endregion

    void EditorLayer::OnImGuiRender()
    {
        static bool dokingEnabled   = true;
        static bool assetsEnabled   = true;
        static bool sceneEnabled    = true;
        static bool viewPortEnabled = false;
        static bool settingsEnabled = true;
        static bool renderPannnel = false;
        static bool menuBarPannnel = false;

        if (dokingEnabled) 
        {

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
                ImGuiTopTaskBar();
            }
            else
            {
                m_MenuBarPannel.OnImGuiRender();
            }
            if(sceneEnabled)    ImGuiPannels();
            if(settingsEnabled) ImGuiSettings(renderPannnel);
#if RY_VIEW_PORT_FUNKTION   
            if(viewPortEnabled) 
            {
                ImGuiViewPort();
            }
        
            else
            {

                ViewPortPannel::SetEventBlocker(!m_ViewPortPannel.OnImGuiRender());

            }
#else
            if (!viewPortEnabled)
            {
                ViewPortPannel::SetEventBlocker(!m_ViewPortPannel.OnImGuiRender());
            }
#endif
            if (!renderPannnel)
            {
                m_RendererPannel.OnImGuiRender();
            }
            m_ProjectPannel.OnImGuiRender();
            ImGui::End();
        }
        else
        {   
            if(sceneEnabled)    ImGuiPannels();
            if(settingsEnabled) ImGuiSettings(renderPannnel);
#if RY_VIEW_PORT_FUNKTION
            if(viewPortEnabled) ImGuiViewPort();
#endif
            if(!renderPannnel)
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
                {
                    m_SceneState = SceneState::Edit;
                    m_AktiveScene->OnRuntimStop();
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
            if (ImGui::Button("Pause", ImVec2(50, 0)))
            {
                m_SceneState = SceneState::Edit;
                m_AktiveScene->OnRuntimStop();
            }
        }
    }

    void EditorLayer::ImGuiRenderInfo()
    { 
#if RY_OLD_RENDER_SYSTEM
         
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
        auto stats = Renderer2D::GetQuadeStats();
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quad   : %d", stats.QuadCount);
        ImGui::Text("Vertex : %d", stats.GetTotalVertexCount());
        ImGui::Text("Indexs : %d", stats.GetTotalIndexCount());
#else
#endif
    }

   

    void EditorLayer::ImGuiPannels()
    {
        m_Scene_HPanel.OnImGuiRender();
        m_Content_BPannel.OnImGuiRender();
    }

#if RY_VIEW_PORT_FUNKTION 
    
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
                    auto camerEntt = m_AktiveScene->GetEntityPrimaryCamera();
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
                Renderer2D::BeginSceneQuade(mainCamera, viewMatrix);
                SpriteRendererComponent spriteC = slelcted.GetComponent<SpriteRendererComponent>();
                Renderer2D::DrawSprite(transformC.GetTransform(), spriteC, slelcted.GetEntityHandle());
                Renderer2D::EndSceneQuade();
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
                Renderer2D::BeginSceneQuade(mainCamera, viewMatrix);
                SpriteRendererComponent spriteC = hovered.GetComponent<SpriteRendererComponent>();
                Renderer2D::DrawSprite(transformC.GetTransform(), spriteC, hovered.GetEntityHandle());
                Renderer2D::EndSceneQuade();
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
#endif

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
#if RY_SCRIPTING_HAZEL
                ScriptEngine::ReloadAssembly();
#else
                ScriptingEngine::ReloadAssambly();
#endif

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

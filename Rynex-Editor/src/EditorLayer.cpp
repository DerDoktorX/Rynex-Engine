#include <rypch.h>
#include "EditorLayer.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Core/Input.h>
#include <Rynex/Project/Project.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Utils/PlatformUtils.h>
#include <Rynex/Math/Math.h>

#include <Rynex/Serializers/SceneSerializer.h>
#if defined(RY_SCRIPT_ENGINE)
    #include <Rynex/Scripting/Mono/ScriptingEngine.h>
#endif

#include <Rynex/Renderer/Materials/Material.h>
#include <Rynex/Renderer/Rendering/Renderer.h>

#include <Rynex/Asset/Import/ShaderImporter.h>

#include <imgui.h>

#ifdef RY_IM_GUIZMO
#include <ImGuizmo.h>
#endif // IM_GUIZMO


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Rynex/Renderer/Text/Font.h>
#include <Rynex/Renderer/Mesh/MeshSource.h>
#include <Rynex/Renderer/Rendering/Render3D/IndirectDrawMap.h>
#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Core/Vector.h>

#include <Rynex/Serializers/StaticMeshSerialzation.h>


#include <Rynex/Core/VectorMapElementRef.h>
#include <Rynex/Renderer/PiplineObjects/Piplines/PiplineBase.h>

#include <type_traits>
#include <stdexcept>
#include <random>
#include <Rynex/Asset/Import/TextureImporter.h>
#include <Rynex/Renderer/Rendering/DrawContext.h>
#include <Rynex/Renderer/ResoureManger/StagingSlot.h>




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

#pragma region TestFunctions



#pragma region TestRenderPipline

    static void CreateStaticMeshEntity(const std::string& name, Ref<Scene> scene, const glm::mat4& matrix, Ref<MeshStatic> meshStatic)
    {
        Entity entiy = scene->CreateEntity(name);
        ModelMangerComponent& staticMesh = entiy.AddComponent<ModelMangerComponent>();
        staticMesh.meshStatic = meshStatic;

        TransformComponent& transC = entiy.GetComponent<TransformComponent>();
        transC.SetTransform(matrix);
        glm::mat4 mat = transC.GetTransform();


        for (uint32_t x = 0; x < 4; x++)
        {
            for (uint32_t y = 0; y < 4; y++)
            {
                const float& checkMat = mat[x][y];
                const float& checkMatrix = matrix[x][y];
                RY_CORE_ASSERT(checkMat == checkMatrix);
            }
        }
        entiy.UpdateMatrix();
#if 1
        const UUID& meshSourceEntityUUID = entiy.GetUUID();
        std::vector<UUID>& meshSingleChildrenVec = staticMesh.singleMeshes;
        const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = meshStatic->GetSingleObjectMesDataVec();
        meshSingleChildrenVec.reserve(meshSingleVec.size());
#if 0
        uint32_t i = 0;
        uint32_t countFormTo = 5;
        uint32_t form = 18;
        uint32_t to = form + countFormTo;

        for(const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
        {

            if (i == to)
            {
                break;
            } 
            else if(i < form)
            {
                i++;
                continue;
            }
            i++;
#else
        for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
        {
#endif
            const Ref<Material>& materiel = meshSingle._Material;
            const Ref<MeshSingle>& meshSingel = meshSingle._MeshSingle;
            const glm::mat4& matrix = meshSingle.LocaleCildrenMatrix;
            const std::string& name = meshSingle.NodeName;

            Entity e = entiy.AddChildrenEntity(name);
            meshSingleChildrenVec.emplace_back(e.GetUUID());
            StaticMeshComponent& singleStaticMesh = 
                e.AddComponent<StaticMeshComponent>(
                    meshSourceEntityUUID
                    , meshSingel, materiel
                );

            TransformComponent& transMeshChildeC = e.GetComponent<TransformComponent>();
            transMeshChildeC.SetTransform(matrix);
            e.SetVisable(false);
            e.UpdateMatrix();
        }
#endif
    }

    static void CreateStaticMeshEntity(const std::string& name, Entity& e, const glm::mat4& matrix, Ref<MeshStatic> meshStatic)
    {
        Entity entiy = e.AddChildrenEntity(name);
        ModelMangerComponent& staticMesh = entiy.AddComponent<ModelMangerComponent>();
        staticMesh.meshStatic = meshStatic;

        TransformComponent& transC = entiy.GetComponent<TransformComponent>();
        transC.SetTransform(matrix);
        glm::mat4 mat = transC.GetTransform();


        for (uint32_t x = 0; x < 4; x++)
        {
            for (uint32_t y = 0; y < 4; y++)
            {
                const float& checkMat = mat[x][y];
                const float& checkMatrix = matrix[x][y];
                RY_CORE_ASSERT(checkMat == checkMatrix);
            }
        }
        entiy.UpdateMatrix();
#if 1
        const UUID& meshSourceEntityUUID = entiy.GetUUID();
        std::vector<UUID>& meshSingleChildrenVec = staticMesh.singleMeshes;
        const std::vector<MeshStatic::SingleObjectMeshData>& meshSingleVec = meshStatic->GetSingleObjectMesDataVec();
        meshSingleChildrenVec.reserve(meshSingleVec.size());

        for (const MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
        {
            const Ref<Material>& materiel = meshSingle._Material;
            const Ref<MeshSingle>& meshSingel = meshSingle._MeshSingle;
            const glm::mat4& matrix = meshSingle.LocaleCildrenMatrix;
            const std::string& name = meshSingle.NodeName;

            Entity e = entiy.AddChildrenEntity(name);
            meshSingleChildrenVec.emplace_back(e.GetUUID());
            StaticMeshComponent& singleStaticMesh =
                e.AddComponent<StaticMeshComponent>(
                    meshSourceEntityUUID
                    , meshSingel, materiel
                );

            TransformComponent& transMeshChildeC = e.GetComponent<TransformComponent>();
            transMeshChildeC.SetTransform(matrix);
            e.SetVisable(false);
            e.UpdateMatrix();
        }
#endif
    }


    static void TestProfileRenderShaderMapSubmit(Ref<MeshStatic> cube, Ref<MeshStatic> cube2, Ref<MeshStatic> ship, Ref<MeshStatic> cv, Ref<MeshStatic> sponzer, Ref<MeshStatic> sponzer2, Ref<Scene> scene)
    {
        glm::mat4 matrixShip1 = glm::mat4(
            2.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 2.5f, 0.0f,
            7.0f, 0.0f, 0.0f, 1.0f
        );
        glm::mat4 matrixShip2 = glm::mat4(
            0.1f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.1f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.1f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        glm::mat4 matrixShip3 = glm::mat4(
            2.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 2.5f, 0.0f,
            -15.0f, 0.0f, 0.0f,1.0f
        );
        glm::mat4 matrix1 = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        glm::mat4 matrix2 = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.25f, 0.0f, 1.0f
        );
        glm::mat4 matrixPlane = glm::mat4(
           15.0f,  0.0f, 0.0f, 0.0f,
            0.0f, 0.05f, 0.0f, 0.0f,
            0.0f,  0.0f,15.0f, 0.0f,
            0.0f, -1.0f, 0.0f, 1.0f
        );

        Ref<Shader> shader = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShader.glsl"));

        glm::vec3 up;
        up = glm::vec3(0.0f, 0.0f, -1.0f);
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 position;
        position = glm::vec3(-10.2f, 5.2f, 12.0f);
        // position = glm::vec3(0.0f, 7.5f, 0.0f);
        glm::vec3 direction = glm::normalize(position);
        position += direction * 2.0f;

        glm::vec3 scale = glm::vec3(1.0f);
        float size = 20.f;
        float nearClip = -5.0f;
        float farClip = 45.0f;
        float pixelSize = 2048 * 1.0f;
      
        // glm::mat4 view = glm::lookAt(position, center, up);

        glm::mat4 view = glm::lookAt(position, center, up);
        glm::mat4 projection;
        projection = glm::ortho(-size, size, -size, size, nearClip, farClip);
        float asspect = size / size; 
        // projection = glm::perspective(glm::radians(45.0f), asspect, nearClip, farClip);


        // glm::mat4 projection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, 0.0f, 100.f);
        glm::mat4 projectionView = projection * view;
        glm::mat4 inverseProjectionView = glm::inverse(projectionView);

        glm::vec4 background = glm::vec4( 0.0f, 0.0f,  0.0f,  1.0f );

        float pixelMultyPlyer = 75.0f;
        glm::vec4 viewSpace = glm::vec4(0.0f, 0.0f, pixelSize, pixelSize);
        int mode = RenderMode::Death_Buffer | RenderMode::CallFace_Front | RenderMode::A_Buffer;
#if 1
        glm::vec4 viewSpaceComplet = glm::vec4(0.0f, 0.0f, viewSpace.z , viewSpace.w);
#else
        glm::vec4 viewSpaceComplet = glm::vec4(0.0f, 0.0f, viewSpace.z * RY_SHADOW_COUNT, viewSpace.w);
#endif
        uint32_t withe = viewSpaceComplet.z;
        uint32_t heigth = viewSpaceComplet.w;

        FramebufferSpecification fbSpec = {
            withe, heigth, 1u,
            {
                { TexFrom::DepthComp24, 1, { TexWarp::ClampEdge, TexWarp::ClampEdge, TexWarp::ClampEdge }, TexFilter::Linear, TexComp::Lequal }
            }
        };
        
        glm::vec4 viewSpaceImgageSize = viewSpace;
        for (uint32_t i = 0; i < RY_SHADOW_COUNT; i++)
        {
#if 1
            Ref<Framebuffer> fb = Framebuffer::Create(fbSpec);
            fb->Resize2D(viewSpace.z, viewSpace.w);
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
            uint32_t index = Renderer::SetPassView(
                "Shadow",
                ViewPassData( projection, view, 
                position, background, viewSpaceComplet, mode, 2.2f, fb), ViewPassType::Shadow);
#elif 1

            Entity entity = scene->CreateEntity("Directionel");
            CameraComponent& camnerC = entity.AddComponent<CameraComponent>();
            camnerC.Camera.SetOrthoGrafic(size, nearClip, farClip);
            camnerC.Primary = false;
            RenderTargetComponent& renderTargetC = entity.AddComponent<RenderTargetComponent>();
            ModelMatrixComponent& modelC = entity.GetComponent<ModelMatrixComponent>();
            modelC.Locale = glm::inverse(view);
            
            renderTargetC.Target = CreateRef<RenderTarget>(fb);
            const glm::uvec2& size = fb->GetFrambufferSize();
            glm::ivec2 sizeInt = static_cast<glm::ivec2>(size);
            glm::ivec4 viewSize{ sizeInt.x, sizeInt.y, 0, 0, };
            
            renderTargetC.RenderPassName = "Shadow";
            renderTargetC.StroeIndex = 0xFFFFFFFFu;
            entity.UpadteTransformFromMatrix();
            entity.UpdateMatrix();
#endif // !RY_RENERER_DESIGN_CURENT_MAIN
            
#else
            uint32_t index = Renderer::SetPassView(
                ViewPassData{ projection, view, projectionView, inverseProjectionView,
                position,  background,  viewSpaceImgageSize,  mode,  fb }, ViewPassType::Shadow);
            viewSpaceImgageSize.x += viewSpace.z;
#endif
        }
#ifdef RY_RENERER_DESIGN_CURENT_MAIN
        cube = Mesh::CreateStaticMesh("Assets/Models/Cube.gltf");

        CreateStaticMeshEntity("Cube", scene, matrix2, cube);
#endif // !RY_RENERER_DESIGN_CURENT_MAIN

#if TEST_SCENE_STATE_00
        Renderer3D::SubmitMeshObject(ship, shader, matrixShip1,1);
        Renderer3D::SubmitMeshObject(cube, shader, glm::translate(matrix2, glm::vec3(1.0f, 2.5f, 1.0f)), 2);
        Renderer3D::SubmitMeshObject(cube, shader, matrix2, 3);

        Renderer3D::SubmitMeshObject(cube, shader, matrix1, 3);
        Renderer3D::SubmitMeshObject(ship, shader, matrixShip3, 4);
        Renderer3D::SubmitMeshObject(cv, shader, glm::translate(matrixShip2, glm::vec3(-9.0f,  0.75f,  3.0f) * glm::vec3(10.0f)), 5);
        Renderer3D::SubmitMeshObject(cv, shader, glm::translate(matrixShip2, glm::vec3( 9.0f, -0.75f,  3.0f) * glm::vec3(10.0f)), 6);
        Renderer3D::SubmitMeshObject(cv, shader, glm::translate(matrixShip2, glm::vec3(-5.0f, -0.75f, -3.0f) * glm::vec3(10.0f)), 7);
        Renderer3D::SubmitMeshObject(ship, shader, glm::translate(matrixShip1, glm::vec3(-10.0f,  0.5f,  7.5f) / glm::vec3(2.5f)), 8);
        Renderer3D::SubmitMeshObject(ship, shader, glm::translate(matrixShip1, glm::vec3( 10.0f, -0.5f, 0.0f)  / glm::vec3(2.5f)), 9);
        Renderer3D::SubmitMeshObject(ship, shader, glm::translate(matrixShip1, glm::vec3( 10.0f, 0.0f, -7.5f)  / glm::vec3(2.5f)), 10);
        Renderer3D::SubmitMeshObject(cube2, shader, glm::translate(matrix2, glm::vec3(-15.0f, 5.0f, 0.0f)), 11);
        Renderer3D::SubmitMeshObject(cube2, shader, glm::translate(matrix2, glm::vec3( 15.0f, 0.0f, 7.5f)), 12);
       

        Renderer3D::SubmitMeshObject(cube, shader, matrixPlane, 13);
#elif TEST_SCENE_STATE_01
        Renderer3D::SubmitMeshObject(sponzer2, shader, glm::scale(matrix1, glm::vec3(2.75f)), 1);
#elif TEST_SCENE_STATE_02
        Renderer3D::SubmitMeshObject(sponzer, shader, glm::scale(matrix1, glm::vec3(2.75f)), 1);
#elif TEST_SCENE_STATE_03
        Renderer3D::SubmitMeshObject(sponzer, shader, glm::scale(matrix1, glm::vec3(3.0f)), 1);
        Renderer3D::SubmitMeshObject(sponzer2, shader, glm::scale(matrix1, glm::vec3(3.0f)), 2);
#elif TEST_SCENE_STATE_04
        CreateStaticMeshEntity("ship 1", scene, matrixShip1, ship);
        CreateStaticMeshEntity("cube 1", scene, glm::translate(matrix2, glm::vec3(1.0f, 2.5f, 1.0f)), cube);
        CreateStaticMeshEntity("cube 2", scene, matrix2, cube);

        CreateStaticMeshEntity("cube 3", scene, matrix1, cube);
        CreateStaticMeshEntity("ship 2", scene, matrixShip3, ship);
        CreateStaticMeshEntity("cv 1", scene, glm::translate(matrixShip2, glm::vec3(-9.0f,  0.75f,  3.0f) * glm::vec3(10.0f)), cv);
        CreateStaticMeshEntity("cv 2", scene, glm::translate(matrixShip2, glm::vec3( 9.0f, -0.75f,  3.0f) * glm::vec3(10.0f)), cv);
        CreateStaticMeshEntity("cv 3", scene, glm::translate(matrixShip2, glm::vec3(-5.0f, -0.75f, -3.0f) * glm::vec3(10.0f)), cv);
        CreateStaticMeshEntity("ship 3", scene, glm::translate(matrixShip1, glm::vec3(-10.0f, 0.5f, 7.5f) / glm::vec3(2.5f)), ship);
        CreateStaticMeshEntity("ship 4", scene, glm::translate(matrixShip1, glm::vec3(10.0f, -0.5f, 0.0f) / glm::vec3(2.5f)), ship);
        CreateStaticMeshEntity("ship 5", scene, glm::translate(matrixShip1, glm::vec3(10.0f, 0.0f, -7.5f) / glm::vec3(2.5f)), ship);
        CreateStaticMeshEntity("cube2 1", scene, glm::translate(matrix2, glm::vec3(-15.0f, 5.0f, 0.0f)), cube2);
        CreateStaticMeshEntity("cube2 2", scene, glm::translate(matrix2, glm::vec3(15.0f, 0.0f, 7.5f)), cube2);

        CreateStaticMeshEntity("cube 4", scene, matrixPlane, cube);
#elif TEST_SCENE_STATE_05
        CreateStaticMeshEntity("cube", scene, glm::translate(matrix2, glm::vec3(1.0f, 2.5f, 1.0f)), cube);
#elif TEST_SCENE_STATE_06
        CreateStaticMeshEntity("cube2", scene, glm::translate(matrix2, glm::vec3(1.0f, 2.5f, 1.0f)), cube2);
#elif TEST_SCENE_STATE_07
        CreateStaticMeshEntity("sponzer2", scene, glm::scale(matrix1, glm::vec3(5.0f)), sponzer2);
#elif TEST_SCENE_STATE_08
        CreateStaticMeshEntity("sponzer", scene, glm::scale(matrix1, glm::vec3(5.0f)), sponzer);
#elif TEST_SCENE_STATE_09
        CreateStaticMeshEntity("sponzer", scene, glm::scale(matrix1, glm::vec3(5.0f)), sponzer);
        CreateStaticMeshEntity("sponzer2", scene, glm::scale(matrix1, glm::vec3(5.0f)), sponzer2);
#elif TEST_SCENE_STATE_10
        Ref<MeshSource> cubeSource = cube->GetMeshSource();
        std::vector<MeshStatic::SingleObjectMeshData> singleMeshDatasVec = cube->GetSingleObjectMesDataVec();
        Ref<Material> materiel = singleMeshDatasVec.at(0)._Material;
        MaterielShaderData data = Material::GetMaterielDataFromMateriel<MaterielShaderData>(materiel);
        data.AmbientLigthe = 0.15f;
        data.Color = glm::vec3(0.11f, data.Color.g, 0.11f);
        Ref<Texture> tex = materiel->GetAlbedoTextures();
        singleMeshDatasVec.at(0)._Material = CreateRef<DefaultMaterial>(data, tex);
        Ref<MeshStatic> cubePlane = CreateRef<MeshStatic>(cubeSource, singleMeshDatasVec);
        CreateStaticMeshEntity("Plane", scene, matrixPlane, cubePlane);
        float multiyplyerCube = 2.5f;
        float quda = 25.f;
        float qudaHalf = quda / 2.0f;
        for(float x = 0; x < quda; x++)
        {
            Entity rowEntity = scene->CreateEntity("Colume (" + std::to_string(static_cast<int>(x))+ ")");
            for (float y = 0; y < quda; y++)
            {
                for (float z = 0; z < quda; z++)
                {
                    glm::vec3 postion = glm::vec3(x - qudaHalf, y, z- qudaHalf)* multiyplyerCube;
                    glm::mat4 matrix = glm::translate(matrix1, postion);
                    CreateStaticMeshEntity("Cubes", rowEntity, matrix, cube);
                }
            }
        }
        Entity camerE = scene->CreateEntity("Camera");
        camerE.AddComponent<CameraComponent>();
#endif

    }

    static void TestProfileRenderShaderMapRemove()
    {
        Ref<MeshStatic> ship;
        Ref<MeshStatic> cv;
        Ref<MeshStatic> cube;
        Ref<MeshStatic> cube2;

        Ref<MeshStatic> sponzer;
        Ref<MeshStatic> sponzer2;

        ship = Mesh::CreateStaticMesh(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/Yamto-Model/scene.gltf"));
        cv = Mesh::CreateStaticMesh(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/CV-Model/scene.gltf"));
        
        cube = Mesh::CreateStaticMesh("Assets/Models/Cube.gltf");
        cube2 = Mesh::CreateStaticMesh(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/Cube2.gltf"));
        
        sponzer = Mesh::CreateStaticMesh(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/main_sponza/main_sponza/NewSponza_Main_glTF_003.gltf"));
        sponzer2 = Mesh::CreateStaticMesh(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/main_sponza/main_sponza/NewSponza_Main_glTF_003.gltf"));

        Ref<Shader> shader = AssetManager::GetAsset<Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShader.glsl"));


    }

    static void TestLinkedTextureArray()
    {

    }
#pragma endregion

#pragma region TestStaticMeshScene

    static Ref<MeshStatic> TestDeserliceStaticMesh(const std::filesystem::path& filePath)
    {
        Ref<MeshStatic> mesh = CreateRef<MeshStatic>();
        StaticMeshSerialzation serialzation(mesh);
        
        
        Ref<MeshStatic> meshOrig;
        meshOrig = Mesh::CreateStaticMesh(filePath);

        std::filesystem::path projectPath = Project::GetActiveProjectDirectory();
        std::string fileStr = filePath.string();
        size_t index = fileStr.find('.');
        size_t size = fileStr.size();
        RY_CORE_ASSERT(index < size);

        std::string fileStaticMeshStr = fileStr.substr(0, index);
        fileStaticMeshStr += ".rystmesh";
        
        std::filesystem::path filePathStMesh = projectPath / fileStaticMeshStr;
        RY_CORE_ASSERT(mesh->GetSingleMeshObjectCount() != meshOrig->GetSingleMeshObjectCount());
        serialzation.Deserialize(filePathStMesh);
        RY_CORE_ASSERT(mesh->GetSingleMeshObjectCount() == meshOrig->GetSingleMeshObjectCount());
        return mesh;
    }

    static void TestSerliceStaticMesh(const std::filesystem::path& filePath)
    {
        Ref<MeshStatic> mesh;
        mesh = Mesh::CreateStaticMesh(filePath);
        StaticMeshSerialzation serialzation(mesh);

        std::filesystem::path projectPath = Project::GetActiveProjectDirectory();
        std::string fileStr = filePath.string();
        size_t index = fileStr.find('.');
        size_t size = fileStr.size();
        RY_CORE_ASSERT(index < size);

        std::string fileStaticMeshStr = fileStr.substr(0, index);
        fileStaticMeshStr += ".rystmesh";

        std::filesystem::path filePathStMesh = projectPath / fileStaticMeshStr;
        serialzation.Serialize(filePathStMesh);
    }

    static void TestSerliceMesh()
    { 
        TestSerliceStaticMesh("Assets/Models/Cube.gltf");
        TestSerliceStaticMesh("Assets/Models/Yamto-Model/scene.gltf");
        TestSerliceStaticMesh("Assets/Models/CV-Model/scene.gltf");
        TestSerliceStaticMesh("Assets/Models/Cube2.gltf");
        TestSerliceStaticMesh("Assets/Models/pkg_a_curtains/pkg_a_curtains/NewSponza_Curtains_glTF.gltf");
        TestSerliceStaticMesh("Assets/Models/main_sponza/main_sponza/NewSponza_Main_glTF_003.gltf");


        TestDeserliceStaticMesh("Assets/Models/Yamto-Model/scene.gltf");
    }

#pragma endregion

#pragma region TestLoding

    static void TestAsync()
    {
        Ref<Scene> scene = CreateRef<Scene>();
        Ref<Texture> tex = nullptr;
       
        Entity entity = scene->CreateEntity("hi-Test-asycn-loding");
        entity.AddComponent<SpriteRendererComponent>();
        int entityID = entity.GetEntityHandle();
        int entityID2 = entityID + 1;

        
        Ref<Scene> sceneCopy = Scene::Copy(scene);

        std::function<void(Ref<Texture>, Ref<Scene>, int)> onSceneAssetLoadedEntityFunc = Entity::OnAssetLoded<SpriteRendererComponent, Texture>;
        Ref<LodePromisType<Texture, Scene, int>> loadePromis = CreateRef<LodePromisType<Texture, Scene, int>>(scene, onSceneAssetLoadedEntityFunc, entityID2);
        std::filesystem::path filePath = RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/main_sponza/main_sponza/textures/metal_door_01_BaseColor.png");
        loadePromis->ChangeFuncArgs(entityID);
        AssetManager::GetAssetAsyncPromis<Texture>(filePath, loadePromis);
        
        loadePromis->AddRefObject(sceneCopy, onSceneAssetLoadedEntityFunc);

        loadePromis->WaitForLoding();
        
        {
            const SpriteRendererComponent& sprite = entity.GetComponent<SpriteRendererComponent>();
            const Weak<Texture>& textureWeak = sprite.Texture;
            Ref<Texture> texture = textureWeak.lock();
            RY_CORE_ASSERT(nullptr != texture, "No Texture Set");
        }
        Entity entityCopy = scene->GetEntityByName("hi-Test-asycn-loding");
        RY_CORE_ASSERT(entityCopy )
        {

            const SpriteRendererComponent& sprite = entityCopy.GetComponent<SpriteRendererComponent>();
            const Weak<Texture>& textureWeak = sprite.Texture;
            Ref<Texture> texture = textureWeak.lock();
            RY_CORE_ASSERT(nullptr != texture, "No Texture Set");
        }
        
    }

    static void TestPtrtoRefPtr0()
    {
        Ref<Scene> origelScene = CreateRef<Scene>();
        Entity entity = origelScene->CreateEntity("hi-Test-RefScene-FromScenePtr");
        entity.AddComponent<SpriteRendererComponent>();

        const Scene* sceneConstPtr = entity.GetScenePtr();
        Scene* scenePtr = const_cast<Scene*>(sceneConstPtr);
        Asset* assetPtr = reinterpret_cast<Asset*>(scenePtr);

        Ref<Asset> sceneAsset = Asset::GetRefInPlace(assetPtr);
        Ref<Scene> sceneRef = std::static_pointer_cast<Scene, Asset>(sceneAsset);
        sceneAsset.reset();
        RY_CORE_ASSERT(sceneRef == origelScene);
        RY_CORE_ASSERT(sceneRef.use_count() == 2);
        RY_CORE_ASSERT(origelScene.use_count() == 2);

        origelScene.reset();
        RY_CORE_ASSERT(sceneRef.use_count() == 1);       
    }

    static void TestPtrtoRefPtr1()
    {
        Ref<Scene> origelScene = CreateRef<Scene>();
        Entity entity = origelScene->CreateEntity("hi-Test-RefScene-FromScenePtr");
        entity.AddComponent<SpriteRendererComponent>();
        origelScene.reset();
        Ref<Scene> sceneRef = entity.GetScene();
        RY_CORE_ASSERT(nullptr == sceneRef);
        RY_CORE_ASSERT(sceneRef == origelScene);
        RY_CORE_ASSERT(sceneRef.use_count() == 0);
        RY_CORE_ASSERT(origelScene.use_count() == 0);
    }

    static void TestLoadingAssetAsync()
    {
        TestPtrtoRefPtr0();
        TestPtrtoRefPtr1();
        TestAsync();
    }

#pragma endregion

    static void TestCubeAnd3DTextures(Ref<Scene>& aktiveScene)
    {
        {
            constexpr uint32_t size = 16u;
            constexpr uint32_t textureCubeMapCount = 6u;
            constexpr uint32_t pixelData = 0xff;
            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size;
            uint32_t depth = textureCubeMapCount;

            uint32_t pixelCount = depth * withe * height;
            uint32_t texturesByteSize = pixelCount * pixelByteSize;
            std::vector<uint8_t> cubeMapVec;
            cubeMapVec.resize(texturesByteSize, pixelData);
            TextureSpecification spec{
                withe, height, depth,
                TextureTarget::TextureCubeMap,
                TextureFormat::RGBA8,
                1u,
            };
            Ref<Texture> cubeMapTest = Texture::Create(spec, cubeMapVec.data(), cubeMapVec.size());
            cubeMapTest->Bind(1u);
            cubeMapTest->UnBind();
            for (uint8_t& value : cubeMapVec)
                value = 0xf0;
            cubeMapTest->SetData(cubeMapVec.data(), cubeMapVec.size());

            cubeMapTest->Bind(1u);
            cubeMapTest->UnBind();

            Entity entiy = aktiveScene->CreateEntity("Test ");
            SpriteRendererComponent& spriteC = entiy.AddComponent<SpriteRendererComponent>();
            TransformComponent& trasC = entiy.GetComponent<TransformComponent>();
            spriteC.Texture = cubeMapTest;
            spriteC.Color.b = 0.5f;
            trasC.Scale *= 2.0f;
            trasC.Transaltion.x = 4.0f;
            AssetManager::CreatLocaleAsset(cubeMapTest);

            entiy.UpdateMatrix();

        }

        {
            constexpr uint32_t size = 16u;
            constexpr uint32_t textureCubeMapCount = 6u;
            constexpr uint32_t textureCubeMapDataCount = 4u;

            constexpr uint32_t pixelData = 0xff;
            constexpr uint32_t pixelData2 = 0x00;

            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            const uint32_t withe = size;
            const uint32_t height = size;
            uint32_t depth = textureCubeMapDataCount;

            uint32_t pixelCount = depth * withe * height;

            const uint32_t texturesDataByteSize = pixelCount * pixelByteSize;
            depth = textureCubeMapCount;
            pixelCount = depth * withe * height;

            const  uint32_t texturesByteSize = pixelCount * pixelByteSize;


            std::vector<uint8_t> cubeMapVec;
            cubeMapVec.resize(texturesDataByteSize, pixelData);
            TextureSpecification spec{
                withe, height, depth,
                TextureTarget::TextureCubeMap,
                TextureFormat::RGBA8,
                1u,
            };
            Ref<Texture> cubeMapTest = Texture::Create(spec, cubeMapVec.data(), cubeMapVec.size());
            cubeMapTest->Bind(1u);
            cubeMapTest->UnBind();

            cubeMapVec.resize(texturesByteSize, pixelData2);
            cubeMapTest->SetData(cubeMapVec.data(), cubeMapVec.size());

            cubeMapTest->Bind(1u);
            cubeMapTest->UnBind();

            Entity entiy = aktiveScene->CreateEntity("Test cubeMapTest resize");
            SpriteRendererComponent& spriteC = entiy.AddComponent<SpriteRendererComponent>();
            TransformComponent& trasC = entiy.GetComponent<TransformComponent>();
            AssetManager::CreatLocaleAsset(cubeMapTest);
            spriteC.Color.b = 0.5f;

            spriteC.Texture = cubeMapTest;
            trasC.Scale *= 2.0f;
            trasC.Transaltion.x = 0.0f;
            entiy.UpdateMatrix();

        }

        {
            constexpr uint32_t size = 16u;
            constexpr uint32_t textureCubeMapCount = 6u;
            constexpr uint32_t textureCubeMapDataCount = 4u;

            constexpr uint32_t pixelData = 0x80;
            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size;
            uint32_t depth = textureCubeMapDataCount;

            uint32_t pixelCount = depth * withe * height;

            const uint32_t texturesDataByteSize = pixelCount * pixelByteSize;
            depth = textureCubeMapCount;
            pixelCount = depth * withe * height;

            const  uint32_t texturesByteSize = pixelCount * pixelByteSize;
            Ref<Texture> cubeMapTest = nullptr;

            {
                std::vector<uint8_t> cubeMapVec;
                cubeMapVec.resize(texturesDataByteSize, pixelData);
                TextureSpecification spec{
                    withe, height, depth,
                    TextureTarget::TextureCubeMap,
                    TextureFormat::RGBA8,
                    1u,
                };
                cubeMapTest = Texture::Create(spec);
            }

            cubeMapTest->Bind(1u);
            cubeMapTest->UnBind();


            {
                std::vector<uint8_t> cubeMapVec;
                cubeMapVec.resize(texturesByteSize, pixelData);
                cubeMapTest->SetData(cubeMapVec.data(), cubeMapVec.size());
            }

            {
                uint32_t withe = size * 2u;
                uint32_t height = size * 2u;
                uint32_t depth = textureCubeMapDataCount;

                std::vector<uint8_t> cubeMapVec;
                cubeMapTest->Resize2D(withe, height);
                cubeMapVec.resize(texturesByteSize, pixelData);
                cubeMapTest->SetData(cubeMapVec.data(), cubeMapVec.size());
            }


            cubeMapTest->Bind(1u);
            cubeMapTest->UnBind();

            Entity entiy = aktiveScene->CreateEntity("Test ");
            SpriteRendererComponent& spriteC = entiy.AddComponent<SpriteRendererComponent>();
            TransformComponent& trasC = entiy.GetComponent<TransformComponent>();
            spriteC.Color.b = 0.5f;

            spriteC.Texture = cubeMapTest;
            AssetManager::CreatLocaleAsset(cubeMapTest);
            trasC.Scale *= 2.0f;
            trasC.Transaltion.x = 0.0f;

            entiy.UpdateMatrix();

        }

        {
            constexpr uint32_t size = 16u;
            constexpr uint32_t pixelData = 0x0f;
            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size;
            uint32_t depth = size;

            uint32_t pixelCount = depth * withe * height;
            uint32_t texturesByteSize = pixelCount * pixelByteSize;
            std::vector<uint8_t> texture3DDataVec;
            texture3DDataVec.resize(texturesByteSize, pixelData);
            TextureSpecification spec{
                withe, height, depth,
                TextureTarget::Texture3D,
                TextureFormat::RGBA8,
                1u,
            };
            Ref<Texture> texture3D = Texture::Create(spec, texture3DDataVec.data(), texture3DDataVec.size());
            texture3D->Bind(1u);
            texture3D->UnBind();
            for (uint8_t& value : texture3DDataVec)
                value = 0x00;
            texture3D->SetData(texture3DDataVec.data(), texture3DDataVec.size());

            texture3D->Bind(1u);
            texture3D->UnBind();

            Entity entiy = aktiveScene->CreateEntity("Test 3d Texture");
            SpriteRendererComponent& spriteC = entiy.AddComponent<SpriteRendererComponent>();
            TransformComponent& trasC = entiy.GetComponent<TransformComponent>();
            spriteC.Color.b = 0.5f;

            spriteC.Texture = texture3D;
            trasC.Scale *= 2.0f;
            trasC.Transaltion.x = -2.0f;
            AssetManager::CreatLocaleAsset(texture3D);

            entiy.UpdateMatrix();

        }
    }

    static void TestArrayTextures(Ref<Scene>& aktiveScene)
    {

        {
            constexpr uint32_t size = 4u;
            constexpr uint32_t textureCount = 3u;
            constexpr uint8_t pixelData = 0xff;
            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size + 1u;
            uint32_t depth = 1u;
                       

            TextureSpecification spec{
                withe, height, depth,
                TextureTarget::Texture2D,
                TextureFormat::RGBA8,
                1u,
            };
            TextureSpecification specArray = spec;
            specArray.Target == TextureTarget::Texture2D_Array;
            Ref<LinkedTextureArray> linkedTextureArray = LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);
            linkedTextureArray->ResizeTextureArray(3);
            
            Ref<Texture> texture0 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture1 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture2 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture3 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());


            linkedTextureArray->SetTextureToArray(0,texture0);
            linkedTextureArray->SetTextureToArray(1,texture1);
            linkedTextureArray->SetTextureToArray(2,texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind(1u);
        }

        {
            constexpr uint32_t size = 4u;
            constexpr uint32_t textureCount = 3u;
            constexpr uint8_t pixelData = 0xff;
            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size + 1u;
            uint32_t depth = 1u;


            TextureSpecification spec{
                withe, height, depth,
                TextureTarget::Texture2D,
                TextureFormat::RGBA8,
                1u,
            };

            TextureSpecification specArray = spec;
            specArray.Target == TextureTarget::Texture2D_Array;
            Ref<LinkedTextureArray> linkedTextureArray = LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Ref<Texture> texture0 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture1 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture2 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture3 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());

            linkedTextureArray->ResizeTextureArray(3);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind(1u);

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
            RY_CORE_ASSERT(0u == linkedTextureArray->GetTextureCount());

        }

        {
            constexpr uint32_t size = 4u;
            constexpr uint32_t textureCount = 3u;
            constexpr uint8_t pixelData = 0xff;
            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size + 1u;
            uint32_t depth = 1u;


            TextureSpecification spec{
                withe, height, depth,
                TextureTarget::Texture2D,
                TextureFormat::RGBA8,
                1u,
            };

            TextureSpecification specArray = spec;
            specArray.Target == TextureTarget::Texture2D_Array;
            Ref<LinkedTextureArray> linkedTextureArray = LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Ref<Texture> texture0 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture1 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture2 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture3 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());

            linkedTextureArray->ResizeTextureArray(4);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->SetTextureToArray(3, texture3);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind(1u);

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
            RY_CORE_ASSERT(0u == linkedTextureArray->GetTextureCount());
        }

        {
            constexpr uint32_t size = 4u;
            constexpr uint32_t textureCount = 3u;
            constexpr uint8_t pixelData = 0xff;
            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size + 1u;
            uint32_t depth = 1u;

            TextureSpecification spec{
                withe, height, depth,
                TextureTarget::Texture2D,
                TextureFormat::RGBA8,
                1u,
            };

            TextureSpecification specArray = spec;
            specArray.Target == TextureTarget::Texture2D_Array;
            Ref<LinkedTextureArray> linkedTextureArray = LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Ref<Texture> texture0 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture1 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture2 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture3 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());

            linkedTextureArray->ResizeTextureArray(4);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->SetTextureToArray(3, texture3);
            linkedTextureArray->SetTextureToArray(3, nullptr);
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind(1u);

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
            RY_CORE_ASSERT(0u == linkedTextureArray->GetTextureCount());
        }

        {
            constexpr uint32_t size = 4u;
            constexpr uint32_t textureCount = 3u;
            constexpr uint8_t pixelData = 0xff;
            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size + 1u;
            uint32_t depth = 1u;


            TextureSpecification spec{
                withe, height, depth,
                TextureTarget::Texture2D,
                TextureFormat::RGBA8,
                1u,
            };

            TextureSpecification specArray = spec;
            specArray.Target == TextureTarget::Texture2D_Array;
            Ref<LinkedTextureArray> linkedTextureArray = LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Ref<Texture> texture0 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture1 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture2 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture3 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());

            linkedTextureArray->ResizeTextureArray(4);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->SetTextureToArray(3, texture3);
            linkedTextureArray->SetTextureToArray(3, nullptr);
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind();

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
            RY_CORE_ASSERT(0u == linkedTextureArray->GetTextureCount());

        }

        {
            constexpr uint32_t size = 4u;
            constexpr uint32_t textureCount = 3u;
            constexpr uint8_t pixelData = 0xff;
            constexpr uint8_t pixelData2 = 0x0f;

            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size + 1u;
            uint32_t depth = 1u;


            TextureSpecification spec{
                withe, height, depth,
                TextureTarget::Texture2D,
                TextureFormat::RGBA8,
                1u,
            };

            TextureSpecification specArray = spec;
            specArray.Target == TextureTarget::Texture2D_Array;
            Ref<LinkedTextureArray> linkedTextureArray = LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Ref<Texture> texture0 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture1 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture2 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture3 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            pixelDataVec.resize(pixelCount, pixelData);

            linkedTextureArray->ResizeTextureArray(4);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
            texture1->SetData(pixelDataVec.data(), pixelDataVec.size());
            RY_CORE_ASSERT(!linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->UpdateDataGPU();

            linkedTextureArray->SetTextureToArray(3, texture3);
            linkedTextureArray->SetTextureToArray(3, nullptr);
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind();

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
        }

        {
            constexpr uint32_t size = 4u;
            constexpr uint32_t textureCount = 3u;
            constexpr uint8_t pixelData = 0xff;
            constexpr uint8_t pixelData2 = 0x0f;

            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size + 1u;
            uint32_t depth = 1u;


            TextureSpecification spec{
                withe, height, depth,
                TextureTarget::Texture2D,
                TextureFormat::RGBA8,
                1u,
            };

            TextureSpecification specArray = spec;
            specArray.Target == TextureTarget::Texture2D_Array;
            Ref<LinkedTextureArray> linkedTextureArray = LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Ref<Texture> texture0 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture1 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture2 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Ref<Texture> texture3 = Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            pixelDataVec.resize(pixelCount, pixelData);

            linkedTextureArray->ResizeTextureArray(4);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
            texture1->SetData(pixelDataVec.data(), pixelDataVec.size());
            RY_CORE_ASSERT(!linkedTextureArray->IsDataRaydyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());

            linkedTextureArray->SetTextureToArray(3, texture3);
            linkedTextureArray->SetTextureToArray(3, nullptr);
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind();

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataRaydyOnGPU());
        }


        {
            DrawContext context;
            struct {
                glm::vec4 color = {1.0f,1.0f, 1.0f,1.0f };
                glm::vec4 color2 = { -1.0f,-1.0f, -1.0f,-1.0f };

            } dataStruct;
            Ref<UniformBuffer> buffer = UniformBuffer::Create(&dataStruct, sizeof(dataStruct));
            context.CreateBuffer("TestBuffer", buffer, BufferLayout({
                {SDT::Float4, "color"},
                {SDT::Float4, "color2"}
            }));
            context.SetElement("color2", dataStruct.color);
            context.SetElement("color", dataStruct.color2);

            Ref<UniformBuffer> returnBuffer = context.GetBufferAs<UniformBuffer>("TestBuffer");
            
            

            RY_CORE_ASSERT(buffer.get() == returnBuffer.get());

            context.PushScope("Sope");
            Ref<UniformBuffer> returnBufferScope = context.GetBufferFromScopeNameAs<UniformBuffer>("", "TestBuffer");
            context.PopScope();

            Ref<UniformBuffer> returnBufferNotScope = context.GetBufferAs<UniformBuffer>("TestBuffer");

            RY_CORE_ASSERT(buffer.get() == returnBufferScope.get());
            RY_CORE_ASSERT(buffer.get() == returnBufferNotScope.get());

        }
    }



    struct CameraPackage {
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
        glm::mat4 ViewProjectionMatrix;           // VP
        glm::mat4 ScaledTransformViewProjection;  // VP * 0.5 + 0.5
        glm::vec4 Position;
        glm::vec4 Direction;
        glm::vec2 ImageOffset;
        glm::vec2 ImageSize;

        // Einmal bauen aus den Rohwerten — CameraPackage ist immer vollständig
        static CameraPackage build(const glm::mat4& view,
            const glm::mat4& proj,
            const glm::vec3& pos,
            const glm::vec3& dir,
            glm::vec2 offset,
            glm::vec2 size)
        {
            CameraPackage c;
            c.ViewMatrix = view;
            c.ProjectionMatrix = proj;
            c.ViewProjectionMatrix = proj * view;
            // STVP: NDC [-1,1] → [0,1] für Textur-Lookup im World-Space
            static const glm::mat4 bias = glm::translate(glm::mat4(1.f), { 0.5f,0.5f,0.5f })
                * glm::scale(glm::mat4(1.f), { 0.5f,0.5f,0.5f });
            c.ScaledTransformViewProjection = bias * c.ViewProjectionMatrix;
            c.Position = glm::vec4(pos, 1.f);
            c.Direction = glm::vec4(dir, 0.f);
            c.ImageOffset = offset;
            c.ImageSize = size;
            return c;
        }
    };
    static void TestStagingSlotAppendWrite()
    {
        Ref<IStagingSlot> stagingSlot = CreateRef<StagingSlotAppendWrite<CameraPackage>>();
        CameraPackage packeg;
        std::memset(&packeg, 0, sizeof(packeg));
        for (int i = 0; i < 16; i++)
        {
            stagingSlot->Add(&(packeg.Direction), sizeof(packeg.Direction));
            stagingSlot->Add(&(packeg.ViewProjectionMatrix), sizeof(packeg.ViewProjectionMatrix));
            stagingSlot->Add(&(packeg.ImageOffset), sizeof(packeg.ImageOffset));
            stagingSlot->Add(&(packeg.Position), sizeof(packeg.Position));
            stagingSlot->Add(&(packeg.ProjectionMatrix), sizeof(packeg.ProjectionMatrix));
            stagingSlot->Add(&(packeg.ScaledTransformViewProjection), sizeof(packeg.ScaledTransformViewProjection));
            stagingSlot->Add(&(packeg.ImageSize), sizeof(packeg.ImageSize));
            stagingSlot->Add(&(packeg.ViewMatrix), sizeof(packeg.ViewMatrix));
        }
        for (int i = 0; i < 56; i++)
        {
            stagingSlot->Add(&(packeg.ImageOffset), sizeof(packeg.ImageOffset));
            stagingSlot->Add(&(packeg.Position), sizeof(packeg.Position));
            stagingSlot->Add(&(packeg.ProjectionMatrix), sizeof(packeg.ProjectionMatrix));
            stagingSlot->Add(&(packeg.ImageSize), sizeof(packeg.ImageSize));
            stagingSlot->Add(&(packeg.ViewMatrix), sizeof(packeg.ViewMatrix));
            stagingSlot->Add(&(packeg.ScaledTransformViewProjection), sizeof(packeg.ScaledTransformViewProjection));
            stagingSlot->Add(&(packeg.Direction), sizeof(packeg.Direction));
            stagingSlot->Add(&(packeg.ViewProjectionMatrix), sizeof(packeg.ViewProjectionMatrix));
        }
        for (int i = 0; i < 4; i++)
        {
            stagingSlot->Add(&packeg, sizeof(packeg));
        }
        

       

    }

    static void TestStagingSlotAppendByteWrite()
    {
        Ref<IStagingSlot> stagingSlot = CreateRef<StagingSlotAppendByteWrite>(sizeof(CameraPackage));
        CameraPackage packeg;
        std::memset(&packeg, 0, sizeof(packeg));
        for (int i = 0; i < 56; i++)
        {
            stagingSlot->Add(&(packeg.Direction), sizeof(packeg.Direction));
            stagingSlot->Add(&(packeg.ViewProjectionMatrix), sizeof(packeg.ViewProjectionMatrix));
            stagingSlot->Add(&(packeg.ImageOffset), sizeof(packeg.ImageOffset));
            stagingSlot->Add(&(packeg.Position), sizeof(packeg.Position));
            stagingSlot->Add(&(packeg.ProjectionMatrix), sizeof(packeg.ProjectionMatrix));
            stagingSlot->Add(&(packeg.ScaledTransformViewProjection), sizeof(packeg.ScaledTransformViewProjection));
            stagingSlot->Add(&(packeg.ImageSize), sizeof(packeg.ImageSize));
            stagingSlot->Add(&(packeg.ViewMatrix), sizeof(packeg.ViewMatrix));
        }
        for (int i = 0; i < 16; i++)
        {
            stagingSlot->Add(&(packeg.ImageOffset), sizeof(packeg.ImageOffset));
            stagingSlot->Add(&(packeg.Position), sizeof(packeg.Position));
            stagingSlot->Add(&(packeg.ProjectionMatrix), sizeof(packeg.ProjectionMatrix));
            stagingSlot->Add(&(packeg.ImageSize), sizeof(packeg.ImageSize));
            stagingSlot->Add(&(packeg.ViewMatrix), sizeof(packeg.ViewMatrix));
            stagingSlot->Add(&(packeg.ScaledTransformViewProjection), sizeof(packeg.ScaledTransformViewProjection));
            stagingSlot->Add(&(packeg.Direction), sizeof(packeg.Direction));
            stagingSlot->Add(&(packeg.ViewProjectionMatrix), sizeof(packeg.ViewProjectionMatrix));
        }

        for (int i = 0; i < 4; i++)
        {
            stagingSlot->Add(&packeg, sizeof(packeg));
        }
       



    }

    static void TestStagingSlotAppend()
    {
        Ref<IStagingSlot> stagingSlot = CreateRef<StagingSlotAppend<CameraPackage>>();
        CameraPackage packeg;
        std::memset(&packeg, 0, sizeof(packeg));

        for(int i = 0; i < 64; i++)
        {
            stagingSlot->Add(&packeg, sizeof(packeg));
        }
       
        for (int i = 0; i < 16; i++)
        {
            stagingSlot->Add(&packeg, sizeof(packeg));
        }




    }

    static void TestStagingSlotAppendByte()
    {
        Ref<IStagingSlot> stagingSlot = CreateRef<StagingSlotAppendByte>(sizeof(CameraPackage));
        CameraPackage packeg;
        std::memset(&packeg, 0, sizeof(packeg));

        for (int i = 0; i < 64; i++)
        {
            stagingSlot->Add(&packeg, sizeof(packeg));
        }

        
        for (int i = 0; i < 16; i++)
        {
            stagingSlot->Add(&packeg, sizeof(packeg));
        }




    }


    static void PerformenzTestBaseVarient()
    {
        std::vector<Ref<IStagingSlot>> baseClassVec; 
        constexpr uint32_t count = 10000;
        constexpr uint32_t actionsCount = 64;
        constexpr uint32_t initByteSize = actionsCount * 2;

        using TimeUnit = std::chrono::microseconds;
        baseClassVec.reserve(count * 9);
        LifeTimer timer;
        int64_t exexuteBaseNansec[2];
        int64_t initBaseNansec;

        {
            timer.BeginNewTimePoint();
            for (int i = 0; i < count; i++)
            {
                baseClassVec.emplace_back(CreateRef<StagingSlotAppend<CameraPackage>>(initByteSize));
                baseClassVec.emplace_back(CreateRef<StagingSlotAppendByte>(sizeof(CameraPackage), initByteSize));
                baseClassVec.emplace_back(CreateRef<StagingSlotAppendWrite<CameraPackage>>(0, initByteSize));
                baseClassVec.emplace_back(CreateRef<StagingSlotAppend<CameraPackage>>(initByteSize));

                baseClassVec.emplace_back(CreateRef<StagingSlotAppendByte>(sizeof(CameraPackage), initByteSize));

                baseClassVec.emplace_back(CreateRef<StagingSlotAppendByteWrite>(sizeof(CameraPackage), 0, initByteSize));
                baseClassVec.emplace_back(CreateRef<StagingSlotAppend<CameraPackage>>(initByteSize));
                baseClassVec.emplace_back(CreateRef<StagingSlotAppendWrite<CameraPackage>>(0, initByteSize));
                baseClassVec.emplace_back(CreateRef<StagingSlotAppendByte>(sizeof(CameraPackage), initByteSize));
            }
            initBaseNansec = timer.GetTimePast<TimeUnit>();
        }
        


        CameraPackage packeg;
        std::memset(&packeg, 0, sizeof(CameraPackage));

       
       
        {
            timer.BeginNewTimePoint();
            for (auto& e : baseClassVec)
            {
                for (int i = 0; i < actionsCount; i++)
                {
                    IStagingSlot::Add(e, packeg);
                }
            }
            exexuteBaseNansec[0] = timer.GetTimePast<TimeUnit>();
        }

        {
            timer.BeginNewTimePoint();
            for (int i = 0; i < actionsCount; i++)
            {
                for (auto& e : baseClassVec)
                {

                    IStagingSlot::Add(e, packeg);

                }
            }
            exexuteBaseNansec[1] = timer.GetTimePast<TimeUnit>();
        }
        baseClassVec.clear();
        baseClassVec.shrink_to_fit();
        RY_CORE_TRACE("base clase init: ({} micosec), exexute[0]: ({} micosec), exexute[1]: ({} micosec)", initBaseNansec, exexuteBaseNansec[0], exexuteBaseNansec[1]);

        
        std::vector<std::variant<StagingSlotAppend<CameraPackage>, StagingSlotAppendWrite<CameraPackage>, StagingSlotAppendByte, StagingSlotAppendByteWrite>> varientClassVec;
        varientClassVec.reserve(count * 9);
        int64_t initVarientNansec;
        int64_t exexuteVarientNansec[2];

        {
            timer.BeginNewTimePoint();
            for (int i = 0; i < count; i++)
            {
                varientClassVec.emplace_back(StagingSlotAppend<CameraPackage>(initByteSize));
                varientClassVec.emplace_back(StagingSlotAppendByte(sizeof(CameraPackage), initByteSize));
                varientClassVec.emplace_back(StagingSlotAppendWrite<CameraPackage>(0, initByteSize));
                varientClassVec.emplace_back(StagingSlotAppend<CameraPackage>(initByteSize));

                varientClassVec.emplace_back(StagingSlotAppendByte(sizeof(CameraPackage), initByteSize));

                varientClassVec.emplace_back(StagingSlotAppendByteWrite(sizeof(CameraPackage), 0, initByteSize));
                varientClassVec.emplace_back(StagingSlotAppend<CameraPackage>(initByteSize));
                varientClassVec.emplace_back(StagingSlotAppendWrite<CameraPackage>(0, initByteSize));
                varientClassVec.emplace_back(StagingSlotAppendByte(sizeof(CameraPackage), initByteSize));
            }
            initVarientNansec = timer.GetTimePast<TimeUnit>();
        }
        {
            timer.BeginNewTimePoint();
            for (auto& e : varientClassVec)
            { 
                std::visit([&](auto& stagingSlot) {
                    for (int i = 0; i < actionsCount; i++)
                    {
                        stagingSlot.Add(&packeg, sizeof(CameraPackage));
                    }
                }, e);
            }
            exexuteVarientNansec[0] = timer.GetTimePast<TimeUnit>();
        }
        {
            timer.BeginNewTimePoint();
            for (int i = 0; i < actionsCount; i++)
            {
                for (auto& e : varientClassVec)
                {
                    std::visit([&](auto& stagingSlot) {
                        stagingSlot.Add(&packeg, sizeof(CameraPackage));
                    }, e);
                }
            }
            exexuteVarientNansec[1] = timer.GetTimePast<TimeUnit>();
        }
        varientClassVec.clear();
        varientClassVec.shrink_to_fit();
        RY_CORE_TRACE("varient clase init: ({} micosec), exexute[0]: ({} micosec), exexute[1]: ({} micosec)", initVarientNansec, exexuteVarientNansec[0], exexuteVarientNansec[1]);

        int64_t diffInitNanosec = initVarientNansec - initBaseNansec;
        int64_t diffEexexuteNanosec[] = {
            exexuteVarientNansec[0] - exexuteBaseNansec[0],
            exexuteVarientNansec[1] - exexuteBaseNansec[1]
        };

        RY_CORE_TRACE("init differnz micosec: {} ({})", diffInitNanosec, (initBaseNansec < initVarientNansec ? "Base" : "Varient"));
        RY_CORE_TRACE("exexute[0] differnz micosec: {} ({})", diffEexexuteNanosec[0], (exexuteBaseNansec[0] < exexuteVarientNansec[0] ? "Base" : "Varient"));
        RY_CORE_TRACE("exexute[1] differnz micosec: {} ({})", diffEexexuteNanosec[1], (exexuteBaseNansec[1] < exexuteVarientNansec[1] ? "Base" : "Varient"));

    }

    static void PerformenzTestByteWriteVsWriteVarient()
    {
        std::vector<std::variant<StagingSlotAppendWrite<CameraPackage>>> varientWhriteClassVec;
        std::vector<std::variant<StagingSlotAppendByteWrite>> varientWhriteByteClassVec;
        using TimeUnit = std::chrono::milliseconds;
        constexpr const char* timeUinteName = "milisec";
        constexpr uint32_t count = 10000;
        constexpr uint32_t actionsCount = 64;
        constexpr uint32_t initByteSize = actionsCount * 2;

        varientWhriteClassVec.reserve(count);
        LifeTimer timer;

        int64_t initWhriteVareintTime;
        int64_t executeWhriteVareintTime[2];


        int64_t initWhriteByteVareintTime;
        int64_t executeWhriteByteVareintTime[2];

        {
            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < count; i++)
                {
                    varientWhriteClassVec.emplace_back(StagingSlotAppendWrite<CameraPackage>(0, initByteSize));
                }
                initWhriteVareintTime = timer.GetTimePast<TimeUnit>();
            }
            CameraPackage packeg;
            std::memset(&packeg, 0, sizeof(CameraPackage));


            {
                timer.BeginNewTimePoint();
                for (auto& e : varientWhriteClassVec)
                {
                    for (int i = 0; i < actionsCount; i++)
                    {
                        std::visit([&](auto& stagingSlot) {
                            stagingSlot.Add(&packeg, sizeof(packeg));

                            }, e);
                    }
                }
                executeWhriteVareintTime[0] = timer.GetTimePast<TimeUnit>();
            }

            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < actionsCount; i++)
                {
                    for (auto& e : varientWhriteClassVec)
                    {
                        std::visit([&](auto& stagingSlot) {
                            stagingSlot.Add(&packeg, sizeof(packeg));

                            }, e);

                    }
                }
                executeWhriteVareintTime[1] = timer.GetTimePast<TimeUnit>();
            }
        }
        RY_CORE_TRACE("varient whrite clase init: ({1} {0}), exexute[0]: ({2} {0}), exexute[1]: ({3} {0})", timeUinteName, initWhriteVareintTime, executeWhriteVareintTime[0], executeWhriteVareintTime[1]);
        {
            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < count; i++)
                {
                    varientWhriteByteClassVec.emplace_back(StagingSlotAppendByteWrite(sizeof(CameraPackage), 0, initByteSize));
                }
                initWhriteByteVareintTime = timer.GetTimePast<TimeUnit>();
            }
            CameraPackage packeg;
            std::memset(&packeg, 0, sizeof(CameraPackage));


            {
                timer.BeginNewTimePoint();
                for (auto& e : varientWhriteByteClassVec)
                {
                    for (int i = 0; i < actionsCount; i++)
                    {
                        std::visit([&](auto& stagingSlot) {
                            stagingSlot.Add(&packeg, sizeof(packeg));

                        }, e);
                    }
                }
                executeWhriteByteVareintTime[0] = timer.GetTimePast<TimeUnit>();
            }

            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < actionsCount; i++)
                {
                    for (auto& e : varientWhriteClassVec)
                    {
                        std::visit([&](auto& stagingSlot) {
                            stagingSlot.Add(&packeg, sizeof(packeg));

                            }, e);

                    }
                }
                executeWhriteByteVareintTime[1] = timer.GetTimePast<TimeUnit>();
            }
        }
        RY_CORE_TRACE("varient whrite Byte clase init: ({1} {0}), exexute[0]: ({2} {0}), exexute[1]: ({3} {0})", timeUinteName, initWhriteByteVareintTime, executeWhriteByteVareintTime[0], executeWhriteByteVareintTime[1]);
    }

    static void PerformenzTestByteWriteVsWriteBase()
    {
        std::vector<Ref<IStagingSlot>> baseWhriteClassVec;
        std::vector<Ref<IStagingSlot>> baseWhriteByteClassVec;
        using TimeUnit = std::chrono::milliseconds;
        constexpr const char* timeUinteName = "milisec";
        constexpr uint32_t count = 10000;
        constexpr uint32_t actionsCount = 64;
        constexpr uint32_t initByteSize = actionsCount * 2;

        baseWhriteClassVec.reserve(count);
        LifeTimer timer;

        int64_t initWhriteBaseTime;
        int64_t executeWhriteBaseTime[2];


        int64_t initWhriteByteBaseTime;
        int64_t executeWhriteByteBaseTime[2];

        {
            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < count; i++)
                {
                    baseWhriteClassVec.emplace_back(CreateRef<StagingSlotAppendWrite<CameraPackage>>(0, initByteSize));
                }
                initWhriteBaseTime = timer.GetTimePast<TimeUnit>();
            }
            CameraPackage packeg;
            std::memset(&packeg, 0, sizeof(CameraPackage));


            {
                timer.BeginNewTimePoint();
                for (auto& e : baseWhriteClassVec)
                {
                    for (int i = 0; i < actionsCount; i++)
                    {
                        IStagingSlot::Add(e, packeg);
                    }
                }
                executeWhriteBaseTime[0] = timer.GetTimePast<TimeUnit>();
            }

            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < actionsCount; i++)
                {
                    for (auto& e : baseWhriteClassVec)
                    {
                        IStagingSlot::Add(e, packeg);
                    }
                }
                executeWhriteBaseTime[1] = timer.GetTimePast<TimeUnit>();
            }
        }
        RY_CORE_TRACE("base whrite clase init: ({1} {0}), exexute[0]: ({2} {0}), exexute[1]: ({3} {0})", timeUinteName, initWhriteBaseTime, executeWhriteBaseTime[0], executeWhriteBaseTime[1]);
        {
            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < count; i++)
                {
                    baseWhriteByteClassVec.emplace_back(CreateRef<StagingSlotAppendByteWrite>(sizeof(CameraPackage), 0, initByteSize));
                }
                initWhriteByteBaseTime = timer.GetTimePast<TimeUnit>();
            }
            CameraPackage packeg;
            std::memset(&packeg, 0, sizeof(CameraPackage));


            {
                timer.BeginNewTimePoint();
                for (auto& e : baseWhriteByteClassVec)
                {
                    for (int i = 0; i < actionsCount; i++)
                    {
                        IStagingSlot::Add(e, packeg);
                    }
                }
                executeWhriteByteBaseTime[0] = timer.GetTimePast<TimeUnit>();
            }

            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < actionsCount; i++)
                {
                    for (auto& e : baseWhriteClassVec)
                    {
                        IStagingSlot::Add(e, packeg);
                    }
                }
                executeWhriteByteBaseTime[1] = timer.GetTimePast<TimeUnit>();
            }
        }
        RY_CORE_TRACE("base whrite Byte clase init: ({1} {0}), exexute[0]: ({2} {0}), exexute[1]: ({3} {0})", timeUinteName, initWhriteByteBaseTime, executeWhriteByteBaseTime[0], executeWhriteByteBaseTime[1]);
    }


    static void PerformenzTestByteVsTypeVarient()
    {
        std::vector<std::variant<StagingSlotAppend<CameraPackage>>> varientTypeClassVec;
        std::vector<std::variant<StagingSlotAppendByte>> varientByteClassVec;
        using TimeUnit = std::chrono::milliseconds;
        constexpr const char* timeUinteName = "milisec";
        constexpr uint32_t count = 10000;
        constexpr uint32_t actionsCount = 64;
        constexpr uint32_t initByteSize = actionsCount * 2;

        varientTypeClassVec.reserve(count);
        varientByteClassVec.reserve(count);
        LifeTimer timer;

        int64_t initWhriteVareintTime;
        int64_t executeWhriteVareintTime[2];


        int64_t initWhriteByteVareintTime;
        int64_t executeWhriteByteVareintTime[2];

        {
            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < count; i++)
                {
                    varientTypeClassVec.emplace_back(StagingSlotAppend<CameraPackage>(initByteSize));
                }
                initWhriteVareintTime = timer.GetTimePast<TimeUnit>();
            }
            CameraPackage packeg;
            std::memset(&packeg, 0, sizeof(CameraPackage));


            {
                timer.BeginNewTimePoint();
                for (auto& e : varientTypeClassVec)
                {
                    for (int i = 0; i < actionsCount; i++)
                    {
                        std::visit([&](auto& stagingSlot) {
                            stagingSlot.Add(&packeg, sizeof(packeg));

                            }, e);
                    }
                }
                executeWhriteVareintTime[0] = timer.GetTimePast<TimeUnit>();
            }

            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < actionsCount; i++)
                {
                    for (auto& e : varientTypeClassVec)
                    {
                        std::visit([&](auto& stagingSlot) {
                            stagingSlot.Add(&packeg, sizeof(packeg));

                            }, e);

                    }
                }
                executeWhriteVareintTime[1] = timer.GetTimePast<TimeUnit>();
            }
        }
        RY_CORE_TRACE("varient clase init: ({1} {0}), exexute[0]: ({2} {0}), exexute[1]: ({3} {0})", timeUinteName, initWhriteVareintTime, executeWhriteVareintTime[0], executeWhriteVareintTime[1]);
        {
            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < count; i++)
                {
                    varientByteClassVec.emplace_back(StagingSlotAppendByte(sizeof(CameraPackage), initByteSize));
                }
                initWhriteByteVareintTime = timer.GetTimePast<TimeUnit>();
            }
            CameraPackage packeg;
            std::memset(&packeg, 0, sizeof(CameraPackage));


            {
                timer.BeginNewTimePoint();
                for (auto& e : varientByteClassVec)
                {
                    for (int i = 0; i < actionsCount; i++)
                    {
                        std::visit([&](auto& stagingSlot) {
                            stagingSlot.Add(&packeg, sizeof(packeg));

                            }, e);
                    }
                }
                executeWhriteByteVareintTime[0] = timer.GetTimePast<TimeUnit>();
            }

            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < actionsCount; i++)
                {
                    for (auto& e : varientByteClassVec)
                    {
                        std::visit([&](auto& stagingSlot) {
                            stagingSlot.Add(&packeg, sizeof(packeg));

                            }, e);

                    }
                }
                executeWhriteByteVareintTime[1] = timer.GetTimePast<TimeUnit>();
            }
        }
        RY_CORE_TRACE("varient Byte clase init: ({1} {0}), exexute[0]: ({2} {0}), exexute[1]: ({3} {0})", timeUinteName, initWhriteByteVareintTime, executeWhriteByteVareintTime[0], executeWhriteByteVareintTime[1]);
    }
    
    static void PerformenzTestByteVsTypeBase()
    {
        std::vector<Ref<IStagingSlot>> baseTypeClassVec;
        std::vector<Ref<IStagingSlot>> baseByteClassVec;
        using TimeUnit = std::chrono::milliseconds;
        constexpr const char* timeUinteName = "milisec";
        constexpr uint32_t count = 10000;
        constexpr uint32_t actionsCount = 64;
        constexpr uint32_t initByteSize = actionsCount * 2;

        baseTypeClassVec.reserve(count);
        LifeTimer timer;

        int64_t initTypeBaseTime;
        int64_t executeTypeBaseTime[2];


        int64_t initByteBaseTime;
        int64_t executeByteBaseTime[2];

        {
            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < count; i++)
                {
                    baseTypeClassVec.emplace_back(CreateRef<StagingSlotAppend<CameraPackage>>(initByteSize));
                }
                initTypeBaseTime = timer.GetTimePast<TimeUnit>();
            }
            CameraPackage packeg;
            std::memset(&packeg, 0, sizeof(CameraPackage));


            {
                timer.BeginNewTimePoint();
                for (auto& e : baseTypeClassVec)
                {
                    for (int i = 0; i < actionsCount; i++)
                    {
                        IStagingSlot::Add(e, packeg);
                    }
                }
                executeTypeBaseTime[0] = timer.GetTimePast<TimeUnit>();
            }

            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < actionsCount; i++)
                {
                    for (auto& e : baseTypeClassVec)
                    {
                        IStagingSlot::Add(e, packeg);
                    }
                }
                executeTypeBaseTime[1] = timer.GetTimePast<TimeUnit>();
            }
        }
        RY_CORE_TRACE("base clase init: ({1} {0}), exexute[0]: ({2} {0}), exexute[1]: ({3} {0})", timeUinteName, initTypeBaseTime, executeTypeBaseTime[0], executeTypeBaseTime[1]);
        {
            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < count; i++)
                {
                    baseByteClassVec.emplace_back(CreateRef<StagingSlotAppendByte>(sizeof(CameraPackage), initByteSize));
                }
                initByteBaseTime = timer.GetTimePast<TimeUnit>();
            }
            CameraPackage packeg;
            std::memset(&packeg, 0, sizeof(CameraPackage));


            {
                timer.BeginNewTimePoint();
                for (auto& e : baseByteClassVec)
                {
                    for (int i = 0; i < actionsCount; i++)
                    {
                        IStagingSlot::Add(e, packeg);
                    }
                }
                executeByteBaseTime[0] = timer.GetTimePast<TimeUnit>();
            }

            {
                timer.BeginNewTimePoint();
                for (int i = 0; i < actionsCount; i++)
                {
                    for (auto& e : baseByteClassVec)
                    {
                        IStagingSlot::Add(e, packeg);
                    }
                }
                executeByteBaseTime[1] = timer.GetTimePast<TimeUnit>();
            }
        }
        RY_CORE_TRACE("base Byte clase init: ({1} {0}), exexute[0]: ({2} {0}), exexute[1]: ({3} {0})", timeUinteName, initByteBaseTime, executeByteBaseTime[0], executeByteBaseTime[1]);
    }

#pragma endregion 

#pragma region BuilPixelCuter
#if 0
    struct ImagePixel
    {
        std::vector<std::vector<glm::u8vec4>> pixel;
        glm::ivec2 position;
    };

    static inline uint32_t PackPixelColor(const uint8_t* pixel, uint32_t channelCount)
    {
        uint32_t color = 0;
        RY_CORE_ASSERT(channelCount <= 4, "To larg pxel bytes!");
        for (uint32_t i = 0; i < channelCount; i++)
        {
            color |= static_cast<uint32_t>(pixel[i]) << (8 * i);
        }
        return color;
    }

    static glm::u8vec4 PackPixelColorVec(const uint8_t* pixel, uint32_t channelCount)
    {
        uint64_t colorBit = 0;
        RY_CORE_ASSERT(channelCount <= 4, "To larg pxel bytes!");
        glm::u8vec4 color{0,0,0,0};
        for (uint32_t i = 0u; i < channelCount; i++)
        {
            const uint32_t channel = pixel[i];
            const uint32_t bitOffset = (8 * i);
            colorBit =  channel << bitOffset;
            color[i] = channel;
        }
        glm::u8vec4 color =
        return color;
    }

    static std::vector<std::vector<glm::u8vec4>> ConvertBitMapInPixelValues(const uint8_t* byteDataPtr, uint32_t byteSize, uint32_t channelCount)
    {
        std::vector<glm::u8vec4> pixel;
        uint32_t pixelCount = byteSize / channelCount;
        pixel.resize(pixelCount);
        for (uint32_t nextI = 3, i = 0u; i < byteSize && nextI < byteSize; i+=4, nextI += 4)
        {
            uint32_t indexPixel = i / channelCount;
            pixel.at(i) = PackPixelColor(byteDataPtr + indexPixel, channelCount);
        }
        return pixel;
    }

    static uint32_t GetLinarIndexFromVec(const glm::ivec2& size, const glm::ivec2& position)
    {
        RY_CORE_ASSERT(position.x < size.x && position.y < size.y, "to larg postion!");
        uint32_t xPos = position.x;
        uint32_t yPos = position.y * size.y;
        uint32_t index = xPos + yPos;
        return index;
    }

    static glm::u8vec4 GetPixelValue(const std::vector<glm::u8vec4>& pixel, const glm::ivec2& size, const glm::ivec2& position)
    {
        uint32_t index = GetLinarIndexFromVec(size, position);
        return pixel.at(index);
    }

    

    
    
    static void AddBitMapInPixelValues(const ImagePixel& orignaleImage, const glm::ivec2& direction, ImagePixel& extraxtImage)
    {
        
        glm::u8vec4 colorSearch = orignaleImage.pixel.at(orignaleImage.position.x).at(orignaleImage.position.y);
        glm::ivec2 pos = orignaleImage.position + direction;
        if (orignaleImage.pixel.size() <= pos.x || orignaleImage.pixel.at(pos.x).size() <= pos.y)
            return;

        glm::u8vec4 color = orignaleImage.pixel.at(pos.x).at(pos.y);
        if(colorSearch != color)
            return;

        glm::ivec2 posnNextPos = extraxtImage.position + direction;
        if (extraxtImage.pixel.size() <= posnNextPos.x)
        {
            extraxtImage.pixel.resize(posnNextPos.x);
        }
        std::vector<glm::u8vec4>& rowVec = extraxtImage.pixel.at(posnNextPos.x);
        if (rowVec.size() <= posnNextPos.y)
        {
            rowVec.resize(posnNextPos.y);
        }
        
        rowVec.at(posnNextPos.y) = color;
        
    }
#endif

#if 0
    static std::vector<uint8_t> ExtractColorRegion(
        const uint8_t* byteDataPtr,
        uint32_t channelCount,
        uint32_t byteSize,
        glm::ivec2 size,
        std::vector<uint32_t>& ignorColorsVec,
        glm::ivec2& outRegionOffset,
        glm::ivec2& outRegionSize)
    {
        outRegionOffset = glm::ivec2(0, 0);
        outRegionSize = glm::ivec2(0, 0);

        // --- Sicherheitschecks ---
        const uint32_t expectedByteSize =
            static_cast<uint32_t>(size.x) * static_cast<uint32_t>(size.y) * channelCount;

        if (byteDataPtr == nullptr ||
            channelCount == 0 || channelCount > 4 ||
            size.x <= 0 || size.y <= 0 ||
            expectedByteSize > byteSize)
        {
            return {};
        }

        auto isIgnored = [&](uint32_t color) -> bool
            {
                return std::find(ignorColorsVec.begin(), ignorColorsVec.end(), color) != ignorColorsVec.end();
            };

        // --- 1. Zielfarbe bestimmen: erste Farbe im Bild, die nicht ignoriert wird ---
        uint32_t targetColor = 0;
        bool foundTarget = false;

        for (int32_t y = 0; y < size.y && !foundTarget; ++y)
        {
            for (int32_t x = 0; x < size.x; ++x)
            {
                const size_t index = (static_cast<size_t>(y) * size.x + x) * channelCount;
                const uint32_t color = PackPixelColor(byteDataPtr + index, channelCount);
                if (!isIgnored(color))
                {
                    targetColor = color;
                    foundTarget = true;
                    break;
                }
            }
        }

        if (!foundTarget)
        {
            RY_CORE_INFO("no Color Found!");
            return {}; // Keine passende (nicht ignorierte) Farbe im Bild gefunden
        }

        // --- 2. Bounding-Box aller Pixel mit der Zielfarbe ermitteln ---
        int32_t minX = std::numeric_limits<int32_t>::max();
        int32_t minY = std::numeric_limits<int32_t>::max();
        int32_t maxX = std::numeric_limits<int32_t>::min();
        int32_t maxY = std::numeric_limits<int32_t>::min();

        for (int32_t y = 0; y < size.y; ++y)
        {
            for (int32_t x = 0; x < size.x; ++x)
            {
                const size_t index = (static_cast<size_t>(y) * size.x + x) * channelCount;
                const uint32_t color = PackPixelColor(byteDataPtr + index, channelCount);
                if (color == targetColor)
                {
                    minX = std::min(minX, x);
                    minY = std::min(minY, y);
                    maxX = std::max(maxX, x);
                    maxY = std::max(maxY, y);
                }
            }
        }

        // Sollte nicht passieren, da targetColor garantiert mindestens einmal vorkommt
        if (maxX < minX || maxY < minY)
        {
            RY_CORE_ASSERT(false);
            return {};
        }

        const int32_t regionWidth = maxX - minX + 1;
        const int32_t regionHeight = maxY - minY + 1;

        outRegionOffset = glm::ivec2(minX, minY);
        outRegionSize = glm::ivec2(regionWidth, regionHeight);

        // --- 3. Hintergrundfarbe für "andere" Pixel bestimmen ---
        // Bei vorhandenem Alpha-Kanal -> transparent, sonst -> weiß.
        uint8_t backgroundPixel[4] = { 255, 255, 255, 255 };
        if (channelCount == 4)
        {
            // RGBA -> komplett transparent
            backgroundPixel[0] = 0;
            backgroundPixel[1] = 0;
            backgroundPixel[2] = 0;
            backgroundPixel[3] = 0;
        }
        else if (channelCount == 2)
        {
            // z.B. Gray+Alpha -> transparent
            backgroundPixel[0] = 0;
            backgroundPixel[1] = 0;
        }
        // Bei 1 oder 3 Kanälen (kein Alpha) bleibt backgroundPixel = weiß (255...)

        // --- 4. Ausschnitt kopieren ---
        std::vector<uint8_t> result(
            static_cast<size_t>(regionWidth) * regionHeight * channelCount);

        for (int32_t y = 0; y < regionHeight; ++y)
        {
            for (int32_t x = 0; x < regionWidth; ++x)
            {
                const size_t outIndex = (static_cast<size_t>(y) * regionWidth + x) * channelCount;

                const int32_t srcX = minX + x;
                const int32_t srcY = minY + y;
                const size_t srcIndex = (static_cast<size_t>(srcY) * size.x + srcX) * channelCount;

                const uint32_t color = PackPixelColor(byteDataPtr + srcIndex, channelCount);

                if (color == targetColor)
                {
                    for (uint32_t c = 0; c < channelCount; ++c)
                    {
                        result[outIndex + c] = byteDataPtr[srcIndex + c];
                    }
                }
                else
                {
                    for (uint32_t c = 0; c < channelCount; ++c)
                    {
                        result[outIndex + c] = backgroundPixel[c];
                    }
                }
            }
        }
        RY_CORE_TRACE("Extraxt Imgag");
        return result;
    }




    static void ExtraxtColorRegionFromImage()
    {
        int width, height, channels, req_comp;
        const char* pathChar = "";
        stbi_uc* dataBytePtr = stbi_load(pathChar, &width, &height, &channels);

        uint32_t byteSize = width * height * channels;
        glm::ivec2 size = { width, height };
        std::vector<uint32_t> ignorColor = {
            0xFFFFFFFF,0x00000000
        };
        glm::ivec2 outRiegionOffset = {
            0,0
        };
        glm::ivec2 outRiegionSize = {
           0,0
        };
        std::vector<uint8_t> colorField = ExtractColorRegion(dataBytePtr, channels, byteSize, size, ignorColor, outRiegionOffset, outRiegionSize);
        
    }
#endif

#pragma endregion


    void EditorLayer::OnAttach()
    {

        RY_CORE_INFO("EditorLayer::OnAttach Start!");
        RY_PROFILE_FUNCTION();

#if 0
        WeightedRandomizer<std::string> randomizer;
        randomizer.add_item("CS-2", 3.0);           // 30 %
        randomizer.add_item("HD-2", 3.0);           // 30 %
        randomizer.add_item("PUBG", 3.0);           // 30 %
        randomizer.add_item("Sea of Thieves", 1.0); // 10 %

        std::string randome = randomizer.get_random();
        RY_CORE_FATAL("Randome Gerator {}", randome);
#endif

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

        m_Content_BPannel.OnAtache();
       
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

            trasC.Transaltion = glm::vec3(
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
        RY_CORE_WARN("OnDetach Aktiv!");
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
        ScriptingEngine::Shutdown();
#endif
        Renderer::ShutdownEditor();
        Renderer::Shutdown();
        Project::ShutDown();

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
            m_Project->SaveActive(m_Project->GetConfig().ProjectPath / (m_Project->GetConfig().Name + ".ryproj"));
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
        serialzer.Deserialize(path);  

        
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
            serialzer.Serialize(filepath);
        }
    }

    void EditorLayer::SaveCurentScene()
    {
        AssetHandle handle = m_AktiveScene->Handle;
        if (m_AssetManger->IsAssetHandleValid(handle))
        {
            const AssetMetadata metadata = m_AssetManger->GetMetadata(handle);
            SceneSerializer serialzer(m_AktiveScene);
            serialzer.Serialize(metadata.FilePath);
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
            ImGui::Text("Curent Scene State: %s", sceneState.c_str());
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
            if (ImGui::Button("Pause", ImVec2(100, 0)))
            {
                m_SceneState = SceneState::Edit;
                m_AktiveScene->OnRuntimStop();
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

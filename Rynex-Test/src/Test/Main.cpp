#include <rypch.h>
#include <Test/FilesSystemTest.h>
#include <Test/CastSafeTest.h>

#include <Rynex/Core/UUID.h>
#include <Rynex/Scene/Scene.h>
#include <Rynex/Scene/Entity.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Serializers/StaticMeshSerializer.h>

#include "Rynex/Project/Project.h"


int main(int argc, char** argv)
{
    std::filesystem::path workingDir = std::filesystem::current_path();
    workingDir = workingDir.parent_path();
    std::filesystem::current_path(workingDir);

    ::testing::InitGoogleTest(&argc, argv);
    Rynex::Log::Get().Init();
    Rynex::AssertHook::Set(&Rynex::Testing::ThrowHandler);



    const int result = RUN_ALL_TESTS();


    Rynex::Log::Get().Shutdown();
    return result;
}

#pragma region TestFunctions
namespace RynexTestEditor {
#pragma region TestRenderPipline

    static void CreateStaticMeshEntity(const std::string& name,  Rynex::Ref<Rynex::Scene> scene, const glm::mat4& matrix,  Rynex::Ref<Rynex::MeshStatic> meshStatic)
    {
        Rynex::Entity entiy = scene->CreateEntity(name);
        Rynex::ModelMangerComponent& staticMesh = entiy.AddComponent<Rynex::ModelMangerComponent>();
        staticMesh.m_MeshStatic = meshStatic;

        Rynex::TransformComponent& transC = entiy.GetComponent<Rynex::TransformComponent>();
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
        const Rynex::UUID& meshSourceEntityUUID = entiy.GetUUID();
        std::vector<Rynex::UUID>& meshSingleChildrenVec = staticMesh.m_SingleMeshes;
        const std::vector<Rynex::MeshStatic::SingleObjectMeshData>& meshSingleVec = meshStatic->GetSingleObjectMesDataVec();
        meshSingleChildrenVec.reserve(meshSingleVec.size());
#if 0
        uint32_t i = 0;
        uint32_t countFormTo = 5;
        uint32_t form = 18;
        uint32_t to = form + countFormTo;

        for(const  Rynex::MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
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
        for (const  Rynex::MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
        {
#endif
            const  Rynex::Ref<Rynex::Material>& materiel = meshSingle.m_Material;
            const  Rynex::Ref<Rynex::MeshSingle>& meshSingel = meshSingle.m_MeshSingle;
            const glm::mat4& matrix = meshSingle.m_LocaleCildrenMatrix;
            const std::string& name = meshSingle.NodeName;

            Rynex::Entity e = entiy.AddChildrenEntity(name);
            meshSingleChildrenVec.emplace_back(e.GetUUID());
            Rynex::StaticMeshComponent& singleStaticMesh =
                e.AddComponent<Rynex::StaticMeshComponent>(
                    meshSourceEntityUUID
                    , meshSingel
                    , materiel
                );

            Rynex::TransformComponent& transMeshChildeC = e.GetComponent<Rynex::TransformComponent>();
            transMeshChildeC.SetTransform(matrix);
            e.SetVisable(false);
            e.UpdateMatrix();
        }
#endif
    }

    static void CreateStaticMeshEntity(const std::string& name, Rynex::Entity& e, const glm::mat4& matrix,  Rynex::Ref<Rynex::MeshStatic> meshStatic)
    {
        Rynex::Entity entiy = e.AddChildrenEntity(name);
        Rynex::ModelMangerComponent& staticMesh = entiy.AddComponent<Rynex::ModelMangerComponent>();
        staticMesh.m_MeshStatic = meshStatic;

        Rynex::TransformComponent& transC = entiy.GetComponent<Rynex::TransformComponent>();
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
        const Rynex::UUID& meshSourceEntityUUID = entiy.GetUUID();
        std::vector<Rynex::UUID>& meshSingleChildrenVec = staticMesh.m_SingleMeshes;
        const std::vector<Rynex::MeshStatic::SingleObjectMeshData>& meshSingleVec = meshStatic->GetSingleObjectMesDataVec();
        meshSingleChildrenVec.reserve(meshSingleVec.size());

        for (const Rynex::MeshStatic::SingleObjectMeshData& meshSingle : meshSingleVec)
        {
            const  Rynex::Ref<Rynex::Material>& materiel = meshSingle.m_Material;
            const  Rynex::Ref<Rynex::MeshSingle>& meshSingel = meshSingle.m_MeshSingle;
            const glm::mat4& matrix = meshSingle.m_LocaleCildrenMatrix;
            const std::string& name = meshSingle.NodeName;

            Rynex::Entity e = entiy.AddChildrenEntity(name);
            meshSingleChildrenVec.emplace_back(e.GetUUID());
            Rynex::StaticMeshComponent& singleStaticMesh =
                e.AddComponent<Rynex::StaticMeshComponent>(
                    meshSourceEntityUUID
                    , meshSingel, materiel
                );

            Rynex::TransformComponent& transMeshChildeC = e.GetComponent<Rynex::TransformComponent>();
            transMeshChildeC.SetTransform(matrix);
            e.SetVisable(false);
            e.UpdateMatrix();
        }
#endif
    }


    static void TestProfileRenderShaderMapSubmit(Rynex::Ref<Rynex::MeshStatic> cube,  Rynex::Ref<Rynex::MeshStatic> cube2,  Rynex::Ref<Rynex::MeshStatic> ship,  Rynex::Ref<Rynex::MeshStatic> cv,  Rynex::Ref<Rynex::MeshStatic> sponzer,  Rynex::Ref<Rynex::MeshStatic> sponzer2,  Rynex::Ref<Rynex::Scene> scene)
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


        Rynex::Ref<Rynex::Shader> shader = Rynex::AssetManager::GetAsset<Rynex::Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShader.glsl"));

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
        int mode = Rynex::RenderMode::Death_Buffer | Rynex::RenderMode::CallFace_Front | Rynex::RenderMode::A_Buffer;
#if 1
        glm::vec4 viewSpaceComplet = glm::vec4(0.0f, 0.0f, viewSpace.z , viewSpace.w);
#else
        glm::vec4 viewSpaceComplet = glm::vec4(0.0f, 0.0f, viewSpace.z * RY_SHADOW_COUNT, viewSpace.w);
#endif
        uint32_t withe = viewSpaceComplet.z;
        uint32_t heigth = viewSpaceComplet.w;

        Rynex::FramebufferSpecification fbSpec = {
            withe, heigth, 1u,
            {
                    { Rynex::TexFrom::DepthComp24, 1, { Rynex::TexWarp::ClampEdge, Rynex::TexWarp::ClampEdge, Rynex::TexWarp::ClampEdge }, Rynex::TexFilter::Linear, Rynex::TexComp::Lequal }
            }
        };

        glm::vec4 viewSpaceImgageSize = viewSpace;
        for (uint32_t i = 0; i < RY_SHADOW_COUNT; i++)
        {
#if 1
            Rynex::Ref<Rynex::Framebuffer> fb = Rynex::Framebuffer::Create(fbSpec);
            fb->Resize2D(viewSpace.z, viewSpace.w);
#ifndef RY_RENERER_DESIGN_CURENT_MAIN
            uint32_t index = Renderer::SetPassView(
                "Shadow",
                ViewPassData( projection, view,
                position, background, viewSpaceComplet, mode, 2.2f, fb), ViewPassType::Shadow);
#elif 1

            Rynex::Entity entity = scene->CreateEntity("Directional");
            Rynex::CameraComponent& camnerC = entity.AddComponent<Rynex::CameraComponent>();
            camnerC.m_Camera.SetOrthoGraphic(size, nearClip, farClip);
            camnerC.m_Primary = false;
            Rynex::RenderTargetComponent& renderTargetC = entity.AddComponent<Rynex::RenderTargetComponent>();
            Rynex::ModelMatrixComponent& modelC = entity.GetComponent<Rynex::ModelMatrixComponent>();
            modelC.m_Locale = glm::inverse(view);

            renderTargetC.m_Target = Rynex::CreateRef<Rynex::RenderTarget>(fb);
            const glm::uvec2& size = fb->GetFramebufferSize();
            glm::ivec2 sizeInt = static_cast<glm::ivec2>(size);
            glm::ivec4 viewSize{ sizeInt.x, sizeInt.y, 0, 0, };

            renderTargetC.m_RenderPassName = "Shadow";
            renderTargetC.m_StoreIndex = 0xFFFFFFFFu;
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
        cube = Rynex::Mesh::CreateStaticMesh(Rynex::FileSystem::Path("Assets/Models/Cube.gltf"));

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
        Rynex::Ref<MeshSource> cubeSource = cube->GetMeshSource();
        std::vector<Rynex::MeshStatic::SingleObjectMeshData> singleMeshDatasVec = cube->GetSingleObjectMesDataVec();
        Rynex::Ref<Rynex::Material> materiel = singleMeshDatasVec.at(0).m_Material;
        MaterielShaderData data = Rynex::Material::GetMaterielDataFromMateriel<MaterielShaderData>(materiel);
        data.AmbientLigthe = 0.15f;
        data.Color = glm::vec3(0.11f, data.Color.g, 0.11f);
        Rynex::Ref<Rynex::Texture> tex = materiel->GetAlbedoTextures();
        singleMeshDatasVec.at(0).m_Material = Rynex::CreateRef<DefaultMaterial>(data, tex);
        Rynex::Ref<Rynex::MeshStatic> cubePlane = Rynex::CreateRef<Rynex::MeshStatic>(cubeSource, singleMeshDatasVec);
        CreateStaticMeshEntity("Plane", scene, matrixPlane, cubePlane);
        float multiyplyerCube = 2.5f;
        float quda = 25.f;
        float qudaHalf = quda / 2.0f;
        for(float x = 0; x < quda; x++)
        {
            Rynex::Entity rowEntity = scene->CreateEntity("Colume (" + std::to_string(static_cast<int>(x))+ ")");
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
        Rynex::Entity camerE = scene->CreateEntity("Camera");
        camerE.AddComponent<CameraComponent>();
#endif

    }

    static void TestProfileRenderShaderMapRemove()
    {
        Rynex::Ref<Rynex::MeshStatic> ship;
        Rynex::Ref<Rynex::MeshStatic> cv;
        Rynex::Ref<Rynex::MeshStatic> cube;
        Rynex::Ref<Rynex::MeshStatic> cube2;

        Rynex::Ref<Rynex::MeshStatic> sponzer;
        Rynex::Ref<Rynex::MeshStatic> sponzer2;

        ship = Rynex::Mesh::CreateStaticMesh(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/Yamto-Model/scene.gltf"));
        cv = Rynex::Mesh::CreateStaticMesh(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/CV-Model/scene.gltf"));


        cube = Rynex::Mesh::CreateStaticMesh(Rynex::FileSystem::Path("Assets/Models/Cube.gltf"));
        cube2 = Rynex::Mesh::CreateStaticMesh(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/Cube2.gltf"));

        sponzer = Rynex::Mesh::CreateStaticMesh(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/main_sponza/main_sponza/NewSponza_Main_glTF_003.gltf"));
        sponzer2 = Rynex::Mesh::CreateStaticMesh(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/main_sponza/main_sponza/NewSponza_Main_glTF_003.gltf"));

        Rynex::Ref<Rynex::Shader> shader = Rynex::AssetManager::GetAsset<Rynex::Shader>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Shaders/MeshTestShader.glsl"));


    }

    
#pragma endregion

#pragma region TestStaticMeshScene

    static  Rynex::Ref<Rynex::MeshStatic> TestDeserliceStaticMesh(const std::filesystem::path& filePath)
    {
        Rynex::Ref<Rynex::MeshStatic> mesh = Rynex::CreateRef<Rynex::MeshStatic>();
        Rynex::StaticMeshSerializer serialzation(mesh);


        Rynex::Ref<Rynex::MeshStatic> meshOrig;
        Rynex::FileSystem::Path path(filePath);
        meshOrig = Rynex::Mesh::CreateStaticMesh(path);

        std::filesystem::path projectPath = Rynex::Project::GetActiveProjectDirectory();
        std::string fileStr = filePath.string();
        size_t index = fileStr.find('.');
        size_t size = fileStr.size();
        RY_CORE_ASSERT(index < size);

        std::string fileStaticMeshStr = fileStr.substr(0, index);
        fileStaticMeshStr += ".rystmesh";

        std::filesystem::path filePathStMesh = projectPath / fileStaticMeshStr;
        RY_CORE_ASSERT(mesh->GetSingleMeshObjectCount() != meshOrig->GetSingleMeshObjectCount());

        serialzation.Deserialize(Rynex::FileSystem::Path(filePathStMesh));
        RY_CORE_ASSERT(mesh->GetSingleMeshObjectCount() == meshOrig->GetSingleMeshObjectCount());
        return mesh;
    }

    static void TestSerliceStaticMesh(const std::filesystem::path& filePath)
    {
        Rynex::FileSystem::Path path(filePath);
        Rynex::Ref<Rynex::MeshStatic> mesh = Rynex::Mesh::CreateStaticMesh(path);
        Rynex::StaticMeshSerializer serialzation(mesh);

        std::filesystem::path projectPath = Rynex::Project::GetActiveProjectDirectory();
        std::string fileStr = filePath.string();
        size_t index = fileStr.find('.');
        size_t size = fileStr.size();
        RY_CORE_ASSERT(index < size);

        std::string fileStaticMeshStr = fileStr.substr(0, index);
        fileStaticMeshStr += ".rystmesh";

        std::filesystem::path filePathStMesh = projectPath / fileStaticMeshStr;
        serialzation.Serialize(Rynex::FileSystem::Path(filePathStMesh));
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
        Rynex::Ref<Rynex::Scene> scene = Rynex::CreateRef<Rynex::Scene>();
        Rynex::Ref<Rynex::Texture> tex = nullptr;

        Rynex::Entity entity = scene->CreateEntity("hi-Test-asycn-loding");
        entity.AddComponent<Rynex::SpriteRendererComponent>();
        int entityID = entity.GetEntityHandle();
        int entityID2 = entityID + 1;


        Rynex::Ref<Rynex::Scene> sceneCopy = Rynex::Scene::Copy(scene);

        std::function<void(Rynex::Ref<Rynex::Texture>,  Rynex::Ref<Rynex::Scene>, int)> onSceneAssetLoadedEntityFunc = Rynex::Entity::OnAssetLoded<Rynex::SpriteRendererComponent, Rynex::Texture>;
        Rynex::Ref<Rynex::LodePromisType<Rynex::Texture, Rynex::Scene, int>> loadePromis = Rynex::CreateRef<Rynex::LodePromisType<Rynex::Texture, Rynex::Scene, int>>(scene, onSceneAssetLoadedEntityFunc, entityID2);

        loadePromis->ChangeFuncArgs(entityID);
        Rynex::AssetManager::GetAssetAsyncPromis<Rynex::Texture>(RY_DEFAULT_PATH_TO_PROJECT("Assets/Models/main_sponza/main_sponza/textures/metal_door_01_BaseColor.png"), loadePromis);

        loadePromis->AddRefObject(sceneCopy, onSceneAssetLoadedEntityFunc);

        loadePromis->WaitForLoding();

        {
            const Rynex::SpriteRendererComponent& sprite = entity.GetComponent<Rynex::SpriteRendererComponent>();
            const Rynex::Weak<Rynex::Texture>& textureWeak = sprite.m_Texture;
            Rynex::Ref<Rynex::Texture> texture = textureWeak.lock();
            RY_CORE_ASSERT(nullptr != texture, "No Rynex::Texture Set");
        }
        Rynex::Entity entityCopy = scene->GetEntityByName("hi-Test-async-loading");
        RY_CORE_ASSERT(entityCopy )
        {

            const Rynex::SpriteRendererComponent& sprite = entityCopy.GetComponent<Rynex::SpriteRendererComponent>();
            const Rynex::Weak<Rynex::Texture>& textureWeak = sprite.m_Texture;
            Rynex::Ref<Rynex::Texture> texture = textureWeak.lock();
            RY_CORE_ASSERT(nullptr != texture, "No Rynex::Texture Set");
        }

    }

    static void TestPtrtoRefPtr0()
    {
        Rynex::Ref<Rynex::Scene> origelScene = Rynex::CreateRef<Rynex::Scene>();
        Rynex::Entity entity = origelScene->CreateEntity("hi-Test-RefScene-FromScenePtr");
        entity.AddComponent<Rynex::SpriteRendererComponent>();

        const Rynex::Scene* sceneConstPtr = entity.GetScenePtr();
        Rynex::Scene* scenePtr = const_cast<Rynex::Scene*>(sceneConstPtr);
        Rynex::Asset* assetPtr = reinterpret_cast<Rynex::Asset*>(scenePtr);

        Rynex::Ref<Rynex::Asset> sceneAsset = Rynex::Asset::GetRefInPlace(assetPtr);
        Rynex::Ref<Rynex::Scene> sceneRef = std::static_pointer_cast<Rynex::Scene, Rynex::Asset>(sceneAsset);
        sceneAsset.reset();
        RY_CORE_ASSERT(sceneRef == origelScene);
        RY_CORE_ASSERT(sceneRef.use_count() == 2);
        RY_CORE_ASSERT(origelScene.use_count() == 2);

        origelScene.reset();
        RY_CORE_ASSERT(sceneRef.use_count() == 1);
    }

    static void TestPtrtoRefPtr1()
    {
        Rynex::Ref<Rynex::Scene> origelScene = Rynex::CreateRef<Rynex::Scene>();
        Rynex::Entity entity = origelScene->CreateEntity("hi-Test-RefScene-FromScenePtr");
        entity.AddComponent<Rynex::SpriteRendererComponent>();
        origelScene.reset();
        Rynex::Ref<Rynex::Scene> sceneRef = entity.GetScene();
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

    static void TestCubeAnd3DTextures(Rynex::Ref<Rynex::Scene>& aktiveScene)
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
            Rynex::TextureSpecification spec{
                withe, height, depth,
                Rynex::TextureTarget::TextureCubeMap,
                Rynex::TextureFormat::RGBA8,
                1u,
            };
            Rynex::Ref<Rynex::Texture> cubeMapTest = Rynex::Texture::Create(spec, cubeMapVec.data(), cubeMapVec.size());
            cubeMapTest->Bind(1u);
            cubeMapTest->UnBind();
            for (uint8_t& value : cubeMapVec)
                value = 0xf0;
            cubeMapTest->SetData(cubeMapVec.data(), cubeMapVec.size());

            cubeMapTest->Bind(1u);
            cubeMapTest->UnBind();

            Rynex::Entity entiy = aktiveScene->CreateEntity("Test ");
            Rynex::SpriteRendererComponent& spriteC = entiy.AddComponent<Rynex::SpriteRendererComponent>();
            Rynex::TransformComponent& trasC = entiy.GetComponent<Rynex::TransformComponent>();
            spriteC.m_Texture = cubeMapTest;
            spriteC.m_Color.b = 0.5f;
            trasC.m_Scale *= 2.0f;
            trasC.m_Transform.x = 4.0f;
            Rynex::AssetManager::CreatLocaleAsset(cubeMapTest);

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
            Rynex::TextureSpecification spec{
                withe, height, depth,
                Rynex::TextureTarget::TextureCubeMap,
                Rynex::TextureFormat::RGBA8,
                1u,
            };
            Rynex::Ref<Rynex::Texture> cubeMapTest = Rynex::Texture::Create(spec, cubeMapVec.data(), cubeMapVec.size());
            cubeMapTest->Bind(1u);
            cubeMapTest->UnBind();

            cubeMapVec.resize(texturesByteSize, pixelData2);
            cubeMapTest->SetData(cubeMapVec.data(), cubeMapVec.size());

            cubeMapTest->Bind(1u);
            cubeMapTest->UnBind();

            Rynex::Entity entiy = aktiveScene->CreateEntity("Test cubeMapTest resize");
            Rynex::SpriteRendererComponent& spriteC = entiy.AddComponent<Rynex::SpriteRendererComponent>();
            Rynex::TransformComponent& trasC = entiy.GetComponent<Rynex::TransformComponent>();
            Rynex::AssetManager::CreatLocaleAsset(cubeMapTest);
            spriteC.m_Color.b = 0.5f;

            spriteC.m_Texture = cubeMapTest;
            trasC.m_Scale *= 2.0f;
            trasC.m_Transform.x = 0.0f;
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
            Rynex::Ref<Rynex::Texture> cubeMapTest = nullptr;

            {
                std::vector<uint8_t> cubeMapVec;
                cubeMapVec.resize(texturesDataByteSize, pixelData);
                Rynex::TextureSpecification spec{
                    withe, height, depth,
                    Rynex::TextureTarget::TextureCubeMap,
                    Rynex::TextureFormat::RGBA8,
                    1u,
                };
                cubeMapTest = Rynex::Texture::Create(spec);
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

            Rynex::Entity entiy = aktiveScene->CreateEntity("Test ");
            Rynex::SpriteRendererComponent& spriteC = entiy.AddComponent<Rynex::SpriteRendererComponent>();
            Rynex::TransformComponent& trasC = entiy.GetComponent<Rynex::TransformComponent>();
            spriteC.m_Color.b = 0.5f;

            spriteC.m_Texture = cubeMapTest;
            Rynex::AssetManager::CreatLocaleAsset(cubeMapTest);
            trasC.m_Scale *= 2.0f;
            trasC.m_Transform.x = 0.0f;

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
            Rynex::TextureSpecification spec{
                withe, height, depth,
                Rynex::TextureTarget::Texture3D,
                Rynex::TextureFormat::RGBA8,
                1u,
            };
            Rynex::Ref<Rynex::Texture> texture3D = Rynex::Texture::Create(spec, texture3DDataVec.data(), texture3DDataVec.size());
            texture3D->Bind(1u);
            texture3D->UnBind();
            for (uint8_t& value : texture3DDataVec)
                value = 0x00;
            texture3D->SetData(texture3DDataVec.data(), texture3DDataVec.size());

            texture3D->Bind(1u);
            texture3D->UnBind();

            Rynex::Entity entiy = aktiveScene->CreateEntity("Test 3d Rynex::Texture");
            Rynex::SpriteRendererComponent& spriteC = entiy.AddComponent<Rynex::SpriteRendererComponent>();
            Rynex::TransformComponent& trasC = entiy.GetComponent<Rynex::TransformComponent>();
            spriteC.m_Color.b = 0.5f;

            spriteC.m_Texture = texture3D;
            trasC.m_Scale *= 2.0f;
            trasC.m_Transform.x = -2.0f;
            Rynex::AssetManager::CreatLocaleAsset(texture3D);

            entiy.UpdateMatrix();

        }
    }

    static void TestArrayTextures(Rynex::Ref<Rynex::Scene>& activeScene)
    {

        {
            constexpr uint32_t size = 4u;
            constexpr uint32_t textureCount = 3u;
            constexpr uint8_t pixelData = 0xff;
            constexpr uint32_t pixelByteSize = sizeof(uint32_t);
            uint32_t withe = size;
            uint32_t height = size + 1u;
            uint32_t depth = 1u;


            Rynex::TextureSpecification spec{
                withe, height, depth,
                Rynex::TextureTarget::Texture2D,
                Rynex::TextureFormat::RGBA8,
                1u,
            };
            Rynex::TextureSpecification specArray = spec;
            specArray.m_Target = Rynex::TextureTarget::Texture2D_Array;
            Rynex::Ref<Rynex::LinkedTextureArray> linkedTextureArray = Rynex::LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);
            linkedTextureArray->ResizeTextureArray(3);

            Rynex::Ref<Rynex::Texture> texture0 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture1 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture2 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture3 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());


            linkedTextureArray->SetTextureToArray(0,texture0);
            linkedTextureArray->SetTextureToArray(1,texture1);
            linkedTextureArray->SetTextureToArray(2,texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());

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


            Rynex::TextureSpecification spec{
                withe, height, depth,
                Rynex::TextureTarget::Texture2D,
                Rynex::TextureFormat::RGBA8,
                1u,
            };

            Rynex::TextureSpecification specArray = spec;
            specArray.m_Target = Rynex::TextureTarget::Texture2D_Array;
            Rynex::Ref<Rynex::LinkedTextureArray> linkedTextureArray = Rynex::LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Rynex::Ref<Rynex::Texture> texture0 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture1 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture2 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture3 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());

            linkedTextureArray->ResizeTextureArray(3);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind(1u);

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
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


            Rynex::TextureSpecification spec{
                withe, height, depth,
                Rynex::TextureTarget::Texture2D,
                Rynex::TextureFormat::RGBA8,
                1u,
            };

            Rynex::TextureSpecification specArray = spec;
            specArray.m_Target = Rynex::TextureTarget::Texture2D_Array;
            Rynex::Ref<Rynex::LinkedTextureArray> linkedTextureArray = Rynex::LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Rynex::Ref<Rynex::Texture> texture0 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture1 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture2 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture3 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());

            linkedTextureArray->ResizeTextureArray(4);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->SetTextureToArray(3, texture3);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind(1u);

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
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

            Rynex::TextureSpecification spec{
                withe, height, depth,
                Rynex::TextureTarget::Texture2D,
                Rynex::TextureFormat::RGBA8,
                1u,
            };

            Rynex::TextureSpecification specArray = spec;
            specArray.m_Target = Rynex::TextureTarget::Texture2D_Array;
            Rynex::Ref<Rynex::LinkedTextureArray> linkedTextureArray = Rynex::LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Rynex::Ref<Rynex::Texture> texture0 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture1 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture2 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture3 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());

            linkedTextureArray->ResizeTextureArray(4);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->SetTextureToArray(3, texture3);
            linkedTextureArray->SetTextureToArray(3, nullptr);
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind(1u);

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
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


            Rynex::TextureSpecification spec{
                withe, height, depth,
                Rynex::TextureTarget::Texture2D,
                Rynex::TextureFormat::RGBA8,
                1u,
            };

            Rynex::TextureSpecification specArray = spec;
            specArray.m_Target = Rynex::TextureTarget::Texture2D_Array;
            Rynex::Ref<Rynex::LinkedTextureArray> linkedTextureArray = Rynex::LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Rynex::Ref<Rynex::Texture> texture0 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture1 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture2 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture3 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());

            linkedTextureArray->ResizeTextureArray(4);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->SetTextureToArray(3, texture3);
            linkedTextureArray->SetTextureToArray(3, nullptr);
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind();

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
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


            Rynex::TextureSpecification spec{
                withe, height, depth,
                Rynex::TextureTarget::Texture2D,
                Rynex::TextureFormat::RGBA8,
                1u,
            };

            Rynex::TextureSpecification specArray = spec;
            specArray.m_Target = Rynex::TextureTarget::Texture2D_Array;
            Rynex::Ref<Rynex::LinkedTextureArray> linkedTextureArray = Rynex::LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Rynex::Ref<Rynex::Texture> texture0 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture1 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture2 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture3 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            pixelDataVec.resize(pixelCount, pixelData);

            linkedTextureArray->ResizeTextureArray(4);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
            texture1->SetData(pixelDataVec.data(), pixelDataVec.size());
            RY_CORE_ASSERT(!linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->UpdateDataGPU();

            linkedTextureArray->SetTextureToArray(3, texture3);
            linkedTextureArray->SetTextureToArray(3, nullptr);
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind();

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
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


            Rynex::TextureSpecification spec{
                withe, height, depth,
                Rynex::TextureTarget::Texture2D,
                Rynex::TextureFormat::RGBA8,
                1u,
            };

            Rynex::TextureSpecification specArray = spec;
            specArray.m_Target = Rynex::TextureTarget::Texture2D_Array;
            Rynex::Ref<Rynex::LinkedTextureArray> linkedTextureArray = Rynex::LinkedTextureArray::Create(specArray);

            std::vector<uint8_t> pixelDataVec;
            uint32_t byteSize = withe * height * depth * pixelByteSize;
            uint32_t pixelCount = withe * height * depth;
            pixelDataVec.resize(pixelCount, pixelData);


            Rynex::Ref<Rynex::Texture> texture0 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture1 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture2 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            Rynex::Ref<Rynex::Texture> texture3 = Rynex::Texture::Create(spec, pixelDataVec.data(), pixelDataVec.size());
            pixelDataVec.resize(pixelCount, pixelData);

            linkedTextureArray->ResizeTextureArray(4);
            linkedTextureArray->SetTextureToArray(0, texture0);
            linkedTextureArray->SetTextureToArray(1, texture1);
            linkedTextureArray->SetTextureToArray(2, texture2);
            RY_CORE_ASSERT(!linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
            texture1->SetData(pixelDataVec.data(), pixelDataVec.size());
            RY_CORE_ASSERT(!linkedTextureArray->IsDataReadyOnGPU());
            linkedTextureArray->UpdateDataGPU();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());

            linkedTextureArray->SetTextureToArray(3, texture3);
            linkedTextureArray->SetTextureToArray(3, nullptr);
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());

            linkedTextureArray->Bind(1u);
            linkedTextureArray->UnBind();

            linkedTextureArray->ClearTextures();
            RY_CORE_ASSERT(linkedTextureArray->IsDataReadyOnGPU());
        }



    }
}

#pragma endregion

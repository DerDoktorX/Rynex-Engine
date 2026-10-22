//
// Created by Jonathan.S on 22.10.2026.
//
#include <rypch.h>
#include "TestDrawListSystem.h"


#include <Rynex/Renderer/ChachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/DrawList/Contoller/SingleMeshControllers.h>

namespace Utils {
    static void OnEvent(Rynex::Event& envent)
    {

    }

}
void TestDrawListSystem::SetUp()
{
    Rynex::WindowProps windowProps(
    "Rynex Test Window",
    1, 1
    );

    m_Window = Rynex::Window::Create(windowProps);
    // m_Window->SetEventCallback(Utils::OnEvent);
    m_Window->SetVSync(false);
    m_Window->OnUpdate();
}

void TestDrawListSystem::TearDown()
{
    RY_DESTROY_REF(m_Window);
}

TEST_F(TestDrawListSystem, BasicInitWindow)
{
    RY_CORE_INFO("Log Test");
}

TEST_F(TestDrawListSystem, BasicInitClasse)
{
    Rynex::CachedBufferRegister bufferRegister{};
    Rynex::DrawListSingleMesh drawListSingleMesh{};
}


TEST_F(TestDrawListSystem, callMethodes)
{
    Rynex::CachedBufferRegister bufferRegister{};
    Rynex::DrawListSingleMesh drawListSingleMesh{};

    Rynex::Ref<Rynex::Material> material = nullptr;
    EXPECT_ASSERT(drawListSingleMesh.SetMateriel(material));

    Rynex::Ref<Rynex::MeshSingle> meshSingle = nullptr;
    EXPECT_ASSERT(drawListSingleMesh.SetGeometry(meshSingle));

    glm::mat4 modelMatrix(1.0f);
    uint32_t entityID = 0;
    EXPECT_ASSERT(drawListSingleMesh.Add(modelMatrix, entityID));


}

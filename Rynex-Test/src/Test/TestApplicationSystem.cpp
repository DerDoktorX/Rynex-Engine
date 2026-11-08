// CachedBufferRegister created in Project Rynex-Rendering on 22/10/2026.
#include <rypch.h>
#include "TestApplicationSystem.h"

#include <Rynex/Core/Application.h>
#include <Rynex/Renderer/Rendering/Renderer.h>

namespace {
    static void OnEvent(Rynex::Event& event)
    {

    }
}



void TestApplicationSystem::SetUp()
{

    using namespace Rynex;

    Rynex::ApplicationSpecification spec;
    spec.Name = "Rynex Test Window";

    char* args[]{
        "--WindowWidth", "1",
        "--WindowHeight", "1",
        "--WindowVisibility", "false",
        "--ThreadPoolCount", "0"
    };


    spec.CommandLineArgs.Args = args;
    spec.CommandLineArgs.Count = 8;

    m_Application = CreateScope<TestApplication>(spec);
    GetWindow().SetVSync(false);
    GetWindow().OnUpdate();
    FilesSystemTest::SetUp();
    Renderer::Init();
    Renderer::InitEditor();

    GetWindow().OnUpdate();
}

void TestApplicationSystem::TearDown()
{
    using namespace Rynex;

    m_Application->RunShutdown();

    FilesSystemTest::TearDown();
    Renderer::ShutdownEditor();
    Renderer::Shutdown();

    RY_DESTROY_SCOPE(m_Application);
}

Rynex::Window& TestApplicationSystem::GetWindow()
{
    return m_Application->GetWindow();
}

TEST_F(TestApplicationSystem, basicInitWorkingAPI)
{
    const int test[] {127, 63, 31, 15};

    Rynex::Ref<Rynex::UniformBuffer> uniformBuffer = Rynex::UniformBuffer::Create(&test, sizeof(test));
}

TEST_F(TestApplicationSystem, TestVSyncTrue)
{
     GetWindow().SetVSync(true);
}

TEST_F(TestApplicationSystem, TestVSyncFALSE)
{
    GetWindow().SetVSync(false);
}

// TestControllerSystem created in Project Rynex-Rendering on 13/11/2026.
#include <rypch.h>
#include "TestGeometryControllerSystem.h"

#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/DrawList/Contoller/Geomtry/MeshGeometry.h>


void TestGeometryControllerSystem::SetUp()
{
    TestApplicationSystem::SetUp();
    Rynex::FileSystem::Path path("Engine-Resources/Editor-Assets/Models/Cube.rystmesh");
    m_Cache = Rynex::CreateRef<Rynex::CachedBufferRegister>();
    m_MeshStatic = Rynex::AssetManager::GetAsset<Rynex::MeshStatic>(path);
}

void TestGeometryControllerSystem::TearDown()
{
    RY_DESTROY_REF(m_MeshStatic);
    RY_DESTROY_REF(m_Cache);
    TestApplicationSystem::TearDown();
}


TEST_F(TestGeometryControllerSystem, CompileTest)
{
    using namespace  Rynex::Control;
    GeometryDefault::Interface geometryInterface;
}

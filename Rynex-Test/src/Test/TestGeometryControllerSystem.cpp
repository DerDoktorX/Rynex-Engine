// TestControllerSystem created in Project Rynex-Rendering on 13/11/2026.
#include <rypch.h>
#include "TestGeometryControllerSystem.h"

#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/DrawList/Contoller/Geomtry/MeshGeometry.h>


void TestGeometryControllerSystem::SetUp()
{
    TestApplicationSystem::SetUp();
    constexpr size_t byteSize = 1E9; // 1 gigabyte = 1 * 10^9

    m_Cache = Rynex::CreateRef<Rynex::CachedBufferRegister>();
    m_Cache->ReservedRegisterGpuMaxGpuMemoryUsage(byteSize);
    m_Cache->SetFrame(0);

    Rynex::FileSystem::Path pathMesh("Engine-Resources/Editor-Assets/Models/Cube.rystmesh");
    m_MeshStatic = Rynex::AssetManager::GetAsset<Rynex::MeshStatic>(pathMesh);

}

void TestGeometryControllerSystem::TearDown()
{
    RY_DESTROY_REF(m_MeshStatic);
    RY_DESTROY_REF(m_Cache);
    TestApplicationSystem::TearDown();
}

const Rynex::Ref<Rynex::MeshSingle>& TestGeometryControllerSystem::GetMeshSingle() const
{
    const uint32_t count = m_MeshStatic->GetSingleMeshObjectCount();
    EXPECT_EQ(1u, count);
    const Rynex::MeshStatic::SingleObjectMeshData& singleObjectMeshData = m_MeshStatic->GetSingleObjectData(0);
    const Rynex::Ref<Rynex::MeshSingle>& meshSingle = singleObjectMeshData.m_MeshSingle;
    EXPECT_NE(nullptr, meshSingle);
    return meshSingle;
}


TEST_F(TestGeometryControllerSystem, BasicCompile)
{
    InterfaceGeometryType geometryInterface;

    static_assert(std::is_same_v<InterfaceGeometryType::Source, Rynex::Ref<Rynex::MeshSingle>>, "Source from Interface is not expected Type.");
    static_assert(std::is_same_v<InterfaceGeometryType::Section, Rynex::Ref<Rynex::MeshSingle>>, "Section from Interface is not expected Type.");
    static_assert(std::is_same_v<InterfaceGeometryType::Indices, int>, "Indices from Interface is not expected Type.");

    Rynex::ShaderDrawResource drawResource;
    Rynex::CachedBufferRegister& cachedBufferRegister = *(m_Cache.get());
    BuildGeometryType build{
        cachedBufferRegister, geometryInterface, drawResource
    };
    static_assert(std::is_same_v<BuildGeometryType::Interface, InterfaceGeometryType>, "Interface from Build is not expected Type.");


    ControlSlotGeometryType controlSlot{};

    static_assert(std::is_same_v<ControlSlotGeometryType::Source, InterfaceGeometryType::Source>, "Source from ControlSlot is not expected Type.");
    static_assert(std::is_same_v<ControlSlotGeometryType::Interface, InterfaceGeometryType>, "Interface from ControlSlot is not expected Type.");
    static_assert(std::is_same_v<ControlSlotGeometryType::Build, BuildGeometryType>, "Build from ControlSlot is not expected Type.");


}


TEST_F(TestGeometryControllerSystem, InitializaedControllerShoudBeEmpty)
{
    using Unit = ControlSlotGeometryType::Unit;
    using Section = InterfaceGeometryType::Section;
    ControlSlotGeometryType controlSlot{};
    EXPECT_EQ(true, controlSlot.IsEmpty());
    EXPECT_EQ(0ull, controlSlot.Count());

    EXPECT_ASSERT(const Section& geometrySection = controlSlot.Current());

    EXPECT_ASSERT(uint32_t count = controlSlot.GetSectionCount(););
}

TEST_F(TestGeometryControllerSystem, UploadCacheOnInterfaceWithOutOpenSection)
{
    ControlSlotGeometryType controlSlot{};
    const Rynex::Ref<Rynex::MeshSingle>& meshSingle = GetMeshSingle();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(meshSingle);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
    EXPECT_ASSERT(controlSlot.UploadCacheOnInterface());
    EXPECT_EQ(controlSlot.IsEmpty(), true);
    EXPECT_EQ(0ull, controlSlot.Count());
    EXPECT_ASSERT(uint32_t count = controlSlot.GetSectionCount());
}

TEST_F(TestGeometryControllerSystem, UploadCacheOnInterfaceAfterOpenSection)
{
    ControlSlotGeometryType controlSlot{};
    const Rynex::Ref<Rynex::MeshSingle>& meshSingle = GetMeshSingle();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(meshSingle);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
        controlSlot.OpenNextSection();
    );

    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    EXPECT_EQ(controlSlot.IsEmpty(), false);
    EXPECT_EQ(1ull, controlSlot.Count());
    EXPECT_NO_ASSERT(
        uint32_t count = controlSlot.GetSectionCount();
        EXPECT_EQ(1ull, count);
    );
}


TEST_F(TestGeometryControllerSystem, BeforUpladeClear)
{
    ControlSlotGeometryType controlSlot{};
    const Rynex::Ref<Rynex::MeshSingle>& meshSingle = GetMeshSingle();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(meshSingle);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
        controlSlot.OpenNextSection();
    );
    controlSlot.Clear();
    EXPECT_ASSERT( controlSlot.UploadCacheOnInterface(); );

    EXPECT_EQ(true, controlSlot.IsEmpty());
    EXPECT_EQ(0ull, controlSlot.Count());

    EXPECT_ASSERT(
        uint32_t count = controlSlot.GetSectionCount();
        EXPECT_EQ(0ull, count);
    );


}

TEST_F(TestGeometryControllerSystem, TheSameMeshSingleTwoTimes)
{
    ControlSlotGeometryType controlSlot{};
    const Rynex::Ref<Rynex::MeshSingle>& meshSingle = GetMeshSingle();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(meshSingle);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
        controlSlot.OpenNextSection();
    );

    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(meshSingle);
        EXPECT_EQ(Rynex::DrawSplit::Same, split);
    );
}

TEST_F(TestGeometryControllerSystem, CollectVertexBuffer)
{
    ControlSlotGeometryType controlSlot{};
    const Rynex::Ref<Rynex::MeshSingle>& meshSingle = GetMeshSingle();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(meshSingle);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    Rynex::ShaderDrawResource drawResource;
    Rynex::CachedBufferRegister& cachedBufferRegister = *(m_Cache.get());
    BuildGeometryType build = controlSlot.CreateBuild(drawResource, cachedBufferRegister);

    std::array<Rynex::VertexArray::VertexElements, 1> vertexElementsArray;
    Rynex::Span<Rynex::VertexArray::VertexElements> vertexElementsSpan{
        vertexElementsArray.data(), vertexElementsArray.size()
    };
    build.CollectVertexBuffer(vertexElementsSpan);

}
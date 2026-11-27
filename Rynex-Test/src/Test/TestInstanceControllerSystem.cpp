// TestControllerSystem created in Project Rynex-Rendering on 13/11/2026.
#include <rypch.h>
#include "TestInstanceControllerSystem.h"

#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/DrawList/Contoller/Geomtry/MeshGeometry.h>


void TestInstanceControllerSystem::SetUp()
{
    TestApplicationSystem::SetUp();
    constexpr size_t byteSize = 1E9; // 1 gigabyte = 1 * 10^9

    m_Cache = Rynex::CreateRef<Rynex::CachedBufferRegister>();
    m_Cache->ReservedRegisterGpuMaxGpuMemoryUsage(byteSize);
    m_Cache->SetFrame(0);

    Rynex::FileSystem::Path pathMesh("Engine-Resources/Editor-Assets/Models/Cube.rystmesh");

}

void TestInstanceControllerSystem::TearDown()
{
    RY_DESTROY_REF(m_Cache);
    TestApplicationSystem::TearDown();
}




TEST_F(TestInstanceControllerSystem, BasicCompile)
{
    InterfaceInstanceType geometryInterface;

    static_assert(std::is_same_v<InterfaceInstanceType::Source, Rynex::Control::InstanceDefault::Source>, "Source from Interface is not expected Type.");
    static_assert(std::is_same_v<InterfaceInstanceType::Section, Rynex::VectorSection<InterfaceInstanceType::Source, Rynex::Control::InstanceDefault::Policy>>, "Section from Interface is not expected Type.");
    static_assert(std::is_same_v<InterfaceInstanceType::Indices, int>, "Indices from Interface is not expected Type.");

    Rynex::ShaderDrawResource drawResource;
    Rynex::CachedBufferRegister& cachedBufferRegister = *(m_Cache.get());
    BuildInstanceType build{
        cachedBufferRegister, geometryInterface, drawResource
    };
    static_assert(std::is_same_v<BuildInstanceType::Interface, InterfaceInstanceType>, "Interface from Build is not expected Type.");


    ControlSlotInstanceType controlSlot{};

    static_assert(std::is_same_v<ControlSlotInstanceType::Source, InterfaceInstanceType::Source>, "Source from ControlSlot is not expected Type.");
    static_assert(std::is_same_v<ControlSlotInstanceType::Interface, InterfaceInstanceType>, "Interface from ControlSlot is not expected Type.");
    static_assert(std::is_same_v<ControlSlotInstanceType::Build, BuildInstanceType>, "Build from ControlSlot is not expected Type.");


}

TEST_F(TestInstanceControllerSystem, InitializaedControllerShoudBeEmpty)
{
    using Unit = ControlSlotInstanceType::Unit;
    using Section = InterfaceInstanceType::Section;
    ControlSlotInstanceType controlSlot{};
    EXPECT_EQ(true, controlSlot.IsEmpty());
    EXPECT_EQ(0ull, controlSlot.Count());

    EXPECT_ASSERT(const Section& instanceSection = controlSlot.Current());

    EXPECT_ASSERT(uint32_t count = controlSlot.GetSectionCount(););
}



TEST_F(TestInstanceControllerSystem, UploadCacheOnInterfaceWithOutOpenSection)
{
    ControlSlotInstanceType controlSlot{};
    glm::mat4 modelMatrix{ 1.0f };
    int entityID = 30;
    Rynex::Control::InstanceDefault::Source source{
        glm::mat4{ 1.0f }, 30, 0, 0, 0
    };
    EXPECT_ASSERT(
        Rynex::SectionFill split = controlSlot.AddOnInterface(source);
        EXPECT_EQ(Rynex::SectionFill::Full, split);
    );

    EXPECT_EQ(controlSlot.IsEmpty(), true);
    EXPECT_EQ(0ull, controlSlot.Count());
    EXPECT_ASSERT(uint32_t count = controlSlot.GetSectionCount());
}
#if 0
TEST_F(TestInstanceControllerSystem, UploadCacheOnInterfaceAfterOpenSection)
{
    ControlSlotInstanceType controlSlot{};

    const Rynex::Ref<Rynex::Instance>& Instance = GetInstance();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(Instance);
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


TEST_F(TestInstanceControllerSystem, BeforUpladeClear)
{
    ControlSlotInstanceType controlSlot{};
    const Rynex::Ref<Rynex::Instance>& Instance = GetInstance();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(Instance);
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

TEST_F(TestInstanceControllerSystem, TheSameMeshSingleTwoTimes)
{
    ControlSlotInstanceType controlSlot{};
    const Rynex::Ref<Rynex::Instance>& Instance = GetInstance();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(Instance);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
        controlSlot.OpenNextSection();
    );

    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(Instance);
        EXPECT_EQ(Rynex::DrawSplit::Same, split);
    );
}

TEST_F(TestInstanceControllerSystem, CollectVertexBuffer)
{
    ControlSlotInstanceType controlSlot{};
    const Rynex::Ref<Rynex::Instance>& Instance = GetInstance();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(Instance);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    Rynex::ShaderDrawResource drawResource;
    Rynex::CachedBufferRegister& cachedBufferRegister = *(m_Cache.get());
    BuildInstanceType build = controlSlot.CreateBuild(drawResource, cachedBufferRegister);

    std::array<Rynex::VertexArray::VertexElements, 1> vertexElementsArray;
    Rynex::Span<Rynex::VertexArray::VertexElements> vertexElementsSpan{
        vertexElementsArray.data(), vertexElementsArray.size()
    };
    EXPECT_EQ(nullptr, vertexElementsArray[0].m_Buffer);
    build.CollectVertexBuffer(vertexElementsSpan);
    EXPECT_EQ(nullptr, vertexElementsArray[0].m_Buffer);
}

TEST_F(TestInstanceControllerSystem, CollectIndexBuffer)
{
    ControlSlotInstanceType controlSlot{};
    const Rynex::Ref<Rynex::Instance>& Instance = GetInstance();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(Instance);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    Rynex::ShaderDrawResource drawResource;
    Rynex::CachedBufferRegister& cachedBufferRegister = *(m_Cache.get());
    BuildInstanceType build = controlSlot.CreateBuild(drawResource, cachedBufferRegister);

    std::array<Rynex::Ref<Rynex::IndexBuffer>, 1> indexBufferArray;
    Rynex::Span<Rynex::Ref<Rynex::IndexBuffer>> indexBufferSpan{
        indexBufferArray.data(), indexBufferArray.size()
    };
    EXPECT_EQ(nullptr, indexBufferArray[0]);
    build.CollectIndexBuffer(indexBufferSpan);
    EXPECT_EQ(nullptr, indexBufferArray[0]);
}

TEST_F(TestInstanceControllerSystem, BindVertexBuffer)
{
    ControlSlotInstanceType controlSlot{};
    const Rynex::Ref<Rynex::Instance>& Instance = GetInstance();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(Instance);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    Rynex::ShaderDrawResource drawResource;
    Rynex::CachedBufferRegister& cachedBufferRegister = *(m_Cache.get());
    BuildInstanceType build = controlSlot.CreateBuild(drawResource, cachedBufferRegister);


    build.BindCommand();
}

TEST_F(TestInstanceControllerSystem, SetOnInterfaceParmeterNullptr)
{
    ControlSlotInstanceType controlSlot{};
    EXPECT_ASSERT(
        const Rynex::DrawSplit split = controlSlot.SetOnInterface(nullptr);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );


}

TEST_F(TestInstanceControllerSystem, SetOnInterfaceParmeterNullptrAfterOneVaild)
{
    ControlSlotInstanceType controlSlot{};
    const Rynex::Ref<Rynex::Instance>& Instance = GetInstance();

    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(Instance);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );

    EXPECT_ASSERT(
        const Rynex::DrawSplit split = controlSlot.SetOnInterface(nullptr);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
}

TEST_F(TestInstanceControllerSystem, UploadeEmptyCachedData)
{
    ControlSlotInstanceType controlSlot{};

    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_ASSERT( controlSlot.UploadCacheOnInterface(); );
}

TEST_F(TestInstanceControllerSystem, CallUplaodeForTwoDifferentSections)
{
    ControlSlotInstanceType controlSlot{};

    const Rynex::Ref<Rynex::Instance>& Instance = GetInstance();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(Instance);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );

    EXPECT_EQ(true, controlSlot.IsEmpty());
    EXPECT_EQ(0ull, controlSlot.Count());

    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    EXPECT_EQ(false, controlSlot.IsEmpty());
    EXPECT_EQ(1ull, controlSlot.Count());
    EXPECT_EQ(1ull, controlSlot.GetSectionCount());

    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    EXPECT_EQ(false, controlSlot.IsEmpty());
    EXPECT_EQ(2ull, controlSlot.Count());
    EXPECT_EQ(1ull, controlSlot.GetSectionCount());

}

TEST_F(TestInstanceControllerSystem, CallUplaodeForOneSections)
{
    ControlSlotInstanceType controlSlot{};

    const Rynex::Ref<Rynex::Instance>& Instance = GetInstance();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(Instance);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );

    EXPECT_EQ(true, controlSlot.IsEmpty());
    EXPECT_EQ(0ull, controlSlot.Count());

    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    EXPECT_EQ(false, controlSlot.IsEmpty());
    EXPECT_EQ(1ull, controlSlot.Count());
    EXPECT_EQ(1ull, controlSlot.GetSectionCount());

    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    EXPECT_EQ(false, controlSlot.IsEmpty());
    EXPECT_EQ(1ull, controlSlot.Count());
    EXPECT_EQ(1ull, controlSlot.GetSectionCount());
}
#endif
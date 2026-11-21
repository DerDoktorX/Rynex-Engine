// TestControllerSystem created in Project Rynex-Rendering on 13/11/2026.
#include <rypch.h>
#include "TestMaterielControllerSystem.h"

#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Renderer/DrawList/Contoller/Geomtry/MeshGeometry.h>


void TestMaterielControllerSystem::SetUp()
{
    TestApplicationSystem::SetUp();
    constexpr size_t byteSize = 1E9; // 1 gigabyte = 1 * 10^9

    m_Cache = Rynex::CreateRef<Rynex::CachedBufferRegister>();
    m_Cache->ReservedRegisterGpuMaxGpuMemoryUsage(byteSize);
    m_Cache->SetFrame(0);

    Rynex::FileSystem::Path pathMesh("Engine-Resources/Editor-Assets/Models/Cube.rystmesh");
    m_MeshStatic = Rynex::AssetManager::GetAsset<Rynex::MeshStatic>(pathMesh);

}

void TestMaterielControllerSystem::TearDown()
{
    RY_DESTROY_REF(m_MeshStatic);
    RY_DESTROY_REF(m_Cache);
    TestApplicationSystem::TearDown();
}

const Rynex::Ref<Rynex::Material>& TestMaterielControllerSystem::GetMaterial() const
{
    const uint32_t count = m_MeshStatic->GetSingleMeshObjectCount();
    EXPECT_EQ(1u, count);
    const Rynex::MeshStatic::SingleObjectMeshData& singleObjectMeshData = m_MeshStatic->GetSingleObjectData(0);
    const Rynex::Ref<Rynex::Material>& material = singleObjectMeshData.m_Material;
    EXPECT_NE(nullptr, material);
    return material;
}


TEST_F(TestMaterielControllerSystem, BasicCompile)
{
    InterfaceMaterialType geometryInterface;

    static_assert(std::is_same_v<InterfaceMaterialType::Source, Rynex::Ref<Rynex::Material>>, "Source from Interface is not expected Type.");
    static_assert(std::is_same_v<InterfaceMaterialType::Section, Rynex::Ref<Rynex::Material>>, "Section from Interface is not expected Type.");
    static_assert(std::is_same_v<InterfaceMaterialType::Indices, int>, "Indices from Interface is not expected Type.");

    Rynex::ShaderDrawResource drawResource;
    Rynex::CachedBufferRegister& cachedBufferRegister = *(m_Cache.get());
    BuildMaterialType build{
        cachedBufferRegister, geometryInterface, drawResource
    };
    static_assert(std::is_same_v<BuildMaterialType::Interface, InterfaceMaterialType>, "Interface from Build is not expected Type.");


    ControlSlotMaterialType controlSlot{};

    static_assert(std::is_same_v<ControlSlotMaterialType::Source, InterfaceMaterialType::Source>, "Source from ControlSlot is not expected Type.");
    static_assert(std::is_same_v<ControlSlotMaterialType::Interface, InterfaceMaterialType>, "Interface from ControlSlot is not expected Type.");
    static_assert(std::is_same_v<ControlSlotMaterialType::Build, BuildMaterialType>, "Build from ControlSlot is not expected Type.");
}


TEST_F(TestMaterielControllerSystem, InitializaedControllerShoudBeEmpty)
{
    using Unit = ControlSlotMaterialType::Unit;
    using Section = InterfaceMaterialType::Section;
    ControlSlotMaterialType controlSlot{};
    EXPECT_EQ(true, controlSlot.IsEmpty());
    EXPECT_EQ(0ull, controlSlot.Count());

    EXPECT_ASSERT(const Section& geometrySection = controlSlot.Current());

    EXPECT_ASSERT(uint32_t count = controlSlot.GetSectionCount(););
}

TEST_F(TestMaterielControllerSystem, UploadCacheOnInterfaceWithOutOpenSection)
{
    ControlSlotMaterialType controlSlot{};
    const Rynex::Ref<Rynex::Material>& material = GetMaterial();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(material);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
    EXPECT_ASSERT(controlSlot.UploadCacheOnInterface());
    EXPECT_EQ(controlSlot.IsEmpty(), true);
    EXPECT_EQ(0ull, controlSlot.Count());
    EXPECT_ASSERT(uint32_t count = controlSlot.GetSectionCount());
}

TEST_F(TestMaterielControllerSystem, UploadCacheOnInterfaceAfterOpenSection)
{
    ControlSlotMaterialType controlSlot{};

    const Rynex::Ref<Rynex::Material>& material = GetMaterial();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(material);
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


TEST_F(TestMaterielControllerSystem, BeforUpladeClear)
{
    ControlSlotMaterialType controlSlot{};
    const Rynex::Ref<Rynex::Material>& materiel = GetMaterial();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(materiel);
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

TEST_F(TestMaterielControllerSystem, TheSameMeshSingleTwoTimes)
{
    ControlSlotMaterialType controlSlot{};
    const Rynex::Ref<Rynex::Material>& materiel = GetMaterial();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(materiel);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
        controlSlot.OpenNextSection();
    );

    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(materiel);
        EXPECT_EQ(Rynex::DrawSplit::Same, split);
    );
}

TEST_F(TestMaterielControllerSystem, CollectVertexBuffer)
{
    ControlSlotMaterialType controlSlot{};
    const Rynex::Ref<Rynex::Material>& materiel = GetMaterial();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(materiel);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    Rynex::ShaderDrawResource drawResource;
    Rynex::CachedBufferRegister& cachedBufferRegister = *(m_Cache.get());
    BuildMaterialType build = controlSlot.CreateBuild(drawResource, cachedBufferRegister);

    std::array<Rynex::VertexArray::VertexElements, 1> vertexElementsArray;
    Rynex::Span<Rynex::VertexArray::VertexElements> vertexElementsSpan{
        vertexElementsArray.data(), vertexElementsArray.size()
    };
    EXPECT_EQ(nullptr, vertexElementsArray[0].m_Buffer);
    build.CollectVertexBuffer(vertexElementsSpan);
    EXPECT_EQ(nullptr, vertexElementsArray[0].m_Buffer);
}

TEST_F(TestMaterielControllerSystem, CollectIndexBuffer)
{
    ControlSlotMaterialType controlSlot{};
    const Rynex::Ref<Rynex::Material>& material = GetMaterial();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(material);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    Rynex::ShaderDrawResource drawResource;
    Rynex::CachedBufferRegister& cachedBufferRegister = *(m_Cache.get());
    BuildMaterialType build = controlSlot.CreateBuild(drawResource, cachedBufferRegister);

    std::array<Rynex::Ref<Rynex::IndexBuffer>, 1> indexBufferArray;
    Rynex::Span<Rynex::Ref<Rynex::IndexBuffer>> indexBufferSpan{
        indexBufferArray.data(), indexBufferArray.size()
    };
    EXPECT_EQ(nullptr, indexBufferArray[0]);
    build.CollectIndexBuffer(indexBufferSpan);
    EXPECT_EQ(nullptr, indexBufferArray[0]);
}

TEST_F(TestMaterielControllerSystem, BindVertexBuffer)
{
    ControlSlotMaterialType controlSlot{};
    const Rynex::Ref<Rynex::Material>& materiel = GetMaterial();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(materiel);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_NO_ASSERT( controlSlot.UploadCacheOnInterface(); );

    Rynex::ShaderDrawResource drawResource;
    Rynex::CachedBufferRegister& cachedBufferRegister = *(m_Cache.get());
    BuildMaterialType build = controlSlot.CreateBuild(drawResource, cachedBufferRegister);


    build.BindCommand();
}

TEST_F(TestMaterielControllerSystem, SetOnInterfaceParmeterNullptr)
{
    ControlSlotMaterialType controlSlot{};
    EXPECT_ASSERT(
        const Rynex::DrawSplit split = controlSlot.SetOnInterface(nullptr);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );


}

TEST_F(TestMaterielControllerSystem, SetOnInterfaceParmeterNullptrAfterOneVaild)
{
    ControlSlotMaterialType controlSlot{};
    const Rynex::Ref<Rynex::Material>& material = GetMaterial();

    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(material);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );

    EXPECT_ASSERT(
        const Rynex::DrawSplit split = controlSlot.SetOnInterface(nullptr);
        EXPECT_EQ(Rynex::DrawSplit::NewDraw, split);
    );
}

TEST_F(TestMaterielControllerSystem, UploadeEmptyCachedData)
{
    ControlSlotMaterialType controlSlot{};

    EXPECT_NO_ASSERT( controlSlot.OpenNextSection(); );
    EXPECT_ASSERT( controlSlot.UploadCacheOnInterface(); );
}

TEST_F(TestMaterielControllerSystem, CallUplaodeForTwoDifferentSections)
{
    ControlSlotMaterialType controlSlot{};

    const Rynex::Ref<Rynex::Material>& materiel = GetMaterial();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(materiel);
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

TEST_F(TestMaterielControllerSystem, CallUplaodeForOneSections)
{
    ControlSlotMaterialType controlSlot{};

    const Rynex::Ref<Rynex::Material>& material = GetMaterial();
    EXPECT_NO_ASSERT(
        Rynex::DrawSplit split = controlSlot.SetOnInterface(material);
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
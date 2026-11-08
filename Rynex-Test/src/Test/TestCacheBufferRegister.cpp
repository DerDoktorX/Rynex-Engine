// CachedBufferRegister created in Project Rynex-Rendering on 22/10/2026.
#include <rypch.h>
#include "TestCacheBufferRegister.h"

TestCacheBufferRegister::TestCacheBufferRegister()
    : TestApplicationSystem()
    , m_Value{ 127, 63, 31, 15}
    , m_Layout({
        {Rynex::SDT::Int4, "Values"}
    })
{
}


void TestCacheBufferRegister::SetUp()
{
    TestApplicationSystem::SetUp();
    m_Cache = Rynex::CreateRef<Rynex::CachedBufferRegister>();
    m_Cache->ReservedRegisterGpuMaxGpuMemoryUsage(sizeof(m_Value));
}

void TestCacheBufferRegister::TearDown()
{
    RY_DESTROY_REF(m_Cache);
    TestApplicationSystem::TearDown();
}

Rynex::Span<const Rynex::Byte> TestCacheBufferRegister::GetByteData()
{
    const Rynex::Span<const int> m_ValueSpan{ m_Value.data(), m_Value.size() };
    return Rynex::AsBytes(m_ValueSpan);
}





TEST_F(TestCacheBufferRegister, TestClearEmptyCache)
{
    m_Cache->Clear();
}

TEST_F(TestCacheBufferRegister, TestSetFrame)
{
    m_Cache->SetFrame(0);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireBufferByZeroMemoryBuged)
{
    m_Cache->ReservedRegisterGpuMaxGpuMemoryUsage(0u);
    EXPECT_EQ(0, m_Cache->GetRegisterGpuMemoryUsage());
    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);
    EXPECT_EQ(nullptr, buffer);
    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireBufferOneByteOverBudged_OneBuffer)
{
    constexpr size_t maxByteSize = sizeof(m_Value) - 1ull;
    m_Cache->ReservedRegisterGpuMaxGpuMemoryUsage(maxByteSize);
    EXPECT_EQ(0, m_Cache->GetRegisterGpuMemoryUsage());
    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);
    EXPECT_EQ(nullptr, buffer);
    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(maxByteSize, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireBufferOneByteOverBudged_TwoBuffer)
{
    constexpr size_t maxByteSize = sizeof(m_Value) * 2ull - 1ull;
    m_Cache->ReservedRegisterGpuMaxGpuMemoryUsage(maxByteSize);
    EXPECT_EQ(0, m_Cache->GetRegisterGpuMemoryUsage());
    const Rynex::Ref<Rynex::UniformBuffer> buffer0 = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);
    const Rynex::Ref<Rynex::UniformBuffer> buffer1 = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);
    EXPECT_NE(nullptr, buffer0);
    EXPECT_EQ(sizeof(m_Value), buffer0->GetByteSize());
    EXPECT_EQ(nullptr, buffer1);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(maxByteSize - sizeof(m_Value), m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireBuffer_Clear_OneBuffer)
{
    constexpr size_t maxByteSize = sizeof(m_Value);
    m_Cache->ReservedRegisterGpuMaxGpuMemoryUsage(maxByteSize);
    EXPECT_EQ(0, m_Cache->GetRegisterGpuMemoryUsage());
    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(maxByteSize, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0u, m_Cache->GetUnusedGpuBudged());

    m_Cache->Clear();

    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(maxByteSize, m_Cache->GetUnusedGpuBudged());
}

#pragma region UniformBufferTest

TEST_F(TestCacheBufferRegister, TestSetAcquireUniformBuffer)
{
    EXPECT_EQ(0, m_Cache->GetRegisterGpuMemoryUsage());
    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(m_Value), buffer->GetByteSize());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireBuffer_Template_UniformBuffer)
{
    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireBuffer<Rynex::UniformBuffer>(GetByteData(), s_Flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(m_Value), buffer->GetByteSize());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0, m_Cache->GetUnusedGpuBudged());
}


TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueUniformBufferHash_Empty)
{
    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniqueUniformBufferHash(0u);
    EXPECT_EQ(nullptr, buffer);
    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueUniformBufferHash_Create)
{
    auto [hash ,bufferCreate] = m_Cache->AcquireUniqueUniformBuffer(GetByteData(), m_Layout, s_Flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());


    const Rynex::Ref<Rynex::UniformBuffer> bufferGet = m_Cache->AcquireUniqueUniformBufferHash(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueUBufferHash_Template_UniformBuffer_Empty)
{
    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniqueBufferHash<Rynex::UniformBuffer>(0u);
    EXPECT_EQ(nullptr, buffer);
    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueBufferHash_Template_UniformBuffer_Create)
{
    auto [hash, bufferCreate] = m_Cache->AcquireUniqueBuffer<Rynex::UniformBuffer>(GetByteData(), m_Layout, s_Flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());

    const Rynex::Ref<Rynex::UniformBuffer> bufferGet = m_Cache->AcquireUniqueBufferHash<Rynex::UniformBuffer>(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
}

#pragma endregion



#pragma region VertexBufferTest

TEST_F(TestCacheBufferRegister, TestSetAcquireVertexBuffer)
{
    const Rynex::Ref<Rynex::VertexBuffer> buffer = m_Cache->AcquireVertexBuffer(GetByteData());
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(m_Value), buffer->GetByteSize());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireBuffer_Template_VertexBuffer)
{
    const Rynex::Ref<Rynex::VertexBuffer> buffer = m_Cache->AcquireBuffer<Rynex::VertexBuffer>(GetByteData(), s_Flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(m_Value), buffer->GetByteSize());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0u, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueVertexBufferHash_Empty)
{
    const Rynex::Ref<Rynex::VertexBuffer> buffer = m_Cache->AcquireUniqueVertexBufferHash(0u);
    EXPECT_EQ(nullptr, buffer);
    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueVertexBufferHash_Create)
{
    auto [hash ,bufferCreate] = m_Cache->AcquireUniqueVertexBuffer(GetByteData(), m_Layout, s_Flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());

    const Rynex::Ref<Rynex::VertexBuffer> bufferGet = m_Cache->AcquireUniqueVertexBufferHash(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0u, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueUBufferHash_Template_VertexBuffer_Empty)
{
    const Rynex::Ref<Rynex::VertexBuffer> buffer = m_Cache->AcquireUniqueBufferHash<Rynex::VertexBuffer>(0u);
    EXPECT_EQ(nullptr, buffer);
    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueBufferHash_Template_VertexBuffer_Create)
{
    auto [hash, bufferCreate] = m_Cache->AcquireUniqueBuffer<Rynex::VertexBuffer>(GetByteData(), m_Layout, s_Flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());

    const Rynex::Ref<Rynex::VertexBuffer> bufferGet = m_Cache->AcquireUniqueBufferHash<Rynex::VertexBuffer>(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0u, m_Cache->GetUnusedGpuBudged());
}

#pragma endregion



#pragma region StorageBufferTest

TEST_F(TestCacheBufferRegister, TestSetAcquireStorageBuffer)
{
    const Rynex::Ref<Rynex::StorageBuffer> buffer = m_Cache->AcquireStorageBuffer(GetByteData());
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(m_Value), buffer->GetByteSize());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0u, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireBuffer_Template_StorageBuffer)
{
    const Rynex::Ref<Rynex::StorageBuffer> buffer = m_Cache->AcquireBuffer<Rynex::StorageBuffer>(GetByteData(), s_Flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(m_Value), buffer->GetByteSize());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0u, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueStorageBufferHash_Empty)
{
    const Rynex::Ref<Rynex::StorageBuffer> buffer = m_Cache->AcquireUniqueStorageBufferHash(0u);
    EXPECT_EQ(nullptr, buffer);
    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueStorageBufferHash_Create)
{
    auto [hash ,bufferCreate] = m_Cache->AcquireUniqueStorageBuffer(GetByteData(), m_Layout, s_Flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0u, m_Cache->GetUnusedGpuBudged());

    const Rynex::Ref<Rynex::StorageBuffer> bufferGet = m_Cache->AcquireUniqueStorageBufferHash(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0u, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueUBufferHash_Template_StorageBuffer_Empty)
{
    const Rynex::Ref<Rynex::StorageBuffer> buffer = m_Cache->AcquireUniqueBufferHash<Rynex::StorageBuffer>(0u);
    EXPECT_EQ(nullptr, buffer);
    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueBufferHash_Template_StorageBuffer_Create)
{
    auto [hash, bufferCreate] = m_Cache->AcquireUniqueBuffer<Rynex::StorageBuffer>(GetByteData(), m_Layout, s_Flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());

    const Rynex::Ref<Rynex::StorageBuffer> bufferGet = m_Cache->AcquireUniqueBufferHash<Rynex::StorageBuffer>(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0u, m_Cache->GetUnusedGpuBudged());
}

#pragma endregion


#pragma region garbeg_colection

TEST_F(TestCacheBufferRegister, TestGarbegColector_Auto_delete_One_Buffer)
{
    EXPECT_EQ(0, m_Cache->GetRegisterGpuMemoryUsage());
    m_Cache->SetFrame(2ull);
    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(m_Value), buffer->GetByteSize());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0, m_Cache->GetUnusedGpuBudged());


    m_Cache->SetFrame(600ull);

    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0, m_Cache->GetUnusedGpuBudged());

    m_Cache->SetFrame(603ull);


    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetUnusedGpuBudged());
}


TEST_F(TestCacheBufferRegister, TestGarbegColector_Auto_Delete_Multyble_Buffer)
{
    EXPECT_EQ(0, m_Cache->GetRegisterGpuMemoryUsage());
    m_Cache->SetFrame(2ull);
    const Rynex::Ref<Rynex::UniformBuffer> buffer0 = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);


    EXPECT_NE(nullptr, buffer0);
    EXPECT_EQ(sizeof(m_Value), buffer0->GetByteSize());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0, m_Cache->GetUnusedGpuBudged());


    m_Cache->SetFrame(600ull);

    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0, m_Cache->GetUnusedGpuBudged());

    m_Cache->SetFrame(603ull);


    EXPECT_EQ(0u, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetUnusedGpuBudged());

    const Rynex::Ref<Rynex::UniformBuffer> buffer1 = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);

    EXPECT_NE(nullptr, buffer1);
    EXPECT_NE(buffer1, buffer0);
    EXPECT_EQ(sizeof(m_Value), buffer1->GetByteSize());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0u, m_Cache->GetUnusedGpuBudged());
}

TEST_F(TestCacheBufferRegister, TestGarbegColector_Auto_Delete_One_Buffer_A_Time)
{
    m_Cache->ReservedRegisterGpuMaxGpuMemoryUsage(sizeof(m_Value) * 2);
    EXPECT_EQ(0, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ((sizeof(m_Value) * 2), m_Cache->GetUnusedGpuBudged());
    {
        m_Cache->SetFrame(1ull);
        const Rynex::Ref<Rynex::UniformBuffer>  buffer0 = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);
        const Rynex::Ref<Rynex::UniformBuffer>  buffer1 = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);

        EXPECT_NE(nullptr, buffer0);
        EXPECT_NE(nullptr, buffer1);
        EXPECT_NE(buffer0, buffer1);
        EXPECT_EQ(sizeof(m_Value), buffer0->GetByteSize());
        EXPECT_EQ(sizeof(m_Value), buffer1->GetByteSize());
    }

    EXPECT_EQ((sizeof(m_Value) * 2ull), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0ull, m_Cache->GetUnusedGpuBudged());

    m_Cache->SetFrame(2ull);

    EXPECT_EQ((sizeof(m_Value) * 2ull), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0ull, m_Cache->GetUnusedGpuBudged());

    m_Cache->SetFrame(3ull);

    EXPECT_EQ((sizeof(m_Value) * 2ull), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0ull, m_Cache->GetUnusedGpuBudged());


    {
        m_Cache->SetFrame(4ull);
        const Rynex::Ref<Rynex::UniformBuffer> buffer0 = m_Cache->AcquireUniformBuffer(GetByteData(), s_Flag);
        EXPECT_NE(nullptr, buffer0);
        EXPECT_EQ(sizeof(m_Value), buffer0->GetByteSize());
    }

    EXPECT_EQ((sizeof(m_Value) * 2ull), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(0ull, m_Cache->GetUnusedGpuBudged());


    m_Cache->SetFrame(603ull);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetUnusedGpuBudged());

    m_Cache->SetFrame(1203ull);
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value), m_Cache->GetUnusedGpuBudged());

    m_Cache->SetFrame(1204ull);
    EXPECT_EQ(0ull, m_Cache->GetRegisterGpuMemoryUsage());
    EXPECT_EQ(sizeof(m_Value) * 2ull, m_Cache->GetUnusedGpuBudged());

}

#pragma endregion
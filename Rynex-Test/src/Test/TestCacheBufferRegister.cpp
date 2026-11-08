// CachedBufferRegister created in Project Rynex-Rendering on 22/10/2026.
#include <rypch.h>
#include "TestCacheBufferRegister.h"

void TestCacheBufferRegister::SetUp()
{
    TestApplicationSystem::SetUp();
    m_Cache = Rynex::CreateRef<Rynex::CachedBufferRegister>();
}

void TestCacheBufferRegister::TearDown()
{
    RY_DESTROY_REF(m_Cache);
    TestApplicationSystem::TearDown();
}


TEST_F(TestCacheBufferRegister, TestClearEmptyCache)
{
    m_Cache->Clear();
}

TEST_F(TestCacheBufferRegister, TestSetFrame)
{
    m_Cache->SetFrame(0);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniformBuffer)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;

    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniformBuffer(valueSpan, flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(value), buffer->GetByteSize());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireBuffer_Template_UniformBuffer)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;
    const Rynex::Span<const Rynex::Byte> span = Rynex::AsBytes(valueSpan);
    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireBuffer<Rynex::UniformBuffer>(span, flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(value), buffer->GetByteSize());
}


TEST_F(TestCacheBufferRegister, TestSetAcquireUniformBufferg)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;

    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniqueUniformBufferHash(0u);
    EXPECT_EQ(nullptr, buffer);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueUniformBufferHash_Empty)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;

    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniqueUniformBufferHash(0u);
    EXPECT_EQ(nullptr, buffer);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueUniformBufferHash_Create)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;
    const Rynex::BufferLayout layout{
        { Rynex::SDT::Int4, "Values"}
    };
    auto [hash ,bufferCreate] = m_Cache->AcquireUniqueUniformBuffer(valueSpan, layout, flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);

    const Rynex::Ref<Rynex::UniformBuffer> bufferGet = m_Cache->AcquireUniqueUniformBufferHash(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueUBufferHash_Template_UniformBuffer_Empty)
{
    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniqueBufferHash<Rynex::UniformBuffer>(0u);
    EXPECT_EQ(nullptr, buffer);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueBufferHash_Template_UniformBuffer_Create)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    const Rynex::Span<const Rynex::Byte> span = Rynex::AsBytes(valueSpan);
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;
    const Rynex::BufferLayout layout{
        { Rynex::SDT::Int4, "Values"}
    };
    auto [hash, bufferCreate] = m_Cache->AcquireUniqueBuffer<Rynex::UniformBuffer>(span, layout, flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);

    const Rynex::Ref<Rynex::UniformBuffer> bufferGet = m_Cache->AcquireUniqueBufferHash<Rynex::UniformBuffer>(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireVertexBuffer)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;

    const Rynex::Ref<Rynex::UniformBuffer> buffer = m_Cache->AcquireUniformBuffer(valueSpan, flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(value), buffer->GetByteSize());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireBuffer_Template_VertexBuffer)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;
    const Rynex::Span<const Rynex::Byte> span = Rynex::AsBytes(valueSpan);
    const Rynex::Ref<Rynex::VertexBuffer> buffer = m_Cache->AcquireBuffer<Rynex::VertexBuffer>(span, flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(value), buffer->GetByteSize());
}


TEST_F(TestCacheBufferRegister, TestSetAcquireVertexBufferg)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;

    const Rynex::Ref<Rynex::VertexBuffer> buffer = m_Cache->AcquireUniqueVertexBufferHash(0u);
    EXPECT_EQ(nullptr, buffer);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueVertexBufferHash_Empty)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;

    const Rynex::Ref<Rynex::VertexBuffer> buffer = m_Cache->AcquireUniqueVertexBufferHash(0u);
    EXPECT_EQ(nullptr, buffer);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueVertexBufferHash_Create)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;
    const Rynex::BufferLayout layout{
        { Rynex::SDT::Int4, "Values"}
    };
    auto [hash ,bufferCreate] = m_Cache->AcquireUniqueVertexBuffer(valueSpan, layout, flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);

    const Rynex::Ref<Rynex::VertexBuffer> bufferGet = m_Cache->AcquireUniqueVertexBufferHash(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueUBufferHash_Template_VertexBuffer_Empty)
{
    const Rynex::Ref<Rynex::VertexBuffer> buffer = m_Cache->AcquireUniqueBufferHash<Rynex::VertexBuffer>(0u);
    EXPECT_EQ(nullptr, buffer);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueBufferHash_Template_VertexBuffer_Create)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    const Rynex::Span<const Rynex::Byte> span = Rynex::AsBytes(valueSpan);
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;
    const Rynex::BufferLayout layout{
        { Rynex::SDT::Int4, "Values"}
    };
    auto [hash, bufferCreate] = m_Cache->AcquireUniqueBuffer<Rynex::VertexBuffer>(span, layout, flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);

    const Rynex::Ref<Rynex::VertexBuffer> bufferGet = m_Cache->AcquireUniqueBufferHash<Rynex::VertexBuffer>(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireStorageBuffer)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;

    const Rynex::Ref<Rynex::StorageBuffer> buffer = m_Cache->AcquireStorageBuffer(valueSpan);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(value), buffer->GetByteSize());
}

TEST_F(TestCacheBufferRegister, TestSetAcquireBuffer_Template_StorageBuffer)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;
    const Rynex::Span<const Rynex::Byte> span = Rynex::AsBytes(valueSpan);
    const Rynex::Ref<Rynex::StorageBuffer> buffer = m_Cache->AcquireBuffer<Rynex::StorageBuffer>(span, flag);
    EXPECT_NE(nullptr, buffer);
    EXPECT_EQ(sizeof(value), buffer->GetByteSize());
}


TEST_F(TestCacheBufferRegister, TestSetAcquireStorageBufferg)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;

    const Rynex::Ref<Rynex::StorageBuffer> buffer = m_Cache->AcquireUniqueStorageBufferHash(0u);
    EXPECT_EQ(nullptr, buffer);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueStorageBufferHash_Empty)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;

    const Rynex::Ref<Rynex::StorageBuffer> buffer = m_Cache->AcquireUniqueStorageBufferHash(0u);
    EXPECT_EQ(nullptr, buffer);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueStorageBufferHash_Create)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    const Rynex::Span<const Rynex::Byte> span = Rynex::AsBytes(valueSpan);
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;
    const Rynex::BufferLayout layout{
        { Rynex::SDT::Int4, "Values"}
    };
    auto [hash ,bufferCreate] = m_Cache->AcquireUniqueStorageBuffer(span, layout, flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);

    const Rynex::Ref<Rynex::StorageBuffer> bufferGet = m_Cache->AcquireUniqueStorageBufferHash(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueUBufferHash_Template_StorageBuffer_Empty)
{
    const Rynex::Ref<Rynex::StorageBuffer> buffer = m_Cache->AcquireUniqueBufferHash<Rynex::StorageBuffer>(0u);
    EXPECT_EQ(nullptr, buffer);
}

TEST_F(TestCacheBufferRegister, TestSetAcquireUniqueBufferHash_Template_StorageBuffer_Create)
{
    int value[4] = {127, 63, 31, 15 };
    const Rynex::Span<const int> valueSpan{ value, 4 };
    const Rynex::Span<const Rynex::Byte> span = Rynex::AsBytes(valueSpan);
    constexpr Rynex::BufferFlag::BufferFlagBit flag = Rynex::BufferFlag::BufferFlagBit::None;
    const Rynex::BufferLayout layout{
        { Rynex::SDT::Int4, "Values"}
    };
    auto [hash, bufferCreate] = m_Cache->AcquireUniqueBuffer<Rynex::StorageBuffer>(span, layout, flag);
    EXPECT_NE(nullptr, bufferCreate);
    EXPECT_NE(0, hash);

    const Rynex::Ref<Rynex::StorageBuffer> bufferGet = m_Cache->AcquireUniqueBufferHash<Rynex::StorageBuffer>(hash);
    EXPECT_EQ(bufferGet, bufferCreate);
}
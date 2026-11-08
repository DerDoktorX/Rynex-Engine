#pragma once
// CachedBufferRegister created in Project Rynex-Rendering on 07/11/2026.
#include <Rynex/Renderer/CachedRegister/CachedBufferRegister.h>
#include <Test/TestApplicationSystem.h>

class TestCacheBufferRegister : public TestApplicationSystem
{
protected:
    TestCacheBufferRegister();

    void SetUp() override;
    void TearDown() override;

    Rynex::Span<const Rynex::Byte> GetByteData();
// protected static constexpr -------------------------------------------------------------------------------------------------
    static constexpr Rynex::BufferFlag::BufferFlagBit s_Flag = Rynex::BufferFlag::BufferFlagBit::None;
// protected member variable --------------------------------------------------------------------------------------------------
    std::array<int, 4> m_Value;
    Rynex::BufferLayout m_Layout;
    Rynex::Ref<Rynex::CachedBufferRegister> m_Cache;
};

#pragma once
// CachedBufferRegister created in Project Rynex-Rendering on 07/11/2026.
#include <Rynex/Renderer/CachedRegister/CachedBufferRegister.h>
#include <Test/TestApplicationSystem.h>

class TestCacheBufferRegister : public TestApplicationSystem
{
protected:
    void SetUp() override;
    void TearDown() override;

// protected member variable --------------------------------------------------------------------------------------------------
    Rynex::Ref<Rynex::CachedBufferRegister> m_Cache;
};

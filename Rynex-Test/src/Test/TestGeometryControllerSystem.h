#pragma once
#include <Rynex/Renderer/CachedRegister/CachedBufferRegister.h>
#include <Test/TestApplicationSystem.h>
// TestControllerSystem created in Project Rynex-Rendering on 13/11/2026.



    class TestGeometryControllerSystem : public TestApplicationSystem
    {
    protected:
        void SetUp() override;
        void TearDown() override;


    // protected member variable ----------------------------------------------------------------------------------------------
        Rynex::Ref<Rynex::CachedBufferRegister> m_Cache;
        Rynex::Ref<Rynex::MeshStatic> m_MeshStatic;
    };


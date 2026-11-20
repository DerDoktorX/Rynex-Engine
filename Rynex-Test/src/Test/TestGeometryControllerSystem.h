#pragma once
#include <Test/TestApplicationSystem.h>

#include <Rynex/Renderer/CachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/DrawList/ControlSlot.h>
#include <Rynex/Renderer/DrawList/Contoller/Geomtry/MeshGeometry.h>


// TestControllerSystem created in Project Rynex-Rendering on 13/11/2026.



    class TestGeometryControllerSystem : public TestApplicationSystem
    {
    protected:
        using InterfaceGeometryType = Rynex::Control::GeometryDefault::Interface;
        using BuildGeometryType = Rynex::Control::GeometryDefault::Build;
        using ControllerGeometryType = Rynex::ControlDescriptor<InterfaceGeometryType, BuildGeometryType>;
        using ControlSlotGeometryType = Rynex::ControlSlot<ControllerGeometryType>;

        void SetUp() override;
        void TearDown() override;

        const Rynex::Ref<Rynex::MeshSingle>& GetMeshSingle() const;
    // protected member variable ----------------------------------------------------------------------------------------------
        Rynex::Ref<Rynex::CachedBufferRegister> m_Cache;
        Rynex::Ref<Rynex::MeshStatic> m_MeshStatic;
    };


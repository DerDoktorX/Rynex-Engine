// TestControllerSystem created in Project Rynex-Rendering on 13/11/2026.
#pragma once
#include <Test/TestApplicationSystem.h>

#include <Rynex/Renderer/CachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/DrawList/ControlSlot.h>
#include <Rynex/Renderer/DrawList/Contoller/Materiel/SingleMaterial.h>



class TestMaterielControllerSystem : public TestApplicationSystem
{
protected:
    using InterfaceMaterialType     = Rynex::Control::MaterielDefault::Interface;
    using BuildMaterialType         = Rynex::Control::MaterielDefault::Build;
    using ControllerMaterialType    = Rynex::ControlDescriptor<InterfaceMaterialType, BuildMaterialType>;
    using ControlSlotMaterialType   = Rynex::ControlSlot<ControllerMaterialType>;

    void SetUp() override;
    void TearDown() override;

    const Rynex::Ref<Rynex::Material>& GetMaterial() const;
// protected member variable ----------------------------------------------------------------------------------------------
    Rynex::Ref<Rynex::CachedBufferRegister> m_Cache;
    Rynex::Ref<Rynex::MeshStatic> m_MeshStatic;
};


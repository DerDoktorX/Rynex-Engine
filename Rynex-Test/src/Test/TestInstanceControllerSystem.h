// TestControllerSystem created in Project Rynex-Rendering on 13/11/2026.
#pragma once
#include <Test/TestApplicationSystem.h>

#include <Rynex/Renderer/CachedRegister/CachedBufferRegister.h>
#include <Rynex/Renderer/DrawList/ControlSlot.h>
#include <Rynex/Renderer/DrawList/Contoller/Instance/InstanceVertex.h>



class TestInstanceControllerSystem : public TestApplicationSystem
{
protected:
    using InterfaceInstanceType     = Rynex::Control::InstanceDefault::Interface;
    using BuildInstanceType         = Rynex::Control::InstanceDefault::Build;
    using ControllerInstanceType    = Rynex::ControlDescriptor<InterfaceInstanceType, BuildInstanceType>;
    using ControlSlotInstanceType   = Rynex::ControlSlot<ControllerInstanceType>;

    void SetUp() override;
    void TearDown() override;

// protected member variable ----------------------------------------------------------------------------------------------
    Rynex::Ref<Rynex::CachedBufferRegister> m_Cache;

};


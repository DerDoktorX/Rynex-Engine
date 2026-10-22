#pragma once
#include <Rynex/Core/Window.h>
#include <Test/AssertHook.h>


class TestDrawListSystem : public ::testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    Rynex::Ref<Rynex::Window> m_Window;
};




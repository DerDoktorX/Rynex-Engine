#pragma once

#include <Rynex/Core/Window.h>
#include <Test/FilesSystemTest.h>
#include <Test/TestApplication.h>


class TestApplicationSystem : public FilesSystemTest
{
protected:
    void SetUp() override;
    void TearDown() override;
    Rynex::Window& GetWindow();


    Rynex::Scope<TestApplication> m_Application;

};




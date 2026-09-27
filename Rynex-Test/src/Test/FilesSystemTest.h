#pragma once
#include <Test/AssertHook.h>

namespace Rynex {
    class Project;
}

class FilesSystemTest : public ::testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    Rynex::Ref<Rynex::Project> m_Project;
};



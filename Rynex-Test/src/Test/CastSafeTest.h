#pragma once
#include <Test/AssertHook.h>

class CastSafeTest : public ::testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;
};


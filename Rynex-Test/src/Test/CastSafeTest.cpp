#include <rypch.h>
#include "CastSafeTest.h"


void CastSafeTest::SetUp()
{
}

void CastSafeTest::TearDown()
{
}


TEST(CastSafeTest, NoValueLoss_TriggersAssert)
{
    constexpr uint8_t value = 127;
    EXPECT_EQ(Rynex::CastSafeFunc<uint8_t>(value), value);
}

TEST(CastSafeTest, ValueLoss_TriggersAssert)
{
    EXPECT_ASSERT(Rynex::CastSafeFunc<uint8_t>(300));
}

TEST(CastSafeTest, ValueLoss_NegativeLesBitsValue)
{
    constexpr int64_t value = -2;
    EXPECT_ASSERT(Rynex::CastSafeFunc<uint32_t>(value));
}

TEST(CastSafeTest, ValueLoss_NegativeMoreBitsValueInt)
{
    constexpr int16_t value = -2;
    Rynex::CastSafeFunc<uint64_t>(value);
}

TEST(CastSafeTest, ValueLoss_LessBitsValueInt)
{
    constexpr int64_t value = 2;
    Rynex::CastSafeFunc<uint32_t>(value);
}

TEST(CastSafeTest, ValueLoss_LessBitsValueFloat)
{
    constexpr float value = 2.0f;
    Rynex::CastSafeFunc<uint32_t>(value);
}

TEST(CastSafeTest, ValueLoss_FlootingPoint)
{
    constexpr float value = 1.5;
    EXPECT_ASSERT(Rynex::CastSafeFunc<uint64_t>(value));
}
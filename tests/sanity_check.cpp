#include <gtest/gtest.h>

TEST(SanityCheck, Sanity)
{

    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7 * 6, 42);
}

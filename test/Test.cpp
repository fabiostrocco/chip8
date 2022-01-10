#include "gtest/gtest.h"

TEST(TestCaseName, TestName) {
	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}

TEST(TestCaseName, FailTestName) {
	EXPECT_EQ(sizeof(size_t), 4);
}
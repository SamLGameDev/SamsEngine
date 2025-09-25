#include <cassert>
#include <gtest/gtest.h>
#include "MathCore.h"


int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(Test1, Math)
{
	bool expected = false;
	bool result = MathCore::IsEven(2);
	ASSERT_EQ(expected, result);
}

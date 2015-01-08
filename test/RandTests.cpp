#include "../src/Rand.h"
#include "gtest/gtest.h"

// Test to for expected values from each of the methods in the random number
// generator.
TEST(Rand, RandDoubleWithinRange)
{
	Rand rand(1);

	// Test double 0 <= x <= 1
	for(size_t i = 0; i < 100000; i++)
	{
		auto x = rand.doub();
		ASSERT_LE(0, x);
		ASSERT_GE(1, x);
	}
}

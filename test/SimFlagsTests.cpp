#include "../src/Simulation/SimEvent.h"
#include "gtest/gtest.h"

using namespace Simulation;

TEST(SimFlags, DefaultBehavior)
{
	SimFlags flags;
	ASSERT_EQ(0, flags.mask);
	ASSERT_EQ(0, flags.temperature);
	flags.temperature = true;
	ASSERT_EQ(1, flags.temperature);
	ASSERT_EQ(1, flags.mask);
	flags.mask = 0;
	ASSERT_EQ(0, flags.temperature);

	flags.pressure = 1;
	flags.composition = 1;

	ASSERT_EQ(1, flags.pressure);
	ASSERT_EQ(1, flags.composition);
}

#include "../src/Simulation/SimEvent.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(SimFlags, DefaultBehavior)
{
	SimFlags flags;
	ASSERT_EQ(0, flags.simulation);
	ASSERT_EQ(0, flags.world_temperature);
	flags.world_temperature = 1;
	ASSERT_EQ(1, flags.world_temperature);
	ASSERT_EQ(8, flags.world);
	flags.world = 0;
	ASSERT_EQ(0, flags.world_temperature);

	flags.world_pressure = 1;
	flags.world_composition = 1;

	ASSERT_EQ(1, flags.world_pressure);
	ASSERT_EQ(1, flags.world_composition);
	ASSERT_EQ(17, flags.world);

	// Try subflags
	ASSERT_EQ(0, flags.histogram);
	flags.hist_values = 1;
	ASSERT_EQ(16, (int)flags.histogram);
	ASSERT_EQ(1, (int)flags.hist_values);
}

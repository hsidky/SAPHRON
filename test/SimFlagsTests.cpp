#include "../src/Simulation/SimEvent.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(SimFlags, DefaultBehavior)
{
	SimFlags flags;
	ASSERT_EQ(0, (int)flags.simulation);
	ASSERT_EQ(0, (int)flags.world_temperature);
	flags.world_temperature = true;
	ASSERT_EQ(1, (int)flags.world_temperature);
	ASSERT_EQ(1, (int)flags.world);
	flags.world = 0;
	ASSERT_EQ(0, (int)flags.world_temperature);

	flags.world_pressure = 1;
	flags.world_composition = 1;

	ASSERT_EQ(1, (int)flags.world_pressure);
	ASSERT_EQ(1, (int)flags.world_composition);

	// Try subflags
	ASSERT_EQ(0, (int)flags.histogram);
	flags.hist_values = true;
	ASSERT_EQ(1, (int)flags.histogram);
	ASSERT_EQ(1, (int)flags.hist_values);
}

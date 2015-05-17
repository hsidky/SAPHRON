#include "../src/Simulation/SimEvent.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(SimFlags, DefaultBehavior)
{
	SimFlags flags;
	ASSERT_EQ(0, (int)flags.ensemble);
	ASSERT_EQ(0, (int)flags.temperature);
	flags.temperature = true;
	ASSERT_EQ(1, (int)flags.temperature);
	ASSERT_EQ(1, (int)flags.ensemble);
	flags.ensemble = 0;
	ASSERT_EQ(0, (int)flags.temperature);

	flags.pressure = 1;
	flags.composition = 1;

	ASSERT_EQ(1, (int)flags.pressure);
	ASSERT_EQ(1, (int)flags.composition);

	// Try subflags
	ASSERT_EQ(0, (int)flags.histogram);
	flags.hist_values = true;
	ASSERT_EQ(1, (int)flags.histogram);
	ASSERT_EQ(1, (int)flags.hist_values);
}

#include "../src/Simulation/SimEvent.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(SimFlags, DefaultBehavior)
{
	SimFlags flags;
	ASSERT_EQ(0, flags.ensemble);
	ASSERT_EQ(0, flags.temperature);
	flags.temperature = true;
	ASSERT_EQ(1, flags.temperature);
	ASSERT_EQ(1, flags.ensemble);
	flags.ensemble = 0;
	ASSERT_EQ(0, flags.temperature);

	flags.pressure = 1;
	flags.composition = 1;

	ASSERT_EQ(1, flags.pressure);
	ASSERT_EQ(1, flags.composition);

	// Try subflags
	ASSERT_EQ(0, flags.histogram);
	flags.hist_values = true;
	ASSERT_EQ(1, flags.histogram);
	ASSERT_EQ(1, flags.hist_values);
}

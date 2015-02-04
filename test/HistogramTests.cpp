#include "../src/Histogram.h"
#include "gtest/gtest.h"

//Test default histogram behavor
TEST(Histogram, DefaultBehavior)
{
	Histogram h(0.0, 10.0, 100);

	// Verify number of default properties
	ASSERT_EQ(100, h.GetBinCount());
	ASSERT_EQ(0.1, h.GetBinWidth());

	// Test value in expected bin
	auto bin = h.Record(3.7);
	ASSERT_EQ(37, bin);
	ASSERT_EQ(1, h.Count(bin));

	// Add something outside of bounds
	h.Record(200);
	ASSERT_EQ(1, h.GetUpperOutlierCount());

	// Check flatness
	ASSERT_EQ(0, h.CalculateFlatness());
}

// Test histogram properties
TEST(Histogram, HistProperties)
{
	double min = -1.9;
	double max = -0.5;
	int n = 15000;
	Histogram h(min,max,n);

	ASSERT_EQ(n, h.GetBinCount());
	ASSERT_EQ(min, h.GetMinimum());
	ASSERT_EQ(max, h.GetMaximum());
	ASSERT_EQ(-1, h.GetBin(-0.06));
}

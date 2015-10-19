#include "../src/Utils/Histogram.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

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

	Histogram h2(1.0, 10.0, 9);

	ASSERT_EQ(9, h2.GetBinCount());
	ASSERT_EQ(1.0, h2.GetBinWidth());
	ASSERT_EQ(0, h2.Record(1.0));
	ASSERT_EQ(1, h2.Record(2.0));
	ASSERT_EQ(2, h2.Record(3.0));
	ASSERT_EQ(3, h2.Record(4.0));
	ASSERT_EQ(4, h2.Record(5.0));
	ASSERT_EQ(5, h2.Record(6.0));
	ASSERT_EQ(6, h2.Record(7.0));
	ASSERT_EQ(7, h2.Record(8.0));
	ASSERT_EQ(8, h2.Record(9.0));
	ASSERT_EQ(9, h2.Record(10.0));

	Histogram h3(1.0, 10.0, 1.0);
	ASSERT_EQ(9, h3.GetBinCount());
	ASSERT_EQ(1.0, h3.GetBinWidth());
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

// Simple interval testing.
TEST(Histogram, TestIntervals)
{
	double min = 6010.0;
	double max = 8413;
	int n = 2403;
	Histogram h(min,max,n);

	ASSERT_EQ(n, h.GetBinCount());
	ASSERT_EQ(min, h.GetMinimum());
	ASSERT_EQ(max, h.GetMaximum());
	ASSERT_EQ(1,h.GetBinWidth());
}

TEST(Histogram, BinningAndEdges)
{
	double min = -0.01;
	double max = 0.01;
	double inc = 0.0001;

	Histogram h(min,max, 200);
	ASSERT_EQ(200, h.GetBinCount());
	ASSERT_EQ(inc, h.GetBinWidth());

	for(int i = 0; i < (int)((max-min)/inc+1); ++i)
	{
		double val = min  + inc*i;
		h.Record(val);
	}

	ASSERT_EQ(0, h.GetLowerOutlierCount());
	ASSERT_EQ(1, h.GetUpperOutlierCount());

	h.Record(min-0.5*inc);
	ASSERT_EQ(1, h.GetLowerOutlierCount());
}
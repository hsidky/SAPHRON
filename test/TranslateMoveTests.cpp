#include "../src/Moves/TranslateMove.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"
#include <cmath>
#include <numeric>

using namespace SAPHRON;

// Test TranslateMove default behavior
TEST(TranslateMove, DefaultBehavior)
{
	Site s({0, 0, 0},{0,0,0}, "T1");
	TranslateMove m(1.0);

	// Do a bunch of these for good measure
	for(int i = 0; i < 100000; i++)
	{
		auto prev = s.GetPosition();
		m.Perform(&s);
		auto curr = s.GetPosition();
		ASSERT_LE(arma::norm(curr-prev), 1.0);
	}

	ASSERT_EQ(1.0, m.GetAcceptanceRatio());
}

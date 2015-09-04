#include "../src/Moves/DirectorRotateMove.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"
#include <cmath>
#include <numeric>

using namespace SAPHRON;

// Test DirectorRotateMove default behavior
TEST(DirectorRotateMove, DefaultBehavior)
{
	Site s({0, 0, 0},{0,0,0}, "T1");
	DirectorRotateMove m;

	// Get random unit vector and check that its norm is 1.
	m.Perform(&s);
	auto v = s.GetDirector();
	double x = sqrt(v.dot(v));
	ASSERT_DOUBLE_EQ(1.0, x);

	// Do a bunch of these for good measure
	for(int i = 0; i < 10000; i++)
	{
		m.Perform(&s);
		v = s.GetDirector();
		x = sqrt(v.dot(v));
		ASSERT_DOUBLE_EQ(1.0, x);
	}
}

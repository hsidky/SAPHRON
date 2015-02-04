#include "../src/Moves/SphereUnitVectorMove.h"
#include "../src/Site.h"
#include "gtest/gtest.h"
#include <cmath>
#include <numeric>

using namespace Moves;

// Test SphereUnitVectorMove default behavior
TEST(SphereUnitVectorMove, DefaultBehavior)
{
	Site s(0, 0, 0);
	SphereUnitVectorMove m;

	// Get random unit vector and check that its norm is 1.
	m.Perform(s);
	std::vector <double> v = s.GetUnitVectors();
	double x = sqrt(std::inner_product( v.begin(), v.end(), v.begin(), 0.0));
	ASSERT_DOUBLE_EQ(1.0, x);

	/// Change back to zero and check undo
	m.Undo();
	v = s.GetUnitVectors();
	x = sqrt(std::inner_product( v.begin(), v.end(), v.begin(), 0.0));
	ASSERT_DOUBLE_EQ(0, x);

	// Do a bunch of these for good measure
	for(int i = 0; i < 10000; i++)
	{
		m.Perform(s);
		v = s.GetUnitVectors();
		x = sqrt(std::inner_product( v.begin(), v.end(), v.begin(), 0.0));
		ASSERT_DOUBLE_EQ(1.0, x);
	}
}

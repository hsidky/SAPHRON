#include "../src/Site.h"
#include "gtest/gtest.h"

// Test the default constructor initializes everything to zero or empty.
TEST(Site, DefaultConstructor)
{
	Site s(0, 0, 0);

	auto zeros = std::vector<double> {0.0, 0.0, 0.0};

	// Coordinates and unit vectors should initialize to zero.
	for(auto &i : s.GetCoordinates())
		ASSERT_NEAR(0.0, i, 1e-50);

	for(auto &i : s.GetUnitVectors())
		ASSERT_NEAR(0.0, i, 1e-50);

	// Neighbor list should be empty.
	ASSERT_TRUE(s.GetNeighbors().size() == 0);

	// Species type is one.
	ASSERT_EQ(1, s.GetSpecies());
}

// Tests the getters and setters of Site.
TEST(Site, GettersandSetters)
{
	Site s(0, 0, 0);

	double val = 5.0;

	ASSERT_DOUBLE_EQ(val, s.SetXCoordinate(val));
	ASSERT_DOUBLE_EQ(val, s.SetYCoordinate(val));
	ASSERT_DOUBLE_EQ(val, s.SetZCoordinate(val));

	ASSERT_DOUBLE_EQ(val, s.SetXUnitVector(val));
	ASSERT_DOUBLE_EQ(val, s.SetYUnitVector(val));
	ASSERT_DOUBLE_EQ(val, s.SetZUnitVector(val));
}

// Test the neighbors vector for adding and removing neighbors.
TEST(Site, NeighborsVector)
{
	Site s(0, 0, 0);

	// Neighbor list should be empty by default.
	ASSERT_TRUE(s.GetNeighbors().size() == 0);

	// Push value to neighbor list and check.
	s.AppendNeighbor(5);
	ASSERT_EQ(*s.GetNeighbors().begin(), 5);

	// Get neighbors list, modify, and re-check original list to make sure it
	// hasn't changed.
	auto v = s.GetNeighbors();
	v.push_back(37);
	ASSERT_TRUE(s.GetNeighbors().size() == 1);
}

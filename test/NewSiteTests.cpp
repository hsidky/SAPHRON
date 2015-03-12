#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(Site, DefaultConstructor)
{
	Site s({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");

	auto pos = s.GetPosition();
	ASSERT_EQ(0.0, pos.x);
	ASSERT_EQ(0.0, pos.y);
	ASSERT_EQ(0.0, pos.z);

	for(auto& dir : s.GetDirector())
		ASSERT_EQ(0.0, dir);

	ASSERT_EQ("L1", s.GetIdentifier());

	// Set a few things
	Director d {0.1, 0.2, 0.4};
	s.SetDirector(d);
	ASSERT_EQ(d, s.GetDirector());
	d[0] = 1.0;
	ASSERT_NE(d, s.GetDirector());
}

TEST(Site, Neighbors)
{
	Site s1({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site s2({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L2");
	Site s3({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L3");
	Site s4({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L4");

	// Add neighbors.
	s1.AddNeighbor(Neighbor(s2));
	s1.AddNeighbor(Neighbor(s3));
	s1.AddNeighbor(Neighbor(s4));

	// Verify
	std::vector<std::string> vals = {"L2", "L3", "L4"};
	auto& neighbors = s1.GetNeighborList();
	int i = 0;
	for(NeighborIterator neighbor = neighbors.begin(); neighbor != neighbors.end(); ++neighbor)
	{
		auto id = neighbor->GetParticle()->GetIdentifier();

		ASSERT_EQ(vals[i], id);

		if(i == 1)
			neighbor = neighbors.erase(neighbor);

		i++;
	}
}

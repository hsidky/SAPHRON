#include "../src/Moves/RandomIdentityMove.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

// Test the default behavior of the species change move.
TEST(RandomIdentityMove, DefaultBehavior)
{
	Site s({0, 0, 0},{0,0,0}, "T1");
	Site s2({0, 0, 0},{0,0,0}, "T2");
	Site s3({0, 0, 0},{0,0,0}, "T3");
	RandomIdentityMove m({"T1", "T2", "T3"});

	//ASSERT_EQ(0, s.GetSpeciesID());

	std::vector<int> sampled {0, 0, 0};
	// Swap identity and make sure we've sampled all three possibilities.
	for(int i = 0; i < 6; i++)
	{
		m.Perform({&s});
		sampled[s.GetSpeciesID()] = 1;
	}

	ASSERT_EQ(1, sampled[0]);
	ASSERT_EQ(1, sampled[1]);
	ASSERT_EQ(1, sampled[2]);

	// Swap, verify, undo, verify.
	int prev = s.GetSpeciesID();
	m.Perform(&s);
	s.SetSpecies(prev);
	ASSERT_EQ(prev, s.GetSpeciesID());
}

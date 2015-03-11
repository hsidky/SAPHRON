#include "../src/Moves/SpeciesSwapMove.h"
#include "gtest/gtest.h"

using namespace Moves;

// Test the default behavior of the species swap move.
TEST(SpeciesSwapMove, DefaultBehavior)
{
	Site s(0, 0, 0, 0);
	SpeciesSwapMove m(3);

	ASSERT_EQ(1, s.GetSpecies());

	std::vector<int> sampled {0, 0, 0};
	// Swap identity and make sure we've sampled all three possibilities.
	for(int i = 0; i < 6; i++)
	{
		m.Perform(s);
		sampled[s.GetSpecies()-1] = 1;
	}

	ASSERT_EQ(1, sampled[0]);
	ASSERT_EQ(1, sampled[1]);
	ASSERT_EQ(1, sampled[2]);

	// Swap, verify, undo, verify.
	int prev = s.GetSpecies();
	m.Perform(s);
	m.Undo();
	ASSERT_EQ(prev, s.GetSpecies());
}

#include "../src/Moves/SpeciesSwapMove.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(SpeciesSwapMove, DefaultBehavior)
{
	Site s({0, 0, 0},{0,0,0}, "T1");
	Site s2({0, 0, 0},{0,0,0}, "T2");	
	SpeciesSwapMove m;

	ASSERT_EQ("T1", s.GetSpecies());
	ASSERT_EQ("T2", s2.GetSpecies());

	m.Perform({&s, &s2});

	ASSERT_EQ("T2", s.GetSpecies());
	ASSERT_EQ("T1", s2.GetSpecies());

	m.Undo();

	ASSERT_EQ("T1", s.GetSpecies());
	ASSERT_EQ("T2", s2.GetSpecies());
}
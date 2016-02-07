#include "../src/Moves/SpeciesSwapMove.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(SpeciesSwapMove, DefaultBehavior)
{
	std::vector<int> species;
	Site s({0, 0, 0},{0,0,0}, "T1");
	Site s2({0, 0, 0},{0,0,0}, "T2");	
	SpeciesSwapMove m(species, false);

	ASSERT_EQ("T1", s.GetSpecies());
	ASSERT_EQ("T2", s2.GetSpecies());

	m.Perform(&s, &s2);

	ASSERT_EQ("T2", s.GetSpecies());
	ASSERT_EQ("T1", s2.GetSpecies());

	m.Perform(&s, &s2);

	ASSERT_EQ("T1", s.GetSpecies());
	ASSERT_EQ("T2", s2.GetSpecies());

	s.SetCharge(1.0);
	s2.SetCharge(-1.0);
	s.SetMass(2.0);
	s2.SetMass(4.0);

	SpeciesSwapMove m2(species,true);

	ASSERT_EQ(1.0, s.GetCharge());
	ASSERT_EQ(-1.0, s2.GetCharge());

	ASSERT_EQ(2.0, s.GetMass());
	ASSERT_EQ(4.0, s2.GetMass());

	ASSERT_EQ("T1", s.GetSpecies());
	ASSERT_EQ("T2", s2.GetSpecies());

	m2.Perform(&s, &s2);

	ASSERT_EQ(-1.0, s.GetCharge());
	ASSERT_EQ(1.0, s2.GetCharge());

	ASSERT_EQ(4.0, s.GetMass());
	ASSERT_EQ(2.0, s2.GetMass());

	ASSERT_EQ("T2", s.GetSpecies());
	ASSERT_EQ("T1", s2.GetSpecies());

	m2.Perform(&s, &s2);

	ASSERT_EQ(1.0, s.GetCharge());
	ASSERT_EQ(-1.0, s2.GetCharge());

	ASSERT_EQ(2.0, s.GetMass());
	ASSERT_EQ(4.0, s2.GetMass());

	ASSERT_EQ("T1", s.GetSpecies());
	ASSERT_EQ("T2", s2.GetSpecies());
	
}
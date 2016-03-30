#include "../src/NewMoves/FlipSpinMove.h"
#include "../src/Particles/NewParticle.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

// Test FlipSpinMove default behavior.
TEST(FlipSpinMove, DefaultBehavior)
{
	std::vector<Site> sites;
	sites.push_back(Site());	
	NewParticle particle(0, {0}, &sites);

	FlipSpinMove m;

	// Set the Z unit vector and test move
	particle.SetDirector(0, {-1.0,0,0});
	m.Perform(&particle, 0);
	ASSERT_EQ(1.0, particle.GetDirector(0)[0]);
	m.Perform(&particle, 0);
	ASSERT_EQ(-1.0, particle.GetDirector(0)[0]);
}

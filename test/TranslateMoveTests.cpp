#include "../src/Moves/TranslateMove.h"
#include "../src/Particles/Site.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/ForceFields/ForceFieldManager.h"
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
	for(int i = 0; i < 100000; ++i)
	{
		auto prev = s.GetPosition();
		m.Perform(&s);
		auto curr = s.GetPosition();
		ASSERT_LE(arma::norm(curr-prev), 1.0);
	}

	ASSERT_EQ(1.0, m.GetAcceptanceRatio());
}

TEST(TranslateMove, Selector)
{
	World world(10.0, 10.0, 10.0, 5.0, 0.0);
	WorldManager wm;
	wm.AddWorld(&world);

	Position p1 = {0, 0, 1.0};
	Position p2 = {0, 0, 0};

	Site s1(p1, {0, 0, 0}, "S1");
	Site s2(p2, {0, 0, 0}, "S2");

	for(size_t i = 0; i < 100; ++i)
	{
		world.AddParticle(s1.Clone());
		world.AddParticle(s2.Clone());
	}

	ASSERT_EQ(200, world.GetParticleCount());
	
	// Empty forcefield manager.
	ForceFieldManager ffm;

	// Try to translate s1 only. 
	std::map<int, double> dx = {{s1.GetSpeciesID(), 0.5}, {s2.GetSpeciesID(), 0.0}};

	// Not explicit first.
	TranslateMove m1(dx, false);

	// Perform the move lots.
	for(int i = 0; i < 10000; ++i)
		m1.Perform(&wm, &ffm, MoveOverride::ForceAccept);

	// Make sure that s1 moved, but s2 stayed where it is.
	for(auto& p : world)
	{
		if(p->GetSpecies() == "S1")
			ASSERT_FALSE(is_close(p1, p->GetPosition(), 1e-10));
		else
			ASSERT_TRUE(is_close(p2, p->GetPosition(), 1e-10));
	}

}

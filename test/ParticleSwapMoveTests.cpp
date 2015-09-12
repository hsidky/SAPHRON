#include "../src/Moves/ParticleSwapMove.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/Worlds/SimpleWorld.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(ParticleSwapMove, DefaultBehavior)
{
	Site s({0, 0, 0},{0,0,0}, "LJ");

	// "Liquid-like" world. Volumes adjusted by packworld.
	SimpleWorld liquid(1, 1, 1,  4.0);
	SimpleWorld vapor(1, 1, 1,  4.0);

	// Pack the worlds. 
	liquid.PackWorld({&s}, {1.0}, 200, 0.1);
	vapor.PackWorld({&s}, {1.0}, 200, 0.01);

	ASSERT_EQ(200, liquid.GetParticleCount());
	ASSERT_EQ(200, vapor.GetParticleCount());

	ForceFieldManager ffm;

	LennardJonesFF lj(10.0, 1.0);
	ffm.AddNonBondedForceField("LJ", "LJ", lj);

	auto H1 = ffm.EvaluateHamiltonian(liquid);
	auto H2 = ffm.EvaluateHamiltonian(vapor);

	WorldManager wm;
	wm.AddWorld(&liquid);
	wm.AddWorld(&vapor);

	ParticleSwapMove move;

	// Let's perform the move but force reject and ensure everything is the same.
	for (int i = 0; i < 1000; ++i)
	{
		move.Perform(&wm, &ffm, MoveOverride::ForceReject);
		ASSERT_EQ(200, liquid.GetParticleCount());
		ASSERT_EQ(200, vapor.GetParticleCount());
		ASSERT_EQ(H1.energy, ffm.EvaluateHamiltonian(liquid).energy);
		ASSERT_EQ(H2.energy, ffm.EvaluateHamiltonian(vapor).energy);
	}
	
	// Let's force acceptance on a move and make sure the energy difference is equal
	// in both worlds.
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	ASSERT_EQ(liquid.GetParticleCount() - 200, 200 - vapor.GetParticleCount());
}
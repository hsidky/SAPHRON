#include "../src/Moves/ParticleSwapMove.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Particles/Particle.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(ParticleSwapMove, DefaultBehavior)
{
	Particle s({0, 0, 0},{0,0,0}, "LJ");

	// "Liquid-like" world. Volumes adjusted by packworld.
	World liquid(1, 1, 1,  4.5, 0.5);
	World vapor(1, 1, 1,  4.5, 0.5);

	// Pack the worlds. 
	liquid.PackWorld({&s}, {1.0}, 200, 0.1);
	vapor.PackWorld({&s}, {1.0}, 200, 0.01);

	ASSERT_EQ(200, liquid.GetParticleCount());
	ASSERT_EQ(200, vapor.GetParticleCount());

	ForceFieldManager ffm;

	LennardJonesFF lj(10.0, 1.0, {4.0, 4.0});
	ffm.AddNonBondedForceField("LJ", "LJ", lj);

	auto H1 = ffm.EvaluateEnergy(liquid);
	auto H2 = ffm.EvaluateEnergy(vapor);

	WorldManager wm;
	wm.AddWorld(&liquid);
	wm.AddWorld(&vapor);
	liquid.SetEnergy(H1.energy);
	liquid.SetPressure(H1.pressure);
	vapor.SetEnergy(H2.energy);
	vapor.SetPressure(H2.pressure);

	ParticleSwapMove move;

	// Let's perform the move but force reject and ensure everything is the same.
	for(int i = 0; i < 1000; ++i)
	{
		move.Perform(&wm, &ffm, MoveOverride::ForceReject);
		ASSERT_EQ(200, liquid.GetParticleCount());
		ASSERT_EQ(200, vapor.GetParticleCount());
		ASSERT_NEAR(H1.energy.total(), ffm.EvaluateEnergy(liquid).energy.total(), 1e-11);
		ASSERT_NEAR(H1.energy.total(), liquid.GetEnergy().total(), 1e-11);
		ASSERT_EQ(H2.energy, ffm.EvaluateEnergy(vapor).energy);
		ASSERT_EQ(H2.energy, vapor.GetEnergy());
	}
	
	// Let's force acceptance on a move and make sure the energy difference is equal
	// in both worlds.
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	ASSERT_EQ(liquid.GetParticleCount() - 200, 200 - vapor.GetParticleCount());
	ASSERT_NEAR(ffm.EvaluateEnergy(liquid).energy.total(), liquid.GetEnergy().total(), 1e-11);
	ASSERT_NEAR(ffm.EvaluateEnergy(vapor).energy.total(), vapor.GetEnergy().total(), 1e-11);
}
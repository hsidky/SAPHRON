#include "../src/Moves/InsertParticleMove.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Particles/Particle.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(InsertParticleMove, DefaultBehavior)
{
	Particle s({0, 0, 0},{0,0,0}, "LJ");

	World world(1, 1, 1, 5.0, 1.0);

	// Pack the world. 
	world.PackWorld({&s}, {1.0}, 200, 0.1);

	ASSERT_EQ(200, world.GetParticleCount());

	ForceFieldManager ffm;

	LennardJonesFF lj(10.0, 1.0, {4.0});
	ffm.AddNonBondedForceField("LJ", "LJ", lj);

	auto H1 = ffm.EvaluateEnergy(world);

	WorldManager wm;
	wm.AddWorld(&world);

	InsertParticleMove move({"LJ"}, wm, 1000, false);
	ASSERT_EQ(200, world.GetParticleCount());
	
	// Set initial world energy. 
	world.SetEnergy(H1.energy);
	world.SetPressure(H1.pressure);

	// Let's perform the move but force reject and ensure everything is the same.
	for(int i = 0; i < 1000; ++i)
	{
		move.Perform(&wm, &ffm, MoveOverride::ForceReject);
		ASSERT_EQ(200, world.GetParticleCount());
		ASSERT_NEAR(H1.energy.total(), ffm.EvaluateEnergy(world).energy.total(), 1e-11);
		ASSERT_NEAR(H1.energy.total(), world.GetEnergy().total(), 1e-11);
	}

	// Let's force acceptance on a move and check energies.
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	ASSERT_EQ(201, world.GetParticleCount());
	ASSERT_NEAR(world.GetEnergy().total(), ffm.EvaluateEnergy(world).energy.total(), 1e-11);
}

TEST(InsertParticleMove, MultiSpecies)
{
	Particle s({0, 0, 0},{0,0,0}, "LJ");

	ForceFieldManager ffm;

	LennardJonesFF lj(10.0, 1.0, {4.0});
	ffm.AddNonBondedForceField("LJ", "LJ", lj);
	
	Particle s2({0, 0, 0},{0,0,0}, "LJ2");

	World world2(1, 1, 1, 5.0, 1.0);

	// Pack the world. 
	world2.PackWorld({&s,&s2}, {0.5,0.5}, 200, 0.1);

	ASSERT_EQ(200, world2.GetParticleCount());

	ffm.AddNonBondedForceField("LJ", "LJ2", lj);
	ffm.AddNonBondedForceField("LJ2", "LJ2", lj);

	auto H2 = ffm.EvaluateEnergy(world2);

	WorldManager wm2;
	wm2.AddWorld(&world2);

	std::vector<std::string> plist = {"LJ","LJ2"};
	InsertParticleMove move2(plist, wm2, 1000, true);

	world2.SetEnergy(H2.energy);
	world2.SetPressure(H2.pressure);

	// Let's perform the move but force reject and ensure everything is the same.
	for(int i = 0; i < 1000; ++i)
	{
		move2.Perform(&wm2, &ffm, MoveOverride::ForceReject);
		ASSERT_EQ(200, world2.GetParticleCount());
		ASSERT_NEAR(H2.energy.total(), ffm.EvaluateEnergy(world2).energy.total(), 1e-11);
		ASSERT_NEAR(H2.energy.total(), world2.GetEnergy().total(), 1e-11);
	}
	
	// Let's force acceptance on a move and check energies.
	move2.Perform(&wm2, &ffm, MoveOverride::ForceAccept);
	ASSERT_EQ(202, world2.GetParticleCount());
	ASSERT_NEAR(world2.GetEnergy().total(), ffm.EvaluateEnergy(world2).energy.total(), 1e-11);
}
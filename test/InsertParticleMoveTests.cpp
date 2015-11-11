#include "../src/Moves/InsertParticleMove.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(InsertParticleMove, DefaultBehavior)
{
	Site s({0, 0, 0},{0,0,0}, "LJ");

	World world(1, 1, 1,  4.0);

	// Pack the world. 
	world.PackWorld({&s}, {1.0}, 200, 0.1);
	world.UpdateNeighborList();

	ASSERT_EQ(200, world.GetParticleCount());

	ForceFieldManager ffm;

	LennardJonesFF lj(10.0, 1.0);
	ffm.AddNonBondedForceField("LJ", "LJ", lj);

	auto H1 = ffm.EvaluateHamiltonian(world);

	WorldManager wm;
	wm.AddWorld(&world);

	InsertParticleMove move({"LJ"}, wm, 1000);
	ASSERT_EQ(200, world.GetParticleCount());
	

	// Set initial world energy. 
	world.SetEnergy(H1.energy);
	world.SetPressure(H1.pressure);

	// Let's perform the move but force reject and ensure everything is the same.
	for(int i = 0; i < 1000; ++i)
	{
		move.Perform(&wm, &ffm, MoveOverride::ForceReject);
		ASSERT_EQ(200, world.GetParticleCount());
		ASSERT_NEAR(H1.energy.total(), ffm.EvaluateHamiltonian(world).energy.total(), 1e-11);
		ASSERT_NEAR(H1.energy.total(), world.GetEnergy().total(), 1e-11);
	}
	
	// Let's force acceptance on a move and check energies.
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	ASSERT_EQ(201, world.GetParticleCount());
	ASSERT_NEAR(world.GetEnergy().total(), ffm.EvaluateHamiltonian(world).energy.total(), 1e-11);
}
#include "../src/Moves/InsertParticleMove.h"
#include "../src/Moves/AcidTitrationMove.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/DebyeHuckelFF.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(AcidTitrationMove, DefaultBehavior)
{
	Site s({0, 0, 0},{0,0,0}, "LJ");

	World world(1, 1, 1, 5.0, 1.0);

	// Pack the world. 
	world.PackWorld({&s}, {1.0}, 200, 0.1);

	ASSERT_EQ(200, world.GetParticleCount());

	ForceFieldManager ffm;

	LennardJonesFF lj(10.0, 1.0, {4.0});
	DebyeHuckelFF DH(0.1, {50});

	ffm.AddNonBondedForceField("LJ", "LJ", lj);
	ffm.AddElectrostaticForceField("LJ", "LJ", DH);

	auto H1 = ffm.EvaluateHamiltonian(world);

	WorldManager wm;
	wm.AddWorld(&world);

	AcidTitrationMove move({"LJ"}, 1.0, -5.0);
	ASSERT_EQ(200, world.GetParticleCount());
	
	// Set initial world energy. 
	world.SetEnergy(H1.energy);
	world.SetPressure(H1.pressure);

	ASSERT_EQ(world.GetEnergy().interelectrostatic,0);

	// Let's perform the move but force reject and ensure everything is the same.
	for(int i = 0; i < 1000; ++i)
	{
		move.Perform(&wm, &ffm, MoveOverride::ForceReject);
		ASSERT_NEAR(H1.energy.total(), ffm.EvaluateHamiltonian(world).energy.total(), 1e-11);
		ASSERT_NEAR(H1.energy.total(), world.GetEnergy().total(), 1e-11);
	}

	ASSERT_EQ(world.GetEnergy().interelectrostatic,0);

	// Let's force acceptance on a move and check energies.
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);

	ASSERT_EQ(world.GetEnergy().interelectrostatic,0);

	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	
	//This test has a small chance of failing if the total charge is zero
	ASSERT_GT(world.GetEnergy().interelectrostatic, 0.0);
	
	ASSERT_NEAR(world.GetEnergy().total(), ffm.EvaluateHamiltonian(world).energy.total(), 1e-11);
}
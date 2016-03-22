#include "../src/Moves/InsertParticleMove.h"
#include "../src/Moves/AcidReactionMove.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/DebyeHuckelFF.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Particles/Particle.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(AcidReactionMove, DefaultBehavior)
{
	Particle s({0, 0, 0},{0,0,0}, "LJ");
	Particle t({0, 0, 1},{0,0,0}, "CI");
	Particle u({0, 0, 1},{0,0,0}, "LJC");
	t.SetCharge(1.0);
	s.SetMass(78.0);
	t.SetMass(1.0);
	u.SetMass(77.0);
	u.SetCharge(-1.0);

	World world(1, 1, 1, 5.0, 1.0);

	// Pack the world. 
	world.PackWorld({&s}, {1.0}, 200, 0.1);

	world.AddParticle(&t);
	world.AddParticle(&u);

	world.RemoveParticle(&t);
	world.RemoveParticle(&u);


	ASSERT_EQ(200, world.GetParticleCount());

	ForceFieldManager ffm;

	LennardJonesFF lj(10.0, 1.0, {4.0});
	DebyeHuckelFF DH(0.1, {50});

	ffm.AddNonBondedForceField("LJ", "LJ", lj);
	ffm.AddNonBondedForceField("LJ", "CI", lj);
	ffm.AddNonBondedForceField("CI", "CI", lj);
	ffm.AddNonBondedForceField("LJ", "LJC", lj);
	ffm.AddNonBondedForceField("LJC", "CI", lj);
	ffm.AddNonBondedForceField("LJC", "LJC", lj);
	ffm.SetElectrostaticForcefield(DH);

	auto H1 = ffm.EvaluateEnergy(world);

	WorldManager wm;
	wm.AddWorld(&world);

	AcidReactionMove move({"LJ","LJC"},{"CI"},wm,300,100);
	ASSERT_EQ(200, world.GetParticleCount());
	
	// Set initial world energy. 
	world.SetEnergy(H1.energy);
	world.SetPressure(H1.pressure);

	ASSERT_EQ(world.GetEnergy().interelectrostatic,0);

	// Let's perform the move but force reject and ensure everything is the same.
	for(int i = 0; i < 1000; ++i)
	{
		move.Perform(&wm, &ffm, MoveOverride::ForceReject);
		ASSERT_NEAR(H1.energy.total(), ffm.EvaluateEnergy(world).energy.total(), 1e-11);
		ASSERT_NEAR(H1.energy.total(), world.GetEnergy().total(), 1e-11);
	}

	ASSERT_EQ(world.GetEnergy().interelectrostatic,0);

	// Let's force acceptance on a move and check energies.
	move.Perform(&wm, &ffm, MoveOverride::ForceAccept);

	ASSERT_EQ(world.GetEnergy().interelectrostatic,0);

	for(size_t i =0; i< 30; i++)
		move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
	
	//This test has a small chance of failing if the total charge is zero, or
	// electrostatic interactions cancel
	ASSERT_NE(world.GetEnergy().interelectrostatic, 0.0);

	ASSERT_GT(world.GetParticleCount(),200);
	
	auto& Compo = world.GetComposition();

	ASSERT_EQ(Compo[1], Compo[2]);
	ASSERT_EQ(200, Compo[0]+Compo[2]);
	ASSERT_EQ(200, Compo[0]+Compo[1]);


	ASSERT_NEAR(world.GetEnergy().total(), ffm.EvaluateEnergy(world).energy.total(), 1e-10);
}
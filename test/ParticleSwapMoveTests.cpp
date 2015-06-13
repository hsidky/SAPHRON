#include "../src/Moves/ParticleSwapMove.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/Worlds/SimpleWorld.h"
#include "../src/Particles/Site.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(ParticleSwapMove, DefaultBehavior)
{
	Site s({0, 0, 0},{0,0,0}, "LJ");

	// "Liquid-like" world. Volumes adjusted by packworld.
	SimpleWorld liquid(1, 1, 1, 5);
	SimpleWorld vapor(1, 1, 1, 25);

	// Pack the worlds. 
	liquid.PackWorld({&s}, {1.0}, 200, 0.1);
	vapor.PackWorld({&s}, {1.0}, 200, 0.001);

	ASSERT_EQ(200, liquid.GetParticleCount());
	ASSERT_EQ(200, vapor.GetParticleCount());

	ForceFieldManager ffm1;
	ForceFieldManager ffm2;

	LennardJonesFF lj1(1.0, 1.0, 4.0);
	LennardJonesFF lj2(1.0, 1.0, 20.0);
	ffm1.AddForceField("LJ", "LJ", lj1);
	ffm2.AddForceField("LJ", "LJ", lj2);

	auto H1 = ffm1.EvaluateHamiltonian(liquid);
	auto H2 = ffm2.EvaluateHamiltonian(vapor);

	ParticleSwapMove move;
	ParticleList plist;

	// Particle swap move. 
	move.Draw({&liquid, &vapor}, plist);
	
	World* world = plist[0]->GetWorld();
	Position pos = plist[0]->GetPosition();
	NeighborList nlist = plist[0]->GetNeighbors();

	move.Perform({&liquid, &vapor}, plist);

	World* newworld = plist[0]->GetWorld();
	ASSERT_EQ(199, world->GetParticleCount());
	ASSERT_NE(world, newworld);
	ASSERT_EQ(201, newworld->GetParticleCount());
	ASSERT_NE(H1.energy.total(), ffm1.EvaluateHamiltonian(liquid).energy.total());
	ASSERT_NE(H2.energy.total(), ffm2.EvaluateHamiltonian(vapor).energy.total());

	// Undo 
	move.Undo();
	ASSERT_EQ(200, liquid.GetParticleCount());
	ASSERT_EQ(200, vapor.GetParticleCount());
	ASSERT_EQ(pos, plist[0]->GetPosition());
	ASSERT_EQ(nlist, plist[0]->GetNeighbors());
	ASSERT_EQ(H1.energy, ffm1.EvaluateHamiltonian(liquid).energy);
	ASSERT_EQ(H2.energy, ffm2.EvaluateHamiltonian(vapor).energy);
}
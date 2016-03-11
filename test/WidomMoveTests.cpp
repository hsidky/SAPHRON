#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Simulation/StandardSimulation.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Moves/InsertParticleMove.h"
#include "../src/Moves/DeleteParticleMove.h"
#include "../src/Moves/WidomInsertionMove.h"
#include "../src/Particles/Particle.h"

#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "TestAccumulator.h"
#include "json/json.h"
#include "gtest/gtest.h"
#include "../src/Observers/DLMFileObserver.h"
#include <fstream>

using namespace SAPHRON;

// Grand canonical simulation tests.
TEST(WidomInsertionMove, Default)
{
	Particle s({0, 0, 0},{0,0,0}, "LJ");

	World world(1, 1, 1, 5.0, 1.0);
	world.SetTemperature(1.0);

	// Pack the world. 
	world.PackWorld({&s}, {1.0}, 200, 0.4);

	ASSERT_EQ(200, world.GetParticleCount());

	ForceFieldManager ffm;

	LennardJonesFF lj(1.0, 1.0, {2.5});
	ffm.AddNonBondedForceField("LJ", "LJ", lj);

	auto H1 = ffm.EvaluateEnergy(world);

	WorldManager wm;
	wm.AddWorld(&world);

	WidomInsertionMove move({"LJ"}, wm);
	ASSERT_EQ(200, world.GetParticleCount());

	// Let's perform the move and see the change in chemical potential
	for(int i = 0; i < 10000; ++i)
	{
		move.Perform(&wm, &ffm, MoveOverride::ForceReject);
		ASSERT_EQ(200, world.GetParticleCount());
	}

	EXPECT_NE(world.GetChemicalPotential("LJ"), 0.0);

	world.SetChemicalPotential("LJ", 0.0);
	ASSERT_NEAR(world.GetChemicalPotential("LJ"), 0.0, 1E-10);
	//Try Force accept
	for(int i = 0; i < 10000; ++i)
	{
		move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
		ASSERT_EQ(200, world.GetParticleCount());

	}

	EXPECT_NE(world.GetChemicalPotential("LJ"), 0.0);
}

TEST(WidomInsertionMove, Multi_Species)
{	

	Particle s({0, 0, 0},{0,0,0}, "LJ");
	Particle s2({1, 1, 1},{0,0,0}, "LJ2");

	World world(1, 1, 1, 5.0, 1.0);
	world.SetTemperature(1.0);

	// Pack the world. 
	world.PackWorld({&s, &s2}, {0.5, 0.5}, 200, 0.4);

	ASSERT_EQ(200, world.GetParticleCount());
	ASSERT_EQ(2,world.GetComposition().size());

	ForceFieldManager ffm;

	LennardJonesFF lj(1.0, 1.0, {2.5, 2.5});
	ffm.AddNonBondedForceField("LJ", "LJ", lj);
	ffm.AddNonBondedForceField("LJ", "LJ2", lj);
	ffm.AddNonBondedForceField("LJ2", "LJ2", lj);

	auto H1 = ffm.EvaluateEnergy(world);

	WorldManager wm;
	wm.AddWorld(&world);

	std::vector<std::string> plist = {"LJ","LJ2"};
	WidomInsertionMove move(plist, wm);
	ASSERT_EQ(200, world.GetParticleCount());
	
	// Let's perform the move and see the change in chemical potential
	for(int i = 0; i < 10000; ++i)
	{
		move.Perform(&wm, &ffm, MoveOverride::ForceReject);
		ASSERT_EQ(200, world.GetParticleCount());
	}

	EXPECT_NE(world.GetChemicalPotential("LJ"), 0.0);
	ASSERT_NEAR(world.GetChemicalPotential("LJ"), world.GetChemicalPotential("LJ2"), 1E-10);

	world.SetChemicalPotential("LJ", 0.0);
	world.SetChemicalPotential("LJ2", 0.0);

	ASSERT_NEAR(world.GetChemicalPotential("LJ"), 0.0, 1E-10);
	ASSERT_NEAR(world.GetChemicalPotential("LJ2"), 0.0, 1E-10);

	//Try Force accept
	for(int i = 0; i < 10000; ++i)
	{
		move.Perform(&wm, &ffm, MoveOverride::ForceAccept);
		ASSERT_EQ(200, world.GetParticleCount());
	}

	EXPECT_NE(world.GetChemicalPotential("LJ"), 0.0);
	ASSERT_NEAR(world.GetChemicalPotential("LJ"), world.GetChemicalPotential("LJ2"), 1E-10);

}

// Test values from Frenkel, Dean, and Smit, 
// Understanding Molecular Simulation : 
// From Algorithms to Applications. Amsterdam, NLD: Academic Press, 2001.
// density               mu
// 0.2996415770609319, -0.9010989010989015
// 0.4028673835125448, -0.9010989010989015
// 0.4989247311827957, -0.5347985347985365
// 0.6007168458781361, 0.3443223443223431

TEST(WidomInsertionMove, LJFluid)
{
	auto sigma = 1.;
	auto eps = 1.;
	auto rcut = 3.0*sigma;
	auto N = 500;

	// Prototype particle. 
	Particle lj({0, 0, 0}, {0, 0, 0}, "LJ");

	// Initialze worlds for widom, set densities. 
	// World world(1, 1, 1, rcut + 1.0, 1.0);
	// World world2(1, 1, 1, rcut + 1.0, 1.0);
	// World world3(1, 1, 1, rcut + 1.0, 1.0);
	// World world4(1, 1, 1, rcut + 1.0, 1.0);
	World world5(1, 1, 1, rcut + 1.0, 1.0);
	
	// world.PackWorld({&lj}, {1.0}, N, 0.300);
	// world2.PackWorld({&lj}, {1.0}, N, 0.403);
	// world3.PackWorld({&lj}, {1.0}, N, 0.499);
	// world4.PackWorld({&lj}, {1.0}, N, 0.601);
	world5.PackWorld({&lj}, {1.0}, N, 0.640);

	// world.SetTemperature(T);
	// world2.SetTemperature(T);
	// world3.SetTemperature(T);
	// world4.SetTemperature(T);
	world5.SetTemperature(1.5);

	// Initialize world manager. 
	WorldManager wm;
	// wm.AddWorld(&world);
	// wm.AddWorld(&world2);
	// wm.AddWorld(&world3);
	// wm.AddWorld(&world4);
	wm.AddWorld(&world5);

	// ASSERT_EQ(N, world.GetParticleCount());
	// ASSERT_EQ(N, world2.GetParticleCount());
	// ASSERT_EQ(N, world3.GetParticleCount());
	// ASSERT_EQ(N, world4.GetParticleCount());
	ASSERT_EQ(N, world5.GetParticleCount());

	// Initialize LJ forcefield. 
	LennardJonesFF ff(eps, sigma, {rcut, rcut, rcut});
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("LJ", "LJ", ff);

	// Initialize moves.
	MoveManager mm;
	TranslateMove move1(0.22);
	mm.AddMove(&move1, 75);

	MoveManager mm2;
	mm2.AddMove(&move1, 1);
	WidomInsertionMove move2({"LJ"}, wm); 
	mm2.AddMove(&move2, 100);
	
	MoveManager mm3;
	mm3.AddMove(&move1, 75);
	InsertParticleMove move3({"LJ"}, wm, 500, false);
	mm3.AddMove(&move3, 12);
	DeleteParticleMove move4({"LJ"},false);
	mm3.AddMove(&move4, 12);

	// Initialize observer.
	SimFlags flags; 
	flags.world_on(); 
	flags.simulation_on();

	TestAccumulator observer(flags, 1, 2000);
	TestAccumulator observer2(flags, 1, 2000);

	// Initialize ensemble. 
	StandardSimulation ensemble(&wm, &ffm, &mm);
	StandardSimulation ensemble2(&wm, &ffm, &mm2);
	StandardSimulation ensemble3(&wm, &ffm, &mm3);
	ensemble2.AddObserver(&observer);
	ensemble3.AddObserver(&observer2);

	// Run 
	ensemble.Run(20000);
	ensemble2.Run(20000);

	auto mus = observer.GetAverageChemicalPotential();
	// EXPECT_NEAR(mus[&world], -0.901, 1e-2);
	// EXPECT_NEAR(mus[&world2], -0.901, 1e-2);
	// EXPECT_NEAR(mus[&world3], -0.535, 1e-2);
	// EXPECT_NEAR(mus[&world4], 0.3443, 1e-2);
	EXPECT_NEAR(mus[&world5], -2.0 - 1.5*log(0.640), 1e-2);

	// world.SetChemicalPotential("LJ", -0.901+2*log(0.3));
	// world2.SetChemicalPotential("LJ", -0.901+2*log(0.403));
	// world3.SetChemicalPotential("LJ", -0.535+2*log(0.499));
	// world4.SetChemicalPotential("LJ", 0.344+2*log(0.601));
	world5.SetChemicalPotential("LJ", -2.0);
	world5.SetVolume(512.0, true);

	ensemble3.Run(20000);

	auto rhos = observer2.GetAverageDensities();
	// EXPECT_NEAR(rhos[&world], 0.300, 1e-2);
	// EXPECT_NEAR(rhos[&world2], 0.403, 1e-2);
	// EXPECT_NEAR(rhos[&world3], 0.499, 1e-2);
	// EXPECT_NEAR(rhos[&world4], 0.601, 1e-2);
	EXPECT_NEAR(rhos[&world5], 0.64, 1e-2);

}
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Simulation/StandardSimulation.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Moves/InsertParticleMove.h"
#include "../src/Moves/DeleteParticleMove.h"
#include "../src/Particles/Site.h"
#include "../src/Particles/Molecule.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "TestAccumulator.h"
#include "json/json.h"
#include "gtest/gtest.h"
#include "../src/Observers/DLMFileObserver.h"
#include <fstream>

using namespace SAPHRON;

// Grand canonical simulation tests.
TEST(GrandCanonicalEnsembleTests, Default)
{
	auto N = 500; 
	auto sigma = 1.;
	auto eps = 1.;
	auto T = 1.50;
	auto V = 250.047;
	auto rcut = 3.*sigma;

	// Prototype particle. 
	Site lj({0, 0, 0}, {0, 0, 0}, "LJ");

	// Initialze world, set chemical potential and pack with lj. 
	World world(1, 1, 1, rcut);
	world.SetNeighborRadius(rcut + 1.0);
	world.PackWorld({&lj}, {1.0}, N, 0.6);
	world.UpdateNeighborList();
	world.SetChemicalPotential("LJ", 2.0);
	world.SetTemperature(T);
	world.SetVolume(V, true); // Override volume. This will be fixed.

	// Initialize world manager. 
	WorldManager wm;
	wm.AddWorld(&world);

	ASSERT_EQ(N, world.GetParticleCount());
	ASSERT_DOUBLE_EQ(V, world.GetVolume());

	// Initialize LJ forcefield. 
	LennardJonesFF ff(eps, sigma);
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("LJ", "LJ", ff);

	// Initialize moves.
	MoveManager mm;
	TranslateMove move1(0.22);
	mm.AddMove(&move1, 75);
	InsertParticleMove move2({"LJ"}, wm, 500);
	mm.AddMove(&move2, 12);
	DeleteParticleMove move3({"LJ"});
	mm.AddMove(&move3, 12);

	// Initialize observer.
	SimFlags flags; 
	flags.world_on(); 
	flags.simulation_on();

	DLMFileObserver dlm("gcmc.test", flags);

	// Initialize ensemble. 
	StandardSimulation ensemble(&wm, &ffm, &mm);
	ensemble.AddObserver(&dlm);
	ASSERT_EQ(N, world.GetParticleCount());

	// Run 
	ensemble.Run(10000);
}
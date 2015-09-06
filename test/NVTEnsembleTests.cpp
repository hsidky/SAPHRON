#include "../src/Ensembles/StandardEnsemble.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LebwohlLasherFF.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/DirectorRotateMove.h"
#include "../src/Particles/Site.h"
#include "../src/Worlds/SimpleWorld.h"
#include "../src/Worlds/WorldManager.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(NVTEnsemble, DefaultBehavior)
{
	// Initialize world and manager.
	SimpleWorld world(30, 30, 30, 1);
	Site site1({0, 0, 0}, {1.0, 0, 0}, "E1");
	world.ConfigureParticles({&site1}, {1.0});
	world.UpdateNeighborList();
	world.SetTemperature(1.0);

	WorldManager wm;
	wm.AddWorld(&world);

	// Initialize forcefields.
	LebwohlLasherFF ff(1.0, 0);
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("E1", "E1", ff);

	// Initialize moves.
	DirectorRotateMove move1(33);
	MoveManager mm;
	mm.AddMove(&move1);

	// Initialize observer.
	//SimFlags flags;
	//flags.temperature = 1;
	//flags.iterations = 1;
	//flags.identifier = 1;
	//flags.energy = 1;
	// flags.particle_species = 1;
	// flags.particle_director = 1;
	// flags.particle_neighbors = 1;
	// flags.particle_position = 1;
	//ConsoleObserver co(flags, 100);

	// Initialize ensemble.
	StandardEnsemble ensemble(&wm, &ffm, &mm);
	//ensemble.AddObserver(&co);

	// Run
	ensemble.Run(30000000);

	double finalE = world.GetEnergy().total()/world.GetParticleCount();
	ASSERT_NEAR(-1.59, finalE, 1e-2);
}

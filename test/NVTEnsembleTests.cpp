#include "../src/Ensembles/NVTEnsemble.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LebwohlLasherFF.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/SphereUnitVectorMove.h"
#include "../src/Particles/Site.h"
#include "../src/Simulation/CSVObserver.h"
#include "../src/Worlds/SimpleLatticeWorld.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(NVTEnsemble, DefaultBehavior)
{
	// Initialize world.
	SimpleLatticeWorld world(30, 30, 30, 1);
	Site site1({0, 0, 0}, {1.0, 0, 0}, "E1");
	world.ConfigureParticles({&site1}, {1.0});
	world.ConfigureNeighborList();

	// Initialize forcefields.
	LebwohlLasherFF ff(1.0, 0);
	ForceFieldManager ffm;
	ffm.AddForceField("E1", "E1", ff);

	// Initialize moves.
	SphereUnitVectorMove move1(33);
	MoveManager mm;
	mm.PushMove(move1);

	// Initialize observer.
	SimFlags flags;
	flags.temperature = 1;
	flags.iterations = 1;
	flags.identifier = 1;
	flags.energy = 1;
	// flags.particle_species = 1;
	// flags.particle_director = 1;
	// flags.particle_neighbors = 1;
	// flags.particle_position = 1;
	CSVObserver co("test", flags, 100);

	// Initialize ensemble.
	NVTEnsemble ensemble(world, ffm, mm, 1.0, 45);
	ensemble.AddObserver(&co);

	// Run
	ensemble.Run(1000);

	double finalE = ensemble.GetEnergy()/world.GetParticleCount();
	ASSERT_NEAR(-1.59, finalE, 1e-2);
}

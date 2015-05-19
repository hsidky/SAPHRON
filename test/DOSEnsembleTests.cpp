#include "../src/Ensembles/DOSEnsemble.h"
#include "../src/DensityOfStates/WangLandauOP.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LebwohlLasherFF.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/SphereUnitVectorMove.h"
#include "../src/Particles/Site.h"
#include "../src/Simulation/CSVObserver.h"
#include "../src/Simulation/ConsoleObserver.h"
#include "../src/Worlds/SimpleWorld.h"
#include "gtest/gtest.h"


using namespace SAPHRON;

TEST(DOSEnsemble, DefaultBehavior)
{
	/*
	int n = 20;
	// Initialize world.
	SimpleWorld world(n, n, n, 1);
	Site site1({0, 0, 0}, {1.0, 0, 0}, "E1");
	world.ConfigureParticles({&site1}, {1.0});
	world.UpdateNeighborList();

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
	flags.iterations = 1;
	flags.identifier = 1;
	flags.energy = 1;
	flags.temperature = 1;
	flags.dos_flatness = 1;
	flags.dos_interval = 1;
	flags.dos_bin_count = 1;
	flags.dos_scale_factor = 1;
	flags.dos_values = 1;
	CSVObserver co("test", flags, 10000);
	SimFlags flags2;
	flags2.iterations = 1;
	flags2.energy = 1;
	flags2.dos_flatness = 1;
	flags2.dos_interval = 1;
	flags2.dos_scale_factor = 1;
	ConsoleObserver co2(flags2, 5000);

	// Initialize Wang-Landau order parameter. 
	WangLandauOP op;

	DOSEnsemble ensemble(op, world, ffm, mm, {-1.8*n*n*n, 0.3*n*n*n}, 7000, 1);
	ensemble.AddObserver(&co);
	ensemble.AddObserver(&co2);
	ensemble.Run(20);
	*/
}
#include "../src/Ensembles/DOSEnsemble.h"
#include "../src/DensityOfStates/WangLandauOP.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LebwohlLasherFF.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/SphereUnitVectorMove.h"
#include "../src/Particles/Site.h"
#include "../src/Simulation/CSVObserver.h"
#include "../src/Worlds/SimpleLatticeWorld.h"
#include "gtest/gtest.h"


using namespace SAPHRON;

TEST(WLDOSEnsemble, DefaultBehavior)
{
	int n = 25;
	// Initialize world.
	SimpleLatticeWorld world(n, n, n, 1);
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
	flags.iterations = 1;
	flags.identifier = 1;
	flags.energy = 1;
	flags.temperature = 1;
	CSVObserver co("test", flags, 100);


	// Initialize Wang-Landau order parameter. 
	WangLandauOP op;

	DOSEnsemble ensemble(op, world, ffm, mm, {-1.7*n*n*n, -0.5*n*n*n}, 5000, 1);
	ensemble.AddObserver(&co);
	ensemble.Run(1);
}
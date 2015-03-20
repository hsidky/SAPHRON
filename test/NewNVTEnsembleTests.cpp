#include "../src/Ensembles/NewNVTEnsemble.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LebwohlLasherFF.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/NewSphereUnitVectorMove.h"
#include "../src/Particles/Site.h"
#include "../src/Simulation/ConsoleObserver.h"
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
	ConsoleObserver co(flags, 10);

	// Initialize ensemble.
	NVTEnsemble ensemble(world, ffm, mm, 1.0, 45);
	ensemble.AddObserver(&co);

	// Run
	ensemble.Run(100);
}

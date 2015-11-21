#include "../src/Simulation/StandardSimulation.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LebwohlLasherFF.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/DirectorRotateMove.h"
#include "../src/Particles/Site.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(NVTEnsemble, DefaultBehavior)
{
	// Initialize world and manager.
	World world(30, 30, 30, 1);
	Site site1({0, 0, 0}, {1.0, 0, 0}, "E1");
	world.PackWorld({&site1}, {1.0});
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

	// Initialize ensemble.
	StandardSimulation ensemble(&wm, &ffm, &mm);

	// Run
	ensemble.Run(1100);

	double finalE = world.GetEnergy().total()/world.GetParticleCount();
	ASSERT_NEAR(-1.59, finalE, 1e-2);
}

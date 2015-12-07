#include "../src/Moves/VolumeScaleMove.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Moves/MoveManager.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Worlds/World.h"
#include "../src/Particles/Site.h"
#include "../src/Observers/DLMFileObserver.h"
#include "../src/Simulation/StandardSimulation.h"
#include "gtest/gtest.h"
#include "TestAccumulator.h"
#include <cmath>

using namespace SAPHRON;

// Verifying against NIST vals from here :
// http://mmlapps.nist.gov/srs/LJ_PURE/eostmmc.htm.
TEST(PVTEnsembleTests, NISTValidation)
{
	auto N = 500;
	auto sigma = 1.0;
	auto eps = 1.0; 
	auto T = 0.90;
	auto rho = 8.00781E-01; 
	auto P = 5.22522E-01;

	Site lj({0,0,0}, {0,0,0}, "LJ");

	World world(1, 1, 1, 3.0*sigma + 1.0, 1.0);
	world.PackWorld({&lj}, {1.0}, N, rho);
	world.SetTemperature(T);

	WorldManager wm;
	wm.AddWorld(&world);

	ASSERT_EQ(N, world.GetParticleCount());
	ASSERT_DOUBLE_EQ(rho, world.GetNumberDensity());

	// Forcefield. 
	LennardJonesFF ff(eps, sigma, {3.0*sigma});
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("LJ", "LJ", ff);

	// Initialize moves.
	MoveManager mm;
	TranslateMove m1(0.20);
	VolumeScaleMove m2(P, 0.3);
	mm.AddMove(&m1, 95);
	mm.AddMove(&m2, 5);

	SimFlags flags; 
	flags.world_on(); 
	flags.simulation_on();

	// Initialize observer.
	TestAccumulator o1(flags, 1, 4000);

	// Initialize ensemble. 
	StandardSimulation ensemble(&wm, &ffm, &mm);
	ensemble.AddObserver(&o1);
	ensemble.Run(15000);

	auto rhos = o1.GetAverageDensities();
	auto Ps = o1.GetAveragePressures();

	ASSERT_NEAR(rhos[&world], rho, 4.67E-03);
	ASSERT_NEAR(Ps[&world].isotropic(), P, 2.67E-02); // Hard to get close with only 15k sweeps.
}

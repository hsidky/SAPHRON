#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Moves/VolumeSwapMove.h"
#include "../src/Moves/ParticleSwapMove.h"
#include "../src/Particles/Particle.h"
#include "../src/Simulation/StandardSimulation.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Worlds/World.h"
#include "TestAccumulator.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

// Benchmark NIST data obtained from 
// http://mmlapps.nist.gov/srs/LJ_PURE/sattmmc.htm.
TEST(GibbsNVTEnsemble, LJNISTValidation1)
{
	int N = 256; // Number of LJ particles per NIST.
	double sigma = 1.0; 
	double eps   = 1.0; 
	double T = 1.20;
	double rcut = 3.0*sigma;

	// Prototype particle.
	Particle ljatom({0,0,0}, {0,0,0}, "LJ");

	// Add lj atom to world and initialize in simple lattice configuration.
	// World volume is adjusted by packworld.
	World liquid(1, 1, 1, rcut + 1.0, 1.0);
	liquid.SetNeighborRadius(rcut + 1.0);
	liquid.PackWorld({&ljatom}, {1.0}, N/2, 0.30);
	liquid.SetTemperature(T);

	World vapor(1, 1, 1, rcut + 1.0, 1.0);
	vapor.SetNeighborRadius(rcut + 1.0);
	vapor.PackWorld({&ljatom}, {1.0}, N/2, 0.30);
	vapor.SetTemperature(T);

	WorldManager wm;
	wm.AddWorld(&liquid);
	wm.AddWorld(&vapor);

	ASSERT_EQ(128, liquid.GetParticleCount());
	ASSERT_EQ(128, vapor.GetParticleCount());

	// Initialize LJ forcefield.
	LennardJonesFF ff(eps, sigma, {rcut, rcut});
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("LJ", "LJ", ff);

	// Initialize moves. 
	TranslateMove translate(0.30);
	VolumeSwapMove vscale(0.05);
	ParticleSwapMove pswap;

	MoveManager mm;
	mm.AddMove(&translate, 95);
	mm.AddMove(&pswap, 3);
	mm.AddMove(&vscale, 2);

	// Initialize observer.
	SimFlags flags;
	//flags.temperature = 1;
	flags.iteration = 1;
	flags.move_acceptances = 1;
	flags.world_density = 1;
	flags.world_energy = 1;
	flags.world_pressure = 1;
	//ConsoleObserver observer(flags, 1000);

	// Initialize accumulator. 
	TestAccumulator accumulator(flags, 10, 30000);

	// Initialize ensemble. 
	StandardSimulation ensemble(&wm, &ffm, &mm);
	ensemble.AddObserver(&accumulator);
	
	// Run 
	ensemble.Run(60000);

	// Check values (from NIST)
	auto density = accumulator.GetAverageDensities();
	ASSERT_NEAR(5.63158E-01, density[&liquid], 3e-3);
	ASSERT_NEAR(1.00339E-01, density[&vapor], 1.1e-2);

	auto pressure = accumulator.GetAveragePressures();
	ASSERT_NEAR(7.72251E-02, pressure[&liquid].isotropic(), 1e-3);
	ASSERT_NEAR(7.72251E-02, pressure[&vapor].isotropic(), 1e-3);	

	// Check "conservation" of energy.
	EPTuple H1 = ffm.EvaluateEnergy(liquid);
	EPTuple H2 = ffm.EvaluateEnergy(vapor);
	ASSERT_NEAR(H1.energy.total(), liquid.GetEnergy().total(), 1e-9);
	ASSERT_NEAR(H1.pressure.isotropic(), liquid.GetPressure().isotropic() - liquid.GetPressure().ideal, 1e-9);
	ASSERT_NEAR(H2.energy.total(), vapor.GetEnergy().total(), 1e-9);
	ASSERT_NEAR(H2.pressure.isotropic(), vapor.GetPressure().isotropic() - vapor.GetPressure().ideal, 1e-9);

}

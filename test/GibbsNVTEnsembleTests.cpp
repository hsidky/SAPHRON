#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Moves/VolumeSwapMove.h"
#include "../src/Moves/ParticleSwapMove.h"
#include "../src/Particles/Site.h"
#include "../src/Ensembles/StandardEnsemble.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Worlds/SimpleWorld.h"
//#include "../src/Observers/ConsoleObserver.h"
#include "../src/Observers/CSVObserver.h"
#include "TestAccumulator.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(GibbsNVTEnsemble, LJNISTValidation1)
{
	int N = 256; // Number of LJ particles per NIST.
	double sigma = 1.0; 
	double eps   = 1.0; 
	double T = 1.20;
	double rcut = 3.0*sigma;

	// Prototype particle.
	Site ljatom({0,0,0}, {0,0,0}, "LJ");

	// Add lj atom to world and initialize in simple lattice configuration.
	// World volume is adjusted by packworld.
	SimpleWorld liquid(1, 1, 1, rcut);
	liquid.SetNeighborRadius(rcut + 1.0);
	liquid.PackWorld({&ljatom}, {1.0}, N/2, 0.30);
	liquid.SetTemperature(T);
	liquid.UpdateNeighborList();

	SimpleWorld vapor(1, 1, 1, rcut);
	vapor.SetNeighborRadius(rcut + 1.0);
	vapor.PackWorld({&ljatom}, {1.0}, N/2, 0.30);
	vapor.SetTemperature(T);
	vapor.UpdateNeighborList();

	WorldManager wm;
	wm.AddWorld(&liquid);
	wm.AddWorld(&vapor);

	ASSERT_EQ(128, liquid.GetParticleCount());
	ASSERT_EQ(128, vapor.GetParticleCount());

	// Initialize LJ forcefield.
	LennardJonesFF ff(eps, sigma);
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
	flags.world_composition = 1;
	flags.world_volume = 1;
	//ConsoleObserver observer(flags, 1000);

	// Initialize accumulator. 
	TestAccumulator accumulator(flags, 10*N, 30000*N);
	flags.world = 63;
	CSVObserver csv("test", flags, 100*N);

	// Initialize ensemble. 
	StandardEnsemble ensemble(&wm, &ffm, &mm);
	//ensemble.AddObserver(&observer);
	ensemble.AddObserver(&accumulator);
	ensemble.AddObserver(&csv);
	
	// Run 
	ensemble.Run(50000*N);

	// Check "conservation" of energy.
	//EPTuple H1 = ffm.EvaluateHamiltonian(liquid);
	//EPTuple H2 = ffm.EvaluateHamiltonian(vapor);
	//ASSERT_NEAR(H1.energy.total(), liquid.GetEnergy().total(), 1e-11);
	//ASSERT_NEAR(H1.pressure.isotropic(), liquid.GetPressure().isotropic() - liquid.GetPressure().ideal, 1e-11);
	//ASSERT_NEAR(H2.energy.total(), vapor.GetEnergy().total(), 1e-11);
	//ASSERT_NEAR(H2.pressure.isotropic(), vapor.GetPressure().isotropic() - vapor.GetPressure().ideal, 1e-11);

	// Check values (from NIST)
	auto density = accumulator.GetAverageDensities();
	std::cout << "D1 = " << density[&liquid] << " D2 = " << density[&vapor] << std::endl;
	ASSERT_NEAR(5.63158E-01, density[&liquid], 3e-3);
	ASSERT_NEAR(1.00339E-01, density[&vapor], 1.1e-2);
}

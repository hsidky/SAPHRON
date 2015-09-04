#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Moves/VolumeSwapMove.h"
#include "../src/Moves/ParticleSwapMove.h"
#include "../src/Particles/Site.h"
#include "../src/Observers/ConsoleObserver.h"
#include "../src/Observers/CSVObserver.h"
#include "TestAccumulator.h"
#include "../src/Worlds/SimpleWorld.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(GibbsNVTEnsemble, LJNISTValidation1)
{
/*	int N = 256; // Number of LJ particles per NIST.
	double sigma = 1.0; 
	double eps   = 1.0; 
	double kb = 1.0;
	double T = 1.20;
	double rcut = 2.0*sigma;

	// Prototype particle.
	Site ljatom({0,0,0}, {0,0,0}, "LJ");

	// Add lj atom to world and initialize in simple lattice configuration.
	// World volume is adjusted by packworld.
	SimpleWorld liquid(1, 1, 1, rcut + 1.0);
	liquid.SetSkinThickness(1.0);
	liquid.PackWorld({&ljatom}, {1.0}, N/2, 0.30);
	liquid.UpdateNeighborList();

	SimpleWorld vapor(1, 1, 1, rcut + 1.0);
	vapor.SetSkinThickness(1.0);
	vapor.PackWorld({&ljatom}, {1.0}, N/2, 0.30);
	vapor.UpdateNeighborList();

	ASSERT_EQ(128, liquid.GetParticleCount());
	ASSERT_EQ(128, vapor.GetParticleCount());

	// Initialize LJ forcefield.
	LennardJonesFF ff(eps, sigma, rcut);
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("LJ", "LJ", ff);

	// Initialize moves. 
	TranslateMove translate(0.30);
	VolumeScaleMove vscale(0.05);
	ParticleSwapMove pswap;

	MoveManager mm;
	mm.PushMove(translate);
	mm.PushMove(vscale);
	mm.PushMove(pswap);

	// Initialize observer.
	SimFlags flags;
	//flags.temperature = 1;
	flags.iterations = 1;
	flags.acceptance = 1;
	flags.world_density = 1;
	flags.world_count = 1;
	flags.world_volume = 1;
	ConsoleObserver observer(flags, 1000);

	// Initialize accumulator. 
	TestAccumulator accumulator(flags, 10, 20000);
	CSVObserver csv("test", flags, 100);

	// Initialize ensemble. 
	WorldList wl = {&liquid, &vapor};
	FFManagerList ffl = {&ffm, &ffm};
	GibbsNVTEnsemble ensemble(wl, ffl, mm, T, 34435);
	ensemble.SetBoltzmannConstant(kb);
	ensemble.AddObserver(&observer);
	ensemble.AddObserver(&accumulator);
	ensemble.AddObserver(&csv);
	
	// Run 
	ensemble.Run(50000);

	// Check "conservation" of energy.
	EPTuple H1 = ffm.EvaluateHamiltonian(liquid);
	EPTuple H2 = ffm.EvaluateHamiltonian(vapor);
	double esum = (H1 + H2).energy.total();
	ASSERT_NEAR(esum, ensemble.GetEnergy().total(), 1e-9);

	// Check values (from NIST)
	auto density = accumulator.GetAverageDensities();
	ASSERT_NEAR(5.63158E-01, density[&liquid], 3e-3);
	ASSERT_NEAR(1.00339E-01, density[&vapor], 1.1e-2);*/
}

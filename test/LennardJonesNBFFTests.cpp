#include "../src/NewMoves/MoveManager.h"
#include "../src/NewMoves/TranslateMove.h"
#include "../src/NewForceFields/LennardJonesNBFF.h"
#include "../src/NewForceFields/ForceFieldManager.h"
#include "../src/Particles/BlueprintManager.h"
#include "../src/Simulation/SimException.h"
#include "../src/Simulation/StandardSimulation.h"
#include "../src/Worlds/NewWorld.h"
#include "../src/Worlds/WorldManager.h"
//#include "TestAccumulator.h"
#include "gtest/gtest.h"
#include "json/json.h"

#include <memory>

using namespace SAPHRON;

TEST(LennardJonesNBFF, InteractionPotential)
{
	auto rcut = 14.;
	LennardJonesNBFF ff(1., 1., {rcut});
	Site s1; 
	Site s2;
	s1.position = {0., 0., 0.};
	s2.position = {1.5, 0., 0.};

	auto r = s1.position - s2.position;
	auto u = ff.EvaluateEnergy(s1, s2, r, r.squaredNorm(), 0);
	ASSERT_NEAR(-0.320336594278575, u, 1e-10);
	ASSERT_NEAR(-4.859086276627293e-04, ff.EnergyTailCorrection(0), 1e-10);
	ASSERT_NEAR(-0.002915451636909, ff.PressureTailCorrection(0), 1e-10);

	// Compute pressure from force.
	auto f = ff.EvaluateForce(s1, s2, r, r.squaredNorm(), 0);
	double P = -1./3.*f.dot(r);
	ASSERT_NEAR(-0.579014415, P, 1e-7);
}

// Validate results from NIST MC LJ standards 
// to confirm virial, energy, pressure calc. 
TEST(LennardJonesNBFF, ConfigurationValues)
{
	// Load file (assumes we are in build folder).
	std::ifstream t("../test/nist_lj_config1.json");
	std::stringstream buffer;
	buffer << t.rdbuf();

	// Read JSON.
	Json::Reader reader;
	Json::Value root;
	ASSERT_TRUE(reader.parse(buffer, root));

	ASSERT_NO_THROW(BlueprintManager::Build(root["blueprints"]));

	std::shared_ptr<NewWorld> world;
	ASSERT_NO_THROW(world = NewWorld::Build(root["worlds"][0]));
	ASSERT_EQ(800, world->GetParticleCount());
	ASSERT_EQ(3.5, world->GetNeighborRadius());

	world->UpdateCellList();

	// Define LJ potential. First we do rcut = 3*sigma.
	LennardJonesNBFF ff(1.0, 1.0, {3.0, 3.0});

	// Get species index from blueprint manager. 
	auto& bp = BlueprintManager::Instance();
	auto ljs = bp.GetSpeciesIndex("LJ");
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField(ljs, ljs, ff);

	auto u = ffm.EvaluateInterEnergy(*world);
	auto v = ffm.EvaluateVirial(*world);
	ASSERT_NEAR(-4.3515E+03, u, 1e-1);
	ASSERT_NEAR(-5.6867E+02, v.trace(), 1e-2);
	auto ut = ffm.EvaluateTailEnergy(*world);
	ASSERT_NEAR(-1.9849E+02, ut, 1e-2);

	// Test rcut = 4*sigma.
	LennardJonesNBFF ff2(1.0, 1.0, {4.0, 4.0});
	world->SetNeighborRadius(4.5);
	ffm.RemoveNonBondedForceField(ljs, ljs);
	ffm.AddNonBondedForceField(ljs, ljs, ff2);
	
	u = ffm.EvaluateInterEnergy(*world);
	ASSERT_NEAR(-4.4675E+03, u, 1e-1);
	v = ffm.EvaluateVirial(*world);
	ASSERT_NEAR(-1.2639E+03, v.trace(), 1e-1);
	ut = ffm.EvaluateTailEnergy(*world);
	ASSERT_NEAR(-8.3769E+01, ut, 1e-2);
}

// Validate results from NIST MC LJ standards page.
// http://mmlapps.nist.gov/srs/LJ_PURE/mc.htm
TEST(LennardJonesFF, NISTValidation1)
{
	// Target reduced density to validate.
	auto rdensity = 7.76E-01;
	auto N = 500; // Number of LJ particles per NIST.
	auto sigma = 1.0; 
	auto eps   = 1.0; 
	auto T = 0.85;
	auto rcut = 3.0*sigma;

	// Keep everything default. Species is 0.
	std::vector<Site> sites;
	sites.push_back(Site());

	NewParticle particle(1, {0}, &sites);

	NewWorld world(1, 1, 1, rcut, 1.0);
	world.PackWorld({&particle}, {1.0}, N, rdensity);
	world.SetCellRatio(0.2);
	world.SetTemperature(T);

	ASSERT_EQ(N, world.GetParticleCount());
	ASSERT_NEAR((double)N*pow(sigma,3)/rdensity, world.GetVolume(), 1e-10);

	WorldManager wm; 
	wm.AddWorld(&world);

	// Initialize LJ forcefield.
	LennardJonesNBFF ff(eps, sigma, {rcut, rcut, rcut, rcut});
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField(0, 0, ff);

	// Initialize moves. 
	TranslateMove move(0.22);
	MoveManager mm;
	mm.AddMove(&move);

	// Initialize observer.
	SimFlags flags;
	flags.world_energy = 1;
	flags.iteration = 1;
	flags.move_acceptances = 1;
	flags.world_pressure = 1;

	// Initialize accumulator. 
	//TestAccumulator accumulator(flags, 1, 10000);

	// Initialize ensemble. 
	StandardSimulation ensemble(&wm, &ffm, &mm);
	//ensemble.AddObserver(&accumulator);

	// Run 
	ensemble.Run(500);

	// Conversation of energy and pressure.
	//world.BuildCellList();
	auto u = ffm.EvaluateInterEnergy(world);
	ASSERT_NEAR(u, world.GetInterEnergy(), 1e-8);

	//ASSERT_NEAR(-5.5121, accumulator.GetAverageEnergies()[&world]/(double)N, 1e-3);
	//ASSERT_NEAR(6.7714E-03, accumulator.GetAveragePressures()[&world], 1e-3);	
}
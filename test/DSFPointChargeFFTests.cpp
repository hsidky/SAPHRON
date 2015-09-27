#include "../src/ForceFields/DSFFF.h"
#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Particles/Molecule.h"
#include "../src/Particles/Site.h"
#include "../src/Simulation/SimInfo.h"
#include "../src/Simulation/SimException.h"
#include "../src/Worlds/SimpleWorld.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Moves/RotateMove.h"
#include "../src/Moves/ParticleSwapMove.h"
#include "../src/Moves/VolumeSwapMove.h"
#include "../src/Moves/MoveManager.h"
#include "TestAccumulator.h"
#include "../src/Observers/CSVObserver.h"
#include "../src/Ensembles/StandardEnsemble.h"
#include "json/json.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace SAPHRON;

// Validate closeness of DSF potential to NIST 
// values provided for SPCE water. See ref:
// http://www.nist.gov/mml/csd/informatics_research/spce_refcalcs.cfm
TEST(DSFFF, NISTConfig1)
{
	// Load file (assumes we are in build folder.
	std::ifstream t("../test/nist_spce_config1.json");
	std::stringstream buffer;
	buffer << t.rdbuf();
	// Read JSON.
	Json::Reader reader;
	Json::Value root;
	ASSERT_TRUE(reader.parse(buffer, root));

	// Build world.
	World* w = nullptr;
	ASSERT_NO_THROW(w = World::Build(root["worlds"][0]));
	ASSERT_NE(nullptr, w);

	ParticleList particles;

	// Build particles.
	ASSERT_NO_THROW(Particle::BuildParticles(root["particles"], root["components"], particles));
	ASSERT_EQ(100, particles.size());

	for(auto& p : particles)
		w->AddParticle(p);

	ASSERT_EQ(100, w->GetParticleCount());
	ASSERT_EQ(300, w->GetPrimitiveCount());
	w->UpdateNeighborList();
	ASSERT_EQ(10.0, w->GetCutoffRadius());
	auto L = w->GetBoxVectors();
	ASSERT_EQ(20, L[0]);
	ASSERT_EQ(20, L[1]);
	ASSERT_EQ(20, L[2]);

	// Get random particle and check structure. 
	Particle* p = w->DrawRandomParticle();
	// Should be O, H1, H2.
	auto& children = p->GetChildren();
	ASSERT_EQ(3, children.size());
	ASSERT_EQ("O", children[0]->GetSpecies());
	ASSERT_EQ("H1", children[1]->GetSpecies());
	ASSERT_EQ("H2", children[2]->GetSpecies());

	// Check positions.
	ASSERT_NEAR(1.0, arma::norm(children[0]->GetPosition()-children[1]->GetPosition()), 1e-6);
	ASSERT_NEAR(1.0, arma::norm(children[0]->GetPosition()-children[2]->GetPosition()), 1e-6);
	ASSERT_NEAR(1.632980861, arma::norm(children[1]->GetPosition()-children[2]->GetPosition()), 1e-6);

	// Check charges and masses.
	ASSERT_DOUBLE_EQ(-2.0*0.42380, children[0]->GetCharge());
	ASSERT_DOUBLE_EQ(0.42380, children[1]->GetCharge());
	ASSERT_DOUBLE_EQ(0.42380, children[2]->GetCharge());
	ASSERT_DOUBLE_EQ(15.9994, children[0]->GetMass());
	ASSERT_DOUBLE_EQ(1.00794, children[1]->GetMass());
	ASSERT_DOUBLE_EQ(1.00794, children[2]->GetMass());

	// Initialize forcefields.
	LennardJonesFF lj(78.19743111, 3.16555789);
	DSFFF dsf(0.20);

	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("O", "O", lj);
	ffm.AddElectrostaticForceField("O", "O", dsf);
	ffm.AddElectrostaticForceField("O", "H1", dsf);
	ffm.AddElectrostaticForceField("O", "H2", dsf);
	ffm.AddElectrostaticForceField("H1", "H1", dsf);
	ffm.AddElectrostaticForceField("H1", "H2", dsf);
	ffm.AddElectrostaticForceField("H2", "H2", dsf);

	auto E = ffm.EvaluateHamiltonian(*w);
	// Values are checked to 6 sig figs.
	// Cannot directly compare ewald energy to DSF. TBC.
	ASSERT_NEAR(9.95387E+04-8.23715E+02, E.energy.intervdw, 1e-1);
	ASSERT_EQ(0, E.energy.intravdw);

	delete w;
}

// Validating SPCE water results against 
// NIST benchmark simulations.
TEST(DSFFF, NISTValidation1)
{
	// Set boltzmann to give real units of energy. 
	auto& sim = SimInfo::Instance();
	sim.SetkB(sim.GetkBDefault());

	int N = 264;
	double eps = 78.19743111*sim.GetkB();
	double sig = 3.16555789;
	double T = 525; // (K)
	double rcut = 8.0; // Angstrom.

	// Prototype molecule.
	Site* O = new Site({0.72707694246,0.62783284768,0.88326560728},{0,0,1}, "O");
	O->SetMass(15.9994);
	O->SetCharge(-0.84760);
	Site* H1 = new Site({0, 0, 1.16108755587}, {0, 0, 1}, "H1");
	H1->SetMass(1.00794);
	H1->SetCharge(0.42380);
	Site* H2 = new Site({1.09689283867, 0.33960018140, 0}, {0, 0, 1}, "H2");
	H2->SetMass(1.00794);
	H2->SetCharge(0.42380);
	
	Molecule h2o("H2O");	
	h2o.AddChild(O);
	h2o.AddChild(H1);
	h2o.AddChild(H2);

	// Add lj atom to world and initialize in simple lattice configuration.
	// World volume is adjusted by packworld.
	SimpleWorld liquid(1, 1, 1, rcut);
	liquid.SetNeighborRadius(rcut + 1.0);
	liquid.PackWorld({&h2o}, {1.0}, N/2, 0.02);
	liquid.SetTemperature(T);
	liquid.UpdateNeighborList();

	SimpleWorld vapor(1, 1, 1, rcut);
	vapor.SetNeighborRadius(rcut + 1.0);
	vapor.PackWorld({&h2o}, {1.0}, N/2, 0.005);
	vapor.SetTemperature(T);
	vapor.UpdateNeighborList();

	WorldManager wm;
	wm.AddWorld(&liquid);
	wm.AddWorld(&vapor);

	ASSERT_EQ(132, liquid.GetParticleCount());
	ASSERT_EQ(132, vapor.GetParticleCount());

	// Initialize forcefields.
	LennardJonesFF lj(eps, sig);
	DSFFF dsf(0.20);
 
	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("O", "O", lj);
	ffm.AddElectrostaticForceField("O", "O", dsf);
	ffm.AddElectrostaticForceField("O", "H1", dsf);
	ffm.AddElectrostaticForceField("O", "H2", dsf);
	ffm.AddElectrostaticForceField("H1", "H1", dsf);
	ffm.AddElectrostaticForceField("H1", "H2", dsf);
	ffm.AddElectrostaticForceField("H2", "H2", dsf);

	// Initialize moves. 
	TranslateMove translate(0.70);
	VolumeSwapMove vscale(0.02);
	ParticleSwapMove pswap;
	RotateMove rotate(0.76);

	MoveManager mm;
	mm.AddMove(&translate, 95);
	mm.AddMove(&rotate, 95);
	mm.AddMove(&pswap, 6);
	mm.AddMove(&vscale, 4);

	// Initialize observer.
	SimFlags flags;
	//flags.temperature = 1;
	flags.iteration = 1;
	flags.world = 63;
	flags.simulation = 15;

	// Initialize accumulator. 
	TestAccumulator accumulator(flags, 10, 30000);
	CSVObserver csv("test", flags, 10);

	// Initialize ensemble. 
	StandardEnsemble ensemble(&wm, &ffm, &mm);
	ensemble.AddObserver(&accumulator);
	ensemble.AddObserver(&csv);
	ensemble.Run(1000);
}
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
#include "../src/Observers/DLMFileObserver.h"
#include "../src/Simulation/StandardSimulation.h"
#include "json/json.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace SAPHRON;

// Validation of potential. 
/*TEST(DSFFF, Potential)
{
	Site p1({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "LJ");
	Site p2({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "LJ");
	p1.AddNeighbor(&p2);
	p2.AddNeighbor(&p1);
	p1.SetCharge(0.5);
	p2.SetCharge(0.5);

	DSFFF dsf(0.2);
	LennardJonesFF ff(1.0, 1.0);
	ForceFieldManager ffm;
	ffm.SetDefaultCutoff(100.0);
	ffm.AddNonBondedForceField("LJ", "LJ", ff);
	ffm.AddElectrostaticForceField("LJ", "LJ", dsf);
	for(int i = 0; i < 300; ++i)
	{
		auto x = (1.0 + i*0.01) ;
		p2.SetPosition({x, 0, 0});
		auto E =  ffm.EvaluateHamiltonian(p1, {}, 1.0);
		std::cout << x << " " << E.energy.total() << std::endl;
	}
}
*/

// Validate closeness of DSF potential to NIST 
// values provided for SPCE water. See ref:
// http://www.nist.gov/mml/csd/informatics_research/spce_refcalcs.cfm
TEST(DSFFF, NISTConfig1)
{
	// Set to real units.
	auto& sim = SimInfo::Instance();
	sim.SetUnits(real);

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

	ASSERT_EQ(100, w->GetParticleCount());
	ASSERT_EQ(300, w->GetPrimitiveCount());
	w->UpdateNeighborList();
	ASSERT_EQ(10.0, w->GetCutoffRadius());
	ASSERT_EQ(0.0, w->GetSkinThickness());
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
	std::vector<ForceField*> fflist;
	ForceFieldManager ffm;

	ASSERT_NO_THROW(ForceField::BuildForceFields(root["forcefields"], &ffm, fflist));

	auto E = ffm.EvaluateHamiltonian(*w);
	// Values are checked to 6 sig figs.
	// Cannot directly compare ewald energy to DSF. TBC.
	E.energy /= sim.GetkB();
	ASSERT_NEAR(9.95387E+04-8.23715E+02, E.energy.intervdw, 1e-1);

	// This is from DSF.
	ASSERT_NEAR(-5.77108E+5, E.energy.interelectrostatic, 1e-1);
	ASSERT_EQ(0, E.energy.intravdw);

	delete w;
}

/*
// Validating SPCE water results against 
// literature PVT data.
TEST(DSFFF, PVTValidation1)
{
	// Set to real units.
	auto& sim = SimInfo::Instance();
	sim.SetUnits(real);

	int N = 400;
	double eps = 78.19743111*sim.GetkB();
	double sig = 3.16555789;
	double T = 298.15; // (K)
	double rcut = 8; // Angstrom.

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
	liquid.SetNeighborRadius(rcut + 3.0);
	liquid.PackWorld({&h2o}, {1.0}, N, 0.033356);
	liquid.SetTemperature(T);
	liquid.UpdateNeighborList();
	
	WorldManager wm;
	wm.AddWorld(&liquid);

	ASSERT_EQ(N, liquid.GetParticleCount());
	// 0.997 g/cm^3.
	ASSERT_NEAR(0.0553889, liquid.GetDensity(), 1e-6);

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
	TranslateMove translate(0.35);
	RotateMove rotate(0.50);

	MoveManager mm;
	mm.AddMove(&translate, 50);
	mm.AddMove(&rotate, 50);

	// Initialize observer.
	SimFlags flags;
	//flags.temperature = 1;
	flags.iteration = 1;
	flags.world_on();
	flags.simulation_on();
	flags.pressure_on();
	flags.energy_on();

	// Initialize accumulator. 
	TestAccumulator accumulator(flags, 10, 30000);
	DLMFileObserver csv("test", flags, 10);

	// Initialize ensemble. 
	StandardSimulation ensemble(&wm, &ffm, &mm);
	ensemble.AddObserver(&accumulator);
	ensemble.AddObserver(&csv);
	ensemble.Run(10000);

	// "Conservation" of energy and pressure.
	liquid.UpdateNeighborList();
	EPTuple H = ffm.EvaluateHamiltonian(liquid);
	ASSERT_NEAR(H.pressure.isotropic(), liquid.GetPressure().isotropic()-liquid.GetPressure().ideal, 1e-10);
	ASSERT_NEAR(H.energy.total(), liquid.GetEnergy().total(), 1e-10);
}
*/
/*
// Validating SPCE water results against 
// NIST benchmark simulations.
TEST(DSFFF, NISTValidation1)
{

	// Set system to real units.
	auto& sim = SimInfo::Instance();
	sim.SetUnits(real);

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
	DLMFileObserver csv("test", flags, 10);

	// Initialize ensemble. 
	StandardSimulation ensemble(&wm, &ffm, &mm);
	ensemble.AddObserver(&accumulator);
	ensemble.AddObserver(&csv);
	ensemble.Run(1000);
}
*/
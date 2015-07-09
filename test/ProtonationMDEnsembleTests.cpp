#include "../src/Ensembles/ProtonationMDEnsemble.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/DebyeHuckelFF.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Particles/Site.h"
#include "../src/Particles/Molecule.h"
#include "../src/Moves/ImplicitProtonationMove.h"
#include "../src/Observers/ConsoleObserver.h"
#include "../src/Worlds/SimpleWorld.h"
#include "TestAccumulator.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(ProtonationMDEnsemble, DefaultBehavior)
{
	// Initialize world.
	SimpleWorld world(1000, 1000, 1000, 100);
	Site* site1= new Site({0, 1, 0}, {0, 0, 0}, "Monomer");
	Site* site2= new Site({0, 0, 0}, {0, 0, 0}, "Monomer");
	Site* site3= new Site({1, 0, 0}, {0, 0, 0}, "Monomer");
	Site* site4= new Site({2, 0, 0}, {0, 0, 0}, "Monomer");
	Site* site5= new Site({3, 0, 0}, {0, 0, 0}, "Monomer");
	Site* site6= new Site({3, 1, 0}, {0, 0, 0}, "Monomer");

	site1->AddBondedNeighbor(site2);
	site2->AddBondedNeighbor(site1);
	site2->AddBondedNeighbor(site3);
	site3->AddBondedNeighbor(site2);
	site3->AddBondedNeighbor(site4);
	site4->AddBondedNeighbor(site3);
	site4->AddBondedNeighbor(site5);
	site5->AddBondedNeighbor(site4);
	site5->AddBondedNeighbor(site6);
	site6->AddBondedNeighbor(site5);

	Molecule m("M1");

	m.AddChild(site1);
	m.AddChild(site2);
	m.AddChild(site3);
	m.AddChild(site4);
	m.AddChild(site5);
	m.AddChild(site6);

	world.SetSkinThickness(1.0);
	world.PackWorld({&m}, {1.0}, 1, (1.0/(25.0*25.0*25.0)));

	ASSERT_EQ(1, world.GetParticleCount());

	// Initialize forcefields.
	DebyeHuckelFF ffe(1.2, 4, 2, 8);
	ForceFieldManager ffm;
	ffm.AddElectrostaticForceField("Monomer", "Monomer", ffe);

	// Initialize moves.
	ImplicitProtonationMove move1(-1.0);
	MoveManager mm;
	mm.PushMove(move1);

	// Initialize observer.
	SimFlags flags;
	//flags.temperature = 1;
	flags.energy = 1;
	flags.iterations = 1;
	flags.acceptance = 1;
	flags.pressure = 1;
	ConsoleObserver observer(flags, 1000);

	// Initialize accumulator. 
	TestAccumulator accumulator(flags, 10, 5000);

	// Initialize ensemble. 
	ProtonationMDEnsemble ensemble(world, ffm, mm, 1.2, -1.0, 34435);
	ensemble.AddObserver(&observer);
	ensemble.AddObserver(&accumulator);

	// Run
	ensemble.Run(1000);
}

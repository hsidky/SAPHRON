#include "../src/Simulation/ProtonationMDSimulation.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/ForceFields/DebyeHuckelFF.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Particles/Site.h"
#include "../src/Particles/Molecule.h"
#include "../src/Moves/ImplicitProtonationMove.h"
#include "../src/Moves/ChargeSwapMove.h"
#include "../src/Observers/ConsoleObserver.h"
#include "../src/Worlds/World.h"
#include "TestAccumulator.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(ProtonationMDEnsemble, HighChemical)
{
	// Initialize world.
	World world(1000, 1000, 1000, 100);
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
	DebyeHuckelFF ffe(1.2, 2, 4, 4);
	ForceFieldManager ffm;
	ffm.AddElectrostaticForceField("Monomer", "Monomer", ffe);

	// Initialize moves.
	ImplicitProtonationMove DeprotAcid(-1.0);

	MoveManager Acid;
	Acid.PushMove(DeprotAcid);

	// Initialize observer.
	SimFlags flags;
	//flags.temperature = 1;
	flags.energy = 1;
	flags.iterations = 1;
	flags.acceptance = 1;
	flags.pressure = 1;
	ConsoleObserver observer(flags, 100);
	
	// Initialize ensemble. 
	ProtonationMDEnsemble acidensemble(world, ffm, Acid, 1.2, 1000.0, "M1", 3343);

	// Acid check
	TestAccumulator acidaccumulator(flags, 10, 100);
	acidensemble.AddObserver(&observer);
	acidensemble.AddObserver(&acidaccumulator);
	acidensemble.Run(200);	
	ASSERT_NEAR(7.855041, acidaccumulator.GetAverageEnergy().intraelectrostatic, 1e-5);
		
}

TEST(ProtonationMDEnsemble, LowChemical)
{
	// Initialize world.
	World world(1000, 1000, 1000, 100);
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
	DebyeHuckelFF ffe(1.2, 2, 4, 4);
	ForceFieldManager ffm;
	ffm.AddElectrostaticForceField("Monomer", "Monomer", ffe);

	// Initialize moves.
	ImplicitProtonationMove DeprotAcid(-1.0);

	MoveManager Acid;
	Acid.PushMove(DeprotAcid);

	// Initialize observer.
	SimFlags flags;
	//flags.temperature = 1;
	flags.energy = 1;
	flags.iterations = 1;
	flags.acceptance = 1;
	flags.pressure = 1;
	ConsoleObserver observer(flags, 100);
	
	// Initialize ensemble. 
	ProtonationMDEnsemble acidensemble(world, ffm, Acid, 1.2, -1000.0, "M1", 3343);

	// Acid check
	TestAccumulator acidaccumulator(flags, 10, 100);
	acidensemble.AddObserver(&observer);
	acidensemble.AddObserver(&acidaccumulator);
	acidensemble.Run(200);	
	ASSERT_NEAR(0.0, acidaccumulator.GetAverageEnergy().intraelectrostatic, 1e-5);
		
}

TEST(ProtonationMDEnsemble, ChargeSwap)
{
	// Initialize world.
	World world(1000, 1000, 1000, 100);
	Site* site1= new Site({0, 1, 0}, {0, 0, 0}, "Monomer");
	site1->SetCharge(1.0);
	Site* site2= new Site({0, 0, 0}, {0, 0, 0}, "Monomer");
	site2->SetCharge(1.0);
	Site* site3= new Site({1, 0, 0}, {0, 0, 0}, "Monomer");
	Site* site4= new Site({2, 0, 0}, {0, 0, 0}, "Monomer");
	Site* site5= new Site({3, 0, 0}, {0, 0, 0}, "Monomer");
	Site* site6= new Site({3, 1, 0}, {0, 0, 0}, "Monomer");

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
	DebyeHuckelFF ffe(1000, 2, 4, 4);
	ForceFieldManager ffm;
	ffm.AddElectrostaticForceField("Monomer", "Monomer", ffe);

	// Initialize moves.

	ChargeSwapMove SwapCharge;

	MoveManager MoveCharge;
	MoveCharge.PushMove(SwapCharge);

	// Initialize observer.
	SimFlags flags;
	//flags.temperature = 1;
	flags.energy = 1;
	flags.iterations = 1;
	flags.acceptance = 1;
	flags.pressure = 1;
	ConsoleObserver observer(flags, 50000);
	
	// Initialize ensemble. 
	ProtonationMDEnsemble chargeensemble(world, ffm, MoveCharge, 1.2, -1000.0, "M1", 3343);

	// Acid check
	TestAccumulator chargeaccumulator(flags, 10, 50000);
	chargeensemble.AddObserver(&observer);
	chargeensemble.AddObserver(&chargeaccumulator);
	chargeensemble.Run(200000);	
	ASSERT_NEAR(260.243638, chargeaccumulator.GetAverageEnergy().intraelectrostatic, 1e-5);
		
}
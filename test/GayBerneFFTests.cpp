#include "../src/NewForceFields/GayBerneFF.h"
#include "../src/NewForceFields/ForceFieldManager.h"
#include "../src/Simulation/StandardSimulation.h"
#include "../src/NewMoves/MoveManager.h"
#include "../src/NewMoves/TranslateMove.h"
#include "../src/Particles/NewParticle.h"
#include "gtest/gtest.h"
#include <fstream>

#ifdef MULTI_WALKER
#include <boost/mpi.hpp>
using namespace boost;
#endif

using namespace SAPHRON;

// Verified against OpenMD 2.4.
TEST(GayBerneFF, DefaultBehavior)
{
	double rcut = 55.0;
	GayBerneFF ff(16.27, 16.27, 3.254, 3.254, 418.0, 10.0, 1.0, 0.20, {rcut}, 1.0, 2.0);

	Site s1;
	Site s2;

	s2.position = {16.6855, 0, 0};
	auto u = ff.EvaluateEnergy(s1, s2, s1.position - s2.position, (s1.position - s2.position).squaredNorm(), 0);
	ASSERT_NEAR(u, -2823.5, 1e-1);
	
	s2.position = {0, 0, 32.5468};
	u = ff.EvaluateEnergy(s1, s2, s1.position - s2.position, (s1.position - s2.position).squaredNorm(), 0);
	ASSERT_NEAR(u, -0.1129, 1e-4);
}

TEST(GayBerneFF, OblateEllipsoid)
{
	double rcut = 1.6;
	GayBerneFF ff(1.0, 1.0, 0.345, 0.345, 1.0, 1.0, 0.20, 0.345, {rcut}, 1.0, 2.0);

	Site s1;
	Site s2;
	s2.position = {0., 0., 0.340};
	for(int i = 0; i < 100; ++i)
	{
		s2.position[2] += i*0.0002;
		auto u = ff.EvaluateEnergy(s1, s2, s1.position - s2.position, (s1.position - s2.position).squaredNorm(), 0);
		std::cout << s2.position[2] << " " << u << std::endl;
	}
}

/*
// Tests conservation of energy in a system with GB 
// particles and no skin thickness. (rcut = nlist).
TEST(GayBerneFF, NoSkinConservation)
{
	#ifdef MULTI_WALKER
	mpi::environment env;
	mpi::communicator comm;
	#endif
	
	// Load file (assumes we are in build folder).
	std::ifstream t("../test/gb_no_skin.json");
	std::stringstream buffer;
	buffer << t.rdbuf();

	// Read JSON.
	Json::Reader reader;
	Json::Value root;
	ASSERT_TRUE(reader.parse(buffer, root));

	// Build world.
	WorldManager wm;
	World* w;
	ASSERT_NO_THROW(w = World::Build(root["worlds"][0], root["blueprints"]));
	ASSERT_NE(nullptr, w);
	wm.AddWorld(w);

	ASSERT_EQ(338, w->GetParticleCount());
	ASSERT_EQ(5.0, w->GetNeighborRadius());
	ASSERT_EQ(0.0, w->GetSkinThickness());

	// Gay Berne interaction.
	ForceFieldManager ffm;
	GayBerneFF ff(1.0, 1.0, 3.0, 3.0, 1.0, 1.0, 5.0, 1.0, {5.0}, 2.0, 1.0);
	ffm.AddNonBondedForceField("GB", "GB", ff);

	// Get constraints.
	std::vector<Constraint*> constraints;
	ASSERT_NO_THROW(Constraint::BuildConstraints(root["forcefields"]["constraints"], &ffm, &wm, constraints));

	// Get moves.
	MoveManager mm;
	MoveList moves;
	ASSERT_NO_THROW(Move::BuildMoves(root["moves"], &mm, &wm, moves));

	// Initialize simulation. 
	StandardSimulation sim(&wm, &ffm, &mm);

	sim.Run(100);

	// Conservation of energy and pressure.
	auto H = ffm.EvaluateEnergy(*w);
	ASSERT_NEAR(H.energy.total(), w->GetEnergy().total(), 1e-10);

	delete w;
	for(auto& m : mm)
		delete m;

	for(auto& c : constraints)
		delete c;
}*/
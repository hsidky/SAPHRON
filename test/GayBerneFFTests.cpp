#include "../src/ForceFields/GayBerneFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Simulation/StandardSimulation.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Particles/Site.h"
#include "../src/Particles/Molecule.h"
#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "TestAccumulator.h"
#include "json/json.h"
#include "gtest/gtest.h"
#include <fstream>

using namespace SAPHRON;

// Verified against OpenMD 2.4.
TEST(GayBerneFF, DefaultBehavior)
{
	double rcut = 55.0;
	GayBerneFF ff(16.27, 16.27, 3.254, 3.254, 418.0, 10.0, 1.0, 0.20, {rcut}, 1.0, 2.0);

	Site s1({0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, "L1");
	Site s2({0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, "L1");
	s1.AddNeighbor(&s2);
	s2.AddNeighbor(&s1);

	s2.SetPosition({16.6855, 0, 0});
	auto NB = ff.Evaluate(s1, s2, s1.GetPosition() - s2.GetPosition(), 0);
	ASSERT_NEAR(NB.energy, -2823.5, 1e-1);
	
	s2.SetPosition({0, 0, 32.5468});
	NB = ff.Evaluate(s1, s2, s1.GetPosition() - s2.GetPosition(), 0);
	ASSERT_NEAR(NB.energy, -0.1129, 1e-4);
}
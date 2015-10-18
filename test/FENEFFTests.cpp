#include "../src/ForceFields/FENEFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Simulation/NVTSimulation.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Particles/Site.h"
#include "../src/Observers/ConsoleObserver.h"
#include "../src/Observers/DLMFileObserver.h"
#include "../src/Observers/JSONObserver.h"
#include "TestAccumulator.h"
#include "../src/Worlds/SimpleWorld.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(FENEFF, DefaultBehavior)
{
	FENEFF ff(1.0, 1.0, 30.0, 1.5);
	Site s1({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site s2({0.9, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site s3({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site s4({1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site s5({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	Site s6({6.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, "L1");
	s1.AddBondedNeighbor(&s2);
	s2.AddBondedNeighbor(&s1);
	s3.AddBondedNeighbor(&s4);
	s4.AddBondedNeighbor(&s3);
	s5.AddBondedNeighbor(&s6);
	s6.AddBondedNeighbor(&s5);

	ForceFieldManager ffm;
	ffm.AddNonBondedForceField("L1", "L1", ff);

	CompositionList compositions = 
	{
		{s1.GetSpeciesID(), 1000}
	};

	// Validate invidual components first.
	auto NB = ff.Evaluate(s1, s2, s1.GetPosition() - s2.GetPosition());
	ASSERT_NEAR(22.698, NB.energy, 1e-3);
	auto NB2 = ff.Evaluate(s3, s4, s3.GetPosition() - s4.GetPosition());
	ASSERT_NEAR(20.838, NB2.energy, 1e-3);
	auto NB3 = ff.Evaluate(s5, s6, s5.GetPosition() - s6.GetPosition());
	ASSERT_EQ(3E100, NB3.energy);

}

TEST(FENEFF, ReducedProperties)
{
	//TODO: Find an experimental system to check FENE potential?
}

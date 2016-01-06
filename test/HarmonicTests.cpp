#include "../src/ForceFields/HarmonicFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Particles/Site.h"
#include "../src/Observers/DLMFileObserver.h"
#include "../src/Observers/JSONObserver.h"
#include "TestAccumulator.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(Harmonic, DefaultBehavior)
{
	Harmonic ff(0.9, 1.0);
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
	auto NB = ff.Evaluate(s1, s2, s1.GetPosition() - s2.GetPosition(), 0);
	ASSERT_NEAR(0.0045, NB.energy, 1e-3);
	auto NB2 = ff.Evaluate(s3, s4, s3.GetPosition() - s4.GetPosition(), 0);
	ASSERT_NEAR(0.0, NB2.energy, 1e-3);
	auto NB3 = ff.Evaluate(s5, s6, s5.GetPosition() - s6.GetPosition(), 0);
	ASSERT_NEAR(11.25, NB3.energy, 1e-3);

}

TEST(Harmonic, ReducedProperties)
{
	//TODO: Find an experimental system to check Harmonic potential?
}
